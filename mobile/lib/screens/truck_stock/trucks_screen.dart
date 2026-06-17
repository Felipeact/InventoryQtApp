import 'package:flutter/material.dart';
import 'package:provider/provider.dart';

import '../../config/theme.dart';
import '../../models/truck.dart';
import '../../providers/truck_stock_provider.dart';
import '../../services/api_client.dart';
import '../../widgets/empty_state.dart';
import '../../widgets/error_view.dart';
import '../../widgets/loading_indicator.dart';
import '../../widgets/primary_button.dart';
import '../home_shell.dart';

/// Admin/manager view of all service trucks, with the ability to add one.
class TrucksScreen extends StatefulWidget {
  const TrucksScreen({super.key, this.embedded = false});

  final bool embedded;

  @override
  State<TrucksScreen> createState() => _TrucksScreenState();
}

class _TrucksScreenState extends State<TrucksScreen> {
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
      await context.read<TruckStockProvider>().loadTrucks();
    } on ApiException catch (e) {
      _error = e.message;
    } catch (_) {
      _error = 'Failed to load trucks.';
    } finally {
      if (mounted) setState(() => _loading = false);
    }
  }

  Future<void> _addTruck() async {
    final GlobalKey<FormState> formKey = GlobalKey<FormState>();
    final TextEditingController number = TextEditingController();
    final TextEditingController plate = TextEditingController();
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
                      'Add truck',
                      style: TextStyle(
                        fontSize: 18,
                        fontWeight: FontWeight.w800,
                      ),
                    ),
                    const SizedBox(height: 16),
                    TextFormField(
                      controller: number,
                      decoration: const InputDecoration(
                        labelText: 'Truck number *',
                        prefixIcon: Icon(Icons.tag),
                      ),
                      validator: (String? v) => (v == null || v.trim().isEmpty)
                          ? 'Truck number is required'
                          : null,
                    ),
                    const SizedBox(height: 12),
                    TextFormField(
                      controller: plate,
                      decoration: const InputDecoration(
                        labelText: 'Plate number',
                        prefixIcon: Icon(Icons.directions_car_outlined),
                      ),
                    ),
                    const SizedBox(height: 20),
                    PrimaryButton(
                      label: 'Create truck',
                      icon: Icons.check,
                      loading: saving,
                      onPressed: () async {
                        if (!formKey.currentState!.validate()) return;
                        setSheet(() => saving = true);
                        try {
                          await context
                              .read<TruckStockProvider>()
                              .createTruck(
                                truckNumber: number.text.trim(),
                                plateNumber: plate.text.trim().isEmpty
                                    ? null
                                    : plate.text.trim(),
                              );
                          if (ctx.mounted) Navigator.of(ctx).pop();
                          if (mounted) {
                            showSuccessSnack(context, 'Truck added.');
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
    final TruckStockProvider provider = context.watch<TruckStockProvider>();

    return Scaffold(
      appBar: buildAdaptiveAppBar(
        context,
        embedded: widget.embedded,
        title: 'Trucks',
      ),
      floatingActionButton: FloatingActionButton.extended(
        onPressed: _addTruck,
        icon: const Icon(Icons.add),
        label: const Text('Add truck'),
      ),
      body: _body(provider),
    );
  }

  Widget _body(TruckStockProvider provider) {
    if (_loading && provider.trucks.isEmpty) {
      return const LoadingIndicator(message: 'Loading trucks...');
    }
    if (_error != null && provider.trucks.isEmpty) {
      return ErrorView(message: _error!, onRetry: _load);
    }
    if (provider.trucks.isEmpty) {
      return RefreshIndicator(
        onRefresh: _load,
        child: ListView(
          children: <Widget>[
            SizedBox(height: MediaQuery.of(context).size.height * 0.18),
            EmptyState(
              icon: Icons.fire_truck_outlined,
              title: 'No trucks yet',
              message: 'Add a truck to start assigning stock.',
              actionLabel: 'Add truck',
              onAction: _addTruck,
            ),
          ],
        ),
      );
    }

    return RefreshIndicator(
      onRefresh: _load,
      child: ListView.separated(
        padding: const EdgeInsets.fromLTRB(16, 16, 16, 96),
        itemCount: provider.trucks.length,
        separatorBuilder: (_, __) => const SizedBox(height: 10),
        itemBuilder: (BuildContext context, int i) {
          final Truck truck = provider.trucks[i];
          final Color statusColor = StatusStyle.colorFor(truck.status);
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
                child:
                    const Icon(Icons.local_shipping, color: AppTheme.brand),
              ),
              title: Text(
                'Truck ${truck.truckNumber}',
                style: const TextStyle(
                  fontWeight: FontWeight.w700,
                  color: AppTheme.slate900,
                ),
              ),
              subtitle: Text(
                truck.plateNumber?.isNotEmpty == true
                    ? 'Plate ${truck.plateNumber}'
                    : 'No plate on file',
                style: const TextStyle(color: AppTheme.slate500),
              ),
              trailing: truck.status != null
                  ? Container(
                      padding: const EdgeInsets.symmetric(
                          horizontal: 10, vertical: 4),
                      decoration: BoxDecoration(
                        color: statusColor.withOpacity(0.12),
                        borderRadius: BorderRadius.circular(8),
                      ),
                      child: Text(
                        truck.status!.replaceAll('_', ' '),
                        style: TextStyle(
                          color: statusColor,
                          fontWeight: FontWeight.w700,
                          fontSize: 11,
                        ),
                      ),
                    )
                  : null,
            ),
          );
        },
      ),
    );
  }
}
