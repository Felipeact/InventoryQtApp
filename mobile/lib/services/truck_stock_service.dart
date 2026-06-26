import '../models/extracted_receipt.dart';
import '../models/receipt.dart';
import '../models/stock_movement.dart';
import '../models/truck.dart';
import '../models/truck_stock_assignment.dart';
import '../models/truck_stock_item.dart';
import '../models/truck_stock_template.dart';
import 'api_client.dart';

/// Wraps the `/truck-stock/*` endpoints.
class TruckStockService {
  TruckStockService({ApiClient? client})
      : _client = client ?? ApiClient.instance;

  final ApiClient _client;

  // ---- Trucks --------------------------------------------------------------

  Future<List<Truck>> fetchTrucks() async {
    final dynamic data = await _client.get('/truck-stock/trucks');
    return _list(data, 'trucks', Truck.fromJson);
  }

  Future<Truck> createTruck({
    required String truckNumber,
    String? plateNumber,
    String? status,
  }) async {
    final Map<String, dynamic> body = <String, dynamic>{
      'truckNumber': truckNumber,
    };
    if (plateNumber != null && plateNumber.isNotEmpty) {
      body['plateNumber'] = plateNumber;
    }
    if (status != null && status.isNotEmpty) body['status'] = status;
    final dynamic data = await _client.post('/truck-stock/trucks', body: body);
    return Truck.fromJson(_asMap(data));
  }

  // ---- Templates -----------------------------------------------------------

  Future<List<TruckStockTemplate>> fetchTemplates() async {
    final dynamic data = await _client.get('/truck-stock/templates');
    return _list(data, 'templates', TruckStockTemplate.fromJson);
  }

  Future<TruckStockTemplate> fetchTemplate(String id) async {
    final dynamic data = await _client.get('/truck-stock/templates/$id');
    return TruckStockTemplate.fromJson(_asMap(data));
  }

  /// Creates (when [id] is null) or updates a template, including its allowance
  /// and item list. [items] entries use the API shape (productName,
  /// requiredQuantity, minimumQuantity, optional category/unit/expectedPrice).
  Future<TruckStockTemplate> saveTemplate({
    String? id,
    required String name,
    String? tradeType,
    double? allowance,
    required List<Map<String, dynamic>> items,
  }) async {
    final Map<String, dynamic> body = <String, dynamic>{
      'name': name,
      if (tradeType != null && tradeType.isNotEmpty) 'tradeType': tradeType,
      'allowance': allowance,
      'items': items,
    };
    final dynamic data = id == null
        ? await _client.post('/truck-stock/templates', body: body)
        : await _client.put('/truck-stock/templates/$id', body: body);
    return TruckStockTemplate.fromJson(_asMap(data));
  }

  Future<void> deleteTemplate(String id) async {
    await _client.delete('/truck-stock/templates/$id');
  }

  // ---- Assignments ---------------------------------------------------------

  Future<List<TruckStockAssignment>> fetchAssignments() async {
    final dynamic data = await _client.get('/truck-stock/assignments');
    return _list(data, 'assignments', TruckStockAssignment.fromJson);
  }

  // ---- My stock / low stock ------------------------------------------------

  Future<List<TruckStockItem>> fetchMyStock() async {
    final dynamic data = await _client.get('/truck-stock/my-stock');
    return _list(data, 'items', TruckStockItem.fromJson);
  }

  Future<List<TruckStockItem>> fetchLowStock() async {
    final dynamic data = await _client.get('/truck-stock/low-stock');
    // The API returns an array of templates, each with its own low `items` list.
    // Flatten those into a single list of items (a bare list of items is also
    // tolerated, in case the shape changes).
    final List<TruckStockItem> items = <TruckStockItem>[];
    if (data is List) {
      for (final dynamic entry in data) {
        if (entry is! Map) continue;
        final dynamic nested = entry['items'];
        if (nested is List) {
          // Template wrapper → take its items.
          for (final dynamic item in nested) {
            if (item is Map) {
              items.add(TruckStockItem.fromJson(Map<String, dynamic>.from(item)));
            }
          }
        } else {
          // Already a flat item.
          items.add(TruckStockItem.fromJson(Map<String, dynamic>.from(entry)));
        }
      }
    }
    return items;
  }

  // ---- Mutations -----------------------------------------------------------

  Future<void> updateItemQuantity({
    required String itemId,
    required int quantity,
    String? notes,
  }) async {
    final Map<String, dynamic> body = <String, dynamic>{'quantity': quantity};
    if (notes != null && notes.isNotEmpty) body['notes'] = notes;
    await _client.patch('/truck-stock/items/$itemId/quantity', body: body);
  }

  Future<void> useItem({
    required String truckStockItemId,
    required int quantity,
    String? notes,
  }) async {
    final Map<String, dynamic> body = <String, dynamic>{
      'truckStockItemId': truckStockItemId,
      'quantity': quantity,
    };
    if (notes != null && notes.isNotEmpty) body['notes'] = notes;
    await _client.post('/truck-stock/use-item', body: body);
  }

  Future<void> transferToTruck(Map<String, dynamic> body) async {
    await _client.post('/truck-stock/transfer-to-truck', body: body);
  }

  // ---- Movements -----------------------------------------------------------

  Future<List<StockMovement>> fetchMovements() async {
    final dynamic data = await _client.get('/truck-stock/movements');
    return _list(data, 'movements', StockMovement.fromJson);
  }

  // ---- Receipts ------------------------------------------------------------

  /// Uploads a base64-encoded file and returns the stored file URL.
  Future<String> uploadReceiptFile({
    required String fileBase64,
    required String fileName,
  }) async {
    final dynamic data = await _client.post(
      '/truck-stock/receipts/upload',
      body: <String, dynamic>{
        'fileBase64': fileBase64,
        'fileName': fileName,
      },
    );
    if (data is Map && data['fileUrl'] != null) {
      return data['fileUrl'].toString();
    }
    if (data is Map && data['url'] != null) {
      return data['url'].toString();
    }
    throw const ApiException('Upload did not return a file URL.');
  }

  /// AI-reads a receipt (PDF/image, base64) and returns its total + line items
  /// without persisting. No-ops with a clear error if AI is not configured.
  Future<ExtractedReceipt> extractReceipt({
    required String fileBase64,
    required String fileName,
  }) async {
    final dynamic data = await _client.post(
      '/truck-stock/receipts/extract',
      body: <String, dynamic>{'fileBase64': fileBase64, 'fileName': fileName},
    );
    return ExtractedReceipt.fromJson(_asMap(data));
  }

  Future<Receipt> createReceipt({
    required String truckId,
    required String fileUrl,
    double? totalAmount,
    List<ExtractedReceiptItem>? items,
  }) async {
    final Map<String, dynamic> body = <String, dynamic>{
      'truckId': truckId,
      'fileUrl': fileUrl,
    };
    if (totalAmount != null) body['totalAmount'] = totalAmount;
    if (items != null && items.isNotEmpty) {
      body['items'] = items.map((ExtractedReceiptItem e) => e.toJson()).toList();
    }
    final dynamic data = await _client.post('/truck-stock/receipts', body: body);
    return Receipt.fromJson(_asMap(data));
  }

  /// Reconciles a receipt against the truck's template allowance. Returns the
  /// raw result map (receiptTotal, expectedTotal, difference, hasExpected,
  /// overBudget, status).
  Future<Map<String, dynamic>> reconcileReceipt(String receiptId) async {
    final dynamic data =
        await _client.post('/truck-stock/receipts/$receiptId/reconcile');
    return _asMap(data);
  }

  Future<List<Receipt>> fetchReceipts() async {
    final dynamic data = await _client.get('/truck-stock/receipts');
    return _list(data, 'receipts', Receipt.fromJson);
  }

  /// Approves, rejects, or flags a receipt for review. Requires the
  /// APPROVE_RECEIPTS permission server-side. Returns the updated receipt.
  /// [status] must be one of: APPROVED, REJECTED, NEEDS_REVIEW.
  Future<Receipt> updateReceiptStatus({
    required String receiptId,
    required String status,
  }) async {
    final dynamic data = await _client.patch(
      '/truck-stock/receipts/$receiptId/status',
      body: <String, dynamic>{'status': status},
    );
    return Receipt.fromJson(_asMap(data));
  }

  // ---- Helpers -------------------------------------------------------------

  List<T> _list<T>(
    dynamic data,
    String key,
    T Function(Map<String, dynamic>) fromJson,
  ) {
    Iterable<dynamic> raw;
    if (data is List) {
      raw = data;
    } else if (data is Map && data[key] is List) {
      raw = data[key] as List;
    } else if (data is Map && data['data'] is List) {
      raw = data['data'] as List;
    } else if (data is Map && data['items'] is List) {
      raw = data['items'] as List;
    } else {
      raw = const <dynamic>[];
    }
    return raw
        .whereType<Map>()
        .map((Map e) => fromJson(e.cast<String, dynamic>()))
        .toList();
  }

  Map<String, dynamic> _asMap(dynamic data) {
    if (data is Map) return data.cast<String, dynamic>();
    throw const ApiException('Unexpected truck-stock payload.');
  }
}
