import 'package:flutter/material.dart';
import 'package:provider/provider.dart';

import '../config/theme.dart';
import '../models/receipt.dart';
import '../models/truck_stock_item.dart';
import '../providers/auth_provider.dart';
import '../providers/truck_stock_provider.dart';
import '../services/api_client.dart';
import '../widgets/loading_indicator.dart';
import 'home_shell.dart';

/// Aggregates actionable alerts — low truck-stock items and pending receipts —
/// mirroring the desktop notifications panel. Sections are permission-gated.
class NotificationsScreen extends StatefulWidget {
  const NotificationsScreen({super.key, this.embedded = false});

  final bool embedded;

  @override
  State<NotificationsScreen> createState() => _NotificationsScreenState();
}

class _NotificationsScreenState extends State<NotificationsScreen> {
  bool _loading = true;

  @override
  void initState() {
    super.initState();
    _load();
  }

  Future<void> _load() async {
    setState(() => _loading = true);
    final AuthProvider auth = context.read<AuthProvider>();
    final TruckStockProvider provider = context.read<TruckStockProvider>();
    try {
      if (auth.can(Permissions.viewLowStockAlerts)) {
        await provider.loadLowStock();
      }
      if (auth.can(Permissions.approveReceipts)) {
        await provider.loadReceipts();
      }
    } on ApiException {
      // Non-fatal — show whatever loaded.
    } catch (_) {
      // Ignore; partial data is fine for an alerts view.
    } finally {
      if (mounted) setState(() => _loading = false);
    }
  }

  @override
  Widget build(BuildContext context) {
    final AuthProvider auth = context.watch<AuthProvider>();
    final TruckStockProvider provider = context.watch<TruckStockProvider>();

    final bool seeLow = auth.can(Permissions.viewLowStockAlerts);
    final bool seeReceipts = auth.can(Permissions.approveReceipts);

    final List<TruckStockItem> low =
        seeLow ? provider.lowStock : <TruckStockItem>[];
    final List<Receipt> pending = seeReceipts
        ? provider.receipts
            .where((Receipt r) => (r.status ?? '').toUpperCase() == 'PENDING')
            .toList()
        : <Receipt>[];

    return Scaffold(
      appBar: buildAdaptiveAppBar(
        context,
        embedded: widget.embedded,
        title: 'Notifications',
      ),
      body: _loading && low.isEmpty && pending.isEmpty
          ? const LoadingIndicator(message: 'Loading alerts...')
          : RefreshIndicator(
              onRefresh: _load,
              child: ListView(
                padding: const EdgeInsets.all(16),
                children: <Widget>[
                  if (seeLow)
                    _section(
                      icon: Icons.warning_amber_rounded,
                      title: 'Low stock',
                      count: low.length,
                      color: low.isEmpty ? AppTheme.success : AppTheme.danger,
                      emptyText: 'Everything is stocked — no alerts.',
                      children: low
                          .take(50)
                          .map((TruckStockItem i) => ListTile(
                                dense: true,
                                title: Text(i.productName),
                                trailing: Text(
                                  '${i.currentQuantity}/${i.requiredQuantity}',
                                  style: const TextStyle(
                                      fontWeight: FontWeight.w700,
                                      color: AppTheme.danger),
                                ),
                              ))
                          .toList(),
                    ),
                  if (seeReceipts)
                    _section(
                      icon: Icons.receipt_long_outlined,
                      title: 'Pending receipts',
                      count: pending.length,
                      color:
                          pending.isEmpty ? AppTheme.success : AppTheme.warning,
                      emptyText: 'No receipts awaiting review.',
                      children: pending
                          .take(50)
                          .map((Receipt r) => ListTile(
                                dense: true,
                                title: Text(r.truckNumber != null
                                    ? 'Truck ${r.truckNumber}'
                                    : 'Receipt'),
                                trailing: Text(
                                  r.totalAmount != null
                                      ? '\$${r.totalAmount!.toStringAsFixed(2)}'
                                      : '—',
                                  style: const TextStyle(
                                      fontWeight: FontWeight.w700),
                                ),
                              ))
                          .toList(),
                    ),
                  if (!seeLow && !seeReceipts)
                    const Padding(
                      padding: EdgeInsets.only(top: 80),
                      child: Center(
                        child: Text('No notifications.',
                            style: TextStyle(color: AppTheme.slate500)),
                      ),
                    ),
                ],
              ),
            ),
    );
  }

  Widget _section({
    required IconData icon,
    required String title,
    required int count,
    required Color color,
    required String emptyText,
    required List<Widget> children,
  }) {
    return Card(
      margin: const EdgeInsets.only(bottom: 14),
      child: Padding(
        padding: const EdgeInsets.all(8),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: <Widget>[
            Padding(
              padding: const EdgeInsets.fromLTRB(8, 8, 8, 4),
              child: Row(
                children: <Widget>[
                  Icon(icon, size: 18, color: color),
                  const SizedBox(width: 8),
                  Text(
                    title,
                    style: const TextStyle(
                        fontWeight: FontWeight.w800, color: AppTheme.slate900),
                  ),
                  const SizedBox(width: 8),
                  Container(
                    padding:
                        const EdgeInsets.symmetric(horizontal: 8, vertical: 1),
                    decoration: BoxDecoration(
                      color: color,
                      borderRadius: BorderRadius.circular(8),
                    ),
                    child: Text('$count',
                        style: const TextStyle(
                            color: Colors.white,
                            fontSize: 12,
                            fontWeight: FontWeight.w700)),
                  ),
                ],
              ),
            ),
            if (children.isEmpty)
              Padding(
                padding: const EdgeInsets.fromLTRB(8, 4, 8, 8),
                child: Text(emptyText,
                    style: const TextStyle(color: AppTheme.slate500)),
              )
            else
              ...children,
          ],
        ),
      ),
    );
  }
}
