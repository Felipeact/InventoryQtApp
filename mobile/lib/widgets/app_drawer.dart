import 'package:flutter/material.dart';
import 'package:provider/provider.dart';

import '../config/routes.dart';
import '../config/theme.dart';
import '../models/user.dart';
import '../providers/auth_provider.dart';

/// Navigation drawer whose entries are gated by the signed-in user's
/// role-derived permissions.
class AppDrawer extends StatelessWidget {
  const AppDrawer({super.key});

  @override
  Widget build(BuildContext context) {
    final AuthProvider auth = context.watch<AuthProvider>();
    final User? user = auth.user;

    return Drawer(
      child: SafeArea(
        child: Column(
          children: <Widget>[
            _Header(user: user, role: auth.roleName),
            Expanded(
              child: ListView(
                padding: const EdgeInsets.symmetric(vertical: 8),
                children: <Widget>[
                  const _DrawerItem(
                    icon: Icons.dashboard_outlined,
                    label: 'Dashboard',
                    route: AppRoutes.home,
                    onNavigate: _goHome,
                  ),
                  // Available to every signed-in user; the server enforces what
                  // the assistant may actually do per the user's permissions.
                  const _DrawerItem(
                    icon: Icons.auto_awesome_outlined,
                    label: 'AI Assistant',
                    route: AppRoutes.aiAssistant,
                  ),
                  if (auth.can(Permissions.viewAssignedTruckStock))
                    const _DrawerItem(
                      icon: Icons.local_shipping_outlined,
                      label: 'My Truck Stock',
                      route: AppRoutes.myStock,
                    ),
                  if (auth.can(Permissions.viewLowStockAlerts))
                    const _DrawerItem(
                      icon: Icons.warning_amber_rounded,
                      label: 'Low Stock Alerts',
                      route: AppRoutes.lowStock,
                    ),
                  if (auth.canAny(<String>[
                    Permissions.viewLowStockAlerts,
                    Permissions.approveReceipts,
                  ]))
                    const _DrawerItem(
                      icon: Icons.notifications_none_rounded,
                      label: 'Notifications',
                      route: AppRoutes.notifications,
                    ),
                  if (auth.canAny(<String>[
                    Permissions.scanIn,
                    Permissions.scanOut,
                  ]))
                    const _DrawerItem(
                      icon: Icons.qr_code_scanner,
                      label: 'Scan In / Out',
                      route: AppRoutes.scan,
                    ),
                  if (auth.can(Permissions.uploadReceipt))
                    const _DrawerItem(
                      icon: Icons.receipt_long_outlined,
                      label: 'Receipts',
                      route: AppRoutes.receipts,
                    ),
                  if (auth.can(Permissions.approveReceipts))
                    const _DrawerItem(
                      icon: Icons.payments_outlined,
                      label: 'Truck Costs',
                      route: AppRoutes.truckCosts,
                    ),
                  const _SectionLabel('Inventory'),
                  if (auth.can(Permissions.viewStock))
                    const _DrawerItem(
                      icon: Icons.inventory_2_outlined,
                      label: 'Products',
                      route: AppRoutes.products,
                    ),
                  if (auth.isAdmin)
                    const _DrawerItem(
                      icon: Icons.devices_other_outlined,
                      label: 'Assets',
                      route: AppRoutes.assets,
                    ),
                  if (auth.canAny(<String>[
                    Permissions.viewAllTrucks,
                    Permissions.manageTruckStock,
                  ]))
                    const _DrawerItem(
                      icon: Icons.fire_truck_outlined,
                      label: 'Trucks',
                      route: AppRoutes.trucks,
                    ),
                  if (auth.can(Permissions.manageTruckStock))
                    const _DrawerItem(
                      icon: Icons.checklist_rtl_outlined,
                      label: 'Stock Templates',
                      route: AppRoutes.templates,
                    ),
                  if (auth.can(Permissions.manageTruckStock))
                    const _DrawerItem(
                      icon: Icons.add_link,
                      label: 'Assignments',
                      route: AppRoutes.assignments,
                    ),
                  if (auth.can(Permissions.manageUsers)) ...<Widget>[
                    const _SectionLabel('Team'),
                    const _DrawerItem(
                      icon: Icons.group_outlined,
                      label: 'Team',
                      route: AppRoutes.users,
                    ),
                  ],
                  const _SectionLabel('Account'),
                  const _DrawerItem(
                    icon: Icons.settings_outlined,
                    label: 'Settings',
                    route: AppRoutes.settings,
                  ),
                ],
              ),
            ),
            const Divider(height: 1),
            ListTile(
              leading: const Icon(Icons.logout, color: AppTheme.danger),
              title: const Text(
                'Sign out',
                style: TextStyle(
                  color: AppTheme.danger,
                  fontWeight: FontWeight.w600,
                ),
              ),
              onTap: () => _confirmLogout(context, auth),
            ),
            const SizedBox(height: 8),
          ],
        ),
      ),
    );
  }

