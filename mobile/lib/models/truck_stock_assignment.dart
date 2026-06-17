/// Links a [Truck] to a [TruckStockTemplate].
class TruckStockAssignment {
  final String id;
  final String truckId;
  final String templateId;
  final String? truckNumber;
  final String? templateName;
  final String? technicianName;

  const TruckStockAssignment({
    required this.id,
    required this.truckId,
    required this.templateId,
    this.truckNumber,
    this.templateName,
    this.technicianName,
  });

  factory TruckStockAssignment.fromJson(Map<String, dynamic> json) {
    return TruckStockAssignment(
      id: (json['id'] ?? json['_id'] ?? '').toString(),
      truckId: (json['truckId'] ?? json['truck']?['id'] ?? '').toString(),
      templateId:
          (json['templateId'] ?? json['template']?['id'] ?? '').toString(),
      truckNumber:
          (json['truckNumber'] ?? json['truck']?['truckNumber'])?.toString(),
      templateName:
          (json['templateName'] ?? json['template']?['name'])?.toString(),
      technicianName: (json['technicianName'] ??
              json['technician']?['name'])
          ?.toString(),
    );
  }

  Map<String, dynamic> toJson() => <String, dynamic>{
        'id': id,
        'truckId': truckId,
        'templateId': templateId,
        'truckNumber': truckNumber,
        'templateName': templateName,
        'technicianName': technicianName,
      };
}
