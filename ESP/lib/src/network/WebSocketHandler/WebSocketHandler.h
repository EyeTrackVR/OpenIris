//
// Created by lorow on 17.04.2023.
//

#ifndef ESP_WEBSOCKETHANDLER_H
#define ESP_WEBSOCKETHANDLER_H

#define WS_MAX_QUEUED_MESSAGES 2
#define DEFAULT_MAX_WS_CLIENTS 1

#include <Arduino.h>
#include "AsyncTCP.h"
#include "ESPAsyncWebServer.h"

class WebSocketHandler {
private:
    AsyncWebServer &server;
    AsyncWebSocket webSocket = AsyncWebSocket("/ws");
    AsyncWebSocketClient *client = nullptr;

    unsigned long lastTimeAFrameWasSent = 0;
    static const int fps = 30;
    static constexpr int millisecondsBetweenSendAttempts = 1000/fps;

public:
    explicit WebSocketHandler(AsyncWebServer& server);
    void begin();
    void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
    void stream();
};

#endif //ESP_WEBSOCKETHANDLER_H
