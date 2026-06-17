import 'package:flutter/material.dart';
import 'package:provider/provider.dart';

import '../../config/theme.dart';
import '../../models/asset.dart';
import '../../providers/auth_provider.dart';
import '../../services/api_client.dart';
import '../../services/asset_service.dart';
import '../../widgets/empty_state.dart';
import '../../widgets/error_view.dart';
import '../../widgets/loading_indicator.dart';
import '../../widgets/primary_button.dart';
import '../home_shell.dart';

/// Admin view of trackable assets, with create + delete. Uses [AssetService]
/// directly since assets are an admin-only feature without a dedicated
/// provider.
class AssetsScreen extends StatefulWidget {
  const AssetsScreen({super.key, this.embedded = false});

  final bool embedded;

  @override
  State<AssetsScreen> createState() => _AssetsScreenState();
}

class _AssetsScreenState extends State<AssetsScreen> {
  final AssetService _service = AssetService();
  List<Asset> _assets = <Asset>[];
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
      final List<Asset> assets = await _service.fetchAll();
      if (!mounted) return;
      setState(() {
        _assets = assets;
        _loading = false;
      });
    } on ApiException catch (e) {
      if (!mounted) return;
      setState(() {
        _error = e.message;
        _loading = false;
      });
    }
  }

  Future<void> _delete(Asset asset) async {
    final bool? ok = await showDialog<bool>(
      context: context,
      builder: (BuildContext ctx) => AlertDialog(
        title: const Text('Delete asset?'),
        content: Text('"${asset.name}" will be permanently removed.'),
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
      await _service.delete(asset.id);
      if (mounted) {
        setState(() => _assets =
            _assets.where((Asset a) => a.id != asset.id).toList());
        showSuccessSnack(context, 'Asset deleted.');
      }
    } on ApiException catch (e) {
      if (mounted) showErrorSnack(context, e.message);
    }
  }

  Future<void> _addAsset() async {
    final GlobalKey<FormState> formKey = GlobalKey<FormState>();
    final TextEditingController name = TextEditingController();
    final TextEditingController type = TextEditingController();
    final TextEditingController serial = TextEditingController();
    final TextEditingController description = TextEditingController();
    bool saving = false;

    await showModalBottomSheet<void>(
      context: context,
      isScrollControlled: true,
      backgroundColor: Colors.white,
      shape: const RoundedRectangleBorder(
        borderRadius: BorderRadius.vertical(top: Radius.circular(24)),
      ),
      builder: (BuildContext ctx) {
        return StatefulBuilder(
          builder: (BuildContext ctx, StateSetter setSheet) {
            return Padding(
              padding: EdgeInsets.only(
                left: 20,
                right: 20,
                top: 20,
                bottom: MediaQuery.of(ctx).viewInsets.bottom + 20,
              ),
              child: Form(
                key: formKey,
                child: Column(
                  mainAxisSize: MainAxisSize.min,
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: <Widget>[
                    const Text(
                      'Add asset',
                      style: TextStyle(
                          fontSize: 18, fontWeight: FontWeight.w800),
                    ),
                    const SizedBox(height: 16),
                    TextFormField(
                      controller: name,
                      decoration: const InputDecoration(
                        labelText: 'Name *',
                        prefixIcon: Icon(Icons.label_outline),
                      ),
                      validator: (String? v) =>
                          (v == null || v.trim().isEmpty)
                              ? 'Name is required'
                              : null,
                    ),
                    const SizedBox(height: 12),
                    TextFormField(
                      controller: type,
                      decoration: const InputDecoration(
                        labelText: 'Type *',
                        prefixIcon: Icon(Icons.category_outlined),
                      ),
                      validator: (String? v) =>
                          (v == null || v.trim().isEmpty)
                              ? 'Type is required'
                              : null,
                    ),
                    const SizedBox(height: 12),
                    TextFormField(
                      controller: serial,
                      decoration: const InputDecoration(
                        labelText: 'Serial code *',
                        prefixIcon: Icon(Icons.qr_code),
                      ),
                      validator: (String? v) =>
                          (v == null || v.trim().isEmpty)
                              ? 'Serial code is required'
                              : null,
                    ),
                    const SizedBox(height: 12),
                    TextFormField(
                      controller: description,
                      decoration: const InputDecoration(
                        labelText: 'Description',
                        prefixIcon: Icon(Icons.notes),
                      ),
                    ),
                    const SizedBox(height: 20),
                    PrimaryButton(
                      label: 'Create asset',
                      icon: Icons.check,
                      loading: saving,
                      onPressed: () async {
                        if (!formKey.currentState!.validate()) return;
                        setSheet(() => saving = true);
                        try {
                          final Asset created = await _service.create(
                            Asset(
                              id: '',
                              name: name.text.trim(),
                              type: type.text.trim(),
                              serialCode: serial.text.trim(),
                              description: description.text.trim().isEmpty
                                  ? null
                                  : description.text.trim(),
                            ),
                          );
                          if (ctx.mounted) Navigator.of(ctx).pop();
                          if (mounted) {
                            setState(() =>
                                _assets = <Asset>[created, ..._assets]);
                            showSuccessSnack(context, 'Asset created.');
                          }
                        } on ApiException catch (e) {
                          setSheet(() => saving = false);
                          if (ctx.mounted) {
                            ScaffoldMessenger.of(ctx).showSnackBar(
                              SnackBar(
                                content: Text(e.message),
                                backgroundColor: AppTheme.danger,
                              ),
                            );
                          }
                        }
                      },
                    ),
                    const SizedBox(height: 8),
                  ],
                ),
              ),
            );
          },
        );
      },
    );
  }

  @override
  Widget build(BuildContext context) {
    final AuthProvider auth = context.watch<AuthProvider>();
    final bool canManage = auth.isAdmin;

    return Scaffold(
      appBar: buildAdaptiveAppBar(
        context,
        embedded: widget.embedded,
        title: 'Assets',
      ),
      floatingActionButton: canManage
          ? FloatingActionButton.extended(
              onPressed: _addAsset,
              icon: const Icon(Icons.add),
              label: const Text('Add asset'),
            )
          : null,
      body: _body(canManage),
    );
  }

  Widget _body(bool canManage) {
    if (_loading && _assets.isEmpty) {
      return const LoadingIndicator(message: 'Loading assets...');
    }
    if (_error != null && _assets.isEmpty) {
      return ErrorView(message: _error!, onRetry: _load);
    }
    if (_assets.isEmpty) {
      return RefreshIndicator(
        onRefresh: _load,
        child: ListView(
          children: <Widget>[
            SizedBox(height: MediaQuery.of(context).size.height * 0.18),
            EmptyState(
              icon: Icons.devices_other_outlined,
              title: 'No assets',
              message: 'Track tools and equipment by adding an asset.',
              actionLabel: canManage ? 'Add asset' : null,
              onAction: canManage ? _addAsset : null,
            ),
          ],
        ),
      );
    }

    return RefreshIndicator(
      onRefresh: _load,
      child: ListView.separated(
        padding: const EdgeInsets.fromLTRB(16, 16, 16, 96),
        itemCount: _assets.length,
        separatorBuilder: (_, __) => const SizedBox(height: 10),
        itemBuilder: (BuildContext context, int i) {
          final Asset asset = _assets[i];
          final Color statusColor = StatusStyle.colorFor(asset.status);
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
                child: const Icon(Icons.devices_other, color: AppTheme.brand),
              ),
              title: Text(
                asset.name,
                style: const TextStyle(
                  fontWeight: FontWeight.w700,
                  color: AppTheme.slate900,
                ),
              ),
              subtitle: Text(
                <String>[
                  asset.type,
                  if (asset.serialCode.isNotEmpty) 'SN ${asset.serialCode}',
                ].join(' • '),
                style: const TextStyle(color: AppTheme.slate500),
              ),
              trailing: Row(
                mainAxisSize: MainAxisSize.min,
                children: <Widget>[
                  if (asset.status != null)
                    Container(
                      padding: const EdgeInsets.symmetric(
                          horizontal: 8, vertical: 3),
                      decoration: BoxDecoration(
                        color: statusColor.withOpacity(0.12),
                        borderRadius: BorderRadius.circular(6),
                      ),
                      child: Text(
                        asset.status!.replaceAll('_', ' '),
                        style: TextStyle(
                          fontSize: 10,
                          fontWeight: FontWeight.w800,
                          color: statusColor,
                        ),
                      ),
                    ),
                  if (canManage)
                    IconButton(
                      icon: const Icon(Icons.delete_outline,
                          color: AppTheme.danger),
                      onPressed: () => _delete(asset),
                    ),
                ],
              ),
            ),
          );
        },
      ),
    );
  }
}
