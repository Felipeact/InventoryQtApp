import 'package:flutter/foundation.dart';

import '../models/auth_session.dart';
import '../models/user.dart';
import '../services/api_client.dart';
import '../services/auth_service.dart';
import '../services/token_storage.dart';

/// Permission identifiers used by the backend. We gate the UI by role name
/// heuristically (login only returns a role), mapping roles to these
/// permissions.
///
/// TODO: When a `/auth/me` endpoint exposes a concrete permission list, load it
/// and replace the heuristic mapping below for precise gating.
class Permissions {
  Permissions._();

  static const String addProduct = 'ADD_PRODUCT';
  static const String viewStock = 'VIEW_STOCK';
  static const String scanIn = 'SCAN_IN';
  static const String scanOut = 'SCAN_OUT';
  static const String viewAssignedTruckStock = 'VIEW_ASSIGNED_TRUCK_STOCK';
  static const String uploadReceipt = 'UPLOAD_RECEIPT';
  static const String viewLowStockAlerts = 'VIEW_LOW_STOCK_ALERTS';
  static const String manageTruckStock = 'MANAGE_TRUCK_STOCK';
  static const String approveReceipts = 'APPROVE_RECEIPTS';
  static const String viewAllTrucks = 'VIEW_ALL_TRUCKS';
  static const String viewTruckStock = 'VIEW_TRUCK_STOCK';
  static const String manageUsers = 'MANAGE_USERS';

  /// Permissions granted to admin/manager roles (everything).
  static const Set<String> all = <String>{
    addProduct,
    viewStock,
    scanIn,
    scanOut,
    viewAssignedTruckStock,
    uploadReceipt,
    viewLowStockAlerts,
    manageTruckStock,
    approveReceipts,
    viewAllTrucks,
    viewTruckStock,
    manageUsers,
  };

  /// Field-technician subset.
  static const Set<String> technician = <String>{
    viewStock,
    scanIn,
    scanOut,
    viewAssignedTruckStock,
    uploadReceipt,
    viewLowStockAlerts,
  };
}

enum AuthStatus { unknown, authenticated, unauthenticated }

/// Holds the authenticated session and exposes login/logout plus role-derived
/// permission checks consumed by the UI.
class AuthProvider extends ChangeNotifier {
  AuthProvider({AuthService? authService})
      : _authService = authService ?? AuthService() {
    // Wire the API client's logout signal back to this provider so a failed
    // token refresh ends the session app-wide.
    ApiClient.instance.onUnauthorized = _handleForcedLogout;
  }

  final AuthService _authService;

  AuthStatus _status = AuthStatus.unknown;
  AuthSession? _session;
  bool _busy = false;
  String? _error;

  AuthStatus get status => _status;
  AuthSession? get session => _session;
  User? get user => _session?.user;
  bool get isBusy => _busy;
  String? get error => _error;
  bool get isAuthenticated => _status == AuthStatus.authenticated;

  /// Loads any persisted session at startup.
  Future<void> bootstrap() async {
    final AuthSession? saved = await TokenStorage.instance.readSession();
    if (saved != null && saved.accessToken.isNotEmpty) {
      _session = saved;
      _status = AuthStatus.authenticated;
    } else {
      _status = AuthStatus.unauthenticated;
    }
    notifyListeners();
  }

  Future<bool> login(String email, String password) async {
    return _run(() async {
      final AuthSession session = await _authService.login(
        email: email.trim(),
        password: password,
      );
      await _persist(session);
    });
  }

  Future<bool> register({
    required String email,
    required String password,
    required String code,
    required String companyName,
  }) async {
    return _run(() async {
      final AuthSession session = await _authService.register(
        email: email.trim(),
        password: password,
        code: code.trim(),
        companyName: companyName.trim(),
      );
      await _persist(session);
    });
  }

  Future<bool> changePassword({
    required String currentPassword,
    required String newPassword,
  }) async {
    return _run(() async {
      await _authService.changePassword(
        currentPassword: currentPassword,
        newPassword: newPassword,
      );
      // Clear the mustChangePassword flag locally.
      final User? u = _session?.user;
      if (u != null) {
        _session = _session!.copyWith(user: u.copyWith(mustChangePassword: false));
        await TokenStorage.instance.saveSession(_session!);
      }
    });
  }

  Future<void> requestReset(String email) async {
    await _authService.requestReset(email.trim());
  }

  Future<void> logout() async {
    final String? refresh = _session?.refreshToken;
    if (refresh != null && refresh.isNotEmpty) {
      await _authService.logout(refresh);
    }
    await TokenStorage.instance.clear();
    _session = null;
    _status = AuthStatus.unauthenticated;
    _error = null;
    notifyListeners();
  }

  // ---- Role / permission helpers ------------------------------------------

  String get roleName => _session?.user.role ?? 'Guest';

  bool get isAdmin {
    final String r = roleName.toLowerCase();
    return r.contains('admin') || r.contains('manager') || r.contains('owner');
  }

  bool get isTechnician {
    final String r = roleName.toLowerCase();
    return r.contains('tech') || r.contains('field');
  }

  /// The effective permission set derived from the role name.
  Set<String> get permissions {
    if (isAdmin) return Permissions.all;
    if (isTechnician) return Permissions.technician;
    // Unknown role: grant the conservative technician subset so the app is
    // still usable but does not expose admin-only management screens.
    return Permissions.technician;
  }

  bool can(String permission) => permissions.contains(permission);

  bool canAny(Iterable<String> perms) => perms.any(permissions.contains);

  // ---- Internals -----------------------------------------------------------

  Future<void> _persist(AuthSession session) async {
    _session = session;
    await TokenStorage.instance.saveSession(session);
    _status = AuthStatus.authenticated;
  }

  void _handleForcedLogout() {
    TokenStorage.instance.clear();
    _session = null;
    _status = AuthStatus.unauthenticated;
    _error = 'Your session expired. Please sign in again.';
    notifyListeners();
  }

  Future<bool> _run(Future<void> Function() action) async {
    _busy = true;
    _error = null;
    notifyListeners();
    try {
      await action();
      _busy = false;
      notifyListeners();
      return true;
    } on ApiException catch (e) {
      _busy = false;
      _error = e.message;
      notifyListeners();
      return false;
    } catch (e) {
      _busy = false;
      _error = 'Something went wrong. Please try again.';
      notifyListeners();
      return false;
    }
  }

  void clearError() {
    if (_error != null) {
      _error = null;
      notifyListeners();
    }
  }
}
