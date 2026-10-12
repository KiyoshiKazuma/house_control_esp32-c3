#pragma once

#include <Arduino.h>

// 出力先は build_flags で選択: -DDEBUG_LOG_OUTPUT=DEBUG_LOG_OUTPUT_USB (既定)
#define DEBUG_LOG_OUTPUT_NONE 0
#define DEBUG_LOG_OUTPUT_USB 1
#define DEBUG_LOG_OUTPUT_TELNET 2

#ifndef DEBUG_LOG_OUTPUT
#define DEBUG_LOG_OUTPUT DEBUG_LOG_OUTPUT_USB
#endif

#ifndef DEBUG_LOG_TELNET_PORT
#define DEBUG_LOG_TELNET_PORT 23
#endif

class DebugLogClass : public Print {
 public:
  void begin(unsigned long baud = 115200);
  // TELNET時はloop()から呼ぶ。接続受付をWi-Fi接続後に遅延開始する。
  void handle();

  size_t write(uint8_t c) override;
  size_t write(const uint8_t* buf, size_t size) override;
};

extern DebugLogClass DebugLog;
