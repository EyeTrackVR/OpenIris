#include "UDPStreamHandler.h"
#include "data/StateManager/StateManager.hpp"
#include <esp_camera.h>

const char* const ETVR_HEADER = "\xff\xa0";

UDPStreamHandler::UDPStreamHandler(ProjectConfig &configManager) : configManager(configManager) {}

void UDPStreamHandler::update(ConfigState_e event) {
    if (event == ConfigState_e::configLoaded)
        this->setup();
}

std::string UDPStreamHandler::getName() {
    return "UDPStreamHandler";
}

void UDPStreamHandler::setup() {
    log_d("[SETUP]: Preparing UDP Stream socket, not connected yet, stream not running yet");
    auto deviceConfig = this->configManager.getDeviceConfig();
    this->port = deviceConfig.UDPPort;
}

void UDPStreamHandler::begin() {
    log_d("[SETUP]: UDP Stream socket connecting to a broadcast address, stream not running yet");
    if (udp.listen(this->port)) {
        this->udp.onPacket([&](AsyncUDPPacket packet) {
            this->calculateLatency(packet);
        });
    }
}

void UDPStreamHandler::try_stop() {
    if (this->udp.connected() && streamStateManager.getCurrentState() == StreamState_e::Stream_STOP) {
        this->udp.close();
        this->lastTimeFrameSentTime = 0;
    }
}

bool UDPStreamHandler::canStream() {
    // todo figure out a way to reconnect to the socket when the stream was in STOP state

    // we can add some password validation here as well, but make it cheap, like check a bool if authorized
    // and under a map, like ip: authorized, but that would require having a receiver IP, can we do that?
    // we should stream if:
    auto streamState = streamStateManager.getCurrentState();
    // we were told to stream and there wasn't any errors, and we're connected, and we're past the timeout
    bool can_stream = streamState == DeviceStates::StreamState_e::Stream_ON &&
                      this->udp.connected() &&
                      (!this->lastTimeFrameSentTime || // we didn't send anything yet
                       millis() - this->lastTimeFrameSentTime < UDPStreamHandler::timeBetweenFrames);
    return can_stream;
}

void UDPStreamHandler::stream() {
    // try reconnect if stream is stopped

    if (!this->canStream()) {
        this->try_stop();
        return;
    }
    AsyncUDPMessage message = AsyncUDPMessage();

//    unsigned long data = htonl(123456789); // convert to network byte order
//    message.write((const uint8_t *)&data, sizeof(data));
    auto data = htonl(this->lastFrameTimestamp);
    message.write((const uint8_t *)&data, sizeof(data));
    message.write((uint8_t) atoi(ETVR_HEADER));

    camera_fb_t *fb = esp_camera_fb_get();
    // todo figure out proper network-ordering for the framebuffer
    if (!fb) {
        log_e("Camera capture failed");
    }
    message.write(fb->buf, fb->len);
    esp_camera_fb_return(fb);

    // timestamp updating only for calculating latency purposes
    // we clear it right after calculating the latency
    if (!this->lastFrameTimestamp)
        this->lastFrameTimestamp = millis();

    // add reed solomon error correction here
    this->udp.broadcastTo(message.data(), message.length(), this->port);
    log_d("message size: %d, %s", message.length(), message.data());

    // update the timer because we're done
    this->lastTimeFrameSentTime = millis();
}

void UDPStreamHandler::calculateLatency(AsyncUDPPacket packet) {
    int timestamp = atoi((char *) packet.data());
    if (!timestamp) {
        log_d("did not receive valid timestamp");
        return;
    }

    log_d("data %d", timestamp);
    unsigned long latency = timestamp - this->lastFrameTimestamp;
    log_d("latency: %lu ms i\n\r", latency);
    this->lastFrameTimestamp = 0;
}