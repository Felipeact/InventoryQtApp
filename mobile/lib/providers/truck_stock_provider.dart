import 'package:flutter/foundation.dart';

import '../models/receipt.dart';
import '../models/stock_movement.dart';
import '../models/truck.dart';
import '../models/truck_stock_item.dart';
import '../models/truck_stock_template.dart';
import '../services/api_client.dart';
import '../services/truck_stock_service.dart';
import 'products_provider.dart' show LoadState;

/// Manages the technician's assigned truck stock plus related truck-stock
/// resources (low stock, movements, receipts, trucks, templates).
class TruckStockProvider extends ChangeNotifier {
  TruckStockProvider({TruckStockService? service})
      : _service = service ?? TruckStockService();

  final TruckStockService _service;

  LoadState _myStockState = LoadState.idle;
  String? _error;

  List<TruckStockItem> _myStock = <TruckStockItem>[];
  List<TruckStockItem> _lowStock = <TruckStockItem>[];
  List<StockMovement> _movements = <StockMovement>[];
  List<Receipt> _receipts = <Receipt>[];
  List<Truck> _trucks = <Truck>[];
  List<TruckStockTemplate> _templates = <TruckStockTemplate>[];

  LoadState get myStockState => _myStockState;
  String? get error => _error;

  List<TruckStockItem> get myStock => List<TruckStockItem>.unmodifiable(_myStock);
  List<TruckStockItem> get lowStock =>
      List<TruckStockItem>.unmodifiable(_lowStock);
  List<StockMovement> get movements =>
      List<StockMovement>.unmodifiable(_movements);
  List<Receipt> get receipts => List<Receipt>.unmodifiable(_receipts);
  List<Truck> get trucks => List<Truck>.unmodifiable(_trucks);
  List<TruckStockTemplate> get templates =>
      List<TruckStockTemplate>.unmodifiable(_templates);

  int get lowStockCount => _lowStock.isNotEmpty
      ? _lowStock.length
      : _myStock.where((TruckStockItem i) => i.isLow).length;

  int get itemCount => _myStock.length;

  /// Items grouped by category for sectioned display.
  Map<String, List<TruckStockItem>> get myStockByCategory {
    final Map<String, List<TruckStockItem>> grouped =
        <String, List<TruckStockItem>>{};
    for (final TruckStockItem item in _myStock) {
      final String key = (item.category == null || item.category!.isEmpty)
          ? 'Uncategorized'
          : item.category!;
      grouped.putIfAbsent(key, () => <TruckStockItem>[]).add(item);
    }
    return grouped;
  }

  Future<void> loadMyStock() async {
    _myStockState = LoadState.loading;
    _error = null;
    notifyListeners();
    try {
      _myStock = await _service.fetchMyStock();
      try {
        _lowStock = await _service.fetchLowStock();
      } on ApiException {
        _lowStock = _myStock.where((TruckStockItem i) => i.isLow).toList();
      }
      _myStockState = LoadState.ready;
    } on ApiException catch (e) {
      _error = e.message;
      _myStockState = LoadState.error;
    } catch (_) {
      _error = 'Failed to load your truck stock.';
      _myStockState = LoadState.error;
    }
    notifyListeners();
  }

  Future<void> loadLowStock() async {
    _lowStock = await _service.fetchLowStock();
    notifyListeners();
  }

  Future<void> loadMovements() async {
    _movements = await _service.fetchMovements();
    notifyListeners();
  }

  Future<void> loadReceipts() async {
    _receipts = await _service.fetchReceipts();
    notifyListeners();
  }

  Future<void> loadTrucks() async {
    _trucks = await _service.fetchTrucks();
    notifyListeners();
  }

  Future<void> loadTemplates() async {
    _templates = await _service.fetchTemplates();
    notifyListeners();
  }

  Future<TruckStockTemplate> loadTemplate(String id) {
    return _service.fetchTemplate(id);
  }

  Future<void> updateItemQuantity({
    required String itemId,
    required int quantity,
    String? notes,
  }) async {
    await _service.updateItemQuantity(
      itemId: itemId,
      quantity: quantity,
      notes: notes,
    );
    _applyLocalQuantity(itemId, quantity);
  }

  Future<void> useItem({
    required String itemId,
    required int quantity,
    String? notes,
  }) async {
    await _service.useItem(
      truckStockItemId: itemId,
      quantity: quantity,
      notes: notes,
    );
    final int idx = _myStock.indexWhere((TruckStockItem i) => i.id == itemId);
    if (idx >= 0) {
      final TruckStockItem current = _myStock[idx];
      final int next =
          (current.currentQuantity - quantity).clamp(0, 1 << 31).toInt();
      _applyLocalQuantity(itemId, next);
    }
  }

  Future<Truck> createTruck({
    required String truckNumber,
    String? plateNumber,
    String? status,
  }) async {
    final Truck truck = await _service.createTruck(
      truckNumber: truckNumber,
      plateNumber: plateNumber,
      status: status,
    );
    _trucks = <Truck>[truck, ..._trucks];
    notifyListeners();
    return truck;
  }

  Future<String> uploadReceiptFile({
    required String fileBase64,
    required String fileName,
  }) {
    return _service.uploadReceiptFile(
      fileBase64: fileBase64,
      fileName: fileName,
    );
  }

  Future<Receipt> createReceipt({
    required String truckId,
    required String fileUrl,
    double? totalAmount,
  }) async {
    final Receipt receipt = await _service.createReceipt(
      truckId: truckId,
      fileUrl: fileUrl,
      totalAmount: totalAmount,
    );
    _receipts = <Receipt>[receipt, ..._receipts];
    notifyListeners();
    return receipt;
  }

  /// Approves, rejects, or flags a receipt for review and replaces it in the
  /// local list with the server's updated copy. Requires APPROVE_RECEIPTS.
  Future<Receipt> updateReceiptStatus(String receiptId, String status) async {
    final Receipt updated = await _service.updateReceiptStatus(
      receiptId: receiptId,
      status: status,
    );
    _receipts = _receipts
        .map((Receipt r) => r.id == updated.id ? updated : r)
        .toList(growable: false);
    notifyListeners();
    return updated;
  }

  void _applyLocalQuantity(String itemId, int quantity) {
    _myStock = _myStock.map((TruckStockItem i) {
      return i.id == itemId ? i.copyWith(currentQuantity: quantity) : i;
    }).toList();
    _lowStock = _myStock.where((TruckStockItem i) => i.isLow).toList();
    notifyListeners();
  }
}
