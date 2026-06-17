import 'dart:async';

import 'package:dio/dio.dart';

import '../config/app_config.dart';
import 'token_storage.dart';

/// Thrown by services when a request fails; carries a user-friendly message.
class ApiException implements Exception {
  final String message;
  final int? statusCode;

  const ApiException(this.message, {this.statusCode});

  @override
  String toString() => message;
}

/// Singleton Dio wrapper.
///
/// Responsibilities:
///  * inject the bearer access token on every request,
///  * transparently refresh the access token on a 401 and retry once,
///  * surface a logout signal when refresh ultimately fails.
class ApiClient {
  ApiClient._() {
    _dio = Dio(
      BaseOptions(
        connectTimeout: const Duration(seconds: 20),
        receiveTimeout: const Duration(seconds: 30),
        sendTimeout: const Duration(seconds: 30),
        // Treat any non-5xx as a "successful" HTTP completion so we can map
        // 4xx into ApiException ourselves with the server message.
        validateStatus: (int? status) => status != null && status < 500,
        contentType: Headers.jsonContentType,
        responseType: ResponseType.json,
      ),
    );
    _dio.interceptors.add(_authInterceptor());
  }

  static final ApiClient instance = ApiClient._();

  late final Dio _dio;

  /// Callback invoked when the session can no longer be refreshed and the user
  /// must be logged out. Wired up by [AuthProvider].
  void Function()? onUnauthorized;

  /// Guards against multiple concurrent refresh attempts.
  Completer<String?>? _refreshCompleter;

  Dio get dio => _dio;

  /// Always resolve the base URL lazily so a Settings change takes effect
  /// without recreating the client.
  String get _baseUrl => AppConfig.instance.baseUrl;

  InterceptorsWrapper _authInterceptor() {
    return InterceptorsWrapper(
      onRequest: (RequestOptions options, RequestInterceptorHandler handler) async {
        // Endpoints that never require (and must not carry) a bearer token.
        final bool isAuthFree = options.extra['authFree'] == true;
        if (!isAuthFree) {
          final String? token = await TokenStorage.instance.readAccessToken();
          if (token != null && token.isNotEmpty) {
            options.headers['Authorization'] = 'Bearer $token';
          }
        }
        handler.next(options);
      },
      onResponse: (Response<dynamic> response,
          ResponseInterceptorHandler handler) async {
        final int status = response.statusCode ?? 0;
        // Because validateStatus accepts 4xx, intercept 401 here to refresh.
        if (status == 401 && response.requestOptions.extra['retried'] != true &&
            response.requestOptions.extra['authFree'] != true) {
          try {
            final Response<dynamic> retried = await _refreshAndRetry(response);
            return handler.resolve(retried);
          } on ApiException {
            // Refresh failed; trigger logout and fall through with original.
            onUnauthorized?.call();
            return handler.next(response);
          }
        }
        handler.next(response);
      },
      onError: (DioException error, ErrorInterceptorHandler handler) async {
        final Response<dynamic>? response = error.response;
        if (response?.statusCode == 401 &&
            error.requestOptions.extra['retried'] != true &&
            error.requestOptions.extra['authFree'] != true) {
          try {
            final Response<dynamic> retried = await _refreshAndRetry(response!);
            return handler.resolve(retried);
          } on ApiException {
            onUnauthorized?.call();
          }
        }
        handler.next(error);
      },
    );
  }

  /// Refreshes the access token (deduplicating concurrent calls) and replays
  /// the original failed request exactly once.
  Future<Response<dynamic>> _refreshAndRetry(Response<dynamic> original) async {
    final String? newToken = await _refreshAccessToken();
    if (newToken == null || newToken.isEmpty) {
      throw const ApiException('Session expired', statusCode: 401);
    }

    final RequestOptions ro = original.requestOptions;
    final Options retryOptions = Options(
      method: ro.method,
      headers: Map<String, dynamic>.from(ro.headers)
        ..['Authorization'] = 'Bearer $newToken',
      contentType: ro.contentType,
      responseType: ro.responseType,
      extra: <String, dynamic>{...ro.extra, 'retried': true},
    );

    return _dio.request<dynamic>(
      ro.path,
      data: ro.data,
      queryParameters: ro.queryParameters,
      options: retryOptions,
    );
  }

