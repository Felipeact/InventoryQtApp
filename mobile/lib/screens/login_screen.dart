import 'package:flutter/material.dart';
import 'package:provider/provider.dart';

import '../config/routes.dart';
import '../config/theme.dart';
import '../providers/auth_provider.dart';
import '../widgets/primary_button.dart';

/// Email/password sign-in with a register flow and password-reset request.
class LoginScreen extends StatefulWidget {
  const LoginScreen({super.key});

  @override
  State<LoginScreen> createState() => _LoginScreenState();
}

class _LoginScreenState extends State<LoginScreen> {
  final GlobalKey<FormState> _formKey = GlobalKey<FormState>();
  final TextEditingController _email = TextEditingController();
  final TextEditingController _password = TextEditingController();
  bool _obscure = true;
  bool _registerMode = false;

  // Register-only fields.
  final TextEditingController _code = TextEditingController();
  final TextEditingController _company = TextEditingController();

  @override
  void dispose() {
    _email.dispose();
    _password.dispose();
    _code.dispose();
    _company.dispose();
    super.dispose();
  }

  Future<void> _submit() async {
    final AuthProvider auth = context.read<AuthProvider>();
    auth.clearError();
    if (!_formKey.currentState!.validate()) return;
    FocusScope.of(context).unfocus();

    final bool ok = _registerMode
        ? await auth.register(
            email: _email.text,
            password: _password.text,
            code: _code.text,
            companyName: _company.text,
          )
        : await auth.login(_email.text, _password.text);

    if (!mounted) return;
    if (ok) {
      if (auth.user?.mustChangePassword == true) {
        Navigator.of(context).pushReplacementNamed(AppRoutes.changePassword);
      } else {
        Navigator.of(context).pushReplacementNamed(AppRoutes.home);
      }
    } else if (auth.error != null) {
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(
          content: Text(auth.error!),
          backgroundColor: AppTheme.danger,
        ),
      );
    }
  }

  Future<void> _requestReset() async {
    final TextEditingController resetEmail =
        TextEditingController(text: _email.text);
    final bool? send = await showDialog<bool>(
      context: context,
      builder: (BuildContext ctx) => AlertDialog(
        title: const Text('Reset password'),
        content: Column(
          mainAxisSize: MainAxisSize.min,
          children: <Widget>[
            const Text(
              'Enter your account email and we will send reset instructions.',
            ),
            const SizedBox(height: 16),
            TextField(
              controller: resetEmail,
              keyboardType: TextInputType.emailAddress,
              decoration: const InputDecoration(labelText: 'Email'),
            ),
          ],
        ),
        actions: <Widget>[
          TextButton(
            onPressed: () => Navigator.of(ctx).pop(false),
            child: const Text('Cancel'),
          ),
          FilledButton(
            onPressed: () => Navigator.of(ctx).pop(true),
            child: const Text('Send'),
          ),
        ],
      ),
    );

    if (!mounted) return;
    if (send == true && resetEmail.text.trim().isNotEmpty) {
      try {
        await context.read<AuthProvider>().requestReset(resetEmail.text);
        if (mounted) {
          ScaffoldMessenger.of(context).showSnackBar(
            const SnackBar(
              content: Text('If the email exists, reset instructions were sent.'),
              backgroundColor: AppTheme.success,
            ),
          );
        }
      } catch (_) {
        if (mounted) {
          ScaffoldMessenger.of(context).showSnackBar(
            const SnackBar(content: Text('Could not request a reset.')),
          );
        }
      }
    }
  }

  @override
  Widget build(BuildContext context) {
    final AuthProvider auth = context.watch<AuthProvider>();

    return Scaffold(
      body: SafeArea(
        child: SingleChildScrollView(
          padding: const EdgeInsets.symmetric(horizontal: 24, vertical: 32),
          child: ConstrainedBox(
            constraints: const BoxConstraints(maxWidth: 460),
            child: Form(
              key: _formKey,
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.stretch,
                children: <Widget>[
                  const SizedBox(height: 12),
                  Container(
                    width: 72,
                    height: 72,
                    decoration: BoxDecoration(
                      color: AppTheme.brand,
                      borderRadius: BorderRadius.circular(20),
                    ),
                    child: const Icon(
                      Icons.inventory_2_rounded,
                      color: Colors.white,
                      size: 38,
                    ),
                  ),
                  const SizedBox(height: 16),
                  const Text(
                    'Vantori',
                    style: TextStyle(
                      fontSize: 22,
                      fontWeight: FontWeight.w800,
                      color: AppTheme.brand,
                      letterSpacing: -0.5,
                    ),
                  ),
                  const SizedBox(height: 20),
                  Text(
                    _registerMode ? 'Create your account' : 'Welcome back',
                    style: const TextStyle(
                      fontSize: 26,
                      fontWeight: FontWeight.w800,
                      color: AppTheme.slate900,
                    ),
                  ),
                  const SizedBox(height: 6),
                  Text(
                    _registerMode
                        ? 'Register with your invite code to get started.'
                        : 'Sign in to manage your truck stock and inventory.',
                    style: const TextStyle(color: AppTheme.slate500),
                  ),
                  const SizedBox(height: 28),
                  TextFormField(
                    controller: _email,
                    keyboardType: TextInputType.emailAddress,
                    textInputAction: TextInputAction.next,
                    autofillHints: const <String>[AutofillHints.email],
                    decoration: const InputDecoration(
                      labelText: 'Email',
                      prefixIcon: Icon(Icons.mail_outline),
                    ),
                    validator: (String? v) {
                      if (v == null || v.trim().isEmpty) {
                        return 'Email is required';
                      }
                      if (!v.contains('@') || !v.contains('.')) {
                        return 'Enter a valid email';
                      }
                      return null;
                    },
                  ),
                  const SizedBox(height: 16),
                  TextFormField(
                    controller: _password,
                    obscureText: _obscure,
                    textInputAction:
                        _registerMode ? TextInputAction.next : TextInputAction.done,
                    autofillHints: const <String>[AutofillHints.password],
                    decoration: InputDecoration(
                      labelText: 'Password',
                      prefixIcon: const Icon(Icons.lock_outline),
                      suffixIcon: IconButton(
                        icon: Icon(
                          _obscure ? Icons.visibility_off : Icons.visibility,
                        ),
                        onPressed: () => setState(() => _obscure = !_obscure),
                      ),
                    ),
                    validator: (String? v) {
                      if (v == null || v.isEmpty) return 'Password is required';
                      if (_registerMode && v.length < 6) {
                        return 'Use at least 6 characters';
                      }
                      return null;
                    },
                    onFieldSubmitted: (_) {
                      if (!_registerMode) _submit();
                    },
                  ),
                  if (_registerMode) ...<Widget>[
                    const SizedBox(height: 16),
                    TextFormField(
                      controller: _company,
                      textInputAction: TextInputAction.next,
                      decoration: const InputDecoration(
                        labelText: 'Company name',
                        prefixIcon: Icon(Icons.business_outlined),
                      ),
                      validator: (String? v) =>
                          (v == null || v.trim().isEmpty)
                              ? 'Company name is required'
                              : null,
                    ),
                    const SizedBox(height: 16),
                    TextFormField(
                      controller: _code,
                      textInputAction: TextInputAction.done,
                      decoration: const InputDecoration(
                        labelText: 'Invite / registration code',
                        prefixIcon: Icon(Icons.vpn_key_outlined),
                      ),
                      validator: (String? v) => (v == null || v.trim().isEmpty)
                          ? 'Registration code is required'
                          : null,
                      onFieldSubmitted: (_) => _submit(),
                    ),
                  ],
                  if (!_registerMode) ...<Widget>[
                    Align(
                      alignment: Alignment.centerRight,
                      child: TextButton(
                        onPressed: _requestReset,
                        child: const Text('Forgot password?'),
                      ),
                    ),
                  ] else
                    const SizedBox(height: 8),
                  const SizedBox(height: 8),
                  PrimaryButton(
                    label: _registerMode ? 'Create account' : 'Sign in',
                    loading: auth.isBusy,
                    onPressed: _submit,
                  ),
                  const SizedBox(height: 16),
                  Row(
                    mainAxisAlignment: MainAxisAlignment.center,
                    children: <Widget>[
                      Text(
                        _registerMode
                            ? 'Already have an account?'
                            : "Don't have an account?",
                        style: const TextStyle(color: AppTheme.slate500),
                      ),
                      TextButton(
                        onPressed: auth.isBusy
                            ? null
                            : () {
                                setState(() => _registerMode = !_registerMode);
                                context.read<AuthProvider>().clearError();
                              },
                        child: Text(_registerMode ? 'Sign in' : 'Register'),
                      ),
                    ],
                  ),
                  const SizedBox(height: 8),
                  TextButton.icon(
                    onPressed: () =>
                        Navigator.of(context).pushNamed(AppRoutes.settings),
                    icon: const Icon(Icons.tune, size: 16),
                    label: const Text('Configure server'),
                  ),
                ],
              ),
            ),
          ),
        ),
      ),
    );
  }
}
