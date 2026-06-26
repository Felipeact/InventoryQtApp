import 'package:flutter/material.dart';
import 'package:intl/intl.dart';
import 'package:provider/provider.dart';


import '../../config/theme.dart';
import '../../models/receipt.dart';
import '../../providers/auth_provider.dart';
import '../../providers/truck_stock_provider.dart';
import '../../services/api_client.dart';
import '../../widgets/empty_state.dart';
import '../../widgets/error_view.dart';
import '../../widgets/loading_indicator.dart';
import '../home_shell.dart';
import 'upload_receipt_screen.dart';

/// Lists uploaded receipts. Technicians can upload new ones; users with the
/// APPROVE_RECEIPTS permission can approve or reject pending receipts inline.
class ReceiptsScreen extends StatefulWidget {
  const ReceiptsScreen({super.key, this.embedded = false});

  final bool embedded;

  @override
  State<ReceiptsScreen> createState() => _ReceiptsScreenState();
}

class _ReceiptsScreenState extends State<ReceiptsScreen> {
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
      _error = 'Failed to load receipts.';
    } finally {
      if (mounted) setState(() => _loading = false);
    }
  }

  Future<void> _upload() async {
    final bool? created = await Navigator.of(context).push<bool>(
      MaterialPageRoute<bool>(builder: (_) => const UploadReceiptScreen()),
    );
    if (created == true) _load();
  }

  /// Approves or rejects a receipt via the API and surfaces the result.
  Future<void> _setStatus(Receipt receipt, String status) async {
    try {
      await context
          .read<TruckStockProvider>()
          .updateReceiptStatus(receipt.id, status);
      if (!mounted) return;
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text('Receipt ${status.toLowerCase()}')),
      );
    } on ApiException catch (e) {
      if (!mounted) return;
      ScaffoldMessenger.of(context)
          .showSnackBar(SnackBar(content: Text(e.message)));
    } catch (_) {
      if (!mounted) return;
      ScaffoldMessenger.of(context).showSnackBar(
        const SnackBar(content: Text('Failed to update receipt.')),
      );
    }
  }

  /// Reconciles a receipt against its truck's template allowance and shows the
  /// total-vs-allowance result.
  Future<void> _reconcile(Receipt receipt) async {
    try {
      final Map<String, dynamic> r = await context
          .read<TruckStockProvider>()
          .reconcileReceipt(receipt.id);
      if (!mounted) return;
      await showDialog<void>(
        context: context,
        builder: (_) => _ReconcileDialog(result: r),
      );
    } on ApiException catch (e) {
      if (!mounted) return;
      ScaffoldMessenger.of(context)
          .showSnackBar(SnackBar(content: Text(e.message)));
    } catch (_) {
      if (!mounted) return;
      ScaffoldMessenger.of(context).showSnackBar(
        const SnackBar(content: Text('Could not reconcile the receipt.')),
      );
    }
  }

  @override
  Widget build(BuildContext context) {
    final TruckStockProvider provider = context.watch<TruckStockProvider>();
    final AuthProvider auth = context.watch<AuthProvider>();
    final bool canUpload = auth.can(Permissions.uploadReceipt);

    return Scaffold(
      appBar: buildAdaptiveAppBar(
        context,
        embedded: widget.embedded,
        title: 'Receipts',
      ),
      floatingActionButton: canUpload
          ? FloatingActionButton.extended(
              onPressed: _upload,
              icon: const Icon(Icons.upload_file),
              label: const Text('Upload'),
            )
          : null,
      body: _body(provider, auth),
    );
  }

  Widget _body(TruckStockProvider provider, AuthProvider auth) {
    if (_loading && provider.receipts.isEmpty) {
      return const LoadingIndicator(message: 'Loading receipts...');
    }
    if (_error != null && provider.receipts.isEmpty) {
      return ErrorView(message: _error!, onRetry: _load);
    }
    if (provider.receipts.isEmpty) {
      return RefreshIndicator(
        onRefresh: _load,
        child: ListView(
          children: <Widget>[
            SizedBox(height: MediaQuery.of(context).size.height * 0.16),
            EmptyState(
              icon: Icons.receipt_long_outlined,
              title: 'No receipts yet',
              message: auth.can(Permissions.uploadReceipt)
                  ? 'Upload a purchase receipt to keep your truck expenses '
                      'tracked.'
                  : 'Uploaded receipts will appear here.',
              actionLabel: auth.can(Permissions.uploadReceipt)
                  ? 'Upload receipt'
                  : null,
              onAction:
                  auth.can(Permissions.uploadReceipt) ? _upload : null,
            ),
          ],
        ),
      );
    }

    return RefreshIndicator(
      onRefresh: _load,
      child: ListView.separated(
        padding: const EdgeInsets.fromLTRB(16, 16, 16, 96),
        itemCount: provider.receipts.length,
        separatorBuilder: (_, __) => const SizedBox(height: 10),
        itemBuilder: (BuildContext context, int i) => _ReceiptCard(
          receipt: provider.receipts[i],
          canApprove: auth.can(Permissions.approveReceipts),
          onStatusChange: _setStatus,
          onReconcile: _reconcile,
        ),
      ),
    );
  }
}

