import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:provider/provider.dart';

import '../../config/theme.dart';
import '../../models/truck_stock_item.dart';
import '../../models/truck_stock_template.dart';
import '../../providers/truck_stock_provider.dart';
import '../../services/api_client.dart';
import '../../widgets/primary_button.dart';
import '../home_shell.dart';

/// Create or edit a truck-stock template — name, trade type, spending allowance,
/// and its list of expected items. Pass [template] to edit; null to create.
class TemplateEditorScreen extends StatefulWidget {
  const TemplateEditorScreen({super.key, this.template});

  final TruckStockTemplate? template;

  @override
  State<TemplateEditorScreen> createState() => _TemplateEditorScreenState();
}

class _ItemRow {
  _ItemRow({
    String name = '',
    int requiredQty = 1,
    int minimum = 1,
    String unit = '',
    String price = '',
  })  : name = TextEditingController(text: name),
        requiredQty = TextEditingController(text: '$requiredQty'),
        minimum = TextEditingController(text: '$minimum'),
        unit = TextEditingController(text: unit),
        price = TextEditingController(text: price);

  final TextEditingController name;
  final TextEditingController requiredQty;
  final TextEditingController minimum;
  final TextEditingController unit;
  final TextEditingController price;

  void dispose() {
    name.dispose();
    requiredQty.dispose();
    minimum.dispose();
    unit.dispose();
    price.dispose();
  }
}

class _TemplateEditorScreenState extends State<TemplateEditorScreen> {
  late final TextEditingController _name;
  late final TextEditingController _tradeType;
  late final TextEditingController _allowance;
  final List<_ItemRow> _rows = <_ItemRow>[];
  bool _saving = false;

  bool get _isEdit => widget.template != null;

  @override
  void initState() {
    super.initState();
    final TruckStockTemplate? t = widget.template;
    _name = TextEditingController(text: t?.name ?? '');
    _tradeType = TextEditingController(text: t?.tradeType ?? '');
    _allowance = TextEditingController(
      text: t?.allowance != null ? '${t!.allowance}' : '',
    );
    if (t != null && t.items.isNotEmpty) {
      for (final TruckStockItem i in t.items) {
        _rows.add(_ItemRow(
          name: i.productName,
          requiredQty: i.requiredQuantity,
          minimum: i.minimumQuantity == 0 ? 1 : i.minimumQuantity,
          unit: i.unit ?? '',
          price: i.expectedPrice != null ? '${i.expectedPrice}' : '',
        ));
      }
    } else {
      _rows.add(_ItemRow());
    }
  }

  @override
  void dispose() {
    _name.dispose();
    _tradeType.dispose();
    _allowance.dispose();
    for (final _ItemRow r in _rows) {
      r.dispose();
    }
    super.dispose();
  }

  Future<void> _save() async {
    if (_name.text.trim().isEmpty) {
      showErrorSnack(context, 'Enter a template name.');
      return;
    }
    final List<Map<String, dynamic>> items = <Map<String, dynamic>>[];
    for (final _ItemRow r in _rows) {
      final String name = r.name.text.trim();
      if (name.isEmpty) continue;
      final Map<String, dynamic> item = <String, dynamic>{
        'productName': name,
        'requiredQuantity': int.tryParse(r.requiredQty.text.trim()) ?? 1,
        'minimumQuantity': int.tryParse(r.minimum.text.trim()) ?? 1,
      };
      if (r.unit.text.trim().isNotEmpty) item['unit'] = r.unit.text.trim();
      final double? price = double.tryParse(r.price.text.trim());
      if (price != null) item['expectedPrice'] = price;
      items.add(item);
    }
    if (items.isEmpty) {
      showErrorSnack(context, 'Add at least one item with a product name.');
      return;
    }
    final String allowanceText = _allowance.text.trim();
    final double? allowance =
        allowanceText.isEmpty ? null : double.tryParse(allowanceText);
    if (allowanceText.isNotEmpty && (allowance == null || allowance < 0)) {
      showErrorSnack(context, 'Allowance must be a positive amount.');
      return;
    }

    setState(() => _saving = true);
    try {
      await context.read<TruckStockProvider>().saveTemplate(
            id: widget.template?.id,
            name: _name.text.trim(),
            tradeType: _tradeType.text.trim().isEmpty
                ? null
                : _tradeType.text.trim(),
            allowance: allowance,
            items: items,
          );
      if (!mounted) return;
      showSuccessSnack(
          context, _isEdit ? 'Template updated.' : 'Template created.');
      Navigator.of(context).pop(true);
    } on ApiException catch (e) {
      if (mounted) {
        setState(() => _saving = false);
        showErrorSnack(context, e.message);
      }
    } catch (_) {
      if (mounted) {
        setState(() => _saving = false);
        showErrorSnack(context, 'Could not save the template.');
      }
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: Text(_isEdit ? 'Edit template' : 'New template')),
      body: SafeArea(
        child: ListView(
          padding: const EdgeInsets.all(16),
          children: <Widget>[
            _label('Name'),
            TextField(
              controller: _name,
              decoration: const InputDecoration(hintText: 'e.g. HVAC Van Kit'),
            ),
            const SizedBox(height: 16),
            _label('Trade type (optional)'),
            TextField(
              controller: _tradeType,
              decoration: const InputDecoration(hintText: 'e.g. HVAC'),
            ),
            const SizedBox(height: 16),
            _label('Allowance / budget (optional)'),
            TextField(
              controller: _allowance,
              keyboardType:
                  const TextInputType.numberWithOptions(decimal: true),
              inputFormatters: <TextInputFormatter>[
                FilteringTextInputFormatter.allow(RegExp(r'^\d*\.?\d{0,2}')),
              ],
              decoration: const InputDecoration(
                prefixIcon: Icon(Icons.attach_money),
                hintText: 'e.g. 2100',
              ),
            ),
            const Padding(
              padding: EdgeInsets.only(top: 4),
              child: Text(
                'Spending cap for this template. Receipts reconcile against it.',
                style: TextStyle(fontSize: 12, color: AppTheme.slate400),
              ),
            ),
            const SizedBox(height: 20),
            Row(
              mainAxisAlignment: MainAxisAlignment.spaceBetween,
              children: <Widget>[
                _label('Items'),
                TextButton.icon(
                  onPressed: () => setState(() => _rows.add(_ItemRow())),
                  icon: const Icon(Icons.add, size: 18),
                  label: const Text('Add item'),
                ),
              ],
            ),
            const SizedBox(height: 4),
            ...List<Widget>.generate(_rows.length, (int i) => _itemCard(i)),
            const SizedBox(height: 24),
            PrimaryButton(
              label: _isEdit ? 'Save changes' : 'Create template',
              icon: Icons.save_outlined,
              loading: _saving,
              onPressed: _save,
            ),
            const SizedBox(height: 16),
          ],
        ),
      ),
    );
  }

  Widget _itemCard(int index) {
    final _ItemRow r = _rows[index];
    return Card(
      margin: const EdgeInsets.only(bottom: 10),
      child: Padding(
        padding: const EdgeInsets.all(12),
        child: Column(
          children: <Widget>[
            Row(
              children: <Widget>[
                Expanded(
                  child: TextField(
                    controller: r.name,
                    decoration:
                        const InputDecoration(labelText: 'Product name'),
                  ),
                ),
                if (_rows.length > 1)
                  IconButton(
                    icon: const Icon(Icons.delete_outline,
                        color: AppTheme.danger),
                    onPressed: () => setState(() {
                      _rows.removeAt(index).dispose();
                    }),
                  ),
              ],
            ),
            const SizedBox(height: 8),
            Row(
              children: <Widget>[
                Expanded(child: _numField(r.requiredQty, 'Required')),
                const SizedBox(width: 8),
                Expanded(child: _numField(r.minimum, 'Min')),
                const SizedBox(width: 8),
                Expanded(
                  child: TextField(
                    controller: r.unit,
                    decoration: const InputDecoration(labelText: 'Unit'),
                  ),
                ),
              ],
            ),
            const SizedBox(height: 8),
            TextField(
              controller: r.price,
              keyboardType:
                  const TextInputType.numberWithOptions(decimal: true),
              inputFormatters: <TextInputFormatter>[
                FilteringTextInputFormatter.allow(RegExp(r'^\d*\.?\d{0,2}')),
              ],
              decoration: const InputDecoration(
                labelText: 'Expected price (optional)',
                prefixIcon: Icon(Icons.attach_money),
              ),
            ),
          ],
        ),
      ),
    );
  }

  Widget _numField(TextEditingController c, String label) => TextField(
        controller: c,
        keyboardType: TextInputType.number,
        inputFormatters: <TextInputFormatter>[
          FilteringTextInputFormatter.digitsOnly,
        ],
        decoration: InputDecoration(labelText: label),
      );

  Widget _label(String text) => Padding(
        padding: const EdgeInsets.only(bottom: 6),
        child: Text(
          text,
          style: const TextStyle(
              fontWeight: FontWeight.w700, color: AppTheme.slate700),
        ),
      );
}
