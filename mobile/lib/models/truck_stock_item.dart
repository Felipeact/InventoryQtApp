/// A single item line in a truck-stock template or assignment.
class TruckStockItem {
  final String id;
  final String productName;
  final String? category;
  final int requiredQuantity;
  final int currentQuantity;
  final int minimumQuantity;
  final double? expectedPrice;
  final String? unit;
  final String? notes;

  const TruckStockItem({
    required this.id,
    required this.productName,
    this.category,
    this.requiredQuantity = 0,
    this.currentQuantity = 0,
    this.minimumQuantity = 0,
    this.expectedPrice,
    this.unit,
    this.notes,
  });

  factory TruckStockItem.fromJson(Map<String, dynamic> json) {
    return TruckStockItem(
      id: (json['id'] ?? json['_id'] ?? json['truckStockItemId'] ?? '')
          .toString(),
      productName:
          (json['productName'] ?? json['name'] ?? json['product']?['name'] ?? '')
              .toString(),
      category: json['category']?.toString(),
      requiredQuantity: _asInt(json['requiredQuantity']),
      currentQuantity: _asInt(json['currentQuantity']),
      minimumQuantity: _asInt(json['minimumQuantity']),
      expectedPrice: _asNullableDouble(json['expectedPrice']),
      unit: json['unit']?.toString(),
      notes: json['notes']?.toString(),
    );
  }

  Map<String, dynamic> toJson() => <String, dynamic>{
        'id': id,
        'productName': productName,
        'category': category,
        'requiredQuantity': requiredQuantity,
        'currentQuantity': currentQuantity,
        'minimumQuantity': minimumQuantity,
        'expectedPrice': expectedPrice,
        'unit': unit,
        'notes': notes,
      };

  TruckStockItem copyWith({
    String? id,
    String? productName,
    String? category,
    int? requiredQuantity,
    int? currentQuantity,
    int? minimumQuantity,
    double? expectedPrice,
    String? unit,
    String? notes,
  }) {
    return TruckStockItem(
      id: id ?? this.id,
      productName: productName ?? this.productName,
      category: category ?? this.category,
      requiredQuantity: requiredQuantity ?? this.requiredQuantity,
      currentQuantity: currentQuantity ?? this.currentQuantity,
      minimumQuantity: minimumQuantity ?? this.minimumQuantity,
      expectedPrice: expectedPrice ?? this.expectedPrice,
      unit: unit ?? this.unit,
      notes: notes ?? this.notes,
    );
  }

  bool get isLow =>
      currentQuantity <= minimumQuantity ||
      (requiredQuantity > 0 && currentQuantity < requiredQuantity);

  bool get isCritical => currentQuantity <= minimumQuantity;

  /// 0..1 fill ratio against the required quantity (clamped).
  double get fillRatio {
    if (requiredQuantity <= 0) return currentQuantity > 0 ? 1.0 : 0.0;
    return (currentQuantity / requiredQuantity).clamp(0.0, 1.0);
  }

  static int _asInt(dynamic value) {
    if (value == null) return 0;
    if (value is int) return value;
    if (value is double) return value.round();
    return int.tryParse(value.toString()) ?? 0;
  }

  static double? _asNullableDouble(dynamic value) {
    if (value == null) return null;
    if (value is num) return value.toDouble();
    return double.tryParse(value.toString());
  }
}