class _ReceiptCard extends StatelessWidget {
  const _ReceiptCard({
    required this.receipt,
    required this.canApprove,
    required this.onStatusChange,
    required this.onReconcile,
  });

  final Receipt receipt;
  final bool canApprove;
  final Future<void> Function(Receipt receipt, String status) onStatusChange;
  final Future<void> Function(Receipt receipt) onReconcile;

  @override
  Widget build(BuildContext context) {
    final Color statusColor =
        StatusStyle.colorFor(receipt.status ?? 'PENDING');
    final String dateStr = receipt.createdAt != null
        ? DateFormat('MMM d, y • h:mm a').format(receipt.createdAt!.toLocal())
        : 'Unknown date';
    final String amountStr = receipt.totalAmount != null
        ? NumberFormat.currency(symbol: '\$').format(receipt.totalAmount)
        : '—';

    return Card(
      child: Padding(
        padding: const EdgeInsets.all(14),
        child: Row(
          children: <Widget>[
            Container(
              width: 48,
              height: 48,
              decoration: BoxDecoration(
                color: AppTheme.info.withValues(alpha: 0.1),
                borderRadius: BorderRadius.circular(12),
              ),
              child: const Icon(Icons.receipt_long, color: AppTheme.info),
            ),
            const SizedBox(width: 14),
            Expanded(
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.start,
                children: <Widget>[
                  Row(
                    children: <Widget>[
                      Expanded(
                        child: Text(
                          amountStr,
                          style: const TextStyle(
                            fontWeight: FontWeight.w800,
                            fontSize: 16,
                            color: AppTheme.slate900,
                          ),
                        ),
                      ),
                      Container(
                        padding: const EdgeInsets.symmetric(
                            horizontal: 8, vertical: 3),
                        decoration: BoxDecoration(
                          color: statusColor.withValues(alpha: 0.12),
                          borderRadius: BorderRadius.circular(6),
                        ),
                        child: Text(
                          (receipt.status ?? 'PENDING').toUpperCase(),
                          style: TextStyle(
                            fontSize: 10,
                            fontWeight: FontWeight.w800,
                            color: statusColor,
                          ),
                        ),
                      ),
                    ],
                  ),
                  const SizedBox(height: 4),
                  Text(
                    receipt.truckNumber != null
                        ? 'Truck ${receipt.truckNumber} • $dateStr'
                        : dateStr,
                    style: const TextStyle(
                      fontSize: 12,
                      color: AppTheme.slate500,
                    ),
                  ),
                  if (canApprove) ...<Widget>[
                    const SizedBox(height: 6),
                    Align(
                      alignment: Alignment.centerLeft,
                      child: TextButton.icon(
                        onPressed: () => onReconcile(receipt),
                        icon: const Icon(Icons.rule, size: 16),
                        label: const Text('Reconcile'),
                        style: TextButton.styleFrom(
                          foregroundColor: AppTheme.brand,
                          padding: EdgeInsets.zero,
                          minimumSize: const Size(0, 28),
                          tapTargetSize: MaterialTapTargetSize.shrinkWrap,
                        ),
                      ),
                    ),
                  ],
                  if (canApprove &&
                      (receipt.status ?? 'PENDING').toUpperCase() ==
                          'PENDING') ...<Widget>[
                    const SizedBox(height: 10),
                    Row(
                      children: <Widget>[
                        Expanded(
                          child: OutlinedButton.icon(
                            onPressed: () =>
                                onStatusChange(receipt, 'REJECTED'),
                            icon: const Icon(Icons.close, size: 16),
                            label: const Text('Reject'),
                            style: OutlinedButton.styleFrom(
                              foregroundColor: AppTheme.danger,
                              side: const BorderSide(color: AppTheme.danger),
                              padding:
                                  const EdgeInsets.symmetric(vertical: 6),
                            ),
                          ),
                        ),
                        const SizedBox(width: 10),
                        Expanded(
                          child: FilledButton.icon(
                            onPressed: () =>
                                onStatusChange(receipt, 'APPROVED'),
                            icon: const Icon(Icons.check, size: 16),
                            label: const Text('Approve'),
                            style: FilledButton.styleFrom(
                              backgroundColor: AppTheme.success,
                              padding:
                                  const EdgeInsets.symmetric(vertical: 6),
                            ),
                          ),
                        ),
                      ],
                    ),
                  ],
                ],
              ),
            ),
          ],
        ),
      ),
    );
  }
}

