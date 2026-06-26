/// A line item read from a receipt by the AI extractor.
class ExtractedReceiptItem {
  final String itemName;
  final int quantity;
  final double? unitPrice;
  final double? totalPrice;

  const ExtractedReceiptItem({
    required this.itemName,
    required this.quantity,
    this.unitPrice,
    this.totalPrice,
  });

  factory ExtractedReceiptItem.fromJson(Map<String, dynamic> json) =>
      ExtractedReceiptItem(
        itemName: (json['itemName'] ?? '').toString(),
        quantity: (json['quantity'] is num)
            ? (json['quantity'] as num).toInt()
            : int.tryParse('${json['quantity']}') ?? 0,
        unitPrice: _num(json['unitPrice']),
        totalPrice: _num(json['totalPrice']),
      );

  Map<String, dynamic> toJson() => <String, dynamic>{
        'itemName': itemName,
        'quantity': quantity,
        if (unitPrice != null) 'unitPrice': unitPrice,
        if (totalPrice != null) 'totalPrice': totalPrice,
      };

  static double? _num(dynamic v) =>
      v is num ? v.toDouble() : double.tryParse('${v ?? ''}');
}

/// The structured result of AI-reading a receipt (total + line items).
class ExtractedReceipt {
  final double? total;
  final String? currency;
  final String? supplier;
  final List<ExtractedReceiptItem> items;

  const ExtractedReceipt({
    this.total,
    this.currency,
    this.supplier,
    this.items = const <ExtractedReceiptItem>[],
  });

  factory ExtractedReceipt.fromJson(Map<String, dynamic> json) {
    final dynamic raw = json['items'];
    return ExtractedReceipt(
      total: ExtractedReceiptItem._num(json['total']),
      currency: json['currency']?.toString(),
      supplier: json['supplier']?.toString(),
      items: raw is List
          ? raw
              .whereType<Map>()
              .map((Map e) =>
                  ExtractedReceiptItem.fromJson(e.cast<String, dynamic>()))
              .toList()
          : <ExtractedReceiptItem>[],
    );
  }
}
