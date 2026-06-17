import 'user.dart';

/// Bundles the JWT tokens together with the authenticated [User].
class AuthSession {
  final String accessToken;
  final String refreshToken;
  final User user;

  const AuthSession({
    required this.accessToken,
    required this.refreshToken,
    required this.user,
  });

  factory AuthSession.fromJson(Map<String, dynamic> json) {
    final Map<String, dynamic> userJson =
        (json['user'] as Map?)?.cast<String, dynamic>() ?? <String, dynamic>{};
    return AuthSession(
      accessToken: (json['accessToken'] ?? json['token'] ?? '').toString(),
      refreshToken: (json['refreshToken'] ?? '').toString(),
      user: User.fromJson(userJson),
    );
  }

  Map<String, dynamic> toJson() => <String, dynamic>{
        'accessToken': accessToken,
        'refreshToken': refreshToken,
        'user': user.toJson(),
      };

  AuthSession copyWith({
    String? accessToken,
    String? refreshToken,
    User? user,
  }) {
    return AuthSession(
      accessToken: accessToken ?? this.accessToken,
      refreshToken: refreshToken ?? this.refreshToken,
      user: user ?? this.user,
    );
  }
}
