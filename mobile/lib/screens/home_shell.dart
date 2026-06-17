import 'package:flutter/material.dart';
import 'package:provider/provider.dart';

import '../config/routes.dart';
import '../config/theme.dart';
import '../providers/auth_provider.dart';
import '../widgets/app_drawer.dart';
import 'dashboard_screen.dart';
import 'receipts/receipts_screen.dart';
import 'scan/scan_screen.dart';
import 'truck_stock/my_truck_stock_screen.dart';

/// Top-level scaffold that hosts the bottom navigation bar plus the nav drawer.
///
/// Tabs: Dashboard, My Stock, Scan, Receipts, More. The "More" tab opens the
/// drawer for the remaining (role-gated) destinations.
class HomeShell extends StatefulWidget {
  const HomeShell({super.key});

  @override
  State<HomeShell> createState() => _HomeShellState();
}

class _HomeShellState extends State<HomeShell> {
  final GlobalKey<ScaffoldState> _scaffoldKey = GlobalKey<ScaffoldState>();
  int _index = 0;

  late final List<Widget> _tabs = const <Widget>[
    DashboardScreen(embedded: true),
    MyTruckStockScreen(embedded: true),
    ScanScreen(embedded: true),
    ReceiptsScreen(embedded: true),
  ];

  void _onTap(int i) {
    // The last item ("More") opens the drawer instead of switching tabs.
    if (i == 4) {
      _scaffoldKey.currentState?.openDrawer();
      return;
    }
    setState(() => _index = i);
  }

  @override
  Widget build(BuildContext context) {
    final AuthProvider auth = context.watch<AuthProvider>();
    final bool canSeeStock = auth.can(Permissions.viewAssignedTruckStock);
    final bool canScan = auth.canAny(<String>[
      Permissions.scanIn,
      Permissions.scanOut,
    ]);
    final bool canReceipts = auth.can(Permissions.uploadReceipt);

    return Scaffold(
      key: _scaffoldKey,
      drawer: const AppDrawer(),
      body: HomeShellScope(
        openDrawer: () => _scaffoldKey.currentState?.openDrawer(),
        child: IndexedStack(
          index: _index,
          children: _tabs,
        ),
      ),
      bottomNavigationBar: BottomNavigationBar(
        currentIndex: _index,
        onTap: _onTap,
        items: <BottomNavigationBarItem>[
          const BottomNavigationBarItem(
            icon: Icon(Icons.dashboard_outlined),
            activeIcon: Icon(Icons.dashboard),
            label: 'Home',
          ),
          BottomNavigationBarItem(
            icon: Icon(canSeeStock
                ? Icons.local_shipping_outlined
                : Icons.lock_outline),
            activeIcon: const Icon(Icons.local_shipping),
            label: 'My Stock',
          ),
          BottomNavigationBarItem(
            icon: Icon(
                canScan ? Icons.qr_code_scanner : Icons.qr_code_scanner_outlined),
            label: 'Scan',
          ),
          BottomNavigationBarItem(
            icon: Icon(canReceipts
                ? Icons.receipt_long_outlined
                : Icons.receipt_long_outlined),
            activeIcon: const Icon(Icons.receipt_long),
            label: 'Receipts',
          ),
          const BottomNavigationBarItem(
            icon: Icon(Icons.menu),
            label: 'More',
          ),
        ],
      ),
    );
  }
}

/// Provides embedded tab screens a callback to open the [HomeShell]'s drawer,
/// since each tab hosts its own [Scaffold] (without a drawer) inside the shell's
/// [IndexedStack].
class HomeShellScope extends InheritedWidget {
  const HomeShellScope({
    super.key,
    required this.openDrawer,
    required super.child,
  });

  final VoidCallback openDrawer;

  static HomeShellScope? maybeOf(BuildContext context) {
    return context.dependOnInheritedWidgetOfExactType<HomeShellScope>();
  }

  @override
  bool updateShouldNotify(HomeShellScope oldWidget) =>
      openDrawer != oldWidget.openDrawer;
}

/// Builds an AppBar that adapts to context:
///  * embedded in the [HomeShell] -> a leading menu button opens the shell's
///    drawer (via [HomeShellScope]).
///  * pushed as a standalone route -> the default back button is shown.
///
/// A Settings action is always appended.
PreferredSizeWidget buildAdaptiveAppBar(
  BuildContext context, {
  required bool embedded,
  required String title,
  List<Widget>? actions,
  PreferredSizeWidget? bottom,
}) {
  final List<Widget> trailing = <Widget>[
    ...?actions,
    IconButton(
      icon: const Icon(Icons.settings_outlined),
      tooltip: 'Settings',
      onPressed: () => Navigator.of(context).pushNamed(AppRoutes.settings),
    ),
  ];

  if (embedded) {
    return AppBar(
      title: Text(title),
      automaticallyImplyLeading: false,
      leading: Builder(
        builder: (BuildContext ctx) {
          final HomeShellScope? scope = HomeShellScope.maybeOf(ctx);
          return IconButton(
            icon: const Icon(Icons.menu),
            tooltip: 'Menu',
            onPressed: scope?.openDrawer,
          );
        },
      ),
      actions: trailing,
      bottom: bottom,
    );
  }
  return AppBar(title: Text(title), actions: trailing, bottom: bottom);
}

/// Shared SnackBar helpers.
void showSuccessSnack(BuildContext context, String message) {
  ScaffoldMessenger.of(context).showSnackBar(
    SnackBar(content: Text(message), backgroundColor: AppTheme.success),
  );
}

void showErrorSnack(BuildContext context, String message) {
  ScaffoldMessenger.of(context).showSnackBar(
    SnackBar(content: Text(message), backgroundColor: AppTheme.danger),
  );
}
