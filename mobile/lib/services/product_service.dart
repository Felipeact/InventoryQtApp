import '../models/product.dart';
import 'api_client.dart';

/// Wraps the `/products` endpoints.
class ProductService {
  ProductService({ApiClient? client}) : _client = client ?? ApiClient.instance;

  final ApiClient _client;

  Future<List<Product>> fetchAll() async {
    final dynamic data = await _client.get('/products');
    return _parseList(data);
  }

  Future<List<Product>> fetchLowStock() async {
    final dynamic data = await _client.get('/products/low-stock');
    return _parseList(data);
  }

  Future<Product> fetchById(String id) async {
    final dynamic data = await _client.get('/products/$id');
    return Product.fromJson(_asMap(data));
  }

  Future<Product> create(Product product) async {
    final dynamic data = await _client.post('/products', body: product.toJson());
    return Product.fromJson(_asMap(data));
  }

  Future<Product> update(String id, Product product) async {
    final dynamic data =
        await _client.put('/products/$id', body: product.toJson());
    return Product.fromJson(_asMap(data));
  }

  Future<void> delete(String id) async {
    await _client.delete('/products/$id');
  }

  /// Increments stock for a barcode.
  Future<Product?> scanIn({
    required String barcode,
    required int quantity,
  }) async {
    final dynamic data = await _client.post(
      '/products/scan-in',
      body: <String, dynamic>{'barcode': barcode, 'quantity': quantity},
    );
    return _maybeProduct(data);
  }

  /// Decrements stock for a barcode.
  Future<Product?> scanOut({
    required String barcode,
    required int quantity,
  }) async {
    final dynamic data = await _client.post(
      '/products/scan-out',
      body: <String, dynamic>{'barcode': barcode, 'quantity': quantity},
    );
    return _maybeProduct(data);
  }

  /// Looks up a product by barcode using the full list (the backend has no
  /// dedicated by-barcode endpoint). Returns null if not found.
  Future<Product?> findByBarcode(String barcode) async {
    final List<Product> all = await fetchAll();
    for (final Product p in all) {
      if (p.barcode == barcode) return p;
    }
    return null;
  }

  List<Product> _parseList(dynamic data) {
    Iterable<dynamic> raw;
    if (data is List) {
      raw = data;
    } else if (data is Map && data['products'] is List) {
      raw = data['products'] as List;
    } else if (data is Map && data['data'] is List) {
      raw = data['data'] as List;
    } else {
      raw = const <dynamic>[];
    }
    return raw
        .whereType<Map>()
        .map((Map e) => Product.fromJson(e.cast<String, dynamic>()))
        .toList();
  }

  Product? _maybeProduct(dynamic data) {
    if (data is Map && (data['id'] != null || data['barcode'] != null)) {
      return Product.fromJson(data.cast<String, dynamic>());
    }
    if (data is Map && data['product'] is Map) {
      return Product.fromJson(
          (data['product'] as Map).cast<String, dynamic>());
    }
    return null;
  }

  Map<String, dynamic> _asMap(dynamic data) {
    if (data is Map) return data.cast<String, dynamic>();
    throw const ApiException('Unexpected product payload.');
  }
}
