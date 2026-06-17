// Widget tests for the Inventory mobile app.
//
// These cover pure, plugin-free presentational widgets so the suite runs fast
// and deterministically in CI without needing a backend, secure storage, or a
// camera. (The default `flutter create` test referenced a non-existent `MyApp`;
// the app's root widget is `InventoryApp`.)

import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';

import 'package:inventory_mobile/widgets/primary_button.dart';

void main() {
  testWidgets('PrimaryButton renders its label and fires onPressed', (
    WidgetTester tester,
  ) async {
    var tapped = false;
    await tester.pumpWidget(
      MaterialApp(
        home: Scaffold(
          body: PrimaryButton(
            label: 'Sign in',
            onPressed: () => tapped = true,
          ),
        ),
      ),
    );

    expect(find.text('Sign in'), findsOneWidget);

    await tester.tap(find.byType(PrimaryButton));
    await tester.pump();

    expect(tapped, isTrue);
  });

  testWidgets('PrimaryButton shows a spinner and ignores taps while loading', (
    WidgetTester tester,
  ) async {
    var tapped = false;
    await tester.pumpWidget(
      MaterialApp(
        home: Scaffold(
          body: PrimaryButton(
            label: 'Saving',
            loading: true,
            onPressed: () => tapped = true,
          ),
        ),
      ),
    );

    // While loading, the label is replaced by a progress indicator and the
    // button is disabled.
    expect(find.byType(CircularProgressIndicator), findsOneWidget);
    expect(find.text('Saving'), findsNothing);

    await tester.tap(find.byType(PrimaryButton));
    await tester.pump();

    expect(tapped, isFalse);
  });
}
