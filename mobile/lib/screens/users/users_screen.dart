import 'package:flutter/material.dart';

import '../../config/theme.dart';
import '../../models/user.dart';
import '../../services/api_client.dart';
import '../../services/user_service.dart';
import '../../widgets/empty_state.dart';
import '../../widgets/error_view.dart';
import '../../widgets/loading_indicator.dart';
import '../home_shell.dart';

const List<String> _kRoles = <String>['ADMIN', 'WAREHOUSE', 'TECHNICIAN'];

/// Team management — list users, invite new ones, edit roles, reset passwords,
/// and remove members. Requires MANAGE_USERS server-side.
class UsersScreen extends StatefulWidget {
  const UsersScreen({super.key, this.embedded = false});

  final bool embedded;

  @override
  State<UsersScreen> createState() => _UsersScreenState();
}

class _UsersScreenState extends State<UsersScreen> {
  final UserService _service = UserService();
  List<User> _users = <User>[];
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
      _users = await _service.fetchUsers();
    } on ApiException catch (e) {
      _error = e.message;
    } catch (_) {
      _error = 'Failed to load team members.';
    } finally {
      if (mounted) setState(() => _loading = false);
    }
  }

  Future<void> _invite() async {
    final _UserFormResult? result = await showDialog<_UserFormResult>(
      context: context,
      builder: (_) => const _UserFormDialog(),
    );
    if (result == null) return;
    try {
      final Map<String, dynamic> res = await _service.inviteUser(
        email: result.email,
        role: result.role,
        name: result.name,
      );
      if (!mounted) return;
      final String? tempPw = res['temporaryPassword']?.toString();
      if (tempPw != null && tempPw.isNotEmpty) {
        await _showTempPassword(result.email, tempPw);
      } else {
        showSuccessSnack(context, 'Invite sent to ${result.email}.');
      }
      _load();
    } on ApiException catch (e) {
      if (mounted) showErrorSnack(context, e.message);
    }
  }

  Future<void> _edit(User user) async {
    final _UserFormResult? result = await showDialog<_UserFormResult>(
      context: context,
      builder: (_) => _UserFormDialog(existing: user),
    );
    if (result == null) return;
    try {
      await _service.updateUser(
        user.id,
        name: result.name,
        email: result.email,
        role: result.role,
      );
      if (!mounted) return;
      showSuccessSnack(context, 'Member updated.');
      _load();
    } on ApiException catch (e) {
      if (mounted) showErrorSnack(context, e.message);
    }
  }

  Future<void> _resetPassword(User user) async {
    final bool? ok = await showDialog<bool>(
      context: context,
      builder: (BuildContext ctx) => AlertDialog(
        title: const Text('Reset password?'),
        content: Text('Generate a new temporary password for ${user.name}?'),
        actions: <Widget>[
          TextButton(
              onPressed: () => Navigator.of(ctx).pop(false),
              child: const Text('Cancel')),
          FilledButton(
              onPressed: () => Navigator.of(ctx).pop(true),
              child: const Text('Reset')),
        ],
      ),
    );
    if (ok != true) return;
    try {
      final String temp = await _service.resetPassword(user.id);
      if (!mounted) return;
      await _showTempPassword(user.email, temp);
    } on ApiException catch (e) {
      if (mounted) showErrorSnack(context, e.message);
    }
  }

  Future<void> _delete(User user) async {
    final bool? ok = await showDialog<bool>(
      context: context,
      builder: (BuildContext ctx) => AlertDialog(
        title: const Text('Remove member?'),
        content: Text('Remove ${user.name} from the team? This cannot be undone.'),
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
      await _service.deleteUser(user.id);
      if (!mounted) return;
      showSuccessSnack(context, 'Member removed.');
      _load();
    } on ApiException catch (e) {
      if (mounted) showErrorSnack(context, e.message);
    }
  }

  Future<void> _showTempPassword(String email, String password) {
    return showDialog<void>(
      context: context,
      builder: (BuildContext ctx) => AlertDialog(
        title: const Text('Temporary password'),
        content: Column(
          mainAxisSize: MainAxisSize.min,
          crossAxisAlignment: CrossAxisAlignment.start,
          children: <Widget>[
            Text('Share this with $email — they must change it on first login.'),
            const SizedBox(height: 12),
            SelectableText(
              password,
              style: const TextStyle(
                fontFamily: 'monospace',
                fontWeight: FontWeight.w700,
                fontSize: 16,
              ),
            ),
          ],
        ),
        actions: <Widget>[
          TextButton(
              onPressed: () => Navigator.of(ctx).pop(),
              child: const Text('Done')),
        ],
      ),
    );
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: buildAdaptiveAppBar(
        context,
        embedded: widget.embedded,
        title: 'Team',
      ),
      floatingActionButton: FloatingActionButton.extended(
        onPressed: _invite,
        icon: const Icon(Icons.person_add_alt_1),
        label: const Text('Invite'),
      ),
      body: _body(),
    );
  }

  Widget _body() {
    if (_loading && _users.isEmpty) {
      return const LoadingIndicator(message: 'Loading team...');
    }
    if (_error != null && _users.isEmpty) {
      return ErrorView(message: _error!, onRetry: _load);
    }
    if (_users.isEmpty) {
      return RefreshIndicator(
        onRefresh: _load,
        child: ListView(
          children: <Widget>[
            SizedBox(height: MediaQuery.of(context).size.height * 0.18),
            EmptyState(
              icon: Icons.group_outlined,
              title: 'No team members',
              message: 'Invite teammates to give them access.',
              actionLabel: 'Invite',
              onAction: _invite,
            ),
          ],
        ),
      );
    }
    return RefreshIndicator(
      onRefresh: _load,
      child: ListView.separated(
        padding: const EdgeInsets.fromLTRB(16, 16, 16, 96),
        itemCount: _users.length,
        separatorBuilder: (_, __) => const SizedBox(height: 10),
        itemBuilder: (BuildContext context, int i) {
          final User u = _users[i];
          return Card(
            child: ListTile(
              contentPadding:
                  const EdgeInsets.symmetric(horizontal: 16, vertical: 6),
              leading: CircleAvatar(
                backgroundColor: AppTheme.brand.withValues(alpha: 0.12),
                child: Text(
                  u.initials,
                  style: const TextStyle(
                    color: AppTheme.brand,
                    fontWeight: FontWeight.w800,
                  ),
                ),
              ),
              title: Text(
                u.name.isEmpty ? u.email : u.name,
                style: const TextStyle(
                    fontWeight: FontWeight.w700, color: AppTheme.slate900),
              ),
              subtitle: Text(
                '${u.email} • ${u.role}',
                style: const TextStyle(color: AppTheme.slate500),
              ),
              trailing: PopupMenuButton<String>(
                onSelected: (String v) {
                  if (v == 'edit') {
                    _edit(u);
                  } else if (v == 'reset') {
                    _resetPassword(u);
                  } else if (v == 'delete') {
                    _delete(u);
                  }
                },
                itemBuilder: (_) => const <PopupMenuEntry<String>>[
                  PopupMenuItem<String>(value: 'edit', child: Text('Edit')),
                  PopupMenuItem<String>(
                      value: 'reset', child: Text('Reset password')),
                  PopupMenuItem<String>(value: 'delete', child: Text('Remove')),
                ],
              ),
            ),
          );
        },
      ),
    );
  }
}

