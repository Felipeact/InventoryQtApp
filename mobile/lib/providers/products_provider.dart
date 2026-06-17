import 'package:flutter/foundation.dart';

import '../models/product.dart';
import '../services/api_client.dart';
import '../services/product_service.dart';

/// Generic load state for list-backed providers.
enum LoadState { idle, loading, ready, error }

/// Manages the product catalog plus low-stock and scan operations.
class ProductsProvider extends ChangeNotifier {
  ProductsProvider({ProductService? service})
      : _service = service ?? ProductService();

  final ProductService _service;

  LoadState _state = LoadState.idle;
  List<Product> _products = <Product>[];
  List<Product> _lowStock = <Product>[];
  String? _error;
  String _query = '';

  LoadState get state => _state;
  String? get error => _error;
  String get query => _query;
  bool get isLoading => _state == LoadState.loading;

  List<Product> get all => List<Product>.unmodifiable(_products);
  List<Product> get lowStock => List<Product>.unmodifiable(_lowStock);

  /// Products filtered by the current search query (name/barcode/model).
  List<Product> get filtered {
    if (_query.trim().isEmpty) return all;
    final String q = _query.toLowerCase();
    return _products.where((Product p) {
      return p.name.toLowerCase().contains(q) ||
          p.barcode.toLowerCase().contains(q) ||
          (p.model?.toLowerCase().contains(q) ?? false) ||
          (p.type?.toLowerCase().contains(q) ?? false);
    }).toList();
  }

  int get totalUnits =>
      _products.fold<int>(0, (int sum, Product p) => sum + p.quantity);

  int get lowStockCount =>
      _lowStock.isNotEmpty ? _lowStock.length : _products.where((Product p) => p.isLowStock).length;

  int get outOfStockCount =>
      _products.where((Product p) => p.isOutOfStock).length;

  void setQuery(String value) {
    _query = value;
    notifyListeners();
  }

  Future<void> load() async {
    _state = LoadState.loading;
    _error = null;
    notifyListeners();
    try {
      final List<Product> products = await _service.fetchAll();
      _products = products;
      // Low-stock can fail independently; degrade gracefully.
      try {
        _lowStock = await _service.fetchLowStock();
      } on ApiException {
        _lowStock =
            _products.where((Product p) => p.isLowStock).toList();
      }
      _state = LoadState.ready;
    } on ApiException catch (e) {
      _error = e.message;
      _state = LoadState.error;
    } catch (_) {
      _error = 'Failed to load products.';
      _state = LoadState.error;
    }
    notifyListeners();
  }

  Future<void> loadLowStock() async {
    try {
      _lowStock = await _service.fetchLowStock();
      notifyListeners();
    } on ApiException catch (e) {
      _error = e.message;
      notifyListeners();
    }
  }

  Future<Product> create(Product product) async {
    final Product created = await _service.create(product);
    _products = <Product>[created, ..._products];
    notifyListeners();
    return created;
  }

  Future<Product> update(String id, Product product) async {
    final Product updated = await _service.update(id, product);
    _products = _products
        .map((Product p) => p.id == id ? updated : p)
        .toList();
    notifyListeners();
    return updated;
  }

  Future<void> delete(String id) async {
    await _service.delete(id);
    _products = _products.where((Product p) => p.id != id).toList();
    _lowStock = _lowStock.where((Product p) => p.id != id).toList();
    notifyListeners();
  }

  Future<Product?> scanIn(String barcode, int quantity) async {
    final Product? updated =
        await _service.scanIn(barcode: barcode, quantity: quantity);
    if (updated != null) _mergeProduct(updated);
    return updated;
  }

  Future<Product?> scanOut(String barcode, int quantity) async {
    final Product? updated =
        await _service.scanOut(barcode: barcode, quantity: quantity);
    if (updated != null) _mergeProduct(updated);
    return updated;
  }

  Future<Product?> findByBarcode(String barcode) {
    return _service.findByBarcode(barcode);
  }

  void _mergeProduct(Product updated) {
    final int idx = _products.indexWhere((Product p) =>
        p.id == updated.id || (updated.id.isEmpty && p.barcode == updated.barcode));
    if (idx >= 0) {
      _products[idx] = updated;
    } else {
      _products = <Product>[updated, ..._products];
    }
    _lowStock = _products.where((Product p) => p.isLowStock).toList();
    notifyListeners();
  }
}
