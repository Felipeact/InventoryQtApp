import 'package:fl_chart/fl_chart.dart';
import 'package:flutter/material.dart';
import 'package:provider/provider.dart';

import '../config/routes.dart';
import '../config/theme.dart';
import '../providers/auth_provider.dart';
import '../providers/products_provider.dart';
import '../providers/truck_stock_provider.dart';
import '../widgets/stat_card.dart';
import 'home_shell.dart';

/// Home dashboard: greeting, key metrics, a stock-health chart and quick
/// actions. Content adapts to the signed-in user's permissions.
class DashboardScreen extends StatefulWidget {
  const DashboardScreen({super.key, this.embedded = false});

  final bool embedded;

  @override
  State<DashboardScreen> createState() => _DashboardScreenState();
}

class _DashboardScreenState extends State<DashboardScreen> {
  @override
  void initState() {
    super.initState();
    WidgetsBinding.instance.addPostFrameCallback((_) => _refresh());
  }

  Future<void> _refresh() async {
    final AuthProvider auth = context.read<AuthProvider>();
    final List<Future<void>> jobs = <Future<void>>[];

    if (auth.can(Permissions.viewStock)) {
      jobs.add(context.read<ProductsProvider>().load());
    }
    if (auth.can(Permissions.viewAssignedTruckStock)) {
      jobs.add(context.read<TruckStockProvider>().loadMyStock());
    }
    // The build method watches both providers, so it rebuilds automatically as
    // their data arrives; no extra setState is required here.
    await Future.wait<void>(jobs);
  }

  @override
  Widget build(BuildContext context) {
    final AuthProvider auth = context.watch<AuthProvider>();
    final ProductsProvider products = context.watch<ProductsProvider>();
    final TruckStockProvider truck = context.watch<TruckStockProvider>();

    return Scaffold(
      appBar: buildAdaptiveAppBar(
        context,
        embedded: widget.embedded,
        title: 'Dashboard',
        actions: <Widget>[
          IconButton(
            icon: const Icon(Icons.refresh),
            tooltip: 'Refresh',
            onPressed: _refresh,
          ),
        ],
      ),
      body: RefreshIndicator(
        onRefresh: _refresh,
        child: ListView(
          padding: const EdgeInsets.fromLTRB(16, 16, 16, 32),
          children: <Widget>[
            _Greeting(name: auth.user?.name ?? 'there', role: auth.roleName),
            const SizedBox(height: 20),
            _metrics(context, auth, products, truck),
            const SizedBox(height: 20),
            if (auth.can(Permissions.viewStock))
              _StockHealthCard(
                inStock: (products.all.length -
                        products.lowStockCount -
                        products.outOfStockCount)
                    .clamp(0, 1 << 31),
                low: products.lowStockCount,
                out: products.outOfStockCount,
              ),
            if (auth.can(Permissions.viewAssignedTruckStock) &&
                !auth.can(Permissions.viewStock))
              _TruckHealthCard(truck: truck),
            const SizedBox(height: 20),
            const Text(
              'Quick actions',
              style: TextStyle(
                fontSize: 16,
                fontWeight: FontWeight.w700,
                color: AppTheme.slate900,
              ),
            ),
            const SizedBox(height: 12),
            _QuickActions(auth: auth),
          ],
        ),
      ),
    );
  }

