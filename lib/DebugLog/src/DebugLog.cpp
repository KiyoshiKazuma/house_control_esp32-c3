#include "DebugLog.h"

#if DEBUG_LOG_OUTPUT == DEBUG_LOG_OUTPUT_TELNET
#include <WiFi.h>

namespace {
WiFiServer telnetServer(DEBUG_LOG_TELNET_PORT);
WiFiClient telnetClient;
bool telnetStarted = false;
}  // namespace
#endif

DebugLogClass DebugLog;

void DebugLogClass::begin(unsigned long baud) {
#if DEBUG_LOG_OUTPUT == DEBUG_LOG_OUTPUT_USB
  Serial.begin(baud);
#else
  (void)baud;
#endif
}

void DebugLogClass::handle() {
#if DEBUG_LOG_OUTPUT == DEBUG_LOG_OUTPUT_TELNET
  if (!telnetStarted) {
    if (WiFi.status() != WL_CONNECTED) return;
    telnetServer.begin();
    telnetStarted = true;
  }
  WiFiClient incoming = telnetServer.available();
  if (incoming) {
    if (telnetClient) telnetClient.stop();
    telnetClient = incoming;
  }
#endif
}

size_t DebugLogClass::write(uint8_t c) {
  return write(&c, 1);
}

size_t DebugLogClass::write(const uint8_t* buf, size_t size) {
#if DEBUG_LOG_OUTPUT == DEBUG_LOG_OUTPUT_USB
  return Serial.write(buf, size);
#elif DEBUG_LOG_OUTPUT == DEBUG_LOG_OUTPUT_TELNET
  if (telnetClient && telnetClient.connected()) {
    return telnetClient.write(buf, size);
  }
  return size;
#else
  (void)buf;
  return size;
#endif
}
