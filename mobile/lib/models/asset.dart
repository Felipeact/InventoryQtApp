/// A trackable physical asset (tool, device, equipment).
class Asset {
  final String id;
  final String name;
  final String type;
  final String serialCode;
  final String? status;
  final String? description;
  final DateTime? createdAt;

  const Asset({
    required this.id,
    required this.name,
    required this.type,
    required this.serialCode,
    this.status,
    this.description,
    this.createdAt,
  });

  factory Asset.fromJson(Map<String, dynamic> json) {
    return Asset(
      id: (json['id'] ?? json['_id'] ?? '').toString(),
      name: (json['name'] ?? '').toString(),
      type: (json['type'] ?? '').toString(),
      serialCode: (json['serialCode'] ?? json['serial'] ?? '').toString(),
      status: json['status']?.toString(),
      description: json['description']?.toString(),
      createdAt: _parseDate(json['createdAt']),
    );
  }

  Map<String, dynamic> toJson() {
    final Map<String, dynamic> map = <String, dynamic>{
      'name': name,
      'type': type,
      'serialCode': serialCode,
    };
    if (status != null) map['status'] = status;
    if (description != null) map['description'] = description;
    return map;
  }

  Asset copyWith({
    String? id,
    String? name,
    String? type,
    String? serialCode,
    String? status,
    String? description,
    DateTime? createdAt,
  }) {
    return Asset(
      id: id ?? this.id,
      name: name ?? this.name,
      type: type ?? this.type,
      serialCode: serialCode ?? this.serialCode,
      status: status ?? this.status,
      description: description ?? this.description,
      createdAt: createdAt ?? this.createdAt,
    );
  }

  static DateTime? _parseDate(dynamic value) {
    if (value == null) return null;
    return DateTime.tryParse(value.toString());
  }
}