class _UserFormResult {
  const _UserFormResult({required this.email, required this.role, this.name});
  final String email;
  final String role;
  final String? name;
}

/// Invite (when [existing] is null) or edit a team member.
class _UserFormDialog extends StatefulWidget {
  const _UserFormDialog({this.existing});
  final User? existing;

  @override
  State<_UserFormDialog> createState() => _UserFormDialogState();
}

class _UserFormDialogState extends State<_UserFormDialog> {
  late final TextEditingController _name;
  late final TextEditingController _email;
  late String _role;

  bool get _isEdit => widget.existing != null;

  @override
  void initState() {
    super.initState();
    _name = TextEditingController(text: widget.existing?.name ?? '');
    _email = TextEditingController(text: widget.existing?.email ?? '');
    final String existingRole =
        (widget.existing?.role ?? 'TECHNICIAN').toUpperCase();
    _role = _kRoles.contains(existingRole) ? existingRole : 'TECHNICIAN';
  }

  @override
  void dispose() {
    _name.dispose();
    _email.dispose();
    super.dispose();
  }

  void _submit() {
    final String email = _email.text.trim();
    if (email.isEmpty || !email.contains('@')) return;
    Navigator.of(context).pop(_UserFormResult(
      email: email,
      role: _role,
      name: _name.text.trim().isEmpty ? null : _name.text.trim(),
    ));
  }

  @override
  Widget build(BuildContext context) {
    return AlertDialog(
      title: Text(_isEdit ? 'Edit member' : 'Invite member'),
      content: Column(
        mainAxisSize: MainAxisSize.min,
        children: <Widget>[
          TextField(
            controller: _name,
            decoration: const InputDecoration(labelText: 'Name (optional)'),
          ),
          const SizedBox(height: 12),
          TextField(
            controller: _email,
            keyboardType: TextInputType.emailAddress,
            decoration: const InputDecoration(labelText: 'Email'),
          ),
          const SizedBox(height: 12),
          DropdownButtonFormField<String>(
            initialValue: _role,
            decoration: const InputDecoration(labelText: 'Role'),
            items: _kRoles
                .map((String r) =>
                    DropdownMenuItem<String>(value: r, child: Text(r)))
                .toList(),
            onChanged: (String? v) => setState(() => _role = v ?? _role),
          ),
        ],
      ),
      actions: <Widget>[
        TextButton(
          onPressed: () => Navigator.of(context).pop(),
          child: const Text('Cancel'),
        ),
        FilledButton(
          onPressed: _submit,
          child: Text(_isEdit ? 'Save' : 'Send invite'),
        ),
      ],
    );
  }
}
