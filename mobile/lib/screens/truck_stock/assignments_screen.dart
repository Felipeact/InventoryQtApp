import 'package:flutter/material.dart';

import '../../config/theme.dart';
import '../../models/truck.dart';
import '../../models/truck_stock_assignment.dart';
import '../../models/truck_stock_template.dart';
import '../../services/api_client.dart';
import '../../services/truck_stock_service.dart';
import '../../widgets/empty_state.dart';
import '../../widgets/error_view.dart';
import '../../widgets/loading_indicator.dart';
import '../home_shell.dart';

/// Lists template→truck assignments and lets managers create or remove them.
/// Requires MANAGE_TRUCK_STOCK server-side.
class AssignmentsScreen extends StatefulWidget {
  const AssignmentsScreen({super.key, this.embedded = false});

  final bool embedded;

  @override
  State<AssignmentsScreen> createState() => _AssignmentsScreenState();
}

class _AssignmentsScreenState extends State<AssignmentsScreen> {
  final TruckStockService _service = TruckStockService();
  List<TruckStockAssignment> _assignments = <TruckStockAssignment>[];
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
      _assignments = await _service.fetchAssignments();
    } on ApiException catch (e) {
      _error = e.message;
    } catch (_) {
      _error = 'Failed to load assignments.';
    } finally {
      if (mounted) setState(() => _loading = false);
    }
  }

  Future<void> _create() async {
    List<Truck> trucks;
    List<TruckStockTemplate> templates;
    try {
      trucks = await _service.fetchTrucks();
      templates = await _service.fetchTemplates();
    } on ApiException catch (e) {
      if (mounted) showErrorSnack(context, e.message);
      return;
    }
    if (!mounted) return;
    if (trucks.isEmpty || templates.isEmpty) {
      showErrorSnack(
        context,
        'You need at least one truck and one template before assigning.',
      );
      return;
    }
    final _AssignResult? result = await showDialog<_AssignResult>(
      context: context,
      builder: (_) => _AssignDialog(trucks: trucks, templates: templates),
    );
    if (result == null) return;
    try {
      await _service.createAssignment(
        truckId: result.truckId,
        templateId: result.templateId,
      );
      if (!mounted) return;
      showSuccessSnack(context, 'Template assigned.');
      _load();
    } on ApiException catch (e) {
      if (mounted) showErrorSnack(context, e.message);
    }
  }

  Future<void> _delete(TruckStockAssignment a) async {
    final bool? ok = await showDialog<bool>(
      context: context,
      builder: (BuildContext ctx) => AlertDialog(
        title: const Text('Remove assignment?'),
        content: Text(
          'Unassign ${a.templateName ?? 'template'} from '
          '${a.truckNumber != null ? 'Truck ${a.truckNumber}' : 'this truck'}?',
        ),
        actions: <Widget>[
          TextButton(
              onPressed: () => Navigator.of(ctx).pop(false),
              child: const Text('Cancel')),
          FilledButton(
            style: FilledButton.styleFrom(backgroundColor: AppTheme.danger),
            onPressed: () => Navigator.of(ctx).pop(true),
            child: const Text('Remove'),
          ),
        ],
      ),
    );
    if (ok != true) return;
    try {
      await _service.deleteAssignment(a.id);
      if (!mounted) return;
      showSuccessSnack(context, 'Assignment removed.');
      _load();
    } on ApiException catch (e) {
      if (mounted) showErrorSnack(context, e.message);
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: buildAdaptiveAppBar(
        context,
        embedded: widget.embedded,
        title: 'Assignments',
      ),
      floatingActionButton: FloatingActionButton.extended(
        onPressed: _create,
        icon: const Icon(Icons.add_link),
        label: const Text('Assign'),
      ),
      body: _body(),
    );
  }

  Widget _body() {
    if (_loading && _assignments.isEmpty) {
      return const LoadingIndicator(message: 'Loading assignments...');
    }
    if (_error != null && _assignments.isEmpty) {
      return ErrorView(message: _error!, onRetry: _load);
    }
    if (_assignments.isEmpty) {
      return RefreshIndicator(
        onRefresh: _load,
        child: ListView(
          children: <Widget>[
            SizedBox(height: MediaQuery.of(context).size.height * 0.18),
            EmptyState(
              icon: Icons.link_off_outlined,
              title: 'No assignments',
              message: 'Assign a stock template to a truck to get started.',
              actionLabel: 'Assign',
              onAction: _create,
            ),
          ],
        ),
      );
    }
    return RefreshIndicator(
      onRefresh: _load,
      child: ListView.separated(
        padding: const EdgeInsets.fromLTRB(16, 16, 16, 96),
        itemCount: _assignments.length,
        separatorBuilder: (_, __) => const SizedBox(height: 10),
        itemBuilder: (BuildContext context, int i) {
          final TruckStockAssignment a = _assignments[i];
          return Card(
            child: ListTile(
              contentPadding:
                  const EdgeInsets.symmetric(horizontal: 16, vertical: 6),
              leading: Container(
                width: 44,
                height: 44,
                decoration: BoxDecoration(
                  color: AppTheme.brand.withValues(alpha: 0.1),
                  borderRadius: BorderRadius.circular(12),
                ),
                child: const Icon(Icons.link, color: AppTheme.brand),
              ),
              title: Text(
                a.templateName ?? 'Template',
                style: const TextStyle(
                    fontWeight: FontWeight.w700, color: AppTheme.slate900),
              ),
              subtitle: Text(
                <String>[
                  if (a.truckNumber != null) 'Truck ${a.truckNumber}',
                  if (a.technicianName != null) a.technicianName!,
                ].join(' • '),
                style: const TextStyle(color: AppTheme.slate500),
              ),
              trailing: IconButton(
                icon: const Icon(Icons.delete_outline, color: AppTheme.danger),
                onPressed: () => _delete(a),
              ),
            ),
          );
        },
      ),
    );
  }
}

