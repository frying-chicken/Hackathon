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

- `src/main.cpp`: ビルド対象切替 (`HACK_MODE` / `HACK_TARGET` マクロ)
- `src/Config.hpp`: アプリ層の設定値 (baud, pin, frame size, machine bit)
- `src/Sender.hpp`: Sender 側の更新処理とフレーム生成
- `src/Receiver.hpp`: Receiver 側の受信コールバックとノートスケジューリング
- `src/SheetMusic.hpp`: ノート列 (`startBeat`, `durationBeat`, `pitch`, `amplitude`)
- `src/test_0/*`, `src/test_1/*`: 通信コアの単体検証用ハーネス (本番の `app` モードとは独立)
- `src/hack/*`: 通信コア (エンコード/デコード、タイミング窓、リングバッファ)

通信コアの詳細は `src/hack/README.md` を参照してください。

## 実行モード切替

`src/main.cpp` 冒頭の 2 つのマクロを書き換えてビルド対象を選びます。選択されなかった側のヘッダは `#if` で除外されるため、未使用の Sender/Receiver インスタンスが RAM/Flash を消費することはありません。

```cpp
#define HACK_MODE HACK_MODE_TEST_0      // HACK_MODE_APP / HACK_MODE_TEST_0 / HACK_MODE_TEST_1
#define HACK_TARGET HACK_TARGET_RECEIVER // HACK_TARGET_SENDER / HACK_TARGET_RECEIVER
```

- `HACK_MODE_APP`: 本番アプリ (`src/Sender.hpp` / `src/Receiver.hpp`)
- `HACK_MODE_TEST_0`, `HACK_MODE_TEST_1`: `src/hack` 通信コアの単体検証用ハーネス
- `HACK_TARGET_SENDER` / `HACK_TARGET_RECEIVER`: 送信側/受信側のどちらとして書き込むか

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

# 使用機材

LED部
- LED https://akizukidenshi.com/catalog/g/g106409/ 
    - 2.6V 70mA
- 50Ω抵抗

フォトトランジスタ部
- フォトトランジスタ https://akizukidenshi.com/catalog/g/g102325/
    - レンタル
- 33KΩ抵抗
