import 'package:flutter/material.dart';

import '../screens/ai/ai_assistant_screen.dart';
import '../screens/assets/assets_screen.dart';
import '../screens/change_password_screen.dart';
import '../screens/dashboard_screen.dart';
import '../screens/home_shell.dart';
import '../screens/login_screen.dart';
import '../screens/notifications_screen.dart';
import '../screens/products/add_edit_product_screen.dart';
import '../screens/products/products_screen.dart';
import '../screens/receipts/receipts_screen.dart';
import '../screens/receipts/upload_receipt_screen.dart';
import '../screens/scan/scan_screen.dart';
import '../screens/search_screen.dart';
import '../screens/settings/settings_screen.dart';
import '../screens/splash_screen.dart';
import '../screens/truck_stock/assignments_screen.dart';
import '../screens/truck_stock/low_stock_screen.dart';
import '../screens/truck_stock/my_truck_stock_screen.dart';
import '../screens/truck_stock/templates_screen.dart';
import '../screens/truck_stock/truck_costs_screen.dart';
import '../screens/truck_stock/trucks_screen.dart';
import '../screens/users/users_screen.dart';

/// Centralized route names and the [onGenerateRoute] table.
class AppRoutes {
  AppRoutes._();

  static const String splash = '/';
  static const String login = '/login';
  static const String changePassword = '/change-password';
  static const String home = '/home';
  static const String aiAssistant = '/ai';
  static const String search = '/search';
  static const String notifications = '/notifications';
  static const String dashboard = '/dashboard';
  static const String products = '/products';
  static const String addProduct = '/products/add';
  static const String scan = '/scan';
  static const String myStock = '/my-stock';
  static const String lowStock = '/low-stock';
  static const String trucks = '/trucks';
  static const String templates = '/templates';
  static const String assignments = '/assignments';
  static const String receipts = '/receipts';
  static const String truckCosts = '/truck-costs';
  static const String uploadReceipt = '/receipts/upload';
  static const String assets = '/assets';
  static const String users = '/users';
  static const String settings = '/settings';

  static Route<dynamic> onGenerateRoute(RouteSettings settings) {
    switch (settings.name) {
      case splash:
        return _page(const SplashScreen(), settings);
      case login:
        return _page(const LoginScreen(), settings);
      case changePassword:
        return _page(const ChangePasswordScreen(), settings);
      case home:
        return _page(const HomeShell(), settings);
      case aiAssistant:
        return _page(const AiAssistantScreen(), settings);
      case search:
        return _page(const SearchScreen(), settings);
      case notifications:
        return _page(const NotificationsScreen(), settings);
      case dashboard:
        return _page(const DashboardScreen(), settings);
      case products:
        return _page(const ProductsScreen(), settings);
      case addProduct:
        return _page(const AddEditProductScreen(), settings);
      case scan:
        return _page(const ScanScreen(), settings);
      case myStock:
        return _page(const MyTruckStockScreen(), settings);
      case lowStock:
        return _page(const LowStockScreen(), settings);
      case trucks:
        return _page(const TrucksScreen(), settings);
      case templates:
        return _page(const TemplatesScreen(), settings);
      case assignments:
        return _page(const AssignmentsScreen(), settings);
      case receipts:
        return _page(const ReceiptsScreen(), settings);
      case truckCosts:
        return _page(const TruckCostsScreen(), settings);
      case uploadReceipt:
        return _page(const UploadReceiptScreen(), settings);
      case assets:
        return _page(const AssetsScreen(), settings);
      case users:
        return _page(const UsersScreen(), settings);
      case AppRoutes.settings:
        return _page(const SettingsScreen(), settings);
      default:
        return _page(
          Scaffold(
            appBar: AppBar(title: const Text('Not found')),
            body: Center(child: Text('No route defined for ${settings.name}')),
          ),
          settings,
        );
    }
  }

  static MaterialPageRoute<dynamic> _page(Widget child, RouteSettings settings) {
    return MaterialPageRoute<dynamic>(
      builder: (_) => child,
      settings: settings,
    );
  }
}
