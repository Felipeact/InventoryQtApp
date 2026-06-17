import 'package:flutter/material.dart';
import 'package:provider/provider.dart';

import '../../config/theme.dart';
import '../../models/truck_stock_item.dart';
import '../../models/truck_stock_template.dart';
import '../../providers/truck_stock_provider.dart';
import '../../services/api_client.dart';
import '../../widgets/empty_state.dart';
import '../../widgets/error_view.dart';
import '../../widgets/loading_indicator.dart';
import '../home_shell.dart';

/// Lists truck-stock templates. Tapping one opens its item breakdown.
class TemplatesScreen extends StatefulWidget {
  const TemplatesScreen({super.key, this.embedded = false});

  final bool embedded;

  @override
  State<TemplatesScreen> createState() => _TemplatesScreenState();
}

class _TemplatesScreenState extends State<TemplatesScreen> {
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
      await context.read<TruckStockProvider>().loadTemplates();
    } on ApiException catch (e) {
      _error = e.message;
    } catch (_) {
      _error = 'Failed to load templates.';
    } finally {
      if (mounted) setState(() => _loading = false);
    }
  }

  @override
  Widget build(BuildContext context) {
    final TruckStockProvider provider = context.watch<TruckStockProvider>();

    return Scaffold(
      appBar: buildAdaptiveAppBar(
        context,
        embedded: widget.embedded,
        title: 'Stock Templates',
      ),
      body: _body(provider),
    );
  }

  Widget _body(TruckStockProvider provider) {
    if (_loading && provider.templates.isEmpty) {
      return const LoadingIndicator(message: 'Loading templates...');
    }
    if (_error != null && provider.templates.isEmpty) {
      return ErrorView(message: _error!, onRetry: _load);
    }
    if (provider.templates.isEmpty) {
      return RefreshIndicator(
        onRefresh: _load,
        child: ListView(
          children: <Widget>[
            SizedBox(height: MediaQuery.of(context).size.height * 0.18),
            const EmptyState(
              icon: Icons.checklist_rtl_outlined,
              title: 'No templates',
              message: 'Truck-stock templates will appear here once created.',
            ),
          ],
        ),
      );
    }

    return RefreshIndicator(
      onRefresh: _load,
      child: ListView.separated(
        padding: const EdgeInsets.all(16),
        itemCount: provider.templates.length,
        separatorBuilder: (_, __) => const SizedBox(height: 10),
        itemBuilder: (BuildContext context, int i) {
          final TruckStockTemplate t = provider.templates[i];
          return Card(
            child: ListTile(
              contentPadding:
                  const EdgeInsets.symmetric(horizontal: 16, vertical: 8),
              leading: Container(
                width: 48,
                height: 48,
                decoration: BoxDecoration(
                  color: AppTheme.brand.withOpacity(0.1),
                  borderRadius: BorderRadius.circular(12),
                ),
                child: const Icon(Icons.checklist_rtl, color: AppTheme.brand),
              ),
              title: Text(
                t.name,
                style: const TextStyle(
                  fontWeight: FontWeight.w700,
                  color: AppTheme.slate900,
                ),
              ),
              subtitle: Text(
                <String>[
                  if (t.tradeType != null && t.tradeType!.isNotEmpty)
                    t.tradeType!,
                  '${t.itemCount} items',
                ].join(' • '),
                style: const TextStyle(color: AppTheme.slate500),
              ),
              trailing: const Icon(Icons.chevron_right),
              onTap: () => _openTemplate(t),
            ),
          );
        },
      ),
    );
  }

  Future<void> _openTemplate(TruckStockTemplate template) async {
    // If items weren't included in the list payload, fetch the full template.
    TruckStockTemplate full = template;
    if (template.items.isEmpty) {
      try {
        full = await context
            .read<TruckStockProvider>()
            .loadTemplate(template.id);
      } on ApiException {
        // Use what we have.
      }
    }
    if (!mounted) return;
    Navigator.of(context).push(
      MaterialPageRoute<void>(
        builder: (_) => _TemplateDetail(template: full),
      ),
    );
  }
}

class _TemplateDetail extends StatelessWidget {
  const _TemplateDetail({required this.template});

  final TruckStockTemplate template;

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: Text(template.name)),
      body: template.items.isEmpty
          ? const EmptyState(
              icon: Icons.inbox_outlined,
              title: 'No items',
              message: 'This template has no items defined.',
            )
          : ListView(
              padding: const EdgeInsets.all(16),
              children: <Widget>[
                Card(
                  child: Padding(
                    padding: const EdgeInsets.all(16),
                    child: Row(
                      children: <Widget>[
                        _stat('Items', '${template.itemCount}'),
                        const SizedBox(width: 16),
                        _stat('Total units', '${template.totalRequiredUnits}'),
                        if (template.tradeType != null) ...<Widget>[
                          const SizedBox(width: 16),
                          _stat('Trade', template.tradeType!),
                        ],
                      ],
                    ),
                  ),
                ),
                const SizedBox(height: 16),
                ...template.items.map(
                  (TruckStockItem item) => Padding(
                    padding: const EdgeInsets.only(bottom: 8),
                    child: Card(
                      child: ListTile(
                        title: Text(
                          item.productName,
                          style: const TextStyle(
                            fontWeight: FontWeight.w600,
                            color: AppTheme.slate900,
                          ),
                        ),
                        subtitle: Text(
                          <String>[
                            if (item.category != null) item.category!,
                            'required ${item.requiredQuantity} ${item.unit ?? ''}'
                                .trim(),
                          ].join(' • '),
                          style: const TextStyle(color: AppTheme.slate500),
                        ),
                        trailing: item.expectedPrice != null
                            ? Text(
                                '\$${item.expectedPrice!.toStringAsFixed(2)}',
                                style: const TextStyle(
                                  fontWeight: FontWeight.w700,
                                  color: AppTheme.slate700,
                                ),
                              )
                            : null,
                      ),
                    ),
                  ),
                ),
              ],
            ),
    );
  }

  Widget _stat(String label, String value) {
    return Column(
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
        Text(label,
            style: const TextStyle(fontSize: 12, color: AppTheme.slate500)),
      ],
    );
  }
}
