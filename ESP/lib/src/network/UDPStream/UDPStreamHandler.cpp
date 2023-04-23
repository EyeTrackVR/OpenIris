#include "UDPStreamHandler.h"
#include "data/StateManager/StateManager.hpp"
#include <esp_camera.h>


UDPStreamHandler::UDPStreamHandler(ProjectConfig& configManager) : configManager(configManager) {}

void UDPStreamHandler::update(ConfigState_e event) {
    if (event == ConfigState_e::deviceConfigUpdated)
        this->begin();
}

std::string UDPStreamHandler::getName() {
    return "UDPStreamHandler";
}


void UDPStreamHandler::begin() {
    auto deviceConfig =  this->configManager.getDeviceConfig();
    this->port = deviceConfig.UDPPort;

    auto address = IPAddress(255,255,255,255);
    this->udp.connect(address, this->port);
    if(this->udp.connected()){
        this->udp.onPacket([&](AsyncUDPPacket packet){
            this->calculateLatency(packet);
        });
    }
}

void UDPStreamHandler::stop() {
    if (this->udp.connected()){
        this->udp.close();
    }
}

bool UDPStreamHandler::canStream() {
    // we should stream if:
    auto streamState = streamStateManager.getCurrentState();

    // we were told to stream and there wasn't any errors, we're connected
    return streamState == DeviceStates::StreamState_e::Stream_ON && this->udp.connected() &&
    // and we're past the timeout
    (millis() - this->lastTimeFrameSentTime < UDPStreamHandler::timeBetweenFrames);
}

void UDPStreamHandler::stream() {
    if(!this->canStream()){
        return;
    }

    AsyncUDPMessage message = AsyncUDPMessage();
    message.write(this->lastFrameID);
    message.write('\xff\xd9');

    message.write(this->lastFrameTimestamp);

    camera_fb_t* fb = esp_camera_fb_get();
    if(!fb){
        log_e("Camera capture failed");
    }
    message.write(fb->buf, fb->len);
    esp_camera_fb_return(fb);
    this->lastFrameID++;
    this->lastFrameTimestamp = millis();
    // add reed solomon error correction here
    this->udp.broadcastTo(message.data(), message.length(), this->port);

    // update the timer because we're done
    this->lastTimeFrameSentTime = millis();
}

void UDPStreamHandler::calculateLatency(AsyncUDPPacket packet) const {
    // todo test if this actually works
    uint8_t* data = packet.data();
    auto timestamp = data - this->lastTimeFrameSentTime;
    Serial.printf("latency: %sms \n\r", timestamp);
}
