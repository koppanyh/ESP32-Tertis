#ifndef ESP_TERTIS_WEB_H_
#define ESP_TERTIS_WEB_H_

#include <AsyncTCP.h>  // https://github.com/me-no-dev/AsyncTCP
#include <ESPAsyncWebServer.h>  // https://github.com/me-no-dev/ESPAsyncWebServer
#include <WiFi.h>

#include "tertis.h"

namespace {

const char main_html[] PROGMEM =
  "<!DOCTYPE html>\n"
  "<html>\n"
  "  <head>\n"
  "    <title>Tertis</title>\n"
  "    <meta charset=\"utf-8\">\n"
  "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
  "    <script>\n"
  "      var exampleSocket = new WebSocket(\"ws://%SOFT_AP_IP%/ws\");\n"
  "      //exampleSocket.onopen = function (event) { exampleSocket.send(\"WebSocket is really cool\"); };\n"
  "      exampleSocket.onmessage = function (event) {\n"
  "        if (event.data.startsWith(\"Score: \"))\n"
  "          document.getElementById(\"score\").innerHTML = event.data.substring(7);\n"
  "        else {\n"
  "          document.getElementById(\"messages\").innerHTML += event.data + \"<br>\";\n"
  "        }\n"
  "      }\n"
  "    </script>\n"
  "  </head>\n"
  "  <body>\n"
  "    <h1>Tertis</h1>\n"
  "    <p>This captive portal is your controller. Win...</p>\n"
  "    <button onclick=\"exampleSocket.send(3);\">Start</button>\n"
  "    <h2>Score: <span id=\"score\"></span></h2>\n"
  "    <div onclick=\"exampleSocket.send(0);\" style=\"background-color: green;\"><br><br><br><br>Rotate<br><br><br><br></div>\n"
  "    <div onclick=\"exampleSocket.send(1);\" style=\"background-color: red;\"><br><br><br><br>Left<br><br><br><br></div>\n"
  "    <div onclick=\"exampleSocket.send(2);\" style=\"background-color: blue;\"><br><br><br><br>Right<br><br><br><br></div>\n"
  "    <br><div id=\"messages\"></div>"
  "  </body>\n"
  "</html>\n";

}  // namespace

class Web {
 public:
  Web(Tertis* tertis) {
    tertis_ = tertis;
  }
  void Init() {
    server_ = new AsyncWebServer(80);
    ws_ = new AsyncWebSocket("/ws");
      
    // Attach AsyncWebSocket
    ws_->onEvent(OnEvent);
    server_->addHandler(ws_);

    // Start the AsyncWebServer
    server_->onNotFound(OnRequest);
    server_->begin();
  }
  AsyncWebSocket* GetWebSocket() {
    return ws_;
  }
  int GetWsCount() {
    return ws_->count();
  }
  void BroadcastScore() {
    ws_->printfAll("Score: %u", tertis_->GetScore());
  }

 private:
  static String Processor(const String& var) {
    if(var == "SOFT_AP_IP")
      return WiFi.softAPIP().toString();
    return String();
  }
  static void OnRequest(AsyncWebServerRequest *request){
    Serial.print("OnRequest: ");
    Serial.println(request->url());
    request->send_P(200, "text/html", main_html, Processor);
  }
  static void OnEvent(AsyncWebSocket* ws, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len){
    //Handle WebSocket event
    if(type == WS_EVT_CONNECT){
      // Client connected
      Serial.print("OnEvent[WS_EVT_CONNECT]: ");
      Serial.println(client->id());
      ws->printfAll("Player TERT%u has joined (%u players)!", client->id(), ws->count());
      client->printf("Hello, Player TERT%u.", client->id());
      client->printf("Score: %u", tertis_->GetScore());
      client->ping();
    } else if(type == WS_EVT_DISCONNECT){
      // Client disconnected
      Serial.print("OnEvent[WS_EVT_DISCONNECT]: ");
      Serial.println(client->id());
    } else if(type == WS_EVT_ERROR){
      // Error was received from the other end
      Serial.print("OnEvent[WS_EVT_ERROR][");
      Serial.print(client->id());
      Serial.print("][");
      Serial.print(*((uint16_t*)arg));
      Serial.print("]: ");
      Serial.println((char*)data);
    } else if(type == WS_EVT_PONG){
      // Pong message was received (in response to a ping request maybe)
      Serial.print("OnEvent[WS_EVT_PONG][");
      Serial.print(client->id());
      Serial.print("][");
      Serial.print(len);
      Serial.print("]: ");
      Serial.println((len)?(char*)data:"");
    } else if(type == WS_EVT_DATA){
      // Button presses from the client
      Serial.print("OnEvent[WS_EVT_DATA][");
      Serial.print(client->id());
      Serial.print("][");
      Serial.print(len);
      Serial.print("]: ");
      Serial.println((len)?(char*)data:"");
      if (len == 0)
        return;
      switch (data[0]) {
        case '0':
          tertis_->Rotate();
          break;
        case '1':
          tertis_->Translate(-1, 0);
          break;
        case '2':
          tertis_->Translate(1, 0);
          break;
        case '3':
          tertis_->Start();
        default:
          break;
      }    }
  }

  static Tertis* tertis_;

  AsyncWebServer* server_;
  AsyncWebSocket* ws_;
};

Tertis* Web::tertis_;

#endif  // ESP_TERTIS_WEB_H_
