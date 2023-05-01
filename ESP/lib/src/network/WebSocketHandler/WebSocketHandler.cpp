//
// Created by lorow on 17.04.2023.
//

#include "WebSocketHandler.h"
#include <esp_camera.h>

WebSocketHandler::WebSocketHandler(AsyncWebServer &server) : server(server){}

void WebSocketHandler::begin() {
    this->server.addHandler(&this->webSocket);
    this->webSocket.onEvent([&](AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
        this->onEvent(server, client, type,  arg, data, len);
    });
}

void WebSocketHandler::onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg,
                               uint8_t *data, size_t len) {
    if(type == WS_EVT_CONNECT){
        Serial.println("Websocket client connection received");
        this->client = client;

    } else if(type == WS_EVT_DISCONNECT){
        Serial.println("Client disconnected");
        this->client = nullptr;
    }
}

void WebSocketHandler::stream() {
    if (this->client && this->client->status() == WS_CONNECTED){
        if (millis() - lastTimeAFrameWasSent < millisecondsBetweenSendAttempts){
            return;
        }

        unsigned long newTime = millis();
        this->lastTimeAFrameWasSent = newTime;
        camera_fb_t* fb = esp_camera_fb_get();
        if(!fb){
            log_e("Camera capture failed, a problem with the sensor detected");
        }
        this->webSocket.binaryAll(fb->buf, fb->len);
        esp_camera_fb_return(fb);
    }
}