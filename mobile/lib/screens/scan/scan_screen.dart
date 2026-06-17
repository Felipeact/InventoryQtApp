import 'package:flutter/material.dart';
import 'package:mobile_scanner/mobile_scanner.dart';
import 'package:provider/provider.dart';

import '../../config/theme.dart';
import '../../models/product.dart';
import '../../providers/auth_provider.dart';
import '../../providers/products_provider.dart';
import '../../services/api_client.dart';
import '../../widgets/primary_button.dart';
import '../home_shell.dart';

enum ScanMode { scanIn, scanOut }

/// Camera barcode scanner with a Scan-In / Scan-Out toggle. After a barcode is
/// captured the user confirms a quantity and the corresponding endpoint runs.
class ScanScreen extends StatefulWidget {
  const ScanScreen({super.key, this.embedded = false});

  final bool embedded;

  @override
  State<ScanScreen> createState() => _ScanScreenState();
}

class _ScanScreenState extends State<ScanScreen> {
  final MobileScannerController _controller = MobileScannerController(
    detectionSpeed: DetectionSpeed.noDuplicates,
    facing: CameraFacing.back,
  );

  ScanMode _mode = ScanMode.scanIn;
  bool _processing = false;
  bool _torchOn = false;
  String? _lastBarcode;

  @override
  void dispose() {
    _controller.dispose();
    super.dispose();
  }

  Future<void> _onDetect(BarcodeCapture capture) async {
    if (_processing) return;
    final List<Barcode> codes = capture.barcodes;
    if (codes.isEmpty) return;
    final String? raw = codes.first.rawValue;
    if (raw == null || raw.isEmpty) return;
    if (raw == _lastBarcode) return;

    setState(() {
      _processing = true;
      _lastBarcode = raw;
    });
    await _controller.stop();
    await _handleBarcode(raw);
  }

  Future<void> _handleBarcode(String barcode) async {
    final ProductsProvider provider = context.read<ProductsProvider>();
    Product? existing;
    try {
      existing = await provider.findByBarcode(barcode);
    } on ApiException {
      existing = null;
    }

    if (!mounted) return;
    final int? qty = await _askQuantity(barcode, existing);
    if (qty == null) {
      // Cancelled — resume scanning.
      await _resume();
      return;
    }

    try {
      final Product? updated = _mode == ScanMode.scanIn
          ? await provider.scanIn(barcode, qty)
          : await provider.scanOut(barcode, qty);
      if (!mounted) return;
      final String verb = _mode == ScanMode.scanIn ? 'in' : 'out';
      final String newQty =
          updated != null ? ' (now ${updated.quantity} in stock)' : '';
      showSuccessSnack(
        context,
        'Scanned $verb $qty × ${existing?.name ?? barcode}$newQty',
      );
    } on ApiException catch (e) {
      if (mounted) showErrorSnack(context, e.message);
    } finally {
      await _resume();
    }
  }

  Future<void> _resume() async {
    if (!mounted) return;
    setState(() {
      _processing = false;
      _lastBarcode = null;
    });
    try {
      await _controller.start();
    } catch (_) {
      // Controller may already be starting; ignore.
    }
  }

  Future<int?> _askQuantity(String barcode, Product? product) {
    final TextEditingController qtyController =
        TextEditingController(text: '1');
    final bool scanIn = _mode == ScanMode.scanIn;
    return showModalBottomSheet<int>(
      context: context,
      isScrollControlled: true,
      backgroundColor: Colors.white,
      shape: const RoundedRectangleBorder(
        borderRadius: BorderRadius.vertical(top: Radius.circular(24)),
      ),
      builder: (BuildContext ctx) {
        return Padding(
          padding: EdgeInsets.only(
            left: 20,
            right: 20,
            top: 20,
            bottom: MediaQuery.of(ctx).viewInsets.bottom + 20,
          ),
          child: Column(
            mainAxisSize: MainAxisSize.min,
            crossAxisAlignment: CrossAxisAlignment.start,
            children: <Widget>[
              Row(
                children: <Widget>[
                  Icon(
                    scanIn ? Icons.add_box : Icons.indeterminate_check_box,
                    color: scanIn ? AppTheme.success : AppTheme.danger,
                  ),
                  const SizedBox(width: 10),
                  Text(
                    scanIn ? 'Scan In' : 'Scan Out',
                    style: const TextStyle(
                      fontSize: 18,
                      fontWeight: FontWeight.w800,
                    ),
                  ),
                ],
              ),
              const SizedBox(height: 12),
              Container(
                padding: const EdgeInsets.all(12),
                decoration: BoxDecoration(
                  color: AppTheme.slate100,
                  borderRadius: BorderRadius.circular(12),
                ),
                child: Row(
                  children: <Widget>[
                    const Icon(Icons.qr_code, color: AppTheme.slate500),
                    const SizedBox(width: 10),
                    Expanded(
                      child: Column(
                        crossAxisAlignment: CrossAxisAlignment.start,
                        children: <Widget>[
                          Text(
                            product?.name ?? 'Unknown product',
                            style: const TextStyle(
                              fontWeight: FontWeight.w700,
                              color: AppTheme.slate900,
                            ),
                          ),
                          Text(
                            barcode,
                            style: const TextStyle(
                              fontSize: 12,
                              color: AppTheme.slate500,
                            ),
                          ),
                          if (product != null)
                            Text(
                              'Current: ${product.quantity} in stock',
                              style: const TextStyle(
                                fontSize: 12,
                                color: AppTheme.slate500,
                              ),
                            ),
                        ],
                      ),
                    ),
                  ],
                ),
              ),
              const SizedBox(height: 16),
              TextField(
                controller: qtyController,
                keyboardType: TextInputType.number,
                autofocus: true,
                decoration: const InputDecoration(
                  labelText: 'Quantity',
                  prefixIcon: Icon(Icons.numbers),
                ),
              ),
              const SizedBox(height: 16),
              Row(
                children: <Widget>[
                  Expanded(
                    child: OutlinedButton(
                      onPressed: () => Navigator.of(ctx).pop(),
                      child: const Text('Cancel'),
                    ),
                  ),
                  const SizedBox(width: 12),
                  Expanded(
                    child: ElevatedButton(
                      style: ElevatedButton.styleFrom(
                        backgroundColor:
                            scanIn ? AppTheme.success : AppTheme.danger,
                      ),
                      onPressed: () {
                        final int q =
                            int.tryParse(qtyController.text.trim()) ?? 0;
                        if (q <= 0) {
                          ScaffoldMessenger.of(ctx).showSnackBar(
                            const SnackBar(
                              content: Text('Enter a quantity greater than 0'),
                            ),
                          );
                          return;
                        }
                        Navigator.of(ctx).pop(q);
                      },
                      child: Text(scanIn ? 'Add stock' : 'Remove stock'),
                    ),
                  ),
                ],
              ),
            ],
          ),
        );
      },
    );
  }

  Future<void> _manualEntry() async {
    final TextEditingController codeController = TextEditingController();
    final String? code = await showDialog<String>(
      context: context,
      builder: (BuildContext ctx) => AlertDialog(
        title: const Text('Enter barcode manually'),
        content: TextField(
          controller: codeController,
          autofocus: true,
          decoration: const InputDecoration(
            labelText: 'Barcode',
            prefixIcon: Icon(Icons.qr_code),
          ),
        ),
        actions: <Widget>[
          TextButton(
            onPressed: () => Navigator.of(ctx).pop(),
            child: const Text('Cancel'),
          ),
          FilledButton(
            onPressed: () =>
                Navigator.of(ctx).pop(codeController.text.trim()),
            child: const Text('Continue'),
          ),
        ],
      ),
    );
    if (code != null && code.isNotEmpty) {
      setState(() => _processing = true);
      await _controller.stop();
      await _handleBarcode(code);
    }
  }

  @override
  Widget build(BuildContext context) {
    final AuthProvider auth = context.watch<AuthProvider>();
    final bool canIn = auth.can(Permissions.scanIn);
    final bool canOut = auth.can(Permissions.scanOut);

    // Ensure the selected mode is one the user is permitted to use.
    if (_mode == ScanMode.scanIn && !canIn && canOut) {
      _mode = ScanMode.scanOut;
    } else if (_mode == ScanMode.scanOut && !canOut && canIn) {
      _mode = ScanMode.scanIn;
    }

    return Scaffold(
      appBar: buildAdaptiveAppBar(
        context,
        embedded: widget.embedded,
        title: 'Scan',
        actions: <Widget>[
          IconButton(
            tooltip: 'Toggle torch',
            icon: Icon(_torchOn ? Icons.flash_on : Icons.flash_off),
            onPressed: () async {
              await _controller.toggleTorch();
              setState(() => _torchOn = !_torchOn);
            },
          ),
          IconButton(
            tooltip: 'Switch camera',
            icon: const Icon(Icons.cameraswitch_outlined),
            onPressed: () => _controller.switchCamera(),
          ),
        ],
      ),
      body: Column(
        children: <Widget>[
          Padding(
            padding: const EdgeInsets.all(16),
            child: _ModeToggle(
              mode: _mode,
              canIn: canIn,
              canOut: canOut,
              onChanged: (ScanMode m) => setState(() => _mode = m),
            ),
          ),
          Expanded(
            child: Padding(
              padding: const EdgeInsets.symmetric(horizontal: 16),
              child: ClipRRect(
                borderRadius: BorderRadius.circular(20),
                child: Stack(
                  fit: StackFit.expand,
                  children: <Widget>[
                    MobileScanner(
                      controller: _controller,
                      onDetect: _onDetect,
                      errorBuilder: (BuildContext context,
                          MobileScannerException error, Widget? child) {
                        return _ScannerError(error: error);
                      },
                    ),
                    _ScannerOverlay(
                      color: _mode == ScanMode.scanIn
                          ? AppTheme.success
                          : AppTheme.danger,
                    ),
                    if (_processing)
                      Container(
                        color: Colors.black54,
                        child: const Center(
                          child: CircularProgressIndicator(
                            valueColor:
                                AlwaysStoppedAnimation<Color>(Colors.white),
                          ),
                        ),
                      ),
                  ],
                ),
              ),
            ),
          ),
          Padding(
            padding: const EdgeInsets.all(16),
            child: Column(
              children: <Widget>[
                Text(
                  'Point the camera at a barcode to '
                  '${_mode == ScanMode.scanIn ? 'add' : 'remove'} stock.',
                  textAlign: TextAlign.center,
                  style: const TextStyle(color: AppTheme.slate500),
                ),
                const SizedBox(height: 12),
                PrimaryButton(
                  label: 'Enter barcode manually',
                  icon: Icons.keyboard,
                  onPressed: _processing ? null : _manualEntry,
                ),
              ],
            ),
          ),
        ],
      ),
    );
  }
}

