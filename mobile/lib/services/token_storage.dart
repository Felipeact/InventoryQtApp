import 'dart:convert';

import 'package:flutter_secure_storage/flutter_secure_storage.dart';

import '../models/auth_session.dart';

/// Persists the auth session (tokens + user) securely on-device.
class TokenStorage {
  TokenStorage._();

  static final TokenStorage instance = TokenStorage._();

  static const FlutterSecureStorage _storage = FlutterSecureStorage(
    aOptions: AndroidOptions(encryptedSharedPreferences: true),
  );

  static const String _kAccess = 'auth.access_token';
  static const String _kRefresh = 'auth.refresh_token';
  static const String _kUser = 'auth.user';

  Future<void> saveSession(AuthSession session) async {
    await _storage.write(key: _kAccess, value: session.accessToken);
    await _storage.write(key: _kRefresh, value: session.refreshToken);
    await _storage.write(key: _kUser, value: jsonEncode(session.user.toJson()));
  }

  Future<void> saveAccessToken(String token) async {
    await _storage.write(key: _kAccess, value: token);
  }

  Future<String?> readAccessToken() => _storage.read(key: _kAccess);

  Future<String?> readRefreshToken() => _storage.read(key: _kRefresh);

  /// Reads a full session if all parts are present, otherwise null.
  Future<AuthSession?> readSession() async {
    final String? access = await _storage.read(key: _kAccess);
    final String? refresh = await _storage.read(key: _kRefresh);
    final String? userJson = await _storage.read(key: _kUser);
    if (access == null || access.isEmpty || userJson == null) {
      return null;
    }
    try {
      final Map<String, dynamic> map =
          jsonDecode(userJson) as Map<String, dynamic>;
      return AuthSession.fromJson(<String, dynamic>{
        'accessToken': access,
        'refreshToken': refresh ?? '',
        'user': map,
      });
    } catch (_) {
      return null;
    }
  }

  Future<void> clear() async {
    await _storage.delete(key: _kAccess);
    await _storage.delete(key: _kRefresh);
    await _storage.delete(key: _kUser);
  }
}