  Widget _metrics(
    BuildContext context,
    AuthProvider auth,
    ProductsProvider products,
    TruckStockProvider truck,
  ) {
    final List<Widget> cards = <Widget>[];

    if (auth.can(Permissions.viewStock)) {
      cards.add(StatCard(
        label: 'Products',
        value: '${products.all.length}',
        icon: Icons.inventory_2_outlined,
        color: AppTheme.brand,
        onTap: () => Navigator.of(context).pushNamed(AppRoutes.products),
      ));
      cards.add(StatCard(
        label: 'Total units',
        value: '${products.totalUnits}',
        icon: Icons.numbers,
        color: AppTheme.info,
      ));
      cards.add(StatCard(
        label: 'Low stock',
        value: '${products.lowStockCount}',
        icon: Icons.trending_down,
        color: AppTheme.warning,
        footnote: products.lowStockCount > 0 ? 'Needs attention' : 'All good',
      ));
      cards.add(StatCard(
        label: 'Out of stock',
        value: '${products.outOfStockCount}',
        icon: Icons.remove_shopping_cart_outlined,
        color: AppTheme.danger,
      ));
    }

    if (auth.can(Permissions.viewAssignedTruckStock)) {
      cards.add(StatCard(
        label: 'My stock items',
        value: '${truck.itemCount}',
        icon: Icons.local_shipping_outlined,
        color: AppTheme.brand,
        onTap: () => Navigator.of(context).pushNamed(AppRoutes.myStock),
      ));
      cards.add(StatCard(
        label: 'Truck low stock',
        value: '${truck.lowStockCount}',
        icon: Icons.warning_amber_rounded,
        color: AppTheme.warning,
        footnote: truck.lowStockCount > 0 ? 'Restock soon' : 'Fully stocked',
        onTap: () => Navigator.of(context).pushNamed(AppRoutes.lowStock),
      ));
    }

    if (cards.isEmpty) {
      return const SizedBox.shrink();
    }

    return GridView.count(
      crossAxisCount: 2,
      shrinkWrap: true,
      physics: const NeverScrollableScrollPhysics(),
      mainAxisSpacing: 12,
      crossAxisSpacing: 12,
      childAspectRatio: 1.35,
      children: cards,
    );
  }
}

class _Greeting extends StatelessWidget {
  const _Greeting({required this.name, required this.role});

  final String name;
  final String role;

  String get _timeOfDay {
    final int h = DateTime.now().hour;
    if (h < 12) return 'Good morning';
    if (h < 17) return 'Good afternoon';
    return 'Good evening';
  }

