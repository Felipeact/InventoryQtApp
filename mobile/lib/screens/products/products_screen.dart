import 'package:flutter/material.dart';
import 'package:provider/provider.dart';

import '../../config/routes.dart';
import '../../config/theme.dart';
import '../../models/product.dart';
import '../../providers/auth_provider.dart';
import '../../providers/products_provider.dart';
import '../../widgets/empty_state.dart';
import '../../widgets/error_view.dart';
import '../../widgets/loading_indicator.dart';
import '../home_shell.dart';
import 'product_detail_screen.dart';

/// Searchable, refreshable product catalog. Admins can add new products.
class ProductsScreen extends StatefulWidget {
  const ProductsScreen({super.key, this.embedded = false});

  final bool embedded;

  @override
  State<ProductsScreen> createState() => _ProductsScreenState();
}

class _ProductsScreenState extends State<ProductsScreen> {
  final TextEditingController _search = TextEditingController();

  @override
  void initState() {
    super.initState();
    WidgetsBinding.instance.addPostFrameCallback((_) {
      if (context.read<ProductsProvider>().state == LoadState.idle) {
        context.read<ProductsProvider>().load();
      }
    });
  }

  @override
  void dispose() {
    _search.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    final ProductsProvider provider = context.watch<ProductsProvider>();
    final AuthProvider auth = context.watch<AuthProvider>();
    final bool canAdd = auth.can(Permissions.addProduct);

    return Scaffold(
      appBar: buildAdaptiveAppBar(
        context,
        embedded: widget.embedded,
        title: 'Products',
      ),
      floatingActionButton: canAdd
          ? FloatingActionButton.extended(
              onPressed: () async {
                final bool? created = await Navigator.of(context)
                    .pushNamed<bool>(AppRoutes.addProduct);
                if (created == true && context.mounted) {
                  context.read<ProductsProvider>().load();
                }
              },
              icon: const Icon(Icons.add),
              label: const Text('Add'),
            )
          : null,
      body: Column(
        children: <Widget>[
          Padding(
            padding: const EdgeInsets.fromLTRB(16, 12, 16, 8),
            child: TextField(
              controller: _search,
              onChanged: provider.setQuery,
              decoration: InputDecoration(
                hintText: 'Search by name, barcode, model...',
                prefixIcon: const Icon(Icons.search),
                suffixIcon: provider.query.isNotEmpty
                    ? IconButton(
                        icon: const Icon(Icons.clear),
                        onPressed: () {
                          _search.clear();
                          provider.setQuery('');
                        },
                      )
                    : null,
              ),
            ),
          ),
          Expanded(child: _body(provider)),
        ],
      ),
    );
  }

  Widget _body(ProductsProvider provider) {
    if (provider.state == LoadState.loading && provider.all.isEmpty) {
      return const LoadingIndicator(message: 'Loading products...');
    }
    if (provider.state == LoadState.error && provider.all.isEmpty) {
      return ErrorView(
        message: provider.error ?? 'Failed to load products.',
        onRetry: () => provider.load(),
      );
    }

    final List<Product> items = provider.filtered;
    if (items.isEmpty) {
      return RefreshIndicator(
        onRefresh: provider.load,
        child: ListView(
          children: <Widget>[
            SizedBox(height: MediaQuery.of(context).size.height * 0.18),
            EmptyState(
              icon: Icons.inventory_2_outlined,
              title: provider.query.isEmpty
                  ? 'No products yet'
                  : 'No matches found',
              message: provider.query.isEmpty
                  ? 'Products you add will appear here.'
                  : 'Try a different search term.',
            ),
          ],
        ),
      );
    }

    return RefreshIndicator(
      onRefresh: provider.load,
      child: ListView.separated(
        padding: const EdgeInsets.fromLTRB(16, 4, 16, 96),
        itemCount: items.length,
        separatorBuilder: (_, __) => const SizedBox(height: 10),
        itemBuilder: (BuildContext context, int i) =>
            _ProductTile(product: items[i]),
      ),
    );
  }
}

class _ProductTile extends StatelessWidget {
  const _ProductTile({required this.product});

  final Product product;

  @override
  Widget build(BuildContext context) {
    final Color statusColor = StatusStyle.colorFor(product.computedStatus);
    return Card(
      child: InkWell(
        borderRadius: BorderRadius.circular(16),
        onTap: () => Navigator.of(context).push(
          MaterialPageRoute<void>(
            builder: (_) => ProductDetailScreen(productId: product.id),
          ),
        ),
        child: Padding(
          padding: const EdgeInsets.all(14),
          child: Row(
            children: <Widget>[
              Container(
                width: 48,
                height: 48,
                decoration: BoxDecoration(
                  color: AppTheme.brand.withValues(alpha: 0.1),
                  borderRadius: BorderRadius.circular(12),
                ),
                child: const Icon(Icons.inventory_2_outlined,
                    color: AppTheme.brand),
              ),
              const SizedBox(width: 14),
              Expanded(
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: <Widget>[
                    Text(
                      product.name,
                      maxLines: 1,
                      overflow: TextOverflow.ellipsis,
                      style: const TextStyle(
                        fontWeight: FontWeight.w700,
                        fontSize: 15,
                        color: AppTheme.slate900,
                      ),
                    ),
                    const SizedBox(height: 2),
                    Text(
                      product.barcode.isNotEmpty
                          ? 'Barcode: ${product.barcode}'
                          : (product.model ?? 'No barcode'),
                      maxLines: 1,
                      overflow: TextOverflow.ellipsis,
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
                    '${product.quantity}',
                    style: const TextStyle(
                      fontSize: 18,
                      fontWeight: FontWeight.w800,
                      color: AppTheme.slate900,
                    ),
                  ),
                  const Text(
                    'in stock',
                    style: TextStyle(fontSize: 11, color: AppTheme.slate400),
                  ),
                  const SizedBox(height: 4),
                  Container(
                    padding:
                        const EdgeInsets.symmetric(horizontal: 8, vertical: 2),
                    decoration: BoxDecoration(
                      color: statusColor.withValues(alpha: 0.12),
                      borderRadius: BorderRadius.circular(6),
                    ),
                    child: Text(
                      _statusLabel(product.computedStatus),
                      style: TextStyle(
                        fontSize: 10,
                        fontWeight: FontWeight.w700,
                        color: statusColor,
                      ),
                    ),
                  ),
                ],
              ),
            ],
          ),
        ),
      ),
    );
  }

  String _statusLabel(String status) {
    switch (status.toUpperCase()) {
      case 'OUT_OF_STOCK':
        return 'OUT';
      case 'LOW_STOCK':
      case 'LOW':
        return 'LOW';
      case 'IN_STOCK':
        return 'OK';
      default:
        return status.toUpperCase();
    }
  }
}
