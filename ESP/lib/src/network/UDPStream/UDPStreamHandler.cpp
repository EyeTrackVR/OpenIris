#include "UDPStreamHandler.h"
#include "data/StateManager/StateManager.hpp"
#include <esp_camera.h>

const char* const ETVR_HEADER = "\xff\xa0";

UDPStreamHandler::UDPStreamHandler(ProjectConfig& configManager) : configManager(configManager) {}

void UDPStreamHandler::update(ConfigState_e event) {
    if (event == ConfigState_e::configLoaded)
        this->setup();
}

std::string UDPStreamHandler::getName() {
    return "UDPStreamHandler";
}

void UDPStreamHandler::setup(){
    log_d("[SETUP]: Preparing UDP Stream socket, not connected yet, stream not running yet");
    auto deviceConfig =  this->configManager.getDeviceConfig();
    this->port = deviceConfig.UDPPort;
}

void UDPStreamHandler::begin() {
    log_d("[SETUP]: UDP Stream socket connecting to a broadcast address, stream not running yet");
    if (udp.listen(this->port)){
        this->udp.onPacket([&](AsyncUDPPacket packet){
            this->calculateLatency(packet);
        });
    }

//    auto address = IPAddress(255,255,255,255);
//    this->udp.connect(address, this->port);

    // we may need to wait for a bit here, not sure
//    if(this->udp.connected()){
//        log_d("[SETUP]: UDP Stream socket connected! Stream not running yet");
//        this->udp.onPacket([&](AsyncUDPPacket packet){
//            this->calculateLatency(packet);
//        });
//    }
}

void UDPStreamHandler::stop() {
    if (this->udp.connected() && streamStateManager.getCurrentState() == StreamState_e::Stream_STOP){
        this->udp.close();
        streamStateManager.setState(StreamState_e::Stream_OFF);
    }
}

bool UDPStreamHandler::canStream() {
    // we can add some password validation here as well, but make it cheap, like check a bool if authorized
    // and under a map, like ip: authorized, but that would require having a receiver IP, can we do that?
    // we should stream if:
    auto streamState = streamStateManager.getCurrentState();

    // we were told to stream and there wasn't any errors, we're connected
    return streamState == DeviceStates::StreamState_e::Stream_ON && this->udp.connected() &&
    // and we're past the timeout
    (millis() - this->lastTimeFrameSentTime < UDPStreamHandler::timeBetweenFrames);
}

void UDPStreamHandler::stream() {
    if(!this->canStream()){
        this->stop();
        return;
    }

    AsyncUDPMessage message = AsyncUDPMessage();
    message.write(this->lastFrameID);
    message.write((uint8_t)atoi(ETVR_HEADER));

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
    int timestamp = atoi((char*)packet.data());
    log_d("data %d", timestamp);
    unsigned long latency = timestamp - this->lastFrameTimestamp - 2137;
    log_d("latency: %lu ms i\n\r", latency);
}
