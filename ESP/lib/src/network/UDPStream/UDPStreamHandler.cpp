#include "UDPStreamHandler.h"
#include <esp_camera.h>


void UDPStreamHandler::begin() {
    auto address = IPAddress(255,255,255,255);
//    this->udp.connect(address, this->port);

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

void UDPStreamHandler::stream() {
    if(!this->udp.connected() || (millis() - this->lastTimeFrameSentTime < UDPStreamHandler::timeBetweenFrames)){
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
