# ToDo

## Documentation

- [ ] `src/Config.hpp` の各定数に用途コメントを追加する
- [ ] `src/hack/README.md` に受信状態遷移図 (Idle/Sync/Payload) を追記する
- [ ] CSV 出力の利用例 (受信側ログを別プロセスで読む方法) を README に追記する

## Validation

- [ ] Sender/Receiver の 2 台実機で reset フレーム (`bpm == 0`) の挙動を確認する
- [ ] 低 bpm / 高 bpm 境界でノート取りこぼしの有無を確認する
- [ ] `half_bit_us` と `margin_us` の推奨レンジを実測で記録する

## Code Follow-up

- [ ] `src/hack/Receiver.hpp` の境界ケース (遅延・同期ずれ) のテストを追加する
- [ ] `src/Sender.hpp` の `beat_count` に関する仕様をドキュメント化する