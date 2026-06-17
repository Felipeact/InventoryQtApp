import 'api_client.dart';

/// Aggregated reporting figures used on the dashboard.
class InventoryReport {
  final int totalProducts;
  final int totalUnits;
  final int lowStockCount;
  final int outOfStockCount;
  final double? totalValue;
  final Map<String, dynamic> raw;

  const InventoryReport({
    this.totalProducts = 0,
    this.totalUnits = 0,
    this.lowStockCount = 0,
    this.outOfStockCount = 0,
    this.totalValue,
    this.raw = const <String, dynamic>{},
  });

  factory InventoryReport.fromJson(Map<String, dynamic> json) {
    return InventoryReport(
      totalProducts: _asInt(json['totalProducts'] ?? json['productCount']),
      totalUnits: _asInt(json['totalUnits'] ?? json['totalQuantity']),
      lowStockCount: _asInt(json['lowStockCount'] ?? json['lowStock']),
      outOfStockCount: _asInt(json['outOfStockCount'] ?? json['outOfStock']),
      totalValue: _asNullableDouble(json['totalValue']),
      raw: json,
    );
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

/// Wraps the `/reports/*` endpoints.
class ReportService {
  ReportService({ApiClient? client}) : _client = client ?? ApiClient.instance;

  final ApiClient _client;

  Future<InventoryReport> fetchInventoryReport() async {
    final dynamic data = await _client.get('/reports/inventory');
    if (data is Map) {
      return InventoryReport.fromJson(data.cast<String, dynamic>());
    }
    return const InventoryReport();
  }

  Future<Map<String, dynamic>> fetchAssetsReport() async {
    final dynamic data = await _client.get('/reports/assets');
    if (data is Map) return data.cast<String, dynamic>();
    return <String, dynamic>{};
  }
}