  static void _goHome(BuildContext context) {
    Navigator.of(context).pop();
    Navigator.of(context).pushNamedAndRemoveUntil(
      AppRoutes.home,
      (Route<dynamic> route) => false,
    );
  }

  Future<void> _confirmLogout(BuildContext context, AuthProvider auth) async {
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
    if (ok == true) {
      await auth.logout();
      if (context.mounted) {
        Navigator.of(context).pushNamedAndRemoveUntil(
          AppRoutes.login,
          (Route<dynamic> route) => false,
        );
      }
    }
  }
}

class _Header extends StatelessWidget {
  const _Header({required this.user, required this.role});

  final User? user;
  final String role;

  @override
  Widget build(BuildContext context) {
    return Container(
      width: double.infinity,
      padding: const EdgeInsets.fromLTRB(20, 24, 20, 24),
      decoration: const BoxDecoration(
        gradient: LinearGradient(
          colors: <Color>[AppTheme.brand, AppTheme.brandDark],
          begin: Alignment.topLeft,
          end: Alignment.bottomRight,
        ),
      ),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: <Widget>[
          CircleAvatar(
            radius: 28,
            backgroundColor: Colors.white,
            child: Text(
              user?.initials ?? '?',
              style: const TextStyle(
                color: AppTheme.brand,
                fontWeight: FontWeight.w800,
                fontSize: 20,
              ),
            ),
          ),
          const SizedBox(height: 12),
          Text(
            user?.name.isNotEmpty == true ? user!.name : 'Inventory User',
            style: const TextStyle(
              color: Colors.white,
              fontSize: 18,
              fontWeight: FontWeight.w700,
            ),
          ),
          const SizedBox(height: 2),
          Text(
            user?.email ?? '',
            style: TextStyle(color: Colors.white.withValues(alpha: 0.85), fontSize: 13),
          ),
          const SizedBox(height: 10),
          Container(
            padding: const EdgeInsets.symmetric(horizontal: 10, vertical: 4),
            decoration: BoxDecoration(
              color: Colors.white.withValues(alpha: 0.2),
              borderRadius: BorderRadius.circular(20),
            ),
            child: Text(
              role,
              style: const TextStyle(
                color: Colors.white,
                fontSize: 12,
                fontWeight: FontWeight.w600,
              ),
            ),
          ),
        ],
      ),
    );
  }
}

class _DrawerItem extends StatelessWidget {
  const _DrawerItem({
    required this.icon,
    required this.label,
    required this.route,
    this.onNavigate,
  });

  final IconData icon;
  final String label;
  final String route;
  final void Function(BuildContext context)? onNavigate;

  @override
  Widget build(BuildContext context) {
    final String? current = ModalRoute.of(context)?.settings.name;
    final bool selected = current == route;
    return ListTile(
      leading: Icon(icon, color: selected ? AppTheme.brand : AppTheme.slate500),
      title: Text(
        label,
        style: TextStyle(
          fontWeight: selected ? FontWeight.w700 : FontWeight.w500,
          color: selected ? AppTheme.brand : AppTheme.slate800,
        ),
      ),
      selected: selected,
      selectedTileColor: AppTheme.brand.withValues(alpha: 0.08),
      shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(12)),
      contentPadding: const EdgeInsets.symmetric(horizontal: 16),
      onTap: () {
        if (onNavigate != null) {
          onNavigate!(context);
          return;
        }
        Navigator.of(context).pop();
        if (!selected) {
          Navigator.of(context).pushNamed(route);
        }
      },
    );
  }
}

class _SectionLabel extends StatelessWidget {
  const _SectionLabel(this.label);

  final String label;

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.fromLTRB(20, 16, 20, 8),
      child: Text(
        label.toUpperCase(),
        style: const TextStyle(
          fontSize: 11,
          letterSpacing: 0.8,
          fontWeight: FontWeight.w700,
          color: AppTheme.slate400,
        ),
      ),
    );
  }
}
