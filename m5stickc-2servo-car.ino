#include <M5StickC.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

enum {
  STANDBY= 0,
  STOP = 1,
  RUN = 2,
} g_Status = STANDBY;

enum {
  FORWARD = 0,
  LEFT = 1,
  RIGHT = 2,
} g_Direction = FORWARD;

WebServer server;

Servo servoL;
Servo servoR;

int minUs = 500;
int maxUs = 2400;

int servoLPin = 32;
int servoRPin = 33;

#define LCD_COLOR_WHITE M5.Lcd.color565(0xf0, 0xf0, 0xf0)
#define LCD_COLOR_RED   M5.Lcd.color565(0xf0, 0x00, 0x00)
#define LCD_COLOR_GREEN M5.Lcd.color565(0x00, 0xf0, 0x00)
#define LCD_COLOR_BLUE  M5.Lcd.color565(0x00, 0x00, 0xf0)

void setup() {
  M5.begin();
  Serial.begin(115200);

  WiFi.softAP("M5STICK", "12345678");

  server.on("/", handleGetIndex);
  server.on("/stop", handleGetStop);
  server.on("/run",  handleGetRun);
  server.on("/forward", handleGetForward);
  server.on("/left",    handleGetLeft);
  server.on("/right",   handleGetRight);
  server.begin(80);

  servoL.setPeriodHertz(50);
  servoL.attach(servoLPin, minUs, maxUs);
  servoL.write(90);

  servoR.setPeriodHertz(50);
  servoR.attach(servoRPin, minUs, maxUs);
  servoR.write(90);

  updateUIStatus();
}

void loop() {
  M5.update();

  if (M5.BtnA.wasPressed()) {
    handleButtonClick();
  }

  server.handleClient();

  updateUIStatus();
}

void updateUIStatus() {
  switch (g_Status) {
    case STANDBY:
      M5.Lcd.fillScreen(LCD_COLOR_WHITE);
      break;
    case STOP:
      M5.Lcd.fillScreen(LCD_COLOR_RED);
      break;
    case RUN:
      M5.Lcd.fillScreen(LCD_COLOR_GREEN);
      break;
  }
}

void handleButtonClick() {
  stop();
}

void forward() {
  g_Direction = FORWARD;
  if (g_Status == RUN) {
    run();
  }
}

void left() {
  g_Direction = LEFT;
  if (g_Status == RUN) {
    run();
  }
}

void right() {
  g_Direction = RIGHT;
  if (g_Status == RUN) {
    run();
  }
}

void stop() {
  g_Status = STOP;

  servoL.write(90);
  servoR.write(90);
}

void run() {
  g_Status = RUN;

  switch (g_Direction) {
    case FORWARD:
      servoL.write(120);
      servoR.write(60);
      break;
    case LEFT:
      servoL.write(100);
      servoR.write(60);
      break;
    case RIGHT:
      servoL.write(120);
      servoR.write(80);
      break;
  }
}

void handleGetIndex() {
  server.send(200, "text/html",
"<html lang='ja'>"
"<head>"
"<meta charset='UTF-8'>"
"<meta name='viewport' content='width=device-width,initial-scale=1'/>"
"<title>M5StickC 2Servo Car</title>"
"<style>"
"button {font-size:large;}"
"</style>"
"</head>"
"<body>"
"<center>"
"<h1>M5StickC 2Servo Car</h1>"
"<button type='button' id='left'    onclick='handleButtonClick(this)'>LEFT</button>"
"<button type='button' id='forward' onclick='handleButtonClick(this)'>FORWARD</button>"
"<button type='button' id='right'   onclick='handleButtonClick(this)'>RIGHT</button>"
"<p/>"
"<button type='button' id='stop' onclick='handleButtonClick(this)'>STOP</button>"
"<button type='button' id='run'  onclick='handleButtonClick(this)'>RUN</button>"
"</center>"
"<script type='text/javascript'>"
"function handleButtonClick(button) {"
"let request = new XMLHttpRequest();"
"request.open('GET', 'http://192.168.4.1/'+button.id);"
"request.send(null);"
"}"
"</script>"
"</body>"
"</html");
}

void handleGetLeft() {
  left();
  server.send(200, "text/plain", "OK");
}

void handleGetForward() {
  forward();
  server.send(200, "text/plain", "OK");
}

void handleGetRight() {
  right();
  server.send(200, "text/plain", "OK");
}

void handleGetStop() {
  stop();
  server.send(200, "text/plain", "OK");
}

void handleGetRun() {
  run();
  server.send(200, "text/plain", "OK");
}