class _AssignResult {
  const _AssignResult({required this.truckId, required this.templateId});
  final String truckId;
  final String templateId;
}

class _AssignDialog extends StatefulWidget {
  const _AssignDialog({required this.trucks, required this.templates});
  final List<Truck> trucks;
  final List<TruckStockTemplate> templates;

  @override
  State<_AssignDialog> createState() => _AssignDialogState();
}

class _AssignDialogState extends State<_AssignDialog> {
  String? _truckId;
  String? _templateId;

  @override
  void initState() {
    super.initState();
    _truckId = widget.trucks.first.id;
    _templateId = widget.templates.first.id;
  }

  @override
  Widget build(BuildContext context) {
    return AlertDialog(
      title: const Text('Assign template'),
      content: Column(
        mainAxisSize: MainAxisSize.min,
        children: <Widget>[
          DropdownButtonFormField<String>(
            initialValue: _truckId,
            decoration: const InputDecoration(labelText: 'Truck'),
            items: widget.trucks
                .map((Truck t) => DropdownMenuItem<String>(
                      value: t.id,
                      child: Text(
                        'Truck ${t.truckNumber}'
                        '${t.plateNumber != null ? ' (${t.plateNumber})' : ''}',
                      ),
                    ))
                .toList(),
            onChanged: (String? v) => setState(() => _truckId = v),
          ),
          const SizedBox(height: 12),
          DropdownButtonFormField<String>(
            initialValue: _templateId,
            decoration: const InputDecoration(labelText: 'Template'),
            items: widget.templates
                .map((TruckStockTemplate t) => DropdownMenuItem<String>(
                      value: t.id,
                      child: Text(t.name),
                    ))
                .toList(),
            onChanged: (String? v) => setState(() => _templateId = v),
          ),
        ],
      ),
      actions: <Widget>[
        TextButton(
          onPressed: () => Navigator.of(context).pop(),
          child: const Text('Cancel'),
        ),
        FilledButton(
          onPressed: (_truckId != null && _templateId != null)
              ? () => Navigator.of(context).pop(
                    _AssignResult(truckId: _truckId!, templateId: _templateId!),
                  )
              : null,
          child: const Text('Assign'),
        ),
      ],
    );
  }
}
