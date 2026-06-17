import 'package:flutter/material.dart';

import '../config/theme.dart';

/// Friendly empty-list placeholder with an icon, title and optional action.
class EmptyState extends StatelessWidget {
  const EmptyState({
    super.key,
    required this.title,
    this.message,
    this.icon = Icons.inbox_outlined,
    this.actionLabel,
    this.onAction,
  });

  final String title;
  final String? message;
  final IconData icon;
  final String? actionLabel;
  final VoidCallback? onAction;

  @override
  Widget build(BuildContext context) {
    return Center(
      child: Padding(
        padding: const EdgeInsets.all(32),
        child: Column(
          mainAxisSize: MainAxisSize.min,
          children: <Widget>[
            Container(
              width: 84,
              height: 84,
              decoration: const BoxDecoration(
                color: AppTheme.slate100,
                shape: BoxShape.circle,
              ),
              child: Icon(icon, size: 40, color: AppTheme.slate400),
            ),
            const SizedBox(height: 20),
            Text(
              title,
              textAlign: TextAlign.center,
              style: const TextStyle(
                fontSize: 18,
                fontWeight: FontWeight.w700,
                color: AppTheme.slate700,
              ),
            ),
            if (message != null) ...<Widget>[
              const SizedBox(height: 8),
              Text(
                message!,
                textAlign: TextAlign.center,
                style: const TextStyle(color: AppTheme.slate500, height: 1.4),
              ),
            ],
            if (actionLabel != null && onAction != null) ...<Widget>[
              const SizedBox(height: 20),
              OutlinedButton.icon(
                onPressed: onAction,
                icon: const Icon(Icons.refresh),
                label: Text(actionLabel!),
                style: OutlinedButton.styleFrom(
                  minimumSize: const Size(160, 48),
                ),
              ),
            ],
          ],
        ),
      ),
    );
  }
}
