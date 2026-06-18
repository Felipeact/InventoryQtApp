import 'dart:async';

import 'package:flutter/foundation.dart';
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';

import 'app.dart';
import 'config/app_config.dart';

Future<void> main() async {
  // Run the app inside a guarded zone so asynchronous errors that escape the
  // widget tree are captured in one place instead of being silently lost.
  runZonedGuarded<Future<void>>(() async {
    final WidgetsBinding binding = WidgetsFlutterBinding.ensureInitialized();

    // Framework (build / layout / paint) errors.
    FlutterError.onError = (FlutterErrorDetails details) {
      FlutterError.presentError(details);
      _reportError(details.exception, details.stack);
    };

    // Errors surfaced by the engine/platform (e.g. uncaught in platform callbacks).
    binding.platformDispatcher.onError = (Object error, StackTrace stack) {
      _reportError(error, stack);
      return true;
    };

    // Load the persisted API base URL before building the app so the very first
    // network call (session bootstrap) uses the correct host.
    await AppConfig.instance.load();

    // Lock to portrait for a focused field-technician experience.
    await SystemChrome.setPreferredOrientations(<DeviceOrientation>[
      DeviceOrientation.portraitUp,
      DeviceOrientation.portraitDown,
    ]);

    runApp(const InventoryApp());
  }, (Object error, StackTrace stack) {
    _reportError(error, stack);
  });
}

/// Central crash sink. Logs in debug builds; wire this up to a crash-reporting
/// service (Firebase Crashlytics / Sentry) for production observability.
void _reportError(Object error, StackTrace? stack) {
  debugPrint('Uncaught error: $error');
  if (stack != null) {
    debugPrint(stack.toString());
  }
  // TODO: forward `error`/`stack` to Crashlytics/Sentry here.
}
