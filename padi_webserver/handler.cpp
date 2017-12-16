
#include "handler.h"
#include "common.h"
#include <Arduino.h>
#include <PadiWebServer.h>

void handleCmd() {
  rtl_printf("i am in \n");
  String str = server.arg("plain");
  for (int i = 0; i < str.size(); ++i) {
    rtl_printf(" %d ", str.data()[i]);
  }
  rtl_printf("\n");
  server.send(200, "application/octet-stream", str);
}
