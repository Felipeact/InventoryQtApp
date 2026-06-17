import 'package:flutter/material.dart';
import 'package:provider/provider.dart';

import '../../config/theme.dart';
import '../../models/product.dart';
import '../../providers/auth_provider.dart';
import '../../providers/products_provider.dart';
import '../../services/product_service.dart';
import '../../widgets/error_view.dart';
import '../../widgets/loading_indicator.dart';
import '../home_shell.dart';
import 'add_edit_product_screen.dart';

/// Detailed product view with stock figures, metadata and admin edit/delete.
class ProductDetailScreen extends StatefulWidget {
  const ProductDetailScreen({super.key, required this.productId});

  final String productId;

  @override
  State<ProductDetailScreen> createState() => _ProductDetailScreenState();
}

class _ProductDetailScreenState extends State<ProductDetailScreen> {
  final ProductService _service = ProductService();
  Product? _product;
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
      final Product p = await _service.fetchById(widget.productId);
      if (!mounted) return;
      setState(() {
        _product = p;
        _loading = false;
      });
    } on ApiException catch (e) {
      // Fall back to the cached list entry if the detail endpoint fails.
      final ProductsProvider provider = context.read<ProductsProvider>();
      Product? cached;
      for (final Product p in provider.all) {
        if (p.id == widget.productId) {
          cached = p;
          break;
        }
      }
      if (!mounted) return;
      setState(() {
        _product = cached;
        _error = cached == null ? e.message : null;
        _loading = false;
      });
    }
  }

  Future<void> _delete() async {
    final bool? ok = await showDialog<bool>(
      context: context,
      builder: (BuildContext ctx) => AlertDialog(
        title: const Text('Delete product?'),
        content: Text('"${_product?.name}" will be permanently removed.'),
        actions: <Widget>[
          TextButton(
            onPressed: () => Navigator.of(ctx).pop(false),
            child: const Text('Cancel'),
          ),
          FilledButton(
            style: FilledButton.styleFrom(backgroundColor: AppTheme.danger),
            onPressed: () => Navigator.of(ctx).pop(true),
            child: const Text('Delete'),
          ),
        ],
      ),
    );
    if (ok != true) return;
    try {
      await context.read<ProductsProvider>().delete(widget.productId);
      if (mounted) {
        showSuccessSnack(context, 'Product deleted.');
        Navigator.of(context).pop(true);
      }
    } on ApiException catch (e) {
      if (mounted) showErrorSnack(context, e.message);
    }
  }

  @override
  Widget build(BuildContext context) {
    final AuthProvider auth = context.watch<AuthProvider>();
    final bool canManage = auth.can(Permissions.addProduct);

    return Scaffold(
      appBar: AppBar(
        title: const Text('Product details'),
        actions: <Widget>[
          if (canManage && _product != null) ...<Widget>[
            IconButton(
              icon: const Icon(Icons.edit_outlined),
              tooltip: 'Edit',
              onPressed: () async {
                final bool? changed = await Navigator.of(context).push<bool>(
                  MaterialPageRoute<bool>(
                    builder: (_) => AddEditProductScreen(product: _product),
                  ),
                );
                if (changed == true) _load();
              },
            ),
            IconButton(
              icon: const Icon(Icons.delete_outline, color: AppTheme.danger),
              tooltip: 'Delete',
              onPressed: _delete,
            ),
          ],
        ],
      ),
      body: _build(),
    );
  }

  Widget _build() {
    if (_loading) {
      return const LoadingIndicator(message: 'Loading product...');
    }
    if (_product == null) {
      return ErrorView(
        message: _error ?? 'Product not found.',
        onRetry: _load,
      );
    }

    final Product p = _product!;
    final Color statusColor = StatusStyle.colorFor(p.computedStatus);

    return RefreshIndicator(
      onRefresh: _load,
      child: ListView(
        padding: const EdgeInsets.all(16),
        children: <Widget>[
          Card(
            child: Padding(
              padding: const EdgeInsets.all(20),
              child: Row(
                children: <Widget>[
                  Container(
                    width: 64,
                    height: 64,
                    decoration: BoxDecoration(
                      color: AppTheme.brand.withOpacity(0.1),
                      borderRadius: BorderRadius.circular(16),
                    ),
                    child: const Icon(
                      Icons.inventory_2_outlined,
                      color: AppTheme.brand,
                      size: 30,
                    ),
                  ),
                  const SizedBox(width: 16),
                  Expanded(
                    child: Column(
                      crossAxisAlignment: CrossAxisAlignment.start,
                      children: <Widget>[
                        Text(
                          p.name,
                          style: const TextStyle(
                            fontSize: 20,
                            fontWeight: FontWeight.w800,
                            color: AppTheme.slate900,
                          ),
                        ),
                        const SizedBox(height: 6),
                        Container(
                          padding: const EdgeInsets.symmetric(
                              horizontal: 10, vertical: 4),
                          decoration: BoxDecoration(
                            color: statusColor.withOpacity(0.12),
                            borderRadius: BorderRadius.circular(8),
                          ),
                          child: Text(
                            p.computedStatus.replaceAll('_', ' '),
                            style: TextStyle(
                              color: statusColor,
                              fontWeight: FontWeight.w700,
                              fontSize: 12,
                            ),
                          ),
                        ),
                      ],
                    ),
                  ),
                ],
              ),
            ),
          ),
          const SizedBox(height: 16),
          Row(
            children: <Widget>[
              Expanded(
                child: _MetricBox(
                  label: 'In stock',
                  value: '${p.quantity}',
                  color: statusColor,
                  icon: Icons.numbers,
                ),
              ),
              const SizedBox(width: 12),
              Expanded(
                child: _MetricBox(
                  label: 'Low threshold',
                  value: '${p.lowStockThreshold}',
                  color: AppTheme.warning,
                  icon: Icons.trending_down,
                ),
              ),
            ],
          ),
          const SizedBox(height: 16),
          Card(
            child: Padding(
              padding: const EdgeInsets.all(16),
              child: Column(
                children: <Widget>[
                  _DetailRow(label: 'Barcode', value: p.barcode),
                  _DetailRow(label: 'Model', value: p.model),
                  _DetailRow(label: 'Type', value: p.type),
                  _DetailRow(label: 'Location', value: p.location),
                  _DetailRow(label: 'Project', value: p.project),
                  _DetailRow(label: 'Account', value: p.account),
                  _DetailRow(
                    label: 'Description',
                    value: p.description,
                    last: true,
                  ),
                ],
              ),
            ),
          ),
        ],
      ),
    );
  }
}

