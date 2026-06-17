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

/// Lists uploaded receipts. Technicians can upload new ones; admins also see an
/// approval-status indicator (approval action is a TODO pending an endpoint).
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
        itemBuilder: (BuildContext context, int i) =>
            _ReceiptCard(receipt: provider.receipts[i], isAdmin: auth.isAdmin),
      ),
    );
  }
}

class _ReceiptCard extends StatelessWidget {
  const _ReceiptCard({required this.receipt, required this.isAdmin});

  final Receipt receipt;
  final bool isAdmin;

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
                color: AppTheme.info.withOpacity(0.1),
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
                          color: statusColor.withOpacity(0.12),
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
                  if (isAdmin &&
                      (receipt.status ?? 'PENDING').toUpperCase() ==
                          'PENDING') ...<Widget>[
                    const SizedBox(height: 6),
                    // TODO: Wire up an approve/reject endpoint when available.
                    const Text(
                      'Awaiting approval',
                      style: TextStyle(
                        fontSize: 11,
                        color: AppTheme.warning,
                        fontStyle: FontStyle.italic,
                      ),
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
