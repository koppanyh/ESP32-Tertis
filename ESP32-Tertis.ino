// WiFi Captive Portal "Tertis", by @koppanyh

#include <DNSServer.h>
#include <WiFi.h>

#include "display.h"
#include "tertis.h"
#include "web.h"

Display display;
Tertis tertis(&display);

DNSServer dns_server;
Web web(&tertis);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  Serial.println("BOOT: ESP32 Tertis by @koppany. Copyleft 2022");
  Serial.println("BOOT: ESP32 Tertis Starting!");
  display.Init();

  Serial.println("BOOT: Setting up captive portal AP.");
  WiFi.softAP("ESP32-Tertis");

  web.Init();
  Serial.print("BOOT: Server started at ");
  Serial.println(WiFi.softAPIP().toString());

  // if DNSServer is started with "*" for domain name, it will reply with
  // provided IP to all DNS request
  dns_server.start(53, "*", WiFi.softAPIP());
  Serial.println("BOOT: AP started at ESP32-Tertis.");
}

int prev_count = 0;
int prev_score = 0;

void loop() {
  if (web.GetWsCount()) {  // Reset game if at least one player joins
    if (!prev_count) {
      tertis.Reset();
      tertis.Stop();  // Stop it so the player can start when they're ready
    }
  } else {  // Play while waiting for a player to join
    tertis.Demo();
  }
  prev_count = web.GetWsCount();

  // Update the state of the game and broadcast any score changes
  tertis.Step();
  if (tertis.GetScore() != prev_score) {
    web.BroadcastScore();
    prev_score = tertis.GetScore();
  }

  // Show us that something is happening and delay
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1);
  digitalWrite(LED_BUILTIN, LOW);
  delay(249);

  // Housekeeping
  dns_server.processNextRequest();
  web.GetWebSocket()->cleanupClients();
}
