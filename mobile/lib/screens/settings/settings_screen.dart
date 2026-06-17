import 'package:flutter/material.dart';
import 'package:provider/provider.dart';

import '../../config/app_config.dart';
import '../../config/routes.dart';
import '../../config/theme.dart';
import '../../models/user.dart';
import '../../providers/auth_provider.dart';
import '../../widgets/primary_button.dart';
import '../home_shell.dart';

/// Settings: configure the API base URL, view the current profile/role, change
/// password, and sign out.
class SettingsScreen extends StatefulWidget {
  const SettingsScreen({super.key});

  @override
  State<SettingsScreen> createState() => _SettingsScreenState();
}

class _SettingsScreenState extends State<SettingsScreen> {
  late final TextEditingController _urlController;
  bool _savingUrl = false;

  @override
  void initState() {
    super.initState();
    _urlController =
        TextEditingController(text: AppConfig.instance.baseUrl);
  }

  @override
  void dispose() {
    _urlController.dispose();
    super.dispose();
  }

  Future<void> _saveUrl() async {
    final String value = _urlController.text.trim();
    if (value.isEmpty) {
      showErrorSnack(context, 'API URL cannot be empty.');
      return;
    }
    final Uri? uri = Uri.tryParse(value);
    if (uri == null || !uri.hasScheme || !(uri.isScheme('http') || uri.isScheme('https'))) {
      showErrorSnack(context, 'Enter a valid http(s) URL.');
      return;
    }
    setState(() => _savingUrl = true);
    await AppConfig.instance.setBaseUrl(value);
    if (!mounted) return;
    setState(() {
      _savingUrl = false;
      _urlController.text = AppConfig.instance.baseUrl;
    });
    showSuccessSnack(context, 'API URL updated.');
  }

  Future<void> _resetUrl() async {
    await AppConfig.instance.resetBaseUrl();
    if (!mounted) return;
    setState(() => _urlController.text = AppConfig.instance.baseUrl);
    showSuccessSnack(context, 'Reset to default API URL.');
  }

  Future<void> _logout() async {
    final bool? ok = await showDialog<bool>(
      context: context,
      builder: (BuildContext ctx) => AlertDialog(
        title: const Text('Sign out?'),
        content: const Text('You will need to sign in again to continue.'),
        actions: <Widget>[
          TextButton(
            onPressed: () => Navigator.of(ctx).pop(false),
            child: const Text('Cancel'),
          ),
          FilledButton(
            style: FilledButton.styleFrom(backgroundColor: AppTheme.danger),
            onPressed: () => Navigator.of(ctx).pop(true),
            child: const Text('Sign out'),
          ),
        ],
      ),
    );
    if (!mounted) return;
    if (ok == true) {
      await context.read<AuthProvider>().logout();
      if (mounted) {
        Navigator.of(context).pushNamedAndRemoveUntil(
          AppRoutes.login,
          (Route<dynamic> route) => false,
        );
      }
    }
  }

