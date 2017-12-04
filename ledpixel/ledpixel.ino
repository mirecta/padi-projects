

#include "ws2811.h"

void UserPreInit(void)
{
  Init_CPU_CLK_UART(1, 38400); // 166.7 MHz
  // 0 - 166666666 Hz, 1 - 83333333 Hz, 2 - 41666666 Hz, 3 - 20833333 Hz, 4 - 10416666 Hz, 5 - 4000000 Hz
  // 6 - 200000000 Hz, 7 - 10000000 Hz, 8 - 50000000 Hz, 9 - 25000000 Hz, 10 - 12500000 Hz, 11 - 4000000 Hz
}

void setup() {
 ws2811Init();
 ws2811Clear();
 for(int i = 0; i < 8; ++i){
  ws2811SetPixelHSL(i, 32*i-1, 255,20); 
 }

}

void loop() {
  ws2811Sync();
  delay(100);
  ws2811RotateRight();
  rtl_printf("send\n");
}
