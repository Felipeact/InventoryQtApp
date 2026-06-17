import 'truck_stock_item.dart';

/// A reusable template defining the expected stock for a trade/truck type.
class TruckStockTemplate {
  final String id;
  final String name;
  final String? tradeType;
  final List<TruckStockItem> items;

  const TruckStockTemplate({
    required this.id,
    required this.name,
    this.tradeType,
    this.items = const <TruckStockItem>[],
  });

  factory TruckStockTemplate.fromJson(Map<String, dynamic> json) {
    final dynamic rawItems = json['items'];
    final List<TruckStockItem> parsedItems = rawItems is List
        ? rawItems
            .whereType<Map>()
            .map((Map e) => TruckStockItem.fromJson(e.cast<String, dynamic>()))
            .toList()
        : <TruckStockItem>[];

    return TruckStockTemplate(
      id: (json['id'] ?? json['_id'] ?? '').toString(),
      name: (json['name'] ?? '').toString(),
      tradeType: (json['tradeType'] ?? json['trade'])?.toString(),
      items: parsedItems,
    );
  }

  Map<String, dynamic> toJson() => <String, dynamic>{
        'id': id,
        'name': name,
        'tradeType': tradeType,
        'items': items.map((TruckStockItem e) => e.toJson()).toList(),
      };

  int get itemCount => items.length;

  int get totalRequiredUnits =>
      items.fold<int>(0, (int sum, TruckStockItem i) => sum + i.requiredQuantity);
}
