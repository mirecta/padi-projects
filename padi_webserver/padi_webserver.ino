/*
 This sketch shows how to open/close file and perform read/write to it.
 */
#include "cmsis_os.h"
#include "SdFatFs.h"
#include "batch.h"
#include "common.h"
#include "handler.h"
#include "rtl_crypto.h"
#include "util.h"
#include <WiFi.h>
#include <PadiWebServer.h>

extern "C" {
void UserPreInit(void) {
  Init_CPU_CLK_UART(1, 38400); // 83.3 MHz
  // 0 - 166666666 Hz, 1 - 83333333 Hz, 2 - 41666666 Hz, 3 - 20833333 Hz, 4 -
  // 10416666 Hz, 5 - 4000000 Hz 6 - 200000000 Hz, 7 - 10000000 Hz, 8 - 50000000
  // Hz, 9 - 25000000 Hz, 10 - 12500000 Hz, 11 - 4000000 Hz
}
} // extern "C"

SdFatFs fs;
Config_t cfg = {"padiap", "password", "1", true};

int status = WL_IDLE_STATUS; // the Wifi radio's status
osThreadId batch_tid = 0;

PadiWebServer server(80);

void handleNotFound() {
  rtl_printf("file %s \n", server.uri().c_str());
  int st = server.serveStatic(fs, "/www/");
  rtl_printf("status %d \n", st);
  if (st != -1)
    return;
  server.send(404, "text/plain", "File Not Found\n\n");
}



void setup() {
  // start batch thread
  osThreadDef(batch_task, osPriorityAboveNormal, 1, 4096 * 4);
  batch_tid = osThreadCreate(osThread(batch_task), NULL);
  InitCryptoEngine();
  sys_info();
  fs.begin();

  readConfig(fs, cfg);
  setupWIFI(cfg);
  server.on("/inline",
            []() { server.send(200, "text/plain", "this works as well"); });
  server.on("/cmd", handleCmd);
  server.onNotFound(handleNotFound);

  server.begin();
  // fs.end();
}

void loop() {
  server.handleClient();
  status = WiFi.status();
  if (status != WL_CONNECTED) {
    rtl_printf("wifi disconnected alert !!! \n");
  }
  delay(300);
}
