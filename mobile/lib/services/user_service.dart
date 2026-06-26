import '../models/user.dart';
import 'api_client.dart';

/// Wraps the `/users/*` endpoints (requires MANAGE_USERS server-side).
class UserService {
  UserService({ApiClient? client}) : _client = client ?? ApiClient.instance;

  final ApiClient _client;

  Future<List<User>> fetchUsers() async {
    final dynamic data = await _client.get('/users');
    Iterable<dynamic> raw;
    if (data is List) {
      raw = data;
    } else if (data is Map && data['users'] is List) {
      raw = data['users'] as List;
    } else if (data is Map && data['data'] is List) {
      raw = data['data'] as List;
    } else {
      raw = const <dynamic>[];
    }
    return raw
        .whereType<Map>()
        .map((Map e) => User.fromJson(e.cast<String, dynamic>()))
        .toList();
  }

  /// Invites a user. Returns the raw response (may include a
  /// `temporaryPassword` when no invite email could be sent).
  Future<Map<String, dynamic>> inviteUser({
    required String email,
    required String role,
    String? name,
  }) async {
    final Map<String, dynamic> body = <String, dynamic>{
      'email': email,
      'role': role,
      if (name != null && name.isNotEmpty) 'name': name,
    };
    final dynamic data = await _client.post('/users/invite', body: body);
    return data is Map ? data.cast<String, dynamic>() : <String, dynamic>{};
  }

  Future<User> updateUser(
    String id, {
    String? name,
    String? email,
    String? role,
  }) async {
    final Map<String, dynamic> body = <String, dynamic>{};
    if (name != null) body['name'] = name;
    if (email != null) body['email'] = email;
    if (role != null) body['role'] = role;
    final dynamic data = await _client.put('/users/$id', body: body);
    return User.fromJson((data as Map).cast<String, dynamic>());
  }

  /// Resets a user's password and returns the temporary password.
  Future<String> resetPassword(String id) async {
    final dynamic data = await _client.post('/users/$id/reset-password');
    return data is Map ? (data['temporaryPassword'] ?? '').toString() : '';
  }

  Future<void> deleteUser(String id) async {
    await _client.delete('/users/$id');
  }
}