  /// Calls POST /auth/refresh with the stored refresh token. Returns the new
  /// access token or null. Concurrent callers share a single in-flight call.
  Future<String?> _refreshAccessToken() {
    if (_refreshCompleter != null) {
      return _refreshCompleter!.future;
    }
    final Completer<String?> completer = Completer<String?>();
    _refreshCompleter = completer;

    () async {
      try {
        final String? refreshToken =
            await TokenStorage.instance.readRefreshToken();
        if (refreshToken == null || refreshToken.isEmpty) {
          completer.complete(null);
          return;
        }
        final Response<dynamic> res = await _dio.post<dynamic>(
          '$_baseUrl/auth/refresh',
          data: <String, dynamic>{'refreshToken': refreshToken},
          options: Options(extra: <String, dynamic>{'authFree': true}),
        );
        if (res.statusCode == 200 && res.data is Map) {
          final String? token =
              (res.data as Map)['accessToken']?.toString();
          if (token != null && token.isNotEmpty) {
            await TokenStorage.instance.saveAccessToken(token);
            completer.complete(token);
            return;
          }
        }
        completer.complete(null);
      } catch (_) {
        completer.complete(null);
      } finally {
        _refreshCompleter = null;
      }
    }();

    return completer.future;
  }

  // ---- Convenience verbs that normalize errors into ApiException ----------

  Future<dynamic> get(
    String path, {
    Map<String, dynamic>? query,
    bool authFree = false,
  }) async {
    return _send(() => _dio.get<dynamic>(
          '$_baseUrl$path',
          queryParameters: query,
          options: Options(extra: <String, dynamic>{'authFree': authFree}),
        ));
  }

  Future<dynamic> post(
    String path, {
    Object? body,
    bool authFree = false,
  }) async {
    return _send(() => _dio.post<dynamic>(
          '$_baseUrl$path',
          data: body,
          options: Options(extra: <String, dynamic>{'authFree': authFree}),
        ));
  }

  Future<dynamic> put(String path, {Object? body}) async {
    return _send(() => _dio.put<dynamic>('$_baseUrl$path', data: body));
  }

  Future<dynamic> patch(String path, {Object? body}) async {
    return _send(() => _dio.patch<dynamic>('$_baseUrl$path', data: body));
  }

  Future<dynamic> delete(String path, {Object? body}) async {
    return _send(() => _dio.delete<dynamic>('$_baseUrl$path', data: body));
  }

  Future<dynamic> _send(Future<Response<dynamic>> Function() run) async {
    try {
      final Response<dynamic> res = await run();
      final int status = res.statusCode ?? 0;
      if (status >= 200 && status < 300) {
        return res.data;
      }
      throw ApiException(_messageFromResponse(res), statusCode: status);
    } on DioException catch (e) {
      throw ApiException(_messageFromDio(e), statusCode: e.response?.statusCode);
    }
  }

  String _messageFromResponse(Response<dynamic> res) {
    final dynamic data = res.data;
    if (data is Map) {
      final dynamic msg = data['message'] ?? data['error'] ?? data['detail'];
      if (msg is List && msg.isNotEmpty) return msg.join(', ');
      if (msg != null) return msg.toString();
    }
    if (data is String && data.isNotEmpty) return data;
    return 'Request failed (${res.statusCode}).';
  }

  String _messageFromDio(DioException e) {
    switch (e.type) {
      case DioExceptionType.connectionTimeout:
      case DioExceptionType.sendTimeout:
      case DioExceptionType.receiveTimeout:
        return 'The server took too long to respond. Please try again.';
      case DioExceptionType.connectionError:
        return 'Could not reach the server. Check your connection and the API URL.';
      case DioExceptionType.badCertificate:
        return 'A secure connection could not be established.';
      case DioExceptionType.cancel:
        return 'Request cancelled.';
      case DioExceptionType.badResponse:
        return e.response != null
            ? _messageFromResponse(e.response!)
            : 'The server returned an error.';
      case DioExceptionType.unknown:
        return e.message ?? 'An unexpected network error occurred.';
    }
  }
}
