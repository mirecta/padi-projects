
#include "handler.h"
#include "common.h"
#include "util.h"
#include <Arduino.h>
#include <PadiWebServer.h>

void handleCmd() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Headers","Content-Type");
  
  if (server.method() == HTTP_OPTIONS){
    server.send(200);
    return;
  }
  
  String data = server.arg("plain");
  int len = data.size();
  
  if (len < 4){
    server.send(400);
    return;
  }
    
  char* payload = data.data();
  //rtl_printf("payload %d, %c\n" ,payload[0],payload[0]); 
  switch (payload[0]){

    case 'w':
      cmdWifi(payload,len);
      return;
      
    default:
      
      return;
  }
  
  rtl_printf("\n");
  
  server.send(200, "application/octet-stream", data);
}

void cmdWifi(char* data, int len){

  if(data[1] == 'g'){ //get wifi
    int sz = (cfg.ap ? 2 : 6) + strlen(cfg.ssid) + strlen(cfg.passwd) + strlen(cfg.channel) + 3 ;
    String body;
    body.reserve(sz+4);
    body += "wg";
    body += (char) 0;
    body += (char) 0;
    body += (cfg.ap ? "ap|": "client|");
    body += cfg.ssid;
    body += '|';
    body += cfg.passwd;
    body += '|';
    body += cfg.channel;
    server.send(200, "application/octet-stream",body);
    return ;
  }
  server.send(400);
}

