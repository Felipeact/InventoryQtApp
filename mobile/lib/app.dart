import 'package:flutter/material.dart';
import 'package:provider/provider.dart';

import 'config/routes.dart';
import 'config/theme.dart';
import 'providers/auth_provider.dart';
import 'providers/products_provider.dart';
import 'providers/truck_stock_provider.dart';

/// Root widget: installs the provider tree and the MaterialApp configuration.
class InventoryApp extends StatelessWidget {
  const InventoryApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MultiProvider(
      providers: <ChangeNotifierProvider<dynamic>>[
        ChangeNotifierProvider<AuthProvider>(
          create: (_) => AuthProvider(),
        ),
        ChangeNotifierProvider<ProductsProvider>(
          create: (_) => ProductsProvider(),
        ),
        ChangeNotifierProvider<TruckStockProvider>(
          create: (_) => TruckStockProvider(),
        ),
      ],
      child: MaterialApp(
        title: 'Inventory',
        debugShowCheckedModeBanner: false,
        theme: AppTheme.light,
        darkTheme: AppTheme.dark,
        themeMode: ThemeMode.light,
        initialRoute: AppRoutes.splash,
        onGenerateRoute: AppRoutes.onGenerateRoute,
      ),
    );
  }
}
