/*
 *  本示例演示 WiFiMulti 库的高级用法：
 *  - 多 AP 自动切换（信号最强优先）
 *  - 开放网络自动连接
 *  - 强制门户（Captive Portal）检测
 *  - 非严格模式（允许连接未列出的 AP）
 */

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>

WiFiMulti wifiMulti;

// 网络连通性检测回调
// 连上 WiFi 后尝试访问外网，用于检测"需要弹窗认证"的公共 WiFi
bool testConnection() {
  HTTPClient http;
  http.begin("http://www.espressif.com");
  int httpCode = http.GET();
  // 期望返回 301（重定向到 HTTPS），说明真的能上网
  if (httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
    return true;
  }
  return false;
}

void setup() {
  Serial.begin(115200);
  delay(10);

  // 预存多个 WiFi，自动选信号最强的连
  wifiMulti.addAP("ssid_from_AP_1", "your_password_for_AP_1");
  wifiMulti.addAP("ssid_from_AP_2", "your_password_for_AP_2");
  wifiMulti.addAP("ssid_from_AP_3", "your_password_for_AP_3");

  // ---- 高级选项 ----
  // 非严格模式：当前连的 WiFi 不在列表里也不会断
  wifiMulti.setStrictMode(false);
  // 允许连接无密码的开放网络
  wifiMulti.setAllowOpenAP(true);
  // 设置连通性检测回调（验证是否真的能上网）
  wifiMulti.setConnectionTestCallbackFunc(testConnection);

  Serial.println("正在连接 WiFi...");
  if (wifiMulti.run() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi 已连接");
    Serial.println("IP 地址: ");
    Serial.println(WiFi.localIP());
  }
}

void loop() {
  static bool wasConnected = false;
  uint8_t WiFiStatus = wifiMulti.run();   // 持续运行，自动维护连接

  if (WiFiStatus == WL_CONNECTED) {
    if (!wasConnected) {                  // 从断开→连接，打印一次
      Serial.println("");
      Serial.println("WiFi 已连接");
      Serial.println("IP 地址: ");
      Serial.println(WiFi.localIP());
    }
    wasConnected = true;
  } else {
    Serial.println("WiFi 断开!");
    wasConnected = false;
    delay(5000);
  }
}
