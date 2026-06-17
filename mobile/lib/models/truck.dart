/// A service truck that can be assigned to a technician and stocked.
class Truck {
  final String id;
  final String truckNumber;
  final String? plateNumber;
  final String? status;
  final String? technicianId;

  const Truck({
    required this.id,
    required this.truckNumber,
    this.plateNumber,
    this.status,
    this.technicianId,
  });

  factory Truck.fromJson(Map<String, dynamic> json) {
    return Truck(
      id: (json['id'] ?? json['_id'] ?? '').toString(),
      truckNumber:
          (json['truckNumber'] ?? json['number'] ?? json['name'] ?? '')
              .toString(),
      plateNumber: json['plateNumber']?.toString(),
      status: json['status']?.toString(),
      technicianId:
          (json['technicianId'] ?? json['technician']?['id'])?.toString(),
    );
  }

  Map<String, dynamic> toJson() {
    final Map<String, dynamic> map = <String, dynamic>{
      'truckNumber': truckNumber,
    };
    if (plateNumber != null) map['plateNumber'] = plateNumber;
    if (status != null) map['status'] = status;
    if (technicianId != null) map['technicianId'] = technicianId;
    return map;
  }

  Truck copyWith({
    String? id,
    String? truckNumber,
    String? plateNumber,
    String? status,
    String? technicianId,
  }) {
    return Truck(
      id: id ?? this.id,
      truckNumber: truckNumber ?? this.truckNumber,
      plateNumber: plateNumber ?? this.plateNumber,
      status: status ?? this.status,
      technicianId: technicianId ?? this.technicianId,
    );
  }
}
