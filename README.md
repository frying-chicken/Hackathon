# Hackathon

Arduino UNO R4 WiFi 上で、簡易 Manchester 通信によってテンポ情報を送受信し、
受信側でノートイベントをシリアル CSV として出力するプロジェクトです。

## 目的

- Sender がテンポ情報をフレーム送信する
- Receiver がフレームを復号し、テンポに同期して `sheet_music` を順次出力する
- 出力先はシリアル (他プロセスで CSV として取り込みやすい形式)

## 動作概要

1. Sender が payload 2 byte のフレームを送信する
2. Receiver は対象 machine bit が立ったフレームだけを処理する
3. `bpm == 0` のフレームはリセット信号として扱う
4. 通常フレーム受信後、Receiver は beat 進行に合わせてノートを CSV 出力する

## 主要ファイル

- `src/main.cpp`: 実行モード切替 (`Mode::Sender` / `Mode::Receiver`)
- `src/Config.hpp`: アプリ層の設定値 (baud, pin, frame size, machine bit)
- `src/Sender.hpp`: Sender 側の更新処理とフレーム生成
- `src/Receiver.hpp`: Receiver 側の受信コールバックとノートスケジューリング
- `src/SheetMusic.hpp`: ノート列 (`startBeat`, `durationBeat`, `pitch`, `amplitude`)
- `src/hack/*`: 通信コア (エンコード/デコード、タイミング窓、リングバッファ)

通信コアの詳細は `src/hack/README.md` を参照してください。

## 実行モード切替

`src/main.cpp` の `constexpr Mode mode` を切り替えます。

- `Mode::Sender`: フレーム送信
- `Mode::Receiver`: フレーム受信と CSV 出力

## セットアップ

前提:

- PlatformIO が使える環境
- ボード: Arduino UNO R4 WiFi

主なコマンド:

```bash
platformio run
platformio run --target upload
platformio device monitor --baud 250000
```

## フレーム仕様 (アプリ層)

payload は 2 byte です。

- `byte0`: 宛先識別用ビット群 (`app::machine_id_bit` で判定)
- `byte1`: bpm

Receiver の受信時ポリシー:

- `byte0` の該当ビットが立っていない場合は無視
- `byte1 == 0` は reset 信号として扱い、拍情報と楽譜進行を初期化
- それ以外は有効 bpm として拍タイミングを更新

## Receiver のシリアル出力

ノート開始タイミングに達すると、以下フォーマットで 1 行出力します。

```text
amplitude,pitch,duration_us
```

- `duration_us = durationBeat * beat_period_us`

## チューニング項目

受信安定性の調整は主に `src/hack/Config.hpp` で行います。

- `half_bit_us`: 伝送速度の主設定
- `margin_us`: 受信側のジッタ許容幅
- `threshold_window_us`: baseline 評価窓
- `buffer_size`: 受信窓の保持量

テンポ算出の挙動確認は `src/Sender.hpp` の `bpm_window` を参照してください。

## トラブルシュート

1. Sender 起動時に `Start` がシリアルへ出るか確認
2. Receiver で受信後に CSV が出るか確認
3. 受信が不安定なら `src/hack/Config.hpp` のタイミング定数を調整
4. テンポが揺れるなら Sender 側アナログ入力と平滑窓を確認
5. ノート取りこぼし時は `src/Receiver.hpp` の beat 判定周辺にログを追加

## 実装メモ

- app 層の定数は lower_snake_case を使用
- ヘッダ定義の app モジュール状態/関数は `inline` で ODR リスクを抑制
- Receiver の payload callback は payload 完了時のみ呼び出す設計