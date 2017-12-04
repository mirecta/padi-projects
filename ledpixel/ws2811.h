
#ifndef _WS2811_H_
#define _WS2811_H_
#include <inttypes.h>

#define LED_COUNT 8

struct  Color {
    uint8_t a;
    uint8_t b;
    uint8_t c;
} __attribute__((packed));


extern Color line[LED_COUNT];


void ws2811Init(uint8_t hslrgb = 0, bool ws2812 = true);
void ws2811Sync(void);
void ws2811Clear(uint8_t hslrgb = 0);

void ws2811SetPixelHSL(int index, uint8_t h, uint8_t s, uint8_t l);
void ws2811SetPixelRGB(int index, uint8_t r, uint8_t g, uint8_t b);
void ws2811RotateLeft(void);
void ws2811RotateRight(void);



#endif //_WS2811_H_
