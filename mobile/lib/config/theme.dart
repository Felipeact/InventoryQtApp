import 'package:flutter/material.dart';

/// Centralized theme for the Inventory mobile app.
///
/// Uses a deep-indigo brand color with a slate neutral palette and Material 3.
class AppTheme {
  AppTheme._();

  /// Primary brand color (deep indigo).
  static const Color brand = Color(0xFF4F46E5);
  static const Color brandDark = Color(0xFF4338CA);
  static const Color brandLight = Color(0xFF818CF8);

  // Slate neutral palette.
  static const Color slate900 = Color(0xFF0F172A);
  static const Color slate800 = Color(0xFF1E293B);
  static const Color slate700 = Color(0xFF334155);
  static const Color slate500 = Color(0xFF64748B);
  static const Color slate400 = Color(0xFF94A3B8);
  static const Color slate200 = Color(0xFFE2E8F0);
  static const Color slate100 = Color(0xFFF1F5F9);
  static const Color slate50 = Color(0xFFF8FAFC);

  // Semantic colors.
  static const Color success = Color(0xFF16A34A);
  static const Color warning = Color(0xFFF59E0B);
  static const Color danger = Color(0xFFDC2626);
  static const Color info = Color(0xFF0EA5E9);

  static ThemeData get light {
    final ColorScheme scheme = ColorScheme.fromSeed(
      seedColor: brand,
      primary: brand,
      brightness: Brightness.light,
    ).copyWith(
      surface: Colors.white,
      error: danger,
    );

    return ThemeData(
      useMaterial3: true,
      colorScheme: scheme,
      scaffoldBackgroundColor: slate50,
      visualDensity: VisualDensity.adaptivePlatformDensity,
      appBarTheme: const AppBarTheme(
        backgroundColor: Colors.white,
        foregroundColor: slate900,
        elevation: 0,
        scrolledUnderElevation: 1,
        centerTitle: false,
        titleTextStyle: TextStyle(
          color: slate900,
          fontSize: 20,
          fontWeight: FontWeight.w700,
        ),
      ),
      cardTheme: CardTheme(
        elevation: 0,
        color: Colors.white,
        surfaceTintColor: Colors.white,
        shape: RoundedRectangleBorder(
          borderRadius: BorderRadius.circular(16),
          side: const BorderSide(color: slate200),
        ),
        margin: EdgeInsets.zero,
      ),
      inputDecorationTheme: InputDecorationTheme(
        filled: true,
        fillColor: Colors.white,
        contentPadding:
            const EdgeInsets.symmetric(horizontal: 16, vertical: 14),
        border: OutlineInputBorder(
          borderRadius: BorderRadius.circular(12),
          borderSide: const BorderSide(color: slate200),
        ),
        enabledBorder: OutlineInputBorder(
          borderRadius: BorderRadius.circular(12),
          borderSide: const BorderSide(color: slate200),
        ),
        focusedBorder: OutlineInputBorder(
          borderRadius: BorderRadius.circular(12),
          borderSide: const BorderSide(color: brand, width: 2),
        ),
        errorBorder: OutlineInputBorder(
          borderRadius: BorderRadius.circular(12),
          borderSide: const BorderSide(color: danger),
        ),
        labelStyle: const TextStyle(color: slate500),
      ),
      elevatedButtonTheme: ElevatedButtonThemeData(
        style: ElevatedButton.styleFrom(
          backgroundColor: brand,
          foregroundColor: Colors.white,
          elevation: 0,
          minimumSize: const Size.fromHeight(52),
          shape: RoundedRectangleBorder(
            borderRadius: BorderRadius.circular(12),
          ),
          textStyle: const TextStyle(
            fontSize: 16,
            fontWeight: FontWeight.w600,
          ),
        ),
      ),
      outlinedButtonTheme: OutlinedButtonThemeData(
        style: OutlinedButton.styleFrom(
          foregroundColor: brand,
          minimumSize: const Size.fromHeight(52),
          side: const BorderSide(color: brand),
          shape: RoundedRectangleBorder(
            borderRadius: BorderRadius.circular(12),
          ),
          textStyle: const TextStyle(
            fontSize: 16,
            fontWeight: FontWeight.w600,
          ),
        ),
      ),
      textButtonTheme: TextButtonThemeData(
        style: TextButton.styleFrom(foregroundColor: brand),
      ),
      chipTheme: ChipThemeData(
        backgroundColor: slate100,
        labelStyle: const TextStyle(
          color: slate700,
          fontWeight: FontWeight.w600,
          fontSize: 12,
        ),
        side: BorderSide.none,
        shape: RoundedRectangleBorder(
          borderRadius: BorderRadius.circular(8),
        ),
      ),
      dividerTheme: const DividerThemeData(
        color: slate200,
        thickness: 1,
        space: 1,
      ),
      bottomNavigationBarTheme: const BottomNavigationBarThemeData(
        backgroundColor: Colors.white,
        selectedItemColor: brand,
        unselectedItemColor: slate400,
        type: BottomNavigationBarType.fixed,
        elevation: 8,
        showUnselectedLabels: true,
      ),
      snackBarTheme: SnackBarThemeData(
        behavior: SnackBarBehavior.floating,
        shape: RoundedRectangleBorder(
          borderRadius: BorderRadius.circular(12),
        ),
      ),
      floatingActionButtonTheme: const FloatingActionButtonThemeData(
        backgroundColor: brand,
        foregroundColor: Colors.white,
      ),
      listTileTheme: const ListTileThemeData(
        iconColor: slate500,
      ),
    );
  }

  static ThemeData get dark {
    final ColorScheme scheme = ColorScheme.fromSeed(
      seedColor: brand,
      brightness: Brightness.dark,
    );

    return ThemeData(
      useMaterial3: true,
      colorScheme: scheme,
      scaffoldBackgroundColor: slate900,
      appBarTheme: const AppBarTheme(
        backgroundColor: slate800,
        elevation: 0,
        centerTitle: false,
      ),
    );
  }
}

/// Helper that maps a status string to a color/label for chips.
class StatusStyle {
  StatusStyle._();

  static Color colorFor(String? status) {
    switch ((status ?? '').toUpperCase()) {
      case 'ACTIVE':
      case 'IN_STOCK':
      case 'AVAILABLE':
      case 'APPROVED':
      case 'OK':
        return AppTheme.success;
      case 'LOW':
      case 'LOW_STOCK':
      case 'PENDING':
        return AppTheme.warning;
      case 'OUT_OF_STOCK':
      case 'INACTIVE':
      case 'REJECTED':
      case 'BROKEN':
        return AppTheme.danger;
      case 'IN_USE':
      case 'ASSIGNED':
        return AppTheme.info;
      default:
        return AppTheme.slate500;
    }
  }
}