class _ModeToggle extends StatelessWidget {
  const _ModeToggle({
    required this.mode,
    required this.canIn,
    required this.canOut,
    required this.onChanged,
  });

  final ScanMode mode;
  final bool canIn;
  final bool canOut;
  final ValueChanged<ScanMode> onChanged;

  @override
  Widget build(BuildContext context) {
    return Container(
      padding: const EdgeInsets.all(4),
      decoration: BoxDecoration(
        color: AppTheme.slate100,
        borderRadius: BorderRadius.circular(14),
      ),
      child: Row(
        children: <Widget>[
          Expanded(
            child: _segment(
              label: 'Scan In',
              icon: Icons.add,
              selected: mode == ScanMode.scanIn,
              color: AppTheme.success,
              enabled: canIn,
              onTap: () => onChanged(ScanMode.scanIn),
            ),
          ),
          Expanded(
            child: _segment(
              label: 'Scan Out',
              icon: Icons.remove,
              selected: mode == ScanMode.scanOut,
              color: AppTheme.danger,
              enabled: canOut,
              onTap: () => onChanged(ScanMode.scanOut),
            ),
          ),
        ],
      ),
    );
  }

  Widget _segment({
    required String label,
    required IconData icon,
    required bool selected,
    required Color color,
    required bool enabled,
    required VoidCallback onTap,
  }) {
    return Opacity(
      opacity: enabled ? 1 : 0.4,
      child: GestureDetector(
        onTap: enabled ? onTap : null,
        child: AnimatedContainer(
          duration: const Duration(milliseconds: 180),
          padding: const EdgeInsets.symmetric(vertical: 12),
          decoration: BoxDecoration(
            color: selected ? color : Colors.transparent,
            borderRadius: BorderRadius.circular(10),
          ),
          child: Row(
            mainAxisAlignment: MainAxisAlignment.center,
            children: <Widget>[
              Icon(
                icon,
                size: 18,
                color: selected ? Colors.white : AppTheme.slate500,
              ),
              const SizedBox(width: 6),
              Text(
                label,
                style: TextStyle(
                  fontWeight: FontWeight.w700,
                  color: selected ? Colors.white : AppTheme.slate500,
                ),
              ),
            ],
          ),
        ),
      ),
    );
  }
}

class _ScannerOverlay extends StatelessWidget {
  const _ScannerOverlay({required this.color});

  final Color color;

  @override
  Widget build(BuildContext context) {
    return Center(
      child: Container(
        width: 240,
        height: 240,
        decoration: BoxDecoration(
          border: Border.all(color: color, width: 3),
          borderRadius: BorderRadius.circular(20),
        ),
      ),
    );
  }
}

class _ScannerError extends StatelessWidget {
  const _ScannerError({required this.error});

  final MobileScannerException error;

  @override
  Widget build(BuildContext context) {
    return Container(
      color: AppTheme.slate900,
      child: Center(
        child: Padding(
          padding: const EdgeInsets.all(24),
          child: Column(
            mainAxisSize: MainAxisSize.min,
            children: <Widget>[
              const Icon(Icons.no_photography_outlined,
                  color: Colors.white70, size: 48),
              const SizedBox(height: 16),
              const Text(
                'Camera unavailable',
                style: TextStyle(
                  color: Colors.white,
                  fontWeight: FontWeight.w700,
                  fontSize: 16,
                ),
              ),
              const SizedBox(height: 8),
              Text(
                'Grant camera permission to scan barcodes, or enter the code '
                'manually below.',
                textAlign: TextAlign.center,
                style: TextStyle(color: Colors.white.withValues(alpha: 0.7)),
              ),
            ],
          ),
        ),
      ),
    );
  }
}
