import 'package:flutter/material.dart';
import 'package:provider/provider.dart';

import '../../config/routes.dart';
import '../../config/theme.dart';
import '../../models/truck_stock_item.dart';
import '../../providers/products_provider.dart' show LoadState;
import '../../providers/truck_stock_provider.dart';
import '../../services/api_client.dart';
import '../../widgets/empty_state.dart';
import '../../widgets/error_view.dart';
import '../../widgets/loading_indicator.dart';
import '../home_shell.dart';
import 'use_item_screen.dart';

/// The technician's assigned truck stock, grouped by category, with quick
/// "use" and "adjust" actions per item.
class MyTruckStockScreen extends StatefulWidget {
  const MyTruckStockScreen({super.key, this.embedded = false});

  final bool embedded;

  @override
  State<MyTruckStockScreen> createState() => _MyTruckStockScreenState();
}

class _MyTruckStockScreenState extends State<MyTruckStockScreen> {
  @override
  void initState() {
    super.initState();
    WidgetsBinding.instance.addPostFrameCallback((_) {
      final TruckStockProvider provider = context.read<TruckStockProvider>();
      if (provider.myStockState == LoadState.idle) {
        provider.loadMyStock();
      }
    });
  }

  Future<void> _adjustQuantity(TruckStockItem item) async {
    final TextEditingController controller =
        TextEditingController(text: item.currentQuantity.toString());
    final TextEditingController notes = TextEditingController();
    final int? value = await showDialog<int>(
      context: context,
      builder: (BuildContext ctx) => AlertDialog(
        title: Text('Adjust ${item.productName}'),
        content: Column(
          mainAxisSize: MainAxisSize.min,
          children: <Widget>[
            TextField(
              controller: controller,
              keyboardType: TextInputType.number,
              autofocus: true,
              decoration: const InputDecoration(
                labelText: 'New quantity on truck',
                prefixIcon: Icon(Icons.numbers),
              ),
            ),
            const SizedBox(height: 12),
            TextField(
              controller: notes,
              decoration: const InputDecoration(
                labelText: 'Notes (optional)',
                prefixIcon: Icon(Icons.notes),
              ),
            ),
          ],
        ),
        actions: <Widget>[
          TextButton(
            onPressed: () => Navigator.of(ctx).pop(),
            child: const Text('Cancel'),
          ),
          FilledButton(
            onPressed: () {
              final int? q = int.tryParse(controller.text.trim());
              if (q == null || q < 0) {
                ScaffoldMessenger.of(ctx).showSnackBar(
                  const SnackBar(content: Text('Enter a valid quantity')),
                );
                return;
              }
              Navigator.of(ctx).pop(q);
            },
            child: const Text('Save'),
          ),
        ],
      ),
    );

    if (value == null) return;
    if (!mounted) return;
    try {
      await context.read<TruckStockProvider>().updateItemQuantity(
            itemId: item.id,
            quantity: value,
            notes: notes.text.trim().isEmpty ? null : notes.text.trim(),
          );
      if (mounted) showSuccessSnack(context, 'Quantity updated.');
    } on ApiException catch (e) {
      if (mounted) showErrorSnack(context, e.message);
    }
  }

  Future<void> _useItem(TruckStockItem item) async {
    final bool? changed = await Navigator.of(context).push<bool>(
      MaterialPageRoute<bool>(
        builder: (_) => UseItemScreen(item: item),
      ),
    );
    if (changed == true && mounted) {
      showSuccessSnack(context, 'Recorded usage of ${item.productName}.');
    }
  }

  @override
  Widget build(BuildContext context) {
    final TruckStockProvider provider = context.watch<TruckStockProvider>();

    return Scaffold(
      appBar: buildAdaptiveAppBar(
        context,
        embedded: widget.embedded,
        title: 'My Truck Stock',
        actions: <Widget>[
          IconButton(
            tooltip: 'Low stock',
            icon: Badge(
              isLabelVisible: provider.lowStockCount > 0,
              label: Text('${provider.lowStockCount}'),
              child: const Icon(Icons.warning_amber_rounded),
            ),
            onPressed: () =>
                Navigator.of(context).pushNamed(AppRoutes.lowStock),
          ),
        ],
      ),
      body: _body(provider),
    );
  }

  Widget _body(TruckStockProvider provider) {
    if (provider.myStockState == LoadState.loading &&
        provider.myStock.isEmpty) {
      return const LoadingIndicator(message: 'Loading your stock...');
    }
    if (provider.myStockState == LoadState.error && provider.myStock.isEmpty) {
      return ErrorView(
        message: provider.error ?? 'Failed to load your truck stock.',
        onRetry: provider.loadMyStock,
      );
    }
    if (provider.myStock.isEmpty) {
      return RefreshIndicator(
        onRefresh: provider.loadMyStock,
        child: ListView(
          children: <Widget>[
            SizedBox(height: MediaQuery.of(context).size.height * 0.18),
            const EmptyState(
              icon: Icons.local_shipping_outlined,
              title: 'No assigned stock',
              message:
                  'You have no truck stock assigned yet. Pull to refresh once '
                  'your truck is set up.',
            ),
          ],
        ),
      );
    }

    final Map<String, List<TruckStockItem>> grouped =
        provider.myStockByCategory;
    final List<String> categories = grouped.keys.toList()..sort();

    return RefreshIndicator(
      onRefresh: provider.loadMyStock,
      child: ListView(
        padding: const EdgeInsets.fromLTRB(16, 12, 16, 24),
        children: <Widget>[
          _SummaryBar(provider: provider),
          const SizedBox(height: 12),
          for (final String category in categories) ...<Widget>[
            Padding(
              padding: const EdgeInsets.fromLTRB(4, 12, 4, 8),
              child: Text(
                category,
                style: const TextStyle(
                  fontSize: 13,
                  letterSpacing: 0.4,
                  fontWeight: FontWeight.w700,
                  color: AppTheme.slate500,
                ),
              ),
            ),
            ...grouped[category]!.map(
              (TruckStockItem item) => Padding(
                padding: const EdgeInsets.only(bottom: 10),
                child: _StockItemCard(
                  item: item,
                  onUse: () => _useItem(item),
                  onAdjust: () => _adjustQuantity(item),
                ),
              ),
            ),
          ],
        ],
      ),
    );
  }
}

