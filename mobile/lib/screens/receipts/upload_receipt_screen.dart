import 'dart:convert';
import 'dart:io';

import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:image_picker/image_picker.dart';
import 'package:provider/provider.dart';

import '../../config/theme.dart';
import '../../models/truck.dart';
import '../../providers/truck_stock_provider.dart';
import '../../services/api_client.dart';
import '../../widgets/primary_button.dart';
import '../home_shell.dart';

/// Captures a receipt photo (camera or gallery), uploads it as base64, then
/// creates a receipt record against the selected truck.
class UploadReceiptScreen extends StatefulWidget {
  const UploadReceiptScreen({super.key});

  @override
  State<UploadReceiptScreen> createState() => _UploadReceiptScreenState();
}

class _UploadReceiptScreenState extends State<UploadReceiptScreen> {
  final ImagePicker _picker = ImagePicker();
  final TextEditingController _amount = TextEditingController();

  XFile? _file;
  List<Truck> _trucks = <Truck>[];
  String? _selectedTruckId;
  bool _loadingTrucks = true;
  bool _submitting = false;
  String? _trucksError;

  @override
  void initState() {
    super.initState();
    _loadTrucks();
  }

  @override
  void dispose() {
    _amount.dispose();
    super.dispose();
  }

  Future<void> _loadTrucks() async {
    setState(() => _loadingTrucks = true);
    final TruckStockProvider provider = context.read<TruckStockProvider>();
    try {
      await provider.loadTrucks();
      _trucks = provider.trucks;
      if (_trucks.isNotEmpty) _selectedTruckId = _trucks.first.id;
      _trucksError = null;
    } on ApiException catch (e) {
      _trucksError = e.message;
    } catch (_) {
      _trucksError = 'Could not load trucks.';
    } finally {
      if (mounted) setState(() => _loadingTrucks = false);
    }
  }

  Future<void> _pick(ImageSource source) async {
    try {
      final XFile? picked = await _picker.pickImage(
        source: source,
        maxWidth: 1600,
        imageQuality: 80,
      );
      if (picked != null) {
        setState(() => _file = picked);
      }
    } catch (e) {
      if (mounted) {
        showErrorSnack(context, 'Could not access the ${source.name}.');
      }
    }
  }

  void _showPickerOptions() {
    showModalBottomSheet<void>(
      context: context,
      backgroundColor: Colors.white,
      shape: const RoundedRectangleBorder(
        borderRadius: BorderRadius.vertical(top: Radius.circular(24)),
      ),
      builder: (BuildContext ctx) => SafeArea(
        child: Column(
          mainAxisSize: MainAxisSize.min,
          children: <Widget>[
            const SizedBox(height: 8),
            ListTile(
              leading: const Icon(Icons.photo_camera_outlined),
              title: const Text('Take a photo'),
              onTap: () {
                Navigator.of(ctx).pop();
                _pick(ImageSource.camera);
              },
            ),
            ListTile(
              leading: const Icon(Icons.photo_library_outlined),
              title: const Text('Choose from gallery'),
              onTap: () {
                Navigator.of(ctx).pop();
                _pick(ImageSource.gallery);
              },
            ),
            const SizedBox(height: 8),
          ],
        ),
      ),
    );
  }

