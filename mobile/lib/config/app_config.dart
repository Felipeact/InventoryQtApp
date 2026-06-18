import 'package:shared_preferences/shared_preferences.dart';

/// Holds runtime configuration for the app, primarily the API base URL which is
/// user-configurable from the Settings screen and persisted via
/// [SharedPreferences].
class AppConfig {
  AppConfig._();

  static final AppConfig instance = AppConfig._();

  /// The default backend base URL. Can be overridden from Settings.
  ///
  /// MUST include the scheme — Dio cannot resolve a bare host, so a missing
  /// `https://` here makes every request (including login) fail on a fresh install.
  static const String defaultBaseUrl =
      'https://inventory-system-api-production.up.railway.app';

  String _baseUrl = defaultBaseUrl;

  /// Current API base URL (no trailing slash, always scheme-qualified).
  String get baseUrl => _baseUrl;

  /// Loads the persisted base URL (if any). Call once at startup.
  Future<void> load() async {
    final String? stored = await ConfigStore.readBaseUrl();
    // Normalize on read too, so any legacy/schemeless persisted value is repaired.
    _baseUrl = stored == null ? defaultBaseUrl : _normalize(stored);
  }

  /// Updates and persists the base URL. Strips a trailing slash for safety.
  Future<void> setBaseUrl(String url) async {
    final String cleaned = _normalize(url);
    _baseUrl = cleaned;
    await ConfigStore.writeBaseUrl(cleaned);
  }

  /// Resets to the default base URL.
  Future<void> resetBaseUrl() async {
    _baseUrl = defaultBaseUrl;
    await ConfigStore.writeBaseUrl(defaultBaseUrl);
  }

  static String _normalize(String url) {
    String cleaned = url.trim();
    // Guarantee an absolute, scheme-qualified URL. Default to HTTPS when the
    // user typed a bare host (e.g. "api.example.com").
    if (cleaned.isNotEmpty &&
        !cleaned.startsWith('http://') &&
        !cleaned.startsWith('https://')) {
      cleaned = 'https://$cleaned';
    }
    while (cleaned.endsWith('/')) {
      cleaned = cleaned.substring(0, cleaned.length - 1);
    }
    return cleaned;
  }
}

/// Thin wrapper around [SharedPreferences] for configuration values.
class ConfigStore {
  ConfigStore._();

  static const String _kBaseUrl = 'config.api_base_url';

  static Future<String?> readBaseUrl() async {
    final SharedPreferences prefs = await SharedPreferences.getInstance();
    return prefs.getString(_kBaseUrl);
  }

  static Future<void> writeBaseUrl(String value) async {
    final SharedPreferences prefs = await SharedPreferences.getInstance();
    await prefs.setString(_kBaseUrl, value);
  }
}
