#include "network_utilities.hpp"

void Network_Utilities::SetupWifiScan()
{
    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    WiFi.mode(WIFI_STA);
    WiFi.disconnect(); // Disconnect from the access point if connected before
    delay(100);
    Serial.println("Setup done");
}

bool Network_Utilities::LoopWifiScan()
{
    // WiFi.scanNetworks will return the number of networks found
    log_i("[INFO]: Beginning WiFi Scanner");
    int networks = WiFi.scanNetworks();
    log_i("[INFO]: scan done");
    log_i("%d networks found", networks);
    for (int i = networks; i--;)
    {
        // Print SSID and RSSI for each network found
        //! Add method here to interface with the API and forward the scanned networks to the API
        log_i("%d: %s (%d) %s\n", i - 1, WiFi.SSID(i), WiFi.RSSI(i), (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
        my_delay(0.02L); // delay 20ms
    }
    // Wait a bit before scanning again
    delay(5000);
    return (networks > 0);
}

// Take measurements of the Wi-Fi strength and return the average result.
int Network_Utilities::getStrength(int points) // TODO: add to JSON doc
{
    int32_t rssi = 0, averageRSSI = 0;
    for (int i = 0; i < points; i++)
    {
        rssi += WiFi.RSSI();
        delay(20);
    }
    averageRSSI = rssi / points;
    return averageRSSI;
}

void Network_Utilities::my_delay(volatile long delay_time)
{
    delay_time = delay_time * 1e6L;
    for (volatile long count = delay_time; count > 0L; count--)
        ;
}