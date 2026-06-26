/// A purchase/expense receipt uploaded against a truck.
class Receipt {
  final String id;
  final String truckId;
  final String fileUrl;
  final String? status;
  final double? totalAmount;
  final DateTime? createdAt;
  final String? truckNumber;

  const Receipt({
    required this.id,
    required this.truckId,
    required this.fileUrl,
    this.status,
    this.totalAmount,
    this.createdAt,
    this.truckNumber,
  });

  factory Receipt.fromJson(Map<String, dynamic> json) {
    return Receipt(
      id: (json['id'] ?? json['_id'] ?? '').toString(),
      truckId: (json['truckId'] ?? json['truck']?['id'] ?? '').toString(),
      fileUrl: (json['fileUrl'] ?? json['url'] ?? '').toString(),
      status: json['status']?.toString(),
      totalAmount: _asNullableDouble(json['totalAmount']),
      createdAt: _parseDate(json['createdAt']),
      truckNumber:
          (json['truckNumber'] ?? json['truck']?['truckNumber'])?.toString(),
    );
  }

  /// A copy of this receipt with a new [status] (used after reconcile).
  Receipt copyWithStatus(String newStatus) => Receipt(
        id: id,
        truckId: truckId,
        fileUrl: fileUrl,
        status: newStatus,
        totalAmount: totalAmount,
        createdAt: createdAt,
        truckNumber: truckNumber,
      );

  Map<String, dynamic> toJson() {
    final Map<String, dynamic> map = <String, dynamic>{
      'truckId': truckId,
      'fileUrl': fileUrl,
    };
    if (totalAmount != null) map['totalAmount'] = totalAmount;
    return map;
  }

  static double? _asNullableDouble(dynamic value) {
    if (value == null) return null;
    if (value is num) return value.toDouble();
    return double.tryParse(value.toString());
  }

  static DateTime? _parseDate(dynamic value) {
    if (value == null) return null;
    return DateTime.tryParse(value.toString());
  }
}