  @override
  Widget build(BuildContext context) {
    final AuthProvider auth = context.watch<AuthProvider>();
    final User? user = auth.user;

    return Scaffold(
      appBar: AppBar(title: const Text('Settings')),
      body: ListView(
        padding: const EdgeInsets.all(16),
        children: <Widget>[
          if (user != null) _ProfileCard(user: user, role: auth.roleName),
          const SizedBox(height: 20),
          const _SectionTitle('Server configuration'),
          const SizedBox(height: 8),
          Card(
            child: Padding(
              padding: const EdgeInsets.all(16),
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.start,
                children: <Widget>[
                  const Text(
                    'API base URL',
                    style: TextStyle(
                      fontWeight: FontWeight.w700,
                      color: AppTheme.slate700,
                    ),
                  ),
                  const SizedBox(height: 8),
                  TextField(
                    controller: _urlController,
                    keyboardType: TextInputType.url,
                    autocorrect: false,
                    decoration: const InputDecoration(
                      prefixIcon: Icon(Icons.link),
                      hintText: 'https://api.example.com',
                    ),
                  ),
                  const SizedBox(height: 6),
                  const Text(
                    'Default: ${AppConfig.defaultBaseUrl}',
                    style: TextStyle(
                      fontSize: 11,
                      color: AppTheme.slate400,
                    ),
                  ),
                  const SizedBox(height: 12),
                  Row(
                    children: <Widget>[
                      Expanded(
                        child: PrimaryButton(
                          label: 'Save',
                          icon: Icons.save_outlined,
                          loading: _savingUrl,
                          onPressed: _saveUrl,
                        ),
                      ),
                      const SizedBox(width: 12),
                      Expanded(
                        child: OutlinedButton.icon(
                          onPressed: _resetUrl,
                          icon: const Icon(Icons.restore),
                          label: const Text('Reset'),
                        ),
                      ),
                    ],
                  ),
                ],
              ),
            ),
          ),
          const SizedBox(height: 20),
          if (user != null) ...<Widget>[
            const _SectionTitle('Account'),
            const SizedBox(height: 8),
            Card(
              child: Column(
                children: <Widget>[
                  ListTile(
                    leading: const Icon(Icons.lock_reset),
                    title: const Text('Change password'),
                    trailing: const Icon(Icons.chevron_right),
                    onTap: () => Navigator.of(context)
                        .pushNamed(AppRoutes.changePassword),
                  ),
                  const Divider(height: 1),
                  ListTile(
                    leading:
                        const Icon(Icons.logout, color: AppTheme.danger),
                    title: const Text(
                      'Sign out',
                      style: TextStyle(
                        color: AppTheme.danger,
                        fontWeight: FontWeight.w600,
                      ),
                    ),
                    onTap: _logout,
                  ),
                ],
              ),
            ),
          ],
          const SizedBox(height: 24),
          const Center(
            child: Text(
              'Inventory Mobile • v1.0.0',
              style: TextStyle(color: AppTheme.slate400, fontSize: 12),
            ),
          ),
          const SizedBox(height: 24),
        ],
      ),
    );
  }
}

class _ProfileCard extends StatelessWidget {
  const _ProfileCard({required this.user, required this.role});

  final User user;
  final String role;

  @override
  Widget build(BuildContext context) {
    return Card(
      child: Padding(
        padding: const EdgeInsets.all(20),
        child: Row(
          children: <Widget>[
            CircleAvatar(
              radius: 32,
              backgroundColor: AppTheme.brand.withValues(alpha: 0.12),
              child: Text(
                user.initials,
                style: const TextStyle(
                  color: AppTheme.brand,
                  fontWeight: FontWeight.w800,
                  fontSize: 22,
                ),
              ),
            ),
            const SizedBox(width: 16),
            Expanded(
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.start,
                children: <Widget>[
                  Text(
                    user.name.isNotEmpty ? user.name : 'Inventory User',
                    style: const TextStyle(
                      fontSize: 18,
                      fontWeight: FontWeight.w800,
                      color: AppTheme.slate900,
                    ),
                  ),
                  const SizedBox(height: 2),
                  Text(
                    user.email,
                    style: const TextStyle(color: AppTheme.slate500),
                  ),
                  const SizedBox(height: 8),
                  Container(
                    padding: const EdgeInsets.symmetric(
                        horizontal: 10, vertical: 4),
                    decoration: BoxDecoration(
                      color: AppTheme.brand.withValues(alpha: 0.1),
                      borderRadius: BorderRadius.circular(20),
                    ),
                    child: Text(
                      role,
                      style: const TextStyle(
                        color: AppTheme.brand,
                        fontSize: 12,
                        fontWeight: FontWeight.w700,
                      ),
                    ),
                  ),
                ],
              ),
            ),
          ],
        ),
      ),
    );
  }
}

class _SectionTitle extends StatelessWidget {
  const _SectionTitle(this.title);

  final String title;

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.only(left: 4),
      child: Text(
        title,
        style: const TextStyle(
          fontSize: 13,
          letterSpacing: 0.4,
          fontWeight: FontWeight.w700,
          color: AppTheme.slate500,
        ),
      ),
    );
  }
}