  Future<void> _submit() async {
    if (_file == null) {
      showErrorSnack(context, 'Please add a receipt photo first.');
      return;
    }
    if (_selectedTruckId == null) {
      showErrorSnack(context, 'Please select a truck.');
      return;
    }
    setState(() => _submitting = true);

    final TruckStockProvider provider = context.read<TruckStockProvider>();
    try {
      final List<int> bytes = await File(_file!.path).readAsBytes();
      final String base64Str = base64Encode(bytes);
      final String fileName = _file!.name.isNotEmpty
          ? _file!.name
          : 'receipt_${DateTime.now().millisecondsSinceEpoch}.jpg';

      final String fileUrl = await provider.uploadReceiptFile(
        fileBase64: base64Str,
        fileName: fileName,
      );

      final double? amount = _amount.text.trim().isEmpty
          ? null
          : double.tryParse(_amount.text.trim());

      await provider.createReceipt(
        truckId: _selectedTruckId!,
        fileUrl: fileUrl,
        totalAmount: amount,
      );

      if (!mounted) return;
      showSuccessSnack(context, 'Receipt uploaded successfully.');
      Navigator.of(context).pop(true);
    } on ApiException catch (e) {
      if (mounted) {
        setState(() => _submitting = false);
        showErrorSnack(context, e.message);
      }
    } catch (_) {
      if (mounted) {
        setState(() => _submitting = false);
        showErrorSnack(context, 'Could not upload the receipt.');
      }
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('Upload receipt')),
      body: SafeArea(
        child: ListView(
          padding: const EdgeInsets.all(16),
          children: <Widget>[
            _ImagePreview(file: _file, onTap: _showPickerOptions),
            const SizedBox(height: 20),
            const Text(
              'Truck',
              style: TextStyle(
                fontWeight: FontWeight.w700,
                color: AppTheme.slate700,
              ),
            ),
            const SizedBox(height: 8),
            _truckSelector(),
            const SizedBox(height: 20),
            const Text(
              'Total amount (optional)',
              style: TextStyle(
                fontWeight: FontWeight.w700,
                color: AppTheme.slate700,
              ),
            ),
            const SizedBox(height: 8),
            TextField(
              controller: _amount,
              keyboardType:
                  const TextInputType.numberWithOptions(decimal: true),
              inputFormatters: <TextInputFormatter>[
                FilteringTextInputFormatter.allow(RegExp(r'^\d*\.?\d{0,2}')),
              ],
              decoration: const InputDecoration(
                prefixIcon: Icon(Icons.attach_money),
                hintText: '0.00',
              ),
            ),
            const SizedBox(height: 28),
            PrimaryButton(
              label: 'Upload receipt',
              icon: Icons.cloud_upload_outlined,
              loading: _submitting,
              onPressed: _submit,
            ),
            const SizedBox(height: 16),
          ],
        ),
      ),
    );
  }

  Widget _truckSelector() {
    if (_loadingTrucks) {
      return const Padding(
        padding: EdgeInsets.symmetric(vertical: 16),
        child: Center(
          child: SizedBox(
            height: 22,
            width: 22,
            child: CircularProgressIndicator(strokeWidth: 2.5),
          ),
        ),
      );
    }
    if (_trucksError != null && _trucks.isEmpty) {
      return Container(
        padding: const EdgeInsets.all(14),
        decoration: BoxDecoration(
          color: AppTheme.danger.withValues(alpha: 0.08),
          borderRadius: BorderRadius.circular(12),
        ),
        child: Row(
          children: <Widget>[
            const Icon(Icons.error_outline, color: AppTheme.danger, size: 20),
            const SizedBox(width: 10),
            Expanded(
              child: Text(
                _trucksError!,
                style: const TextStyle(color: AppTheme.slate700),
              ),
            ),
            TextButton(onPressed: _loadTrucks, child: const Text('Retry')),
          ],
        ),
      );
    }
    if (_trucks.isEmpty) {
      return Container(
        padding: const EdgeInsets.all(14),
        decoration: BoxDecoration(
          color: AppTheme.slate100,
          borderRadius: BorderRadius.circular(12),
        ),
        child: const Text(
          'No trucks available. A truck must exist before uploading a receipt.',
          style: TextStyle(color: AppTheme.slate500),
        ),
      );
    }

    return DropdownButtonFormField<String>(
      initialValue: _selectedTruckId,
      decoration: const InputDecoration(
        prefixIcon: Icon(Icons.local_shipping_outlined),
      ),
      items: _trucks
          .map(
            (Truck t) => DropdownMenuItem<String>(
              value: t.id,
              child: Text(
                'Truck ${t.truckNumber}'
                '${t.plateNumber != null ? ' (${t.plateNumber})' : ''}',
              ),
            ),
          )
          .toList(),
      onChanged: (String? v) => setState(() => _selectedTruckId = v),
    );
  }
}

class _ImagePreview extends StatelessWidget {
  const _ImagePreview({required this.file, required this.onTap});

  final XFile? file;
  final VoidCallback onTap;

  @override
  Widget build(BuildContext context) {
    return GestureDetector(
      onTap: onTap,
      child: Container(
        height: 220,
        width: double.infinity,
        decoration: BoxDecoration(
          color: AppTheme.slate100,
          borderRadius: BorderRadius.circular(16),
          border: Border.all(
            color: AppTheme.slate200,
            style: BorderStyle.solid,
          ),
        ),
        clipBehavior: Clip.antiAlias,
        child: file == null
            ? Column(
                mainAxisAlignment: MainAxisAlignment.center,
                children: <Widget>[
                  Container(
                    padding: const EdgeInsets.all(16),
                    decoration: BoxDecoration(
                      color: AppTheme.brand.withValues(alpha: 0.1),
                      shape: BoxShape.circle,
                    ),
                    child: const Icon(
                      Icons.add_a_photo_outlined,
                      size: 32,
                      color: AppTheme.brand,
                    ),
                  ),
                  const SizedBox(height: 12),
                  const Text(
                    'Tap to add receipt photo',
                    style: TextStyle(
                      color: AppTheme.slate700,
                      fontWeight: FontWeight.w600,
                    ),
                  ),
                  const SizedBox(height: 4),
                  const Text(
                    'Camera or gallery',
                    style: TextStyle(color: AppTheme.slate400, fontSize: 12),
                  ),
                ],
              )
            : Stack(
                fit: StackFit.expand,
                children: <Widget>[
                  Image.file(File(file!.path), fit: BoxFit.cover),
                  Positioned(
                    right: 8,
                    top: 8,
                    child: Container(
                      decoration: const BoxDecoration(
                        color: Colors.black54,
                        shape: BoxShape.circle,
                      ),
                      child: IconButton(
                        icon: const Icon(Icons.edit, color: Colors.white),
                        onPressed: onTap,
                        tooltip: 'Change photo',
                      ),
                    ),
                  ),
                ],
              ),
      ),
    );
  }
}