/// Shows a receipt's reconciliation against its truck's template allowance.
class _ReconcileDialog extends StatelessWidget {
  const _ReconcileDialog({required this.result});

  final Map<String, dynamic> result;

  double? _num(dynamic v) =>
      v is num ? v.toDouble() : double.tryParse('${v ?? ''}');

  String _money(double? v) =>
      v == null ? '—' : NumberFormat.currency(symbol: '\$').format(v);

  @override
  Widget build(BuildContext context) {
    final String status = (result['status'] ?? '').toString();
    final double? receiptTotal = _num(result['receiptTotal']);
    final double? allowance = _num(result['expectedTotal']);
    final double? difference = _num(result['difference']);
    final bool hasExpected = result['hasExpected'] == true;
    final bool overBudget = result['overBudget'] == true;
    final Color tone = status == 'RECONCILED' ? AppTheme.success : AppTheme.warning;

    return AlertDialog(
      title: const Text('Reconciliation'),
      content: Column(
        mainAxisSize: MainAxisSize.min,
        crossAxisAlignment: CrossAxisAlignment.start,
        children: <Widget>[
          Container(
            padding: const EdgeInsets.symmetric(horizontal: 10, vertical: 4),
            decoration: BoxDecoration(
              color: tone.withValues(alpha: 0.12),
              borderRadius: BorderRadius.circular(6),
            ),
            child: Text(
              status.isEmpty ? '—' : status,
              style: TextStyle(fontWeight: FontWeight.w800, color: tone),
            ),
          ),
          const SizedBox(height: 14),
          _row('Receipt total', _money(receiptTotal)),
          _row('Allowance (template)', _money(allowance)),
          _row(
            overBudget ? 'Over budget by' : 'Under budget by',
            difference == null ? '—' : _money(difference.abs()),
            valueColor: overBudget ? AppTheme.warning : AppTheme.success,
          ),
          if (!hasExpected) ...<Widget>[
            const SizedBox(height: 10),
            const Text(
              'This truck\'s template has no allowance set, so the total can\'t '
              'be verified. Set an allowance on the template to enable '
              'reconciliation.',
              style: TextStyle(fontSize: 12, color: AppTheme.warning),
            ),
          ],
        ],
      ),
      actions: <Widget>[
        TextButton(
          onPressed: () => Navigator.of(context).pop(),
          child: const Text('Done'),
        ),
      ],
    );
  }

  Widget _row(String label, String value, {Color? valueColor}) => Padding(
        padding: const EdgeInsets.symmetric(vertical: 4),
        child: Row(
          mainAxisAlignment: MainAxisAlignment.spaceBetween,
          children: <Widget>[
            Text(label, style: const TextStyle(color: AppTheme.slate500)),
            Text(
              value,
              style: TextStyle(
                fontWeight: FontWeight.w700,
                color: valueColor ?? AppTheme.slate900,
              ),
            ),
          ],
        ),
      );
}
