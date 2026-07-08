# hack module docs

このディレクトリは、Arduino 上での簡易 Manchester 通信向けのヘッダ専用モジュールです。

## 構成

- Sender.hpp: デジタル出力でフレーム送信
- Receiver.hpp: アナログ入力波形からフレーム復号
- Config.hpp: 通信タイミング・フレーム構成の定数
- Clock.hpp: 一定周期処理用クロック
- PrefixSumWindow.hpp: 時間付き移動平均用プレフィックス和
- RingContainer.hpp: 固定長リングバッファ
- Utility.hpp / Types.hpp: ビット演算と型定義

## フレーム仕様

送信順序は以下です。

1. calibration (8bit)
2. preamble (32bit、0 固定)
3. start (8bit)
4. payload (Capacity byte)

値は Config.hpp で定義されています。

## Sender の使い方

1. setup で begin() を 1 回呼ぶ
2. loop で update() を高頻度で呼ぶ
3. 空き状態で operator()(payload) を呼ぶと送信開始

補足:

- operator() は送信中なら false を返します
- 内部は half bit ごとに位相反転し、Manchester になるように出力します

## Receiver の使い方

1. インスタンスを生成する
2. setup で begin(callback) を呼ぶ
3. loop で update() を高頻度で呼ぶ

補足:

- begin(callback) は受信完了コールバックを登録します
- begin(callback) は受信内部状態の初期化も行います
- callback は payload が Capacity byte そろった時のみ呼ばれます
- 復号は 1bit 幅の前半/後半平均を比較し、必要時のみ baseline を使います
- 受信ループが軽く遅れた場合は想定ビット境界 (`last + bit_us`) で 1 回だけ補間復号します
- 遅延が大きい場合や補間復号に失敗した場合は、現在フレームを破棄して Idle に戻り再同期します

## コーディング規約

- 公開 API 名は既存互換を優先し、破壊的なリネームは避ける
- 内部状態は `_member` で保持し、namespace スコープで先頭 `_` の識別子は使わない
- 低レベル処理は早期 return で分岐を浅く保つ
- 時刻計算は `micros` 基準とし、`time_t` は `Types.hpp` の定義を利用する

## タイミング調整の目安

- half_bit_us: 伝送速度の主設定。小さいほど高速
- margin_us: 受信時の許容ジッタ
- threshold_window_us: baseline 計測に使う窓幅
- buffer_size: PrefixSumWindow の保持点数

受信が不安定な場合は、half_bit_us と margin_us を先に調整し、次に threshold_window_us を調整してください。

## 計算量メモ

- Sender.update: O(1)
- Receiver.update: 平均計算の lower_bound を含み O(log N)
- PrefixSumWindow.push: O(1)
- PrefixSumWindow.average: O(log N)

## 注意点

- Types.hpp の time_t は unsigned long を想定
- millis ではなく micros ベースで動作
- 実機チューニング時は analogRead の実行時間も考慮する