  @override
  Widget build(BuildContext context) {
    return Container(
      padding: const EdgeInsets.all(20),
      decoration: BoxDecoration(
        gradient: const LinearGradient(
          colors: <Color>[AppTheme.brand, AppTheme.brandDark],
          begin: Alignment.topLeft,
          end: Alignment.bottomRight,
        ),
        borderRadius: BorderRadius.circular(20),
      ),
      child: Row(
        children: <Widget>[
          Expanded(
            child: Column(
              crossAxisAlignment: CrossAxisAlignment.start,
              children: <Widget>[
                Text(
                  '$_timeOfDay,',
                  style: TextStyle(
                    color: Colors.white.withValues(alpha: 0.85),
                    fontSize: 14,
                  ),
                ),
                const SizedBox(height: 2),
                Text(
                  name,
                  style: const TextStyle(
                    color: Colors.white,
                    fontSize: 22,
                    fontWeight: FontWeight.w800,
                  ),
                ),
                const SizedBox(height: 6),
                Container(
                  padding:
                      const EdgeInsets.symmetric(horizontal: 10, vertical: 4),
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
          ),
          const Icon(Icons.waving_hand_rounded,
              color: Colors.white, size: 40),
        ],
      ),
    );
  }
}

class _StockHealthCard extends StatelessWidget {
  const _StockHealthCard({
    required this.inStock,
    required this.low,
    required this.out,
  });

  final int inStock;
  final int low;
  final int out;

  @override
  Widget build(BuildContext context) {
    final int total = inStock + low + out;
    return Card(
      child: Padding(
        padding: const EdgeInsets.all(20),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: <Widget>[
            const Text(
              'Stock health',
              style: TextStyle(
                fontSize: 16,
                fontWeight: FontWeight.w700,
                color: AppTheme.slate900,
              ),
            ),
            const SizedBox(height: 16),
            if (total == 0)
              const Padding(
                padding: EdgeInsets.symmetric(vertical: 24),
                child: Center(
                  child: Text(
                    'No product data yet.',
                    style: TextStyle(color: AppTheme.slate400),
                  ),
                ),
              )
            else
              Row(
                children: <Widget>[
                  SizedBox(
                    height: 130,
                    width: 130,
                    child: PieChart(
                      PieChartData(
                        sectionsSpace: 2,
                        centerSpaceRadius: 36,
                        sections: <PieChartSectionData>[
                          _section(inStock, AppTheme.success, total),
                          _section(low, AppTheme.warning, total),
                          _section(out, AppTheme.danger, total),
                        ],
                      ),
                    ),
                  ),
                  const SizedBox(width: 20),
                  Expanded(
                    child: Column(
                      crossAxisAlignment: CrossAxisAlignment.start,
                      children: <Widget>[
                        _Legend(
                          color: AppTheme.success,
                          label: 'In stock',
                          value: inStock,
                        ),
                        const SizedBox(height: 10),
                        _Legend(
                          color: AppTheme.warning,
                          label: 'Low stock',
                          value: low,
                        ),
                        const SizedBox(height: 10),
                        _Legend(
                          color: AppTheme.danger,
                          label: 'Out of stock',
                          value: out,
                        ),
                      ],
                    ),
                  ),
                ],
              ),
          ],
        ),
      ),
    );
  }

  PieChartSectionData _section(int value, Color color, int total) {
    final double pct = total == 0 ? 0 : (value / total) * 100;
    return PieChartSectionData(
      value: value.toDouble(),
      color: color,
      radius: 26,
      showTitle: value > 0,
      title: '${pct.round()}%',
      titleStyle: const TextStyle(
        color: Colors.white,
        fontSize: 11,
        fontWeight: FontWeight.w700,
      ),
    );
  }
}

class _TruckHealthCard extends StatelessWidget {
  const _TruckHealthCard({required this.truck});

  final TruckStockProvider truck;

  @override
  Widget build(BuildContext context) {
    final int total = truck.itemCount;
    final int low = truck.lowStockCount;
    final int ok = (total - low).clamp(0, 1 << 31);
    final double maxY =
        (total == 0 ? 1 : total).toDouble();

    return Card(
      child: Padding(
        padding: const EdgeInsets.all(20),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: <Widget>[
            const Text(
              'Truck stock health',
              style: TextStyle(
                fontSize: 16,
                fontWeight: FontWeight.w700,
                color: AppTheme.slate900,
              ),
            ),
            const SizedBox(height: 16),
            if (total == 0)
              const Padding(
                padding: EdgeInsets.symmetric(vertical: 24),
                child: Center(
                  child: Text(
                    'No assigned stock yet.',
                    style: TextStyle(color: AppTheme.slate400),
                  ),
                ),
              )
            else
              SizedBox(
                height: 150,
                child: BarChart(
                  BarChartData(
                    maxY: maxY,
                    borderData: FlBorderData(show: false),
                    gridData: const FlGridData(show: false),
                    titlesData: FlTitlesData(
                      leftTitles: const AxisTitles(
                        sideTitles: SideTitles(showTitles: false),
                      ),
                      topTitles: const AxisTitles(
                        sideTitles: SideTitles(showTitles: false),
                      ),
                      rightTitles: const AxisTitles(
                        sideTitles: SideTitles(showTitles: false),
                      ),
                      bottomTitles: AxisTitles(
                        sideTitles: SideTitles(
                          showTitles: true,
                          getTitlesWidget: (double value, TitleMeta meta) {
                            const List<String> labels = <String>['OK', 'Low'];
                            final int i = value.toInt();
                            if (i < 0 || i >= labels.length) {
                              return const SizedBox.shrink();
                            }
                            return Padding(
                              padding: const EdgeInsets.only(top: 6),
                              child: Text(
                                labels[i],
                                style: const TextStyle(
                                  fontSize: 12,
                                  color: AppTheme.slate500,
                                ),
                              ),
                            );
                          },
                        ),
                      ),
                    ),
                    barGroups: <BarChartGroupData>[
                      BarChartGroupData(x: 0, barRods: <BarChartRodData>[
                        BarChartRodData(
                          toY: ok.toDouble(),
                          color: AppTheme.success,
                          width: 40,
                          borderRadius: BorderRadius.circular(6),
                        ),
                      ]),
                      BarChartGroupData(x: 1, barRods: <BarChartRodData>[
                        BarChartRodData(
                          toY: low.toDouble(),
                          color: AppTheme.warning,
                          width: 40,
                          borderRadius: BorderRadius.circular(6),
                        ),
                      ]),
                    ],
                  ),
                ),
              ),
          ],
        ),
      ),
    );
  }
}

class _Legend extends StatelessWidget {
  const _Legend({
    required this.color,
    required this.label,
    required this.value,
  });

  final Color color;
  final String label;
  final int value;

  @override
  Widget build(BuildContext context) {
    return Row(
      children: <Widget>[
        Container(
          width: 12,
          height: 12,
          decoration: BoxDecoration(
            color: color,
            borderRadius: BorderRadius.circular(3),
          ),
        ),
        const SizedBox(width: 8),
        Expanded(
          child: Text(
            label,
            style: const TextStyle(color: AppTheme.slate500),
          ),
        ),
        Text(
          '$value',
          style: const TextStyle(
            fontWeight: FontWeight.w700,
            color: AppTheme.slate900,
          ),
        ),
      ],
    );
  }
}

class _QuickActions extends StatelessWidget {
  const _QuickActions({required this.auth});

  final AuthProvider auth;

  @override
  Widget build(BuildContext context) {
    final List<_Action> actions = <_Action>[
      if (auth.canAny(<String>[Permissions.scanIn, Permissions.scanOut]))
        const _Action(
          icon: Icons.qr_code_scanner,
          label: 'Scan',
          color: AppTheme.brand,
          route: AppRoutes.scan,
        ),
      if (auth.can(Permissions.viewLowStockAlerts))
        const _Action(
          icon: Icons.warning_amber_rounded,
          label: 'Low stock',
          color: AppTheme.warning,
          route: AppRoutes.lowStock,
        ),
      if (auth.can(Permissions.uploadReceipt))
        const _Action(
          icon: Icons.upload_file,
          label: 'Upload receipt',
          color: AppTheme.info,
          route: AppRoutes.uploadReceipt,
        ),
      if (auth.can(Permissions.addProduct))
        const _Action(
          icon: Icons.add_box_outlined,
          label: 'Add product',
          color: AppTheme.success,
          route: AppRoutes.addProduct,
        ),
      if (auth.can(Permissions.viewAssignedTruckStock))
        const _Action(
          icon: Icons.local_shipping_outlined,
          label: 'My stock',
          color: AppTheme.brandDark,
          route: AppRoutes.myStock,
        ),
    ];

    if (actions.isEmpty) return const SizedBox.shrink();

    return GridView.count(
      crossAxisCount: 3,
      shrinkWrap: true,
      physics: const NeverScrollableScrollPhysics(),
      mainAxisSpacing: 12,
      crossAxisSpacing: 12,
      childAspectRatio: 0.95,
      children: actions
          .map((_Action a) => _ActionTile(action: a))
          .toList(growable: false),
    );
  }
}

class _Action {
  const _Action({
    required this.icon,
    required this.label,
    required this.color,
    required this.route,
  });

  final IconData icon;
  final String label;
  final Color color;
  final String route;
}

class _ActionTile extends StatelessWidget {
  const _ActionTile({required this.action});

  final _Action action;

  @override
  Widget build(BuildContext context) {
    return Card(
      child: InkWell(
        borderRadius: BorderRadius.circular(16),
        onTap: () => Navigator.of(context).pushNamed(action.route),
        child: Padding(
          padding: const EdgeInsets.all(12),
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: <Widget>[
              Container(
                padding: const EdgeInsets.all(12),
                decoration: BoxDecoration(
                  color: action.color.withValues(alpha: 0.12),
                  borderRadius: BorderRadius.circular(14),
                ),
                child: Icon(action.icon, color: action.color, size: 24),
              ),
              const SizedBox(height: 10),
              Text(
                action.label,
                textAlign: TextAlign.center,
                maxLines: 2,
                overflow: TextOverflow.ellipsis,
                style: const TextStyle(
                  fontSize: 12,
                  fontWeight: FontWeight.w600,
                  color: AppTheme.slate700,
                ),
              ),
            ],
          ),
        ),
      ),
    );
  }
}
