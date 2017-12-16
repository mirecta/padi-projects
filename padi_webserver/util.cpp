
#include "util.h"
#include <Arduino.h>
#include <WiFi.h>


const uint8_t config0[] = "mode=";
const uint8_t config1[] = "channel=";
const uint8_t config2[] = "ssid=";
const uint8_t config3[] = "passwd=";

int wifi_status = WL_IDLE_STATUS; // the Wifi radio's status

bool streq(const char *buff1, const char *buff2) {
  int len1 = getlen(buff1);
  int len2 = getlen(buff2);
  if (len1 != len2) {
    return false;
  }
  for (int i = 0; i < len1; ++i) {
    if (buff1[i] != buff2[i]) {
      return false;
    }
  }
  return true;
}

int getlen(const char *buffer) {
  // obtain len
  int len = 0;
  char c = buffer[len];
  while (c != '\0') {
    ++len;
    c = buffer[len];
  }
  return len;
}

int scpy(char *src, char *dst, int dsize) {
  int count = getlen(src);
  if (count >= dsize) {
    count = dsize - 1;
  }
  dst[count] = '\0';
  for (int i = 0; i < count; ++i) {
    dst[i] = src[i];
  }
  return count;
}

int split(char *buffer, char **strs, const char delimiter, int maxparts) {

  // obtain len
  int len = getlen(buffer);
  int split = 0;
  strs[split] = buffer;

  for (int i = 0; i < len; ++i) {
    if (buffer[i] == delimiter) {
      buffer[i] = '\0';
      if (i + 1 != len) {
        ++split;
        strs[split] = &buffer[i + 1];
        if (split == maxparts - 1)
          break;
      }
    }
  }
  return split + 1;
}

bool space(char c) {
  if ((c >= 0x09 && c <= 0x0d) || c == 0x20) {
    return true;
  }
  return false;
}

char *strip(char *buffer) {

  // obtain len
  int len = getlen(buffer);
  int i;
  for (i = len - 1; i >= 0; --i) {
    if (space(buffer[i])) {
      buffer[i] = '\0';
    } else {
      break;
    }
  }
  len = i + 1;
  for (i = 0; i < len; ++i) {
    if (!space(buffer[i])) {
      break;
    }
  }
  return &buffer[i];
}

int writeConfig(SdFatFs &fat, const Config_t &cfg) {
  int res = 0;
  res = fat.mkdir(CONFIG_DIR);
  if (res < 0) {
    return res;
  }
  SdFatFile file = fat.open(CONFIG, "w");
  // mode
  file.write(config0, sizeof(config0) - 1);
  if (cfg.ap) {
    file.write((const uint8_t *)("ap\n"), 3);
  } else {
    file.write((const uint8_t *)("client\n"), 7);
  }
  // channel
  file.write(config1, sizeof(config1) - 1);
  file.write((const uint8_t *)cfg.channel, getlen(cfg.channel));
  file.write((const uint8_t *)("\n"), 1);
  // ssid
  file.write(config2, sizeof(config2) - 1);
  file.write((const uint8_t *)cfg.ssid, getlen(cfg.ssid));
  file.write((const uint8_t *)("\n"), 1);
  // passwd
  file.write(config3, sizeof(config3) - 1);
  file.write((const uint8_t *)cfg.passwd, getlen(cfg.passwd));
  file.write((const uint8_t *)("\n"), 1);
  file.close();
  return res;
}

int readConfig(SdFatFs &fat, Config_t &cfg) {

  char buf[128];
  char *parts[2];
  int rd;

  int res = 0;

  if (fat.isFile(CONFIG) != 1) {
    writeConfig(fat, cfg);
  }
  SdFatFile file = fat.open(CONFIG, "r");
  // memset(buf, 0, sizeof(buf));
  cfg.ap = false;
  cfg.channel[0] = '1';
  cfg.channel[1] = '\0';
  do {
    rd = file.readline(buf, sizeof(buf));
    // int c = getlen(buf);
    int c = split(buf, parts, '=');
    if (c != 2)
      continue;

    if (streq(strip(parts[0]), "mode")) {
      if (streq(strip(parts[1]), "ap")) {

        cfg.ap = true;
      }
    }
    if (streq(strip(parts[0]), "channel")) {
      scpy(strip(parts[1]), cfg.channel, sizeof(cfg.channel));
    }
    if (streq(strip(parts[0]), "ssid")) {
      scpy(strip(parts[1]), cfg.ssid, sizeof(cfg.ssid));
    }
    if (streq(strip(parts[0]), "passwd")) {
      scpy(strip(parts[1]), cfg.passwd, sizeof(cfg.passwd));
    }
    /*if (c == 2){
      printf(">>>%s<<< >>>%s<<<<\n", strip(parts[0]),strip(parts[1]));
    }*/
  } while (rd != -1);
  file.close();
  return res;
}

void setupWIFI(Config_t &cfg) {
  
  WiFi.status(); // this magic init wifi
  if (cfg.ap) {
    // if(1){
    // attempt to start AP:
    while (wifi_status != WL_CONNECTED) {
      printf("Attempting to start AP with SSID:%s\n ", cfg.ssid);
      wifi_status = WiFi.apbegin(cfg.ssid, cfg.passwd, cfg.channel);
      delay(10000);
    }

  } else {
    // attempt to connect to Wifi network:
    while (wifi_status != WL_CONNECTED) {
      printf("Attempting to connect to WPA SSID: %s\n", cfg.ssid);
      // Connect to WPA/WPA2 network:
      wifi_status = WiFi.begin(cfg.ssid, cfg.passwd);
      // wait 10 seconds for connection:
      delay(20000);
    }
    IPAddress ip = WiFi.localIP();
    printf("IP is %s\n ", ip.get_address());
  }
}
