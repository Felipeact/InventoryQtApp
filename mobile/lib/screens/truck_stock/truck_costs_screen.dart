import 'package:flutter/material.dart';
import 'package:intl/intl.dart';
import 'package:provider/provider.dart';

import '../../config/theme.dart';
import '../../models/receipt.dart';
import '../../providers/truck_stock_provider.dart';
import '../../services/api_client.dart';
import '../../widgets/empty_state.dart';
import '../../widgets/error_view.dart';
import '../../widgets/loading_indicator.dart';
import '../../widgets/stat_card.dart';
import '../home_shell.dart';

/// Per-truck spending from uploaded receipts. Rejected receipts are excluded
/// from cost and surfaced separately. Requires APPROVE_RECEIPTS server-side.
class TruckCostsScreen extends StatefulWidget {
  const TruckCostsScreen({super.key, this.embedded = false});

  final bool embedded;

  @override
  State<TruckCostsScreen> createState() => _TruckCostsScreenState();
}

class _TruckSpend {
  _TruckSpend(this.label);
  final String label;
  double spend = 0;
  int count = 0;
}

class _TruckCostsScreenState extends State<TruckCostsScreen> {
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
    try {
      await context.read<TruckStockProvider>().loadReceipts();
    } on ApiException catch (e) {
      _error = e.message;
    } catch (_) {
      _error = 'Failed to load truck costs.';
    } finally {
      if (mounted) setState(() => _loading = false);
    }
  }

  bool _isRejected(Receipt r) => (r.status ?? '').toUpperCase() == 'REJECTED';

  String _money(double v) =>
      NumberFormat.currency(symbol: '\$', decimalDigits: 0).format(v);

  @override
  Widget build(BuildContext context) {
    final TruckStockProvider provider = context.watch<TruckStockProvider>();
    return Scaffold(
      appBar: buildAdaptiveAppBar(
        context,
        embedded: widget.embedded,
        title: 'Truck Costs',
      ),
      body: _body(provider.receipts),
    );
  }

  Widget _body(List<Receipt> receipts) {
    if (_loading && receipts.isEmpty) {
      return const LoadingIndicator(message: 'Loading truck costs...');
    }
    if (_error != null && receipts.isEmpty) {
      return ErrorView(message: _error!, onRetry: _load);
    }

    final List<Receipt> counted =
        receipts.where((Receipt r) => !_isRejected(r)).toList();
    final List<Receipt> rejected =
        receipts.where(_isRejected).toList();

    final Map<String, _TruckSpend> byTruck = <String, _TruckSpend>{};
    for (final Receipt r in counted) {
      final String key = r.truckId.isNotEmpty ? r.truckId : 'unknown';
      final String label =
          r.truckNumber != null ? 'Truck ${r.truckNumber}' : 'Unassigned';
      final _TruckSpend cur = byTruck.putIfAbsent(key, () => _TruckSpend(label));
      cur.spend += r.totalAmount ?? 0;
      cur.count += 1;
    }
    final List<_TruckSpend> rows = byTruck.values.toList()
      ..sort((_TruckSpend a, _TruckSpend b) => b.spend.compareTo(a.spend));

    final double fleetTotal =
        rows.fold<double>(0, (double s, _TruckSpend r) => s + r.spend);
    final double rejectedTotal = rejected.fold<double>(
        0, (double s, Receipt r) => s + (r.totalAmount ?? 0));

    if (receipts.isEmpty) {
      return RefreshIndicator(
        onRefresh: _load,
        child: ListView(
          children: <Widget>[
            SizedBox(height: MediaQuery.of(context).size.height * 0.16),
            const EmptyState(
              icon: Icons.payments_outlined,
              title: 'No spending yet',
              message: 'Truck spending from uploaded receipts will appear here.',
            ),
          ],
        ),
      );
    }

    return RefreshIndicator(
      onRefresh: _load,
      child: ListView(
        padding: const EdgeInsets.fromLTRB(16, 16, 16, 24),
        children: <Widget>[
          Row(
            children: <Widget>[
              Expanded(
                child: StatCard(
                  label: 'Fleet spend',
                  value: _money(fleetTotal),
                  icon: Icons.account_balance_wallet_outlined,
                ),
              ),
              const SizedBox(width: 12),
              Expanded(
                child: StatCard(
                  label: 'Receipts',
                  value: '${counted.length}',
                  icon: Icons.receipt_long_outlined,
                ),
              ),
            ],
          ),
          if (rejected.isNotEmpty) ...<Widget>[
            const SizedBox(height: 12),
            Container(
              padding: const EdgeInsets.all(12),
              decoration: BoxDecoration(
                color: AppTheme.danger.withValues(alpha: 0.08),
                borderRadius: BorderRadius.circular(12),
              ),
              child: Row(
                children: <Widget>[
                  const Icon(Icons.block, size: 18, color: AppTheme.danger),
                  const SizedBox(width: 10),
                  Expanded(
                    child: Text(
                      '${_money(rejectedTotal)} across ${rejected.length} rejected '
                      'receipt${rejected.length == 1 ? '' : 's'} — excluded from costs.',
                      style: const TextStyle(color: AppTheme.danger),
                    ),
                  ),
                ],
              ),
            ),
          ],
          const SizedBox(height: 16),
          const Text(
            'Spend by truck',
            style: TextStyle(
                fontWeight: FontWeight.w800, color: AppTheme.slate900),
          ),
          const SizedBox(height: 8),
          ...rows.map(
            (_TruckSpend r) => Card(
              child: ListTile(
                leading: Container(
                  width: 44,
                  height: 44,
                  decoration: BoxDecoration(
                    color: AppTheme.brand.withValues(alpha: 0.1),
                    borderRadius: BorderRadius.circular(12),
                  ),
                  child:
                      const Icon(Icons.local_shipping, color: AppTheme.brand),
                ),
                title: Text(
                  r.label,
                  style: const TextStyle(
                      fontWeight: FontWeight.w700, color: AppTheme.slate900),
                ),
                subtitle: Text('${r.count} receipt${r.count == 1 ? '' : 's'}',
                    style: const TextStyle(color: AppTheme.slate500)),
                trailing: Text(
                  _money(r.spend),
                  style: const TextStyle(
                      fontWeight: FontWeight.w800, color: AppTheme.slate900),
                ),
              ),
            ),
          ),
        ],
      ),
    );
  }
}
