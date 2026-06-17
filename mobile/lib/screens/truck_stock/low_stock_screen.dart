import 'package:flutter/material.dart';
import 'package:provider/provider.dart';

import '../../config/theme.dart';
import '../../models/truck_stock_item.dart';
import '../../providers/truck_stock_provider.dart';
import '../../services/api_client.dart';
import '../../widgets/empty_state.dart';
import '../../widgets/error_view.dart';
import '../../widgets/loading_indicator.dart';
import '../home_shell.dart';

/// Lists truck-stock items that are at or below their minimum quantity so the
/// technician knows what to restock.
class LowStockScreen extends StatefulWidget {
  const LowStockScreen({super.key, this.embedded = false});

  final bool embedded;

  @override
  State<LowStockScreen> createState() => _LowStockScreenState();
}

class _LowStockScreenState extends State<LowStockScreen> {
  bool _loading = true;
  String? _error;

  @override
  void initState() {
    super.initState();
    _load();
  }

  Future<void> _load() async {
    setState(() {
      _loading = true;
      _error = null;
    });
    final TruckStockProvider provider = context.read<TruckStockProvider>();
    try {
      await provider.loadLowStock();
    } on ApiException catch (e) {
      // Fall back to deriving from my-stock if the dedicated endpoint fails.
      try {
        if (provider.myStock.isEmpty) await provider.loadMyStock();
      } catch (_) {}
      _error = provider.lowStock.isEmpty ? e.message : null;
    } finally {
      if (mounted) setState(() => _loading = false);
    }
  }

  @override
  Widget build(BuildContext context) {
    final TruckStockProvider provider = context.watch<TruckStockProvider>();
    final List<TruckStockItem> items = provider.lowStock;

    return Scaffold(
      appBar: buildAdaptiveAppBar(
        context,
        embedded: widget.embedded,
        title: 'Low Stock Alerts',
      ),
      body: _body(provider, items),
    );
  }

  Widget _body(TruckStockProvider provider, List<TruckStockItem> items) {
    if (_loading && items.isEmpty) {
      return const LoadingIndicator(message: 'Checking stock levels...');
    }
    if (_error != null && items.isEmpty) {
      return ErrorView(message: _error!, onRetry: _load);
    }
    if (items.isEmpty) {
      return RefreshIndicator(
        onRefresh: _load,
        child: ListView(
          children: <Widget>[
            SizedBox(height: MediaQuery.of(context).size.height * 0.18),
            const EmptyState(
              icon: Icons.check_circle_outline,
              title: 'All stocked up',
              message: 'No items are below their minimum quantity right now.',
            ),
          ],
        ),
      );
    }

    return RefreshIndicator(
      onRefresh: _load,
      child: ListView.separated(
        padding: const EdgeInsets.all(16),
        itemCount: items.length,
        separatorBuilder: (_, __) => const SizedBox(height: 10),
        itemBuilder: (BuildContext context, int i) {
          final TruckStockItem item = items[i];
          final int shortfall =
              (item.requiredQuantity - item.currentQuantity).clamp(0, 1 << 31);
          final Color color =
              item.isCritical ? AppTheme.danger : AppTheme.warning;
          return Card(
            child: Padding(
              padding: const EdgeInsets.all(14),
              child: Row(
                children: <Widget>[
                  Container(
                    width: 44,
                    height: 44,
                    decoration: BoxDecoration(
                      color: color.withValues(alpha: 0.12),
                      borderRadius: BorderRadius.circular(12),
                    ),
                    child: Icon(Icons.warning_amber_rounded, color: color),
                  ),
                  const SizedBox(width: 14),
                  Expanded(
                    child: Column(
                      crossAxisAlignment: CrossAxisAlignment.start,
                      children: <Widget>[
                        Text(
                          item.productName,
                          style: const TextStyle(
                            fontWeight: FontWeight.w700,
                            fontSize: 15,
                            color: AppTheme.slate900,
                          ),
                        ),
                        const SizedBox(height: 2),
                        Text(
                          'On truck ${item.currentQuantity} • '
                          'min ${item.minimumQuantity} • '
                          'need ${item.requiredQuantity}',
                          style: const TextStyle(
                            fontSize: 12,
                            color: AppTheme.slate500,
                          ),
                        ),
                      ],
                    ),
                  ),
                  const SizedBox(width: 8),
                  Column(
                    crossAxisAlignment: CrossAxisAlignment.end,
                    children: <Widget>[
                      Text(
                        '-$shortfall',
                        style: TextStyle(
                          fontSize: 18,
                          fontWeight: FontWeight.w800,
                          color: color,
                        ),
                      ),
                      Text(
                        'short',
                        style: TextStyle(fontSize: 11, color: color),
                      ),
                    ],
                  ),
                ],
              ),
            ),
          );
        },
      ),
    );
  }
}
