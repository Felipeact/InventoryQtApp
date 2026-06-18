# ProGuard / R8 keep rules for the Inventory mobile app.
# Flutter is AOT-compiled to native, so R8 only affects the Java/Kotlin layer and plugins.

# --- Flutter embedding ---
-keep class io.flutter.app.** { *; }
-keep class io.flutter.embedding.** { *; }
-keep class io.flutter.plugin.** { *; }
-keep class io.flutter.util.** { *; }
-keep class io.flutter.view.** { *; }
-keep class io.flutter.** { *; }
-dontwarn io.flutter.embedding.**

# --- mobile_scanner (Google ML Kit Barcode) ---
-keep class com.google.mlkit.** { *; }
-keep class com.google.android.gms.** { *; }
-dontwarn com.google.mlkit.**
-dontwarn com.google.android.gms.**

# --- image_picker ---
-keep class androidx.lifecycle.DefaultLifecycleObserver

# --- flutter_secure_storage ---
-keep class androidx.security.crypto.** { *; }
-dontwarn androidx.security.crypto.**

# Keep annotations and generic signatures used via reflection by plugins.
-keepattributes *Annotation*, Signature, InnerClasses, EnclosingMethod
