/// Represents an authenticated user returned by the auth endpoints.
class User {
  final String id;
  final String name;
  final String email;
  final String role;
  final bool mustChangePassword;

  const User({
    required this.id,
    required this.name,
    required this.email,
    required this.role,
    this.mustChangePassword = false,
  });

  factory User.fromJson(Map<String, dynamic> json) {
    return User(
      id: (json['id'] ?? json['_id'] ?? '').toString(),
      name: (json['name'] ?? json['fullName'] ?? '').toString(),
      email: (json['email'] ?? '').toString(),
      role: (json['role'] ?? json['roleName'] ?? 'Technician').toString(),
      mustChangePassword:
          json['mustChangePassword'] == true || json['mustChangePassword'] == 1,
    );
  }

  Map<String, dynamic> toJson() => <String, dynamic>{
        'id': id,
        'name': name,
        'email': email,
        'role': role,
        'mustChangePassword': mustChangePassword,
      };

  User copyWith({
    String? id,
    String? name,
    String? email,
    String? role,
    bool? mustChangePassword,
  }) {
    return User(
      id: id ?? this.id,
      name: name ?? this.name,
      email: email ?? this.email,
      role: role ?? this.role,
      mustChangePassword: mustChangePassword ?? this.mustChangePassword,
    );
  }

  /// Convenience: initials for avatars.
  String get initials {
    final List<String> parts =
        name.trim().split(RegExp(r'\s+')).where((String p) => p.isNotEmpty).toList();
    if (parts.isEmpty) {
      return email.isNotEmpty ? email[0].toUpperCase() : '?';
    }
    if (parts.length == 1) {
      return parts.first.substring(0, 1).toUpperCase();
    }
    return (parts.first.substring(0, 1) + parts.last.substring(0, 1))
        .toUpperCase();
  }
}
