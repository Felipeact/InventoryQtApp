import 'package:flutter/material.dart';
import 'package:flutter/services.dart';

import 'app.dart';
import 'config/app_config.dart';

Future<void> main() async {
  WidgetsFlutterBinding.ensureInitialized();

  // Load the persisted API base URL before building the app so the very first
  // network call (session bootstrap) uses the correct host.
  await AppConfig.instance.load();

  // Lock to portrait for a focused field-technician experience.
  await SystemChrome.setPreferredOrientations(<DeviceOrientation>[
    DeviceOrientation.portraitUp,
    DeviceOrientation.portraitDown,
  ]);

  runApp(const InventoryApp());
}
