/// A single stock movement / audit record.
class StockMovement {
  final String id;
  final String action;
  final int previousQuantity;
  final int newQuantity;
  final String? notes;
  final DateTime? createdAt;
  final String? productName;

  const StockMovement({
    required this.id,
    required this.action,
    this.previousQuantity = 0,
    this.newQuantity = 0,
    this.notes,
    this.createdAt,
    this.productName,
  });

  factory StockMovement.fromJson(Map<String, dynamic> json) {
    return StockMovement(
      id: (json['id'] ?? json['_id'] ?? '').toString(),
      action: (json['action'] ?? json['type'] ?? '').toString(),
      previousQuantity: _asInt(json['previousQuantity']),
      newQuantity: _asInt(json['newQuantity']),
      notes: json['notes']?.toString(),
      createdAt: _parseDate(json['createdAt']),
      productName:
          (json['productName'] ?? json['product']?['name'] ?? json['itemName'])
              ?.toString(),
    );
  }

  Map<String, dynamic> toJson() => <String, dynamic>{
        'id': id,
        'action': action,
        'previousQuantity': previousQuantity,
        'newQuantity': newQuantity,
        'notes': notes,
        'createdAt': createdAt?.toIso8601String(),
        'productName': productName,
      };

  int get delta => newQuantity - previousQuantity;

  bool get isIncrease => delta > 0;

  static int _asInt(dynamic value) {
    if (value == null) return 0;
    if (value is int) return value;
    if (value is double) return value.round();
    return int.tryParse(value.toString()) ?? 0;
  }

  static DateTime? _parseDate(dynamic value) {
    if (value == null) return null;
    return DateTime.tryParse(value.toString());
  }
}
