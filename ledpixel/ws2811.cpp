


#include "ws2811.h"
#include <SPI.h>

#define SPI_LOW 0x08
#define SPI_HIGH 0x0c

#define BIT_BUFFER_SIZE ((LED_COUNT * 4 * 3) + 3)

Color line[LED_COUNT];
//uint8_t bitsBuffer[BIT_BUFFER_SIZE];

static uint8_t hslrgbState = 0; //default in RGB mode
static bool ws2812State = true;
static SPISettings spiSettings(800000*4, MSBFIRST, SPI_MODE1);
static const uint8_t SS_PIN = 12;


void hslToRgb(uint8_t h, uint8_t s, uint8_t l, uint8_t *r,uint8_t *g, uint8_t *b)
{
    uint8_t region, remainder, p, q, t;

    if (s == 0)
    {
        *r = l;
        *g = l;
        *b = l;
        return ;
    }

    region = h / 43;
    remainder = (h - (region * 43)) * 6;

    p = (l * (255 - s)) >> 8;
    q = (l * (255 - ((s * remainder) >> 8))) >> 8;
    t = (l * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

    switch (region)
    {
        case 0:
            *r = l; *g = t; *b = p;
            break;
        case 1:
            *r = q; *g = l; *b = p;
            break;
        case 2:
            *r = p; *g = l; *b = t;
            break;
        case 3:
            *r = p; *g = q; *b = l;
            break;
        case 4:
            *r = t; *g = p; *b = l;
            break;
        default:
            *r = l; *g = p; *b = q;
            break;
    }
}

void rgbToHsl(uint8_t r,uint8_t g, uint8_t b, uint8_t *h, uint8_t *s, uint8_t *l)
{
    uint8_t rgbMin, rgbMax;

    rgbMin = r < g ? (r < b ? r : b) : (g < b ? g : b);
    rgbMax = r > g ? (r > b ? r : b) : (g > b ? g : b);

    *l = rgbMax;
    if (*l == 0)
    {
        *h = 0;
        *s = 0;
        return ;
    }

    *s = 255 * long(rgbMax - rgbMin) / *l;
    if (*s == 0)
    {
        *h = 0;
        return ;
    }

    if (rgbMax == r)
        *h = 0 + 43 * (g - b) / (rgbMax - rgbMin);
    else if (rgbMax == g)
        *h = 85 + 43 * (b - r) / (rgbMax - rgbMin);
    else
        *h = 171 + 43 * (r - g) / (rgbMax - rgbMin);
}

void ws2811Init( uint8_t hslrgb, bool ws2812){

    hslrgbState = hslrgb;
    ws2812State = ws2812;
    if (ws2812){
      spiSettings.setClock(800000*4);
    }else{
      spiSettings.setClock(400000*4);
    }
    SPI.begin();
}

void setBits(uint8_t *bitsBuffer, uint32_t offset, uint8_t byte){

    bitsBuffer[offset + 0] = ((byte & 0x80)? SPI_HIGH:SPI_LOW) << 4;
    bitsBuffer[offset + 0] |= ((byte & 0x40)? SPI_HIGH:SPI_LOW);
    bitsBuffer[offset + 1] = ((byte & 0x20)? SPI_HIGH:SPI_LOW) << 4;
    bitsBuffer[offset + 1] |= ((byte & 0x10)? SPI_HIGH:SPI_LOW);
    bitsBuffer[offset + 2] = ((byte & 0x08)? SPI_HIGH:SPI_LOW) << 4;
    bitsBuffer[offset + 2] |= ((byte & 0x04)? SPI_HIGH:SPI_LOW);
    bitsBuffer[offset + 3] = ((byte & 0x02)? SPI_HIGH:SPI_LOW) << 4;
    bitsBuffer[offset + 3] |= ((byte & 0x01)? SPI_HIGH:SPI_LOW);

/*
    bitsBuffer[offset + 0] = (byte & 0x80)? HPWM:LPWM;
    bitsBuffer[offset + 1] = (byte & 0x40)? HPWM:LPWM;
    bitsBuffer[offset + 2] = (byte & 0x20)? HPWM:LPWM;
    bitsBuffer[offset + 3] = (byte & 0x10)? HPWM:LPWM;
    bitsBuffer[offset + 4] = (byte & 0x08)? HPWM:LPWM;
    bitsBuffer[offset + 5] = (byte & 0x04)? HPWM:LPWM;
    bitsBuffer[offset + 6] = (byte & 0x02)? HPWM:LPWM;
    bitsBuffer[offset + 7] = (byte & 0x01)? HPWM:LPWM;
*/
}


void setColors(uint8_t *bitsBuffer){

    uint8_t r,g,b;
    //GRB
    uint32_t offset = 0;
    for(int i = 0; i < LED_COUNT; ++i){
        if (hslrgbState == 0){
            r = line[i].a;
            g = line[i].b;
            b = line[i].c;
        }else{
            hslToRgb(line[i].a,line[i].b,line[i].c, &r,&g,&b);
        }
        if(ws2812State){
          setBits(bitsBuffer,offset + 0,g);
          setBits(bitsBuffer,offset + 4,r);
          setBits(bitsBuffer,offset + 8,b);
        }else{
          setBits(bitsBuffer,offset + 0,r);
          setBits(bitsBuffer,offset + 4,g);
          setBits(bitsBuffer,offset + 8,b);
      }
        offset+=12;
    }

    bitsBuffer[BIT_BUFFER_SIZE-1] = 0x00;
    bitsBuffer[BIT_BUFFER_SIZE-2] = 0x00;
    bitsBuffer[BIT_BUFFER_SIZE-3] = 0x00;
}
void ws2811Clear(uint8_t hslrgb)
{
    hslrgbState = hslrgb;

    for(int i = 0; i < LED_COUNT; ++i){
        line[i].a = 0;
        line[i].b = 0;
        line[i].c = 0;
    }

}

void ws2811Sync(void){
    uint8_t bitsBuffer[BIT_BUFFER_SIZE];
    setColors(bitsBuffer);

    SPI.beginTransaction(SS_PIN, spiSettings);
    SPI.writeDMA((void *) bitsBuffer, BIT_BUFFER_SIZE);
    SPI.endTransaction();
}

void ws2811SetPixelHSL(int index, uint8_t h, uint8_t s, uint8_t l){
  if(hslrgbState == 1){
      line[index].a = h;
      line[index].b = s;
      line[index].c = l;
  }else{
      uint8_t r,g,b;
      hslToRgb(h, s, l, &r, &g, &b);
      line[index].a = r;
      line[index].b = g;
      line[index].c = b;
  }
}

void ws2811SetPixelRGB(int index, uint8_t r, uint8_t g, uint8_t b){
  if(hslrgbState == 0){
      line[index].a = r;
      line[index].b = g;
      line[index].c = b;
  }else{
      uint8_t h,s,l;
      rgbToHsl(r, g, b, &h, &s, &l);
      line[index].a = h;
      line[index].b = s;
      line[index].c = l;
    }
}

void ws2811RotateLeft(void){

    uint8_t a,b,c;
    a = line[0].a;
    b = line[0].b;
    c = line[0].c;

    for (int i = 1; i < LED_COUNT; ++i){
        line[i-1].a = line[i].a;
        line[i-1].b = line[i].b;
        line[i-1].c = line[i].c;
    }
    line[LED_COUNT - 1].a = a;
    line[LED_COUNT - 1].b = b;
    line[LED_COUNT - 1].c = c;
}

void ws2811RotateRight(void){

    uint8_t a,b,c;
        a = line[LED_COUNT - 1].a;
        b = line[LED_COUNT - 1].b;
        c = line[LED_COUNT - 1].c;

    for (int i = LED_COUNT - 1; i >= 1; --i){
        line[i].a = line[i-1].a;
        line[i].b = line[i-1].b;
        line[i].c = line[i-1].c;
    }
    line[0].a = a;
    line[0].b = b;
    line[0].c = c;
}
