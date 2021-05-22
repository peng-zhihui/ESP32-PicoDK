/*
   Copyright (c) 2019 Stefan Kremser
   This software is licensed under the MIT License. See the license file for details.
   Source: github.com/spacehuhn/SimpleCLI

   This example is based on the HelloServer example for the ESP8266.
   You connect to the Access-Point, go to 192.168.4.1 and type in "led on" or "led off".
   The parsing magic is happening in handleRoot().
 */

#include <SimpleCLI.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK  "your-password"
#endif // ifndef STASSID

const char* ssid     = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);
SimpleCLI cli;

String  answer;
Command cmdLed;

void handleRoot() {
    // If data was sent
    if (server.args() > 0) {
        // Echo the input on the serial interface
        Serial.print("# ");
        Serial.println(server.arg("cmd"));

        // Parse input
        cli.parse(server.arg("cmd"));

        // Check for commands
        if (cli.available()) {
            Command cmd = cli.getCommand();

            if (cmd == cmdLed) {
                String mode = cmd.getArgument("mode").getValue();

                if (mode == "on") {
                    pinMode(2, HIGH);
                    answer = "turned LED on";
                } else if (mode == "off") {
                    pinMode(2, LOW);
                    answer = "turned LED off";
                }
            }
        }

        // Check for errors
        if (cli.errored()) {
            answer = cli.getError().toString();
        }
    }

    // Build HTML string
    String html =
        "<!DOCYTPE html>"
        "<html>"
        "<head>"
        "<meta charset=\"utf-8\">"
        "<title>SimpleCLI</title>"
        "</head>"
        "<body>"
        "<p>";

    html += answer;

    html +=
        "</p>"
        "<form action=\"index.html\" method=\"GET\">"
        "<input type=\"text\" name=\"cmd\"></input>"
        "<input type=\"submit\"></input>"
        "</form>"
        "</body>"
        "</html>";

    // Send HTML to user
    server.send(200, "text/html", html.c_str());
}

void handleNotFound() {
    String message = "File Not Found\r\n\r\n";

    message += "URI: ";
    message += server.uri();
    message += "\r\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\r\nArguments: ";
    message += server.args();
    message += "\r\n";

    for (uint8_t i = 0; i < server.args(); i++) {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\r\n";
    }
    server.send(404, "text/plain", message);
}

void setup(void) {
    Serial.begin(115200);
    WiFi.softAP(ssid, password);

    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    server.on("/", handleRoot);
    server.begin();
    Serial.println("HTTP server started");

    if (MDNS.begin("esp8266")) {
        Serial.println("MDNS responder started");
    }

    server.on("/", handleRoot);
    server.on("/index.html", handleRoot);

    server.on("/inline", []() {
        server.send(200, "text/plain", "this works as well");
    });

    server.onNotFound(handleNotFound);

    cmdLed = cli.addCommand("led");
    cmdLed.addPositionalArgument("mode", "on");

    server.begin();
    Serial.println("HTTP server started");
}

void loop(void) {
    server.handleClient();
    MDNS.update();
}