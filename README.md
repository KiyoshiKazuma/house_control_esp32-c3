# ESP32 Smart Home Hub

ESP32 をベースにしたスマートホームハブです。Panasonic 製エアコンの赤外線制御、照明制御、環境センサーの取得、Web UI からの操作を一体的に扱える構成を目指しています。

## 1. プロジェクト概要

このプロジェクトは、組み込み機器の制御と Web インターフェースを分離し、各責務を層ごとに整理した設計を採用しています。設計方針の詳細は [doc/ARCHITECTURE_DESIGN.md](doc/ARCHITECTURE_DESIGN.md) に記載しています。

### 主な機能
- Panasonic 製エアコンの赤外線制御
- 照明やスイッチ操作の制御
- BME280 による温度・湿度・気圧の計測
- LittleFS 上に配置した Web UI での状態確認・操作
- I2C / GPIO / Wi-Fi 等のデバイス制御をレイヤー分離して管理

---

## 2. アーキテクチャ方針

保守性・拡張性・テスト性を高めるため、以下の 3 層構成を基本とします。

| レイヤー | 主な責務 | 代表コンポーネント |
| --- | --- | --- |
| Application | API 処理、ユースケース、外部インターフェース | Web, Controller |
| Service | ドメインロジック、機器制御の調整 | AC, RoomLight, Sensor, Joystick, LCD, Switch, Network |
| Hardware | デバイスとの直接通信 | I2C, IR |

### 依存方向
- Application → Service → Hardware の方向に依存します。
- Hardware から Application への依存は避けます。
- 同一層の依存は最小限にします。

---

## 3. ソース構成

実装方針として、機能単位と階層単位を分けて整理します。

```text
src/
├── Application/
│   ├── Controler/
│   └── Web/
├── Service/
│   ├── ACC/
│   ├── Joystick/
│   ├── LCD/
│   ├── Net/
│   ├── RoomLight/
│   ├── Sensor/
│   └── Switch/
├── Hardware/
│   ├── I2C/
│   └── IR/
├── Common/
└── main.cpp
```

### 補足
- `src/old/` には既存実装の参考資料や移行前コードを配置しています。
- `data/` には Web UI 用の静的ファイルを配置します。
- `include/` には設定ファイルや秘密情報を格納します。

---

## 4. ハードウェア構成

### 主な部品
- ESP32-C3
- BME280（温度・湿度・気圧センサー）
- 赤外線送信回路
- 赤外線受信モジュール（必要に応じて解析用）
- GPIO を用いたスイッチ制御回路

### 代表的なピン例
| 機能 | ピン | 備考 |
| --- | --- | --- |
| IR TX | 18 | 赤外線送信用 |
| SDA | 21 | I2C データ |
| SCL | 22 | I2C クロック |
| PC_SW_1 | 16 | 電源スイッチ用 |
| PC_SW_2 | 17 | 電源スイッチ用 |

---

## 5. 開発環境

### 必要ツール
- PlatformIO
- VS Code（推奨）
- ESP32 用のシリアル通信環境

### 依存ライブラリ
- `IRremoteESP8266`
- `Adafruit Unified Sensor`
- `Adafruit BME280 Library`
- `ArduinoJson`
- `ESP32Ping`

---

## 6. セットアップ手順

### 1) 依存関係のインストール
PlatformIO の環境で必要なライブラリを自動取得します。

### 2) ファイルシステムの書き込み
`data/` 配下の HTML / CSS / JavaScript を ESP32 に反映します。

```bash
pio run --target uploadfs
```

### 3) アプリケーションの書き込み
```bash
pio run --target upload
```

### 4) Wi-Fi 設定
`include/pass.h` で以下のように設定してください。

```cpp
#define SSID "your-ssid"
#define PASSWORD "your-password"
```

> `include/pass.h` は秘密情報を含むため、Git 管理対象外にすることを推奨します。

---

## 7. 運用メモ

- 変更後は [doc/ARCHITECTURE_DESIGN.md](doc/ARCHITECTURE_DESIGN.md) の構成と整合性を保つようにしてください。
- 新しい機能を追加する際は、Application / Service / Hardware の責務境界を崩さないようにします。
- 既存コードのリファクタリング時は `src/old/` と比較しながら段階的に移行してください。
