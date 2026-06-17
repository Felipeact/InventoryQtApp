import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:provider/provider.dart';

import '../../models/product.dart';
import '../../providers/products_provider.dart';
import '../../services/api_client.dart';
import '../../widgets/primary_button.dart';
import '../home_shell.dart';

/// Create or edit a product. Pass an existing [product] to edit, otherwise the
/// form creates a new one.
class AddEditProductScreen extends StatefulWidget {
  const AddEditProductScreen({super.key, this.product});

  final Product? product;

  bool get isEditing => product != null;

  @override
  State<AddEditProductScreen> createState() => _AddEditProductScreenState();
}

class _AddEditProductScreenState extends State<AddEditProductScreen> {
  final GlobalKey<FormState> _formKey = GlobalKey<FormState>();

  late final TextEditingController _name;
  late final TextEditingController _barcode;
  late final TextEditingController _quantity;
  late final TextEditingController _threshold;
  late final TextEditingController _model;
  late final TextEditingController _type;
  late final TextEditingController _location;
  late final TextEditingController _project;
  late final TextEditingController _account;
  late final TextEditingController _description;

  bool _saving = false;

  @override
  void initState() {
    super.initState();
    final Product? p = widget.product;
    _name = TextEditingController(text: p?.name ?? '');
    _barcode = TextEditingController(text: p?.barcode ?? '');
    _quantity = TextEditingController(text: (p?.quantity ?? 0).toString());
    _threshold =
        TextEditingController(text: (p?.lowStockThreshold ?? 0).toString());
    _model = TextEditingController(text: p?.model ?? '');
    _type = TextEditingController(text: p?.type ?? '');
    _location = TextEditingController(text: p?.location ?? '');
    _project = TextEditingController(text: p?.project ?? '');
    _account = TextEditingController(text: p?.account ?? '');
    _description = TextEditingController(text: p?.description ?? '');
  }

  @override
  void dispose() {
    _name.dispose();
    _barcode.dispose();
    _quantity.dispose();
    _threshold.dispose();
    _model.dispose();
    _type.dispose();
    _location.dispose();
    _project.dispose();
    _account.dispose();
    _description.dispose();
    super.dispose();
  }

  String? _trim(String s) => s.trim().isEmpty ? null : s.trim();

  Future<void> _save() async {
    if (!_formKey.currentState!.validate()) return;
    FocusScope.of(context).unfocus();
    setState(() => _saving = true);

    final Product payload = Product(
      id: widget.product?.id ?? '',
      name: _name.text.trim(),
      barcode: _barcode.text.trim(),
      quantity: int.tryParse(_quantity.text.trim()) ?? 0,
      lowStockThreshold: int.tryParse(_threshold.text.trim()) ?? 0,
      model: _trim(_model.text),
      type: _trim(_type.text),
      location: _trim(_location.text),
      project: _trim(_project.text),
      account: _trim(_account.text),
      description: _trim(_description.text),
    );

    try {
      final ProductsProvider provider = context.read<ProductsProvider>();
      if (widget.isEditing) {
        await provider.update(widget.product!.id, payload);
      } else {
        await provider.create(payload);
      }
      if (!mounted) return;
      showSuccessSnack(
        context,
        widget.isEditing ? 'Product updated.' : 'Product created.',
      );
      Navigator.of(context).pop(true);
    } on ApiException catch (e) {
      if (mounted) {
        setState(() => _saving = false);
        showErrorSnack(context, e.message);
      }
    } catch (_) {
      if (mounted) {
        setState(() => _saving = false);
        showErrorSnack(context, 'Could not save the product.');
      }
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text(widget.isEditing ? 'Edit product' : 'Add product'),
      ),
      body: SafeArea(
        child: Form(
          key: _formKey,
          child: ListView(
            padding: const EdgeInsets.all(16),
            children: <Widget>[
              _field(
                controller: _name,
                label: 'Name *',
                icon: Icons.label_outline,
                validator: (String? v) => (v == null || v.trim().isEmpty)
                    ? 'Name is required'
                    : null,
              ),
              _field(
                controller: _barcode,
                label: 'Barcode *',
                icon: Icons.qr_code,
                validator: (String? v) => (v == null || v.trim().isEmpty)
                    ? 'Barcode is required'
                    : null,
              ),
              Row(
                children: <Widget>[
                  Expanded(
                    child: _field(
                      controller: _quantity,
                      label: 'Quantity',
                      icon: Icons.numbers,
                      keyboardType: TextInputType.number,
                      inputFormatters: <TextInputFormatter>[
                        FilteringTextInputFormatter.digitsOnly,
                      ],
                    ),
                  ),
                  const SizedBox(width: 12),
                  Expanded(
                    child: _field(
                      controller: _threshold,
                      label: 'Low threshold',
                      icon: Icons.trending_down,
                      keyboardType: TextInputType.number,
                      inputFormatters: <TextInputFormatter>[
                        FilteringTextInputFormatter.digitsOnly,
                      ],
                    ),
                  ),
                ],
              ),
              _field(
                controller: _model,
                label: 'Model',
                icon: Icons.category_outlined,
              ),
              _field(
                controller: _type,
                label: 'Type',
                icon: Icons.style_outlined,
              ),
              _field(
                controller: _location,
                label: 'Location',
                icon: Icons.place_outlined,
              ),
              _field(
                controller: _project,
                label: 'Project',
                icon: Icons.work_outline,
              ),
              _field(
                controller: _account,
                label: 'Account',
                icon: Icons.account_balance_outlined,
              ),
              _field(
                controller: _description,
                label: 'Description',
                icon: Icons.notes,
                maxLines: 3,
              ),
              const SizedBox(height: 12),
              PrimaryButton(
                label: widget.isEditing ? 'Save changes' : 'Create product',
                loading: _saving,
                icon: Icons.check,
                onPressed: _save,
              ),
              const SizedBox(height: 24),
            ],
          ),
        ),
      ),
    );
  }

  Widget _field({
    required TextEditingController controller,
    required String label,
    required IconData icon,
    String? Function(String?)? validator,
    TextInputType? keyboardType,
    List<TextInputFormatter>? inputFormatters,
    int maxLines = 1,
  }) {
    return Padding(
      padding: const EdgeInsets.only(bottom: 14),
      child: TextFormField(
        controller: controller,
        validator: validator,
        keyboardType: keyboardType,
        inputFormatters: inputFormatters,
        maxLines: maxLines,
        decoration: InputDecoration(
          labelText: label,
          prefixIcon: Icon(icon),
          alignLabelWithHint: maxLines > 1,
        ),
      ),
    );
  }
}
