#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include "config.h"

class NetworkManager {
public:
  static NetworkManager& instance() {
    static NetworkManager _instance;
    return _instance;
  }

  void begin(const AppCfg* cfg);
  void startEthernet(const AppCfg* cfg);
  void startWiFiAP(const AppCfg* cfg);
  void stopWiFiAP();
  void applyNetworkConfig(const AppCfg* cfg);
  void hotReloadNetwork(const AppCfg* cfg);
  void stop();

  bool isEthernetConnected() const { return _ethConnected; }
  bool isWiFiAPActive() const { return _wifiApActive; }
  IPAddress getEthernetIP() const { return _ethIp; }
  IPAddress getWiFiAPIP() const { return _wifiApIp; }

private:
  NetworkManager() = default;

  static void onEthEvent(arduino_event_t* event);
  
  bool _ethConnected = false;
  bool _wifiApActive = false;
  IPAddress _ethIp;
  IPAddress _wifiApIp;
};

#define NETMGR NetworkManager::instance()
