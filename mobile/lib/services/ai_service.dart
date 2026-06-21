import 'api_client.dart';

/// One assistant turn's result: the reply text and the names of any tools the
/// assistant ran (so the UI can show what actions it took).
class AiChatResult {
  final String reply;
  final List<String> actions;

  const AiChatResult({required this.reply, this.actions = const <String>[]});
}

/// Wraps the `/ai` endpoints. The assistant answers questions, builds reports,
/// and takes actions across the company's data, always scoped to the signed-in
/// user's own permissions (enforced server-side).
class AiService {
  AiService({ApiClient? client}) : _client = client ?? ApiClient.instance;

  final ApiClient _client;

  /// Sends the full conversation (each entry `{ 'role': 'user'|'assistant',
  /// 'content': '…' }`, last one the new user message) and returns the reply.
  /// Throws [ApiException] on failure (e.g. a 503 when the assistant isn't
  /// configured), which the caller surfaces to the user.
  Future<AiChatResult> chat(List<Map<String, String>> messages) async {
    final dynamic data =
        await _client.post('/ai/chat', body: <String, dynamic>{'messages': messages});

    if (data is Map) {
      final String reply = (data['reply'] ?? '').toString();
      final List<String> actions = <String>[];
      final dynamic acts = data['actions'];
      if (acts is List) {
        for (final dynamic a in acts) {
          if (a is Map && a['tool'] != null) {
            actions.add(a['tool'].toString());
          }
        }
      }
      return AiChatResult(
        reply: reply.isEmpty ? 'Done.' : reply,
        actions: actions,
      );
    }

    return const AiChatResult(reply: 'Done.');
  }
}
