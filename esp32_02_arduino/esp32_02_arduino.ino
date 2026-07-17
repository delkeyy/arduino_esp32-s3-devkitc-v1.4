/*
 *  本示例演示 WiFiMulti 库 + WiFi 事件中断：
 *  - 多 AP 自动切换（信号最强优先）
 *  - WiFi 断开时硬件中断瞬间唤醒任务，立即重连
 *  - 连接成功后任务阻塞休眠，0% CPU 占用
 */

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>

WiFiMulti wifiMulti;
TaskHandle_t wifiTaskHandle = NULL;
bool wasConnected = false;

bool testConnection() {
  HTTPClient http;
  http.begin("http://www.espressif.com");
  int httpCode = http.GET();
  http.end();
  return (httpCode == HTTP_CODE_MOVED_PERMANENTLY);
}

// ---- WiFi 硬件中断回调：断开瞬间唤醒任务 ----
void WiFiEvent(WiFiEvent_t event) {
  if (event == ARDUINO_EVENT_WIFI_STA_DISCONNECTED && wifiTaskHandle) {
    xTaskNotifyGive(wifiTaskHandle);   // 通知任务立即重连
  }
}

// ---- WiFi 监控任务（事件驱动） ----
void WifiMonitorTask(void *arg) {
  while(1) {
    uint8_t status = wifiMulti.run();

    if (status == WL_CONNECTED) {
      if (!wasConnected) {
        Serial.println("WiFi 已连接");
        Serial.print("WiFi名称: ");
        Serial.println(WiFi.SSID());
      }
      wasConnected = true;

      // 阻塞等待 WiFi 断开中断（0% CPU，不轮询）
      ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    } else {
      if (wasConnected) {
        Serial.println("WiFi 断开! 5秒后重试...");
      }
      wasConnected = false;

      // 断线：5 秒后重试（期间若中断触发也会立刻唤醒）
      ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(5000));
    }

    UBaseType_t freeStack = uxTaskGetStackHighWaterMark(NULL);
    Serial.printf("WiFi任务剩余最小栈：%u / 4096\r\n", freeStack);
    Serial.printf("剩余堆内存：%u 字节\r\n", ESP.getFreeHeap());
  }
}

void setup() {
  Serial.begin(115200);
  delay(10);

  wifiMulti.addAP("天网疏疏", "00000000");
  wifiMulti.addAP("TP-LINK_D85A", "18012715003");
  wifiMulti.setStrictMode(false);
  wifiMulti.setAllowOpenAP(true);
  wifiMulti.setConnectionTestCallbackFunc(testConnection);

  // 注册 WiFi 事件中断（必须在连接之前）
  WiFi.onEvent(WiFiEvent);

  Serial.println("正在连接 WiFi...");
  wifiMulti.run();  // 尝试初始连接（状态由任务统一打印）

  xTaskCreate(
    WifiMonitorTask,
    "wifimonitor",
    4096,
    NULL,
    1,
    &wifiTaskHandle            // 保存句柄给中断回调用
  );
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}
