import 'package:flutter/material.dart';

import '../config/theme.dart';
import '../models/asset.dart';
import '../models/product.dart';
import '../models/truck.dart';
import '../services/asset_service.dart';
import '../services/product_service.dart';
import '../services/truck_stock_service.dart';
import '../widgets/loading_indicator.dart';
import 'home_shell.dart';

/// Client-side global search across products, assets, and trucks — the mobile
/// counterpart of the desktop global search.
class SearchScreen extends StatefulWidget {
  const SearchScreen({super.key, this.embedded = false});

  final bool embedded;

  @override
  State<SearchScreen> createState() => _SearchScreenState();
}

class _SearchScreenState extends State<SearchScreen> {
  final TextEditingController _controller = TextEditingController();
  final ProductService _products = ProductService();
  final AssetService _assets = AssetService();
  final TruckStockService _trucks = TruckStockService();

  List<Product> _allProducts = <Product>[];
  List<Asset> _allAssets = <Asset>[];
  List<Truck> _allTrucks = <Truck>[];
  bool _loading = true;
  String _query = '';

  @override
  void initState() {
    super.initState();
    _load();
  }

  @override
  void dispose() {
    _controller.dispose();
    super.dispose();
  }

  Future<void> _load() async {
    final List<dynamic> results = await Future.wait<dynamic>(<Future<dynamic>>[
      _products.fetchAll().catchError((_) => <Product>[]),
      _assets.fetchAll().catchError((_) => <Asset>[]),
      _trucks.fetchTrucks().catchError((_) => <Truck>[]),
    ]);
    if (!mounted) return;
    setState(() {
      _allProducts = results[0] as List<Product>;
      _allAssets = results[1] as List<Asset>;
      _allTrucks = results[2] as List<Truck>;
      _loading = false;
    });
  }

  bool _match(List<String?> values) {
    if (_query.isEmpty) return false;
    return values.any((String? v) => (v ?? '').toLowerCase().contains(_query));
  }

  @override
  Widget build(BuildContext context) {
    final List<Product> products = _query.isEmpty
        ? <Product>[]
        : _allProducts
            .where((Product p) => _match(<String?>[p.name, p.barcode, p.model, p.type]))
            .toList();
    final List<Asset> assets = _query.isEmpty
        ? <Asset>[]
        : _allAssets
            .where((Asset a) => _match(<String?>[a.name, a.type, a.serialCode, a.status]))
            .toList();
    final List<Truck> trucks = _query.isEmpty
        ? <Truck>[]
        : _allTrucks
            .where((Truck t) => _match(<String?>[t.truckNumber, t.plateNumber, t.status]))
            .toList();
    final int total = products.length + assets.length + trucks.length;

    return Scaffold(
      appBar: buildAdaptiveAppBar(
        context,
        embedded: widget.embedded,
        title: 'Search',
      ),
      body: Column(
        children: <Widget>[
          Padding(
            padding: const EdgeInsets.all(16),
            child: TextField(
              controller: _controller,
              autofocus: true,
              onChanged: (String v) =>
                  setState(() => _query = v.trim().toLowerCase()),
              decoration: const InputDecoration(
                prefixIcon: Icon(Icons.search),
                hintText: 'Search products, assets, trucks…',
              ),
            ),
          ),
          Expanded(
            child: _loading
                ? const LoadingIndicator(message: 'Indexing…')
                : _query.isEmpty
                    ? const _Hint('Type to search inventory, tools, and fleet.')
                    : total == 0
                        ? const _Hint('No matches.')
                        : ListView(
                            padding:
                                const EdgeInsets.fromLTRB(16, 0, 16, 24),
                            children: <Widget>[
                              _group(
                                Icons.inventory_2_outlined,
                                'Products',
                                products
                                    .take(50)
                                    .map((Product p) =>
                                        _row(p.name, p.barcode))
                                    .toList(),
                              ),
                              _group(
                                Icons.devices_other_outlined,
                                'Assets',
                                assets
                                    .take(50)
                                    .map((Asset a) => _row(
                                        a.name,
                                        '${a.type}'
                                        '${a.serialCode.isNotEmpty ? ' · SN ${a.serialCode}' : ''}'))
                                    .toList(),
                              ),
                              _group(
                                Icons.local_shipping_outlined,
                                'Trucks',
                                trucks
                                    .take(50)
                                    .map((Truck t) => _row(
                                        'Truck ${t.truckNumber}',
                                        t.plateNumber ?? ''))
                                    .toList(),
                              ),
                            ],
                          ),
          ),
        ],
      ),
    );
  }

  Widget _group(IconData icon, String title, List<Widget> rows) {
    if (rows.isEmpty) return const SizedBox.shrink();
    return Card(
      margin: const EdgeInsets.only(bottom: 12),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: <Widget>[
          Padding(
            padding: const EdgeInsets.fromLTRB(14, 12, 14, 4),
            child: Row(
              children: <Widget>[
                Icon(icon, size: 16, color: AppTheme.brand),
                const SizedBox(width: 8),
                Text(
                  '$title (${rows.length})',
                  style: const TextStyle(
                      fontWeight: FontWeight.w800, color: AppTheme.slate900),
                ),
              ],
            ),
          ),
          ...rows,
          const SizedBox(height: 4),
        ],
      ),
    );
  }

  Widget _row(String label, String sub) => ListTile(
        dense: true,
        title: Text(label),
        subtitle: sub.isEmpty
            ? null
            : Text(sub, style: const TextStyle(color: AppTheme.slate400)),
      );
}

class _Hint extends StatelessWidget {
  const _Hint(this.text);
  final String text;

  @override
  Widget build(BuildContext context) {
    return Center(
      child: Padding(
        padding: const EdgeInsets.all(24),
        child: Text(
          text,
          textAlign: TextAlign.center,
          style: const TextStyle(color: AppTheme.slate500),
        ),
      ),
    );
  }
}
