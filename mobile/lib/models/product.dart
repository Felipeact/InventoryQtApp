/// A product / inventory item.
///
/// The backend may return the quantity nested inside an `inventory` object
/// (`inventory.quantity`) or flat at the top level (`quantity`). [fromJson]
/// handles both shapes defensively.
class Product {
  final String id;
  final String name;
  final String barcode;
  final int quantity;
  final int lowStockThreshold;
  final String? model;
  final String? type;
  final String? location;
  final String? project;
  final String? account;
  final String? status;
  final String? description;
  final String? imageUrl;

  const Product({
    required this.id,
    required this.name,
    required this.barcode,
    this.quantity = 0,
    this.lowStockThreshold = 0,
    this.model,
    this.type,
    this.location,
    this.project,
    this.account,
    this.status,
    this.description,
    this.imageUrl,
  });

  factory Product.fromJson(Map<String, dynamic> json) {
    // Quantity may be flat or nested under `inventory`.
    int parsedQuantity = _asInt(json['quantity']);
    int parsedThreshold = _asInt(json['lowStockThreshold']);
    final dynamic inventory = json['inventory'];
    if (inventory is Map) {
      final Map<String, dynamic> inv = inventory.cast<String, dynamic>();
      if (inv.containsKey('quantity')) {
        parsedQuantity = _asInt(inv['quantity']);
      }
      if (inv.containsKey('lowStockThreshold')) {
        parsedThreshold = _asInt(inv['lowStockThreshold']);
      }
    }

    return Product(
      id: (json['id'] ?? json['_id'] ?? '').toString(),
      name: (json['name'] ?? '').toString(),
      barcode: (json['barcode'] ?? '').toString(),
      quantity: parsedQuantity,
      lowStockThreshold: parsedThreshold,
      model: _asNullableString(json['model']),
      type: _asNullableString(json['type']),
      location: _asNullableString(json['location']),
      project: _asNullableString(json['project']),
      account: _asNullableString(json['account']),
      status: _asNullableString(json['status']),
      description: _asNullableString(json['description']),
      imageUrl: _asNullableString(json['imageUrl']),
    );
  }

  Map<String, dynamic> toJson() {
    final Map<String, dynamic> map = <String, dynamic>{
      'name': name,
      'barcode': barcode,
      'quantity': quantity,
      'lowStockThreshold': lowStockThreshold,
    };
    if (model != null) map['model'] = model;
    if (type != null) map['type'] = type;
    if (location != null) map['location'] = location;
    if (project != null) map['project'] = project;
    if (account != null) map['account'] = account;
    if (status != null) map['status'] = status;
    if (description != null) map['description'] = description;
    if (imageUrl != null) map['imageUrl'] = imageUrl;
    return map;
  }

  Product copyWith({
    String? id,
    String? name,
    String? barcode,
    int? quantity,
    int? lowStockThreshold,
    String? model,
    String? type,
    String? location,
    String? project,
    String? account,
    String? status,
    String? description,
    String? imageUrl,
  }) {
    return Product(
      id: id ?? this.id,
      name: name ?? this.name,
      barcode: barcode ?? this.barcode,
      quantity: quantity ?? this.quantity,
      lowStockThreshold: lowStockThreshold ?? this.lowStockThreshold,
      model: model ?? this.model,
      type: type ?? this.type,
      location: location ?? this.location,
      project: project ?? this.project,
      account: account ?? this.account,
      status: status ?? this.status,
      description: description ?? this.description,
      imageUrl: imageUrl ?? this.imageUrl,
    );
  }

  /// True when stock is at or below the configured threshold.
  bool get isLowStock =>
      lowStockThreshold > 0 && quantity <= lowStockThreshold;

  bool get isOutOfStock => quantity <= 0;

  String get computedStatus {
    if (status != null && status!.isNotEmpty) return status!;
    if (isOutOfStock) return 'OUT_OF_STOCK';
    if (isLowStock) return 'LOW_STOCK';
    return 'IN_STOCK';
  }

  static int _asInt(dynamic value) {
    if (value == null) return 0;
    if (value is int) return value;
    if (value is double) return value.round();
    return int.tryParse(value.toString()) ?? 0;
  }

  static String? _asNullableString(dynamic value) {
    if (value == null) return null;
    final String s = value.toString();
    return s.isEmpty ? null : s;
  }
}
