# Hackathon

Arduino UNO R4 WiFi 上で Manchester 風の簡易通信を行い、
Sender から受けたテンポ情報で Receiver がノートイベントをシリアル出力するプロジェクトです。

## 構成

- main.ino: 実行モード切替 (Sender / Receiver)
- src/Config.hpp: アプリ層設定値
- src/Sender.hpp: アプリ層の送信処理
- src/Receiver.hpp: アプリ層の受信処理とノートスケジューリング
- src/SheetMusic.hpp: ノートイベント列
- src/hack/*: 通信のコア実装 (符号化/復号/時系列窓)

## モード切替

main.ino 内の `constexpr Mode mode` を切り替えます。

- Mode::Sender: フレームを送信
- Mode::Receiver: フレームを受信してノート列を出力

## フレーム仕様

現在の payload は 2 byte です。

- byte0: 宛先 ID ビット (bit index = app::machine_id_bit)
- byte1: bpm

Receiver は byte0 の machine_id_bit が立っているフレームだけを処理します。

## Receiver 出力仕様

Receiver がノート開始時刻に達すると、シリアルへ次の CSV を出力します。

amplitude,pitch,duration_us

duration_us は `durationBeat * periodUs` で計算されます。

## 命名規則とコーディング規約

- app 層の定数は lower_snake_case (`serial_baud`, `frame_size` など)
- アプリ層の namespace 変数・関数は `inline` を付与し、ヘッダ定義時の ODR 問題を回避
- 予約識別子を避けるため、namespace スコープで先頭 `_` の名前は使用しない
- 送受信コア (`src/hack`) の公開 API 名は既存互換を優先 (`readBit`, `writeBit` など)
- 早期 return を基本にし、周期処理 (`loop`) は浅いネストを保つ

## デバッグ手順

1. Sender 側で `Start` が出ることを確認
2. Receiver 側で packet を受信後、CSV 出力が始まることを確認
3. 受信が不安定な場合は `src/hack/Config.hpp` の `half_bit_us`, `margin_us` を調整
4. テンポが不安定な場合は `src/Sender.hpp` の BPM 平滑窓 (`bpm_window`) を確認
5. ノート取りこぼしがある場合は `src/Receiver.hpp` の beat/offset 判定ログを追加して確認

## 今回の保守ポイント

- app 設定定数を lower_snake_case に統一
- `Sender.hpp`, `Receiver.hpp` の namespace スコープ定義を `inline` 化し ODR リスクを低減
- Receiver の note 進行ループを改善し、処理遅延時の取りこぼし耐性を向上
- `hack::Receiver::begin()` で入力ピン設定・内部状態リセットを明示