import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:provider/provider.dart';

import '../../config/theme.dart';
import '../../models/truck_stock_item.dart';
import '../../providers/truck_stock_provider.dart';
import '../../services/api_client.dart';
import '../../widgets/primary_button.dart';
import '../home_shell.dart';

/// Records consumption of a truck-stock item on a job. Decrements the item's
/// current quantity via the `use-item` endpoint.
class UseItemScreen extends StatefulWidget {
  const UseItemScreen({super.key, required this.item});

  final TruckStockItem item;

  @override
  State<UseItemScreen> createState() => _UseItemScreenState();
}

class _UseItemScreenState extends State<UseItemScreen> {
  final GlobalKey<FormState> _formKey = GlobalKey<FormState>();
  final TextEditingController _quantity = TextEditingController(text: '1');
  final TextEditingController _notes = TextEditingController();
  bool _saving = false;

  @override
  void dispose() {
    _quantity.dispose();
    _notes.dispose();
    super.dispose();
  }

  int get _remaining {
    final int used = int.tryParse(_quantity.text.trim()) ?? 0;
    return (widget.item.currentQuantity - used);
  }

  Future<void> _submit() async {
    if (!_formKey.currentState!.validate()) return;
    FocusScope.of(context).unfocus();
    setState(() => _saving = true);
    final int qty = int.parse(_quantity.text.trim());

    try {
      await context.read<TruckStockProvider>().useItem(
            itemId: widget.item.id,
            quantity: qty,
            notes: _notes.text.trim().isEmpty ? null : _notes.text.trim(),
          );
      if (!mounted) return;
      Navigator.of(context).pop(true);
    } on ApiException catch (e) {
      if (mounted) {
        setState(() => _saving = false);
        showErrorSnack(context, e.message);
      }
    }
  }

  @override
  Widget build(BuildContext context) {
    final TruckStockItem item = widget.item;
    return Scaffold(
      appBar: AppBar(title: const Text('Use item')),
      body: SafeArea(
        child: Form(
          key: _formKey,
          child: ListView(
            padding: const EdgeInsets.all(16),
            children: <Widget>[
              Card(
                child: Padding(
                  padding: const EdgeInsets.all(16),
                  child: Row(
                    children: <Widget>[
                      Container(
                        width: 52,
                        height: 52,
                        decoration: BoxDecoration(
                          color: AppTheme.brand.withValues(alpha: 0.1),
                          borderRadius: BorderRadius.circular(12),
                        ),
                        child: const Icon(Icons.handyman_outlined,
                            color: AppTheme.brand),
                      ),
                      const SizedBox(width: 14),
                      Expanded(
                        child: Column(
                          crossAxisAlignment: CrossAxisAlignment.start,
                          children: <Widget>[
                            Text(
                              item.productName,
                              style: const TextStyle(
                                fontSize: 16,
                                fontWeight: FontWeight.w700,
                                color: AppTheme.slate900,
                              ),
                            ),
                            const SizedBox(height: 4),
                            Text(
                              'On truck: ${item.currentQuantity} '
                              '${item.unit ?? 'units'}',
                              style:
                                  const TextStyle(color: AppTheme.slate500),
                            ),
                          ],
                        ),
                      ),
                    ],
                  ),
                ),
              ),
              const SizedBox(height: 20),
              TextFormField(
                controller: _quantity,
                keyboardType: TextInputType.number,
                inputFormatters: <TextInputFormatter>[
                  FilteringTextInputFormatter.digitsOnly,
                ],
                onChanged: (_) => setState(() {}),
                decoration: const InputDecoration(
                  labelText: 'Quantity used',
                  prefixIcon: Icon(Icons.remove_circle_outline),
                ),
                validator: (String? v) {
                  final int? q = int.tryParse((v ?? '').trim());
                  if (q == null || q <= 0) {
                    return 'Enter a quantity greater than 0';
                  }
                  if (q > item.currentQuantity) {
                    return 'Only ${item.currentQuantity} available on truck';
                  }
                  return null;
                },
              ),
              const SizedBox(height: 8),
              Padding(
                padding: const EdgeInsets.symmetric(horizontal: 4),
                child: Text(
                  'Remaining after use: '
                  '${_remaining < 0 ? 0 : _remaining} ${item.unit ?? 'units'}',
                  style: TextStyle(
                    color: _remaining <= item.minimumQuantity
                        ? AppTheme.warning
                        : AppTheme.slate500,
                    fontWeight: FontWeight.w600,
                  ),
                ),
              ),
              const SizedBox(height: 16),
              TextFormField(
                controller: _notes,
                maxLines: 3,
                decoration: const InputDecoration(
                  labelText: 'Notes (e.g. job reference)',
                  prefixIcon: Icon(Icons.notes),
                  alignLabelWithHint: true,
                ),
              ),
              const SizedBox(height: 24),
              PrimaryButton(
                label: 'Record usage',
                icon: Icons.check,
                loading: _saving,
                onPressed: _submit,
              ),
            ],
          ),
        ),
      ),
    );
  }
}