class _SummaryBar extends StatelessWidget {
  const _SummaryBar({required this.provider});

  final TruckStockProvider provider;

  @override
  Widget build(BuildContext context) {
    return Row(
      children: <Widget>[
        Expanded(
          child: _summaryCell(
            'Items',
            '${provider.itemCount}',
            AppTheme.brand,
            Icons.inventory_2_outlined,
          ),
        ),
        const SizedBox(width: 12),
        Expanded(
          child: _summaryCell(
            'Low',
            '${provider.lowStockCount}',
            AppTheme.warning,
            Icons.warning_amber_rounded,
          ),
        ),
      ],
    );
  }

  Widget _summaryCell(String label, String value, Color color, IconData icon) {
    return Card(
      child: Padding(
        padding: const EdgeInsets.all(14),
        child: Row(
          children: <Widget>[
            Container(
              padding: const EdgeInsets.all(8),
              decoration: BoxDecoration(
                color: color.withOpacity(0.12),
                borderRadius: BorderRadius.circular(10),
              ),
              child: Icon(icon, color: color, size: 18),
            ),
            const SizedBox(width: 10),
            Column(
              crossAxisAlignment: CrossAxisAlignment.start,
              children: <Widget>[
                Text(
                  value,
                  style: const TextStyle(
                    fontSize: 18,
                    fontWeight: FontWeight.w800,
                    color: AppTheme.slate900,
                  ),
                ),
                Text(
                  label,
                  style: const TextStyle(
                    fontSize: 11,
                    color: AppTheme.slate500,
                  ),
                ),
              ],
            ),
          ],
        ),
      ),
    );
  }
}

class _StockItemCard extends StatelessWidget {
  const _StockItemCard({
    required this.item,
    required this.onUse,
    required this.onAdjust,
  });

  final TruckStockItem item;
  final VoidCallback onUse;
  final VoidCallback onAdjust;

  @override
  Widget build(BuildContext context) {
    final Color barColor = item.isCritical
        ? AppTheme.danger
        : item.isLow
            ? AppTheme.warning
            : AppTheme.success;

    return Card(
      child: Padding(
        padding: const EdgeInsets.all(14),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: <Widget>[
            Row(
              children: <Widget>[
                Expanded(
                  child: Text(
                    item.productName,
                    style: const TextStyle(
                      fontWeight: FontWeight.w700,
                      fontSize: 15,
                      color: AppTheme.slate900,
                    ),
                  ),
                ),
                if (item.isLow)
                  Container(
                    padding:
                        const EdgeInsets.symmetric(horizontal: 8, vertical: 3),
                    decoration: BoxDecoration(
                      color: barColor.withOpacity(0.12),
                      borderRadius: BorderRadius.circular(6),
                    ),
                    child: Text(
                      item.isCritical ? 'CRITICAL' : 'LOW',
                      style: TextStyle(
                        fontSize: 10,
                        fontWeight: FontWeight.w800,
                        color: barColor,
                      ),
                    ),
                  ),
              ],
            ),
            const SizedBox(height: 10),
            Row(
              children: <Widget>[
                Text(
                  '${item.currentQuantity}',
                  style: TextStyle(
                    fontSize: 22,
                    fontWeight: FontWeight.w800,
                    color: barColor,
                  ),
                ),
                const SizedBox(width: 4),
                Text(
                  '/ ${item.requiredQuantity} ${item.unit ?? ''}'.trim(),
                  style: const TextStyle(color: AppTheme.slate500),
                ),
                const Spacer(),
                Text(
                  'min ${item.minimumQuantity}',
                  style: const TextStyle(
                    fontSize: 12,
                    color: AppTheme.slate400,
                  ),
                ),
              ],
            ),
            const SizedBox(height: 8),
            ClipRRect(
              borderRadius: BorderRadius.circular(6),
              child: LinearProgressIndicator(
                value: item.fillRatio,
                minHeight: 8,
                backgroundColor: AppTheme.slate100,
                valueColor: AlwaysStoppedAnimation<Color>(barColor),
              ),
            ),
            const SizedBox(height: 12),
            Row(
              children: <Widget>[
                Expanded(
                  child: OutlinedButton.icon(
                    onPressed: onAdjust,
                    icon: const Icon(Icons.tune, size: 18),
                    label: const Text('Adjust'),
                    style: OutlinedButton.styleFrom(
                      minimumSize: const Size.fromHeight(44),
                    ),
                  ),
                ),
                const SizedBox(width: 10),
                Expanded(
                  child: ElevatedButton.icon(
                    onPressed: item.currentQuantity > 0 ? onUse : null,
                    icon: const Icon(Icons.handyman_outlined, size: 18),
                    label: const Text('Use'),
                    style: ElevatedButton.styleFrom(
                      minimumSize: const Size.fromHeight(44),
                    ),
                  ),
                ),
              ],
            ),
          ],
        ),
      ),
    );
  }
}
