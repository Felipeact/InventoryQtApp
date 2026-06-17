import 'package:flutter/material.dart';

import '../config/theme.dart';

/// Centered progress spinner with an optional message.
class LoadingIndicator extends StatelessWidget {
  const LoadingIndicator({super.key, this.message});

  final String? message;

  @override
  Widget build(BuildContext context) {
    return Center(
      child: Column(
        mainAxisSize: MainAxisSize.min,
        children: <Widget>[
          const SizedBox(
            width: 36,
            height: 36,
            child: CircularProgressIndicator(strokeWidth: 3),
          ),
          if (message != null) ...<Widget>[
            const SizedBox(height: 16),
            Text(
              message!,
              style: const TextStyle(color: AppTheme.slate500),
            ),
          ],
        ],
      ),
    );
  }
}
