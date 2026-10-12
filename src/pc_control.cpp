#include <Arduino.h>
#include <ArduinoOTA.h>
#include <WebServer.h>
#include <WiFi.h>

#include "DebugLog.h"
#include "pass.h"

namespace {

constexpr uint8_t PIN_CASHEW = 2;
constexpr uint8_t PIN_AZUKI = 3;
constexpr uint32_t PULSE_DURATION_MS = 250;

const IPAddress LOCAL_IP(192, 168, 1, 101);
const IPAddress GATEWAY(192, 168, 1, 1);
const IPAddress SUBNET(255, 255, 255, 0);
const IPAddress DNS(192, 168, 1, 1);

WebServer server(80);

const char PAGE[] PROGMEM = R"HTML(
<!doctype html>
<html lang="ja">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>PC Control</title>
  <style>
    :root { color-scheme: light; font-family: system-ui, sans-serif; background: #eef2f1; color: #172321; }
    * { box-sizing: border-box; }
    body { margin: 0; min-height: 100vh; display: grid; place-items: center; padding: 24px; }
    main { width: min(100%, 520px); }
    h1 { margin: 0 0 8px; font-size: 1.8rem; }
    p { margin: 0 0 24px; color: #52625e; }
    .controls { display: grid; grid-template-columns: repeat(2, minmax(0, 1fr)); gap: 14px; }
    button { min-height: 132px; padding: 20px 12px; border: 1px solid #c9d5d1; border-radius: 8px; background: #fff; color: inherit; font: inherit; cursor: pointer; }
    button:hover { border-color: #16856e; background: #f6fbf9; }
    button:focus-visible { outline: 3px solid #16856e; outline-offset: 3px; }
    button:disabled { cursor: wait; opacity: .65; }
    button strong, button span { display: block; }
    button strong { margin-bottom: 8px; font-size: 1.15rem; }
    button span { color: #52625e; font-size: .9rem; }
    #status { min-height: 1.5em; margin-top: 18px; color: #176b58; }
    @media (max-width: 380px) { .controls { grid-template-columns: 1fr; } button { min-height: 100px; } }
  </style>
</head>
<body>
  <main>
    <h1>PC Control</h1>
    <p>操作するPCを選択してください</p>
    <div class="controls">
      <button type="button" data-endpoint="/api/pc/1/power"><strong>CASHEW</strong><span>電源スイッチを押す</span></button>
      <button type="button" data-endpoint="/api/pc/2/power"><strong>AZUKI</strong><span>電源スイッチを押す</span></button>
    </div>
    <div id="status" role="status" aria-live="polite"></div>
  </main>
  <script>
    const status = document.querySelector('#status');
    document.querySelectorAll('button[data-endpoint]').forEach((button) => {
      button.addEventListener('click', async () => {
        button.disabled = true;
        status.textContent = '送信中...';
        try {
          const response = await fetch(button.dataset.endpoint, { method: 'POST' });
          if (!response.ok) throw new Error('request failed');
          status.textContent = `${button.querySelector('strong').textContent} に信号を送りました`;
        } catch {
          status.textContent = '送信に失敗しました';
        } finally {
          button.disabled = false;
        }
      });
    });
  </script>
</body>
</html>
)HTML";

void pulseGpio(uint8_t gpio) {
  digitalWrite(gpio, HIGH);
  delay(PULSE_DURATION_MS);
  digitalWrite(gpio, LOW);
}

void sendPulseResponse(uint8_t gpio) {
  pulseGpio(gpio);
  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "text/plain; charset=utf-8", "OK");
  DebugLog.print("Pulse sent to GPIO ");
  DebugLog.println(gpio);
}

void connectToWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.config(LOCAL_IP, GATEWAY, SUBNET, DNS);
  WiFi.begin(SSID, PASSWORD);

  DebugLog.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    DebugLog.print('.');
  }
  DebugLog.println();
  DebugLog.print("Web server: http://");
  DebugLog.println(WiFi.localIP());
}

}  // namespace

void setup() {
  DebugLog.begin(115200);

  digitalWrite(PIN_CASHEW, LOW);
  digitalWrite(PIN_AZUKI, LOW);
  pinMode(PIN_CASHEW, OUTPUT);
  pinMode(PIN_AZUKI, OUTPUT);

  connectToWifi();

  server.on("/", HTTP_GET, []() {
    server.send_P(200, "text/html; charset=utf-8", PAGE);
  });
  server.on("/api/pc/1/power", HTTP_POST, []() {
    sendPulseResponse(PIN_CASHEW);
  });
  server.on("/api/pc/2/power", HTTP_POST, []() {
    sendPulseResponse(PIN_AZUKI);
  });
  server.begin();

  // OTAの初期化
	ArduinoOTA.begin();

}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectToWifi();
  }

  server.handleClient();
  ArduinoOTA.handle();
  DebugLog.handle();

  delay(10);
}