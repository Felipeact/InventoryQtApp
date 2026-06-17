import '../models/asset.dart';
import 'api_client.dart';

/// Wraps the `/assets` endpoints.
class AssetService {
  AssetService({ApiClient? client}) : _client = client ?? ApiClient.instance;

  final ApiClient _client;

  Future<List<Asset>> fetchAll() async {
    final dynamic data = await _client.get('/assets');
    return _parseList(data);
  }

  Future<Asset> fetchById(String id) async {
    final dynamic data = await _client.get('/assets/$id');
    return Asset.fromJson(_asMap(data));
  }

  Future<Asset> create(Asset asset) async {
    final dynamic data = await _client.post('/assets', body: asset.toJson());
    return Asset.fromJson(_asMap(data));
  }

  Future<Asset> update(String id, Asset asset) async {
    final dynamic data = await _client.put('/assets/$id', body: asset.toJson());
    return Asset.fromJson(_asMap(data));
  }

  Future<void> delete(String id) async {
    await _client.delete('/assets/$id');
  }

  List<Asset> _parseList(dynamic data) {
    Iterable<dynamic> raw;
    if (data is List) {
      raw = data;
    } else if (data is Map && data['assets'] is List) {
      raw = data['assets'] as List;
    } else if (data is Map && data['data'] is List) {
      raw = data['data'] as List;
    } else {
      raw = const <dynamic>[];
    }
    return raw
        .whereType<Map>()
        .map((Map e) => Asset.fromJson(e.cast<String, dynamic>()))
        .toList();
  }

  Map<String, dynamic> _asMap(dynamic data) {
    if (data is Map) return data.cast<String, dynamic>();
    throw const ApiException('Unexpected asset payload.');
  }
}