class _MetricBox extends StatelessWidget {
  const _MetricBox({
    required this.label,
    required this.value,
    required this.color,
    required this.icon,
  });

  final String label;
  final String value;
  final Color color;
  final IconData icon;

  @override
  Widget build(BuildContext context) {
    return Card(
      child: Padding(
        padding: const EdgeInsets.all(16),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: <Widget>[
            Icon(icon, color: color, size: 22),
            const SizedBox(height: 10),
            Text(
              value,
              style: const TextStyle(
                fontSize: 24,
                fontWeight: FontWeight.w800,
                color: AppTheme.slate900,
              ),
            ),
            Text(
              label,
              style: const TextStyle(fontSize: 12, color: AppTheme.slate500),
            ),
          ],
        ),
      ),
    );
  }
}

class _DetailRow extends StatelessWidget {
  const _DetailRow({
    required this.label,
    required this.value,
    this.last = false,
  });

  final String label;
  final String? value;
  final bool last;

  @override
  Widget build(BuildContext context) {
    final String display =
        (value == null || value!.isEmpty) ? '—' : value!;
    return Column(
      children: <Widget>[
        Padding(
          padding: const EdgeInsets.symmetric(vertical: 12),
          child: Row(
            crossAxisAlignment: CrossAxisAlignment.start,
            children: <Widget>[
              SizedBox(
                width: 110,
                child: Text(
                  label,
                  style: const TextStyle(
                    color: AppTheme.slate500,
                    fontWeight: FontWeight.w500,
                  ),
                ),
              ),
              Expanded(
                child: Text(
                  display,
                  style: const TextStyle(
                    color: AppTheme.slate900,
                    fontWeight: FontWeight.w600,
                  ),
                ),
              ),
            ],
          ),
        ),
        if (!last) const Divider(height: 1),
      ],
    );
  }
}
