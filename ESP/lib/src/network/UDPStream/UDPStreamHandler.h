#ifndef UDPSTREAMHANDLER
#define UDPSTREAMHANDLER

#include <WiFi.h>
#include <AsyncUDP.h>
#include "data/config/project_config.hpp"


class UDPStreamHandler : public IObserver<ConfigState_e> {
    AsyncUDP udp;
    ProjectConfig& configManager;

    unsigned long lastTimeFrameSentTime = 0; // todo find better name
    unsigned long lastFrameTimestamp = 0;
    long lastFrameID = 0;

    int port = 0;

    static const int fps = 60;
    static constexpr int timeBetweenFrames = 1000 / fps;
    bool canStream();

public:
    explicit UDPStreamHandler(ProjectConfig& configManager);
    void update(ConfigState_e event) override;
    void begin();
    void stop();
    void calculateLatency(AsyncUDPPacket packet) const;
    void stream();

    std::string getName() override;
};

#endif