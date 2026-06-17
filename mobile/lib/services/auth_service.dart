import '../models/auth_session.dart';
import 'api_client.dart';

/// Wraps the `/auth/*` endpoints.
class AuthService {
  AuthService({ApiClient? client}) : _client = client ?? ApiClient.instance;

  final ApiClient _client;

  Future<AuthSession> login({
    required String email,
    required String password,
  }) async {
    final dynamic data = await _client.post(
      '/auth/login',
      body: <String, dynamic>{'email': email, 'password': password},
      authFree: true,
    );
    return _parseSession(data);
  }

  Future<AuthSession> register({
    required String email,
    required String password,
    required String code,
    required String companyName,
  }) async {
    final dynamic data = await _client.post(
      '/auth/register',
      body: <String, dynamic>{
        'email': email,
        'password': password,
        'code': code,
        'companyName': companyName,
      },
      authFree: true,
    );
    return _parseSession(data);
  }

  Future<void> changePassword({
    required String currentPassword,
    required String newPassword,
  }) async {
    await _client.post(
      '/auth/change-password',
      body: <String, dynamic>{
        'currentPassword': currentPassword,
        'newPassword': newPassword,
      },
    );
  }

  Future<void> requestReset(String email) async {
    await _client.post(
      '/auth/request-reset',
      body: <String, dynamic>{'email': email},
      authFree: true,
    );
  }

  Future<void> logout(String refreshToken) async {
    try {
      await _client.post(
        '/auth/logout',
        body: <String, dynamic>{'refreshToken': refreshToken},
      );
    } on ApiException {
      // Logout should succeed locally even if the server call fails.
    }
  }

  AuthSession _parseSession(dynamic data) {
    if (data is Map) {
      return AuthSession.fromJson(data.cast<String, dynamic>());
    }
    throw const ApiException('Unexpected response from the server.');
  }
}
