#ifndef UDPSTREAMHANDLER
#define UDPSTREAMHANDLER

#include <WiFi.h>
#include <AsyncUDP.h>


class UDPStreamHandler {
    AsyncUDP udp;
    unsigned long lastTimeFrameSentTime = 0; // todo find better name
    unsigned long lastFrameTimestamp = 0;
    long lastFrameID = 0;

    static const int fps = 60;
    static constexpr int timeBetweenFrames = 1000 / fps;

public:
    void begin();
    void stop();
    void calculateLatency(AsyncUDPPacket packet) const;
    void stream();
};

#endif