/*
  EthernetEnhancements.h - A library that adds useful helper functions for the Ethernet and ArduinoHttpClient Libraries
  Created by Jarrett on July 22, 2022
*/
#ifndef EthernetEnhancements_h
#define EthernetEnhancements_h

#include "Arduino.h"
#include <Ethernet.h>  // Library for the WizNet5100 module, change if using a different one
#include <ArduinoHttpClient.h>

class EthernetEnhancements {
  public:
    /* This is a constructor for EthernetEnhancements which takes a for toggleSwitchesON array.*/
    EthernetEnhancements(char serverAddress[], int port, byte mac[], IPAddress *ip, IPAddress *myDns, IPAddress *gateway, IPAddress *subnet, HttpClient *client);
    void displayReqType(char type[], char path[]);
    int reqStatus(int connectionCode);
    String reqResponse();
    void initializeEthernet(bool bypassDHCP);
    void checkEthernetStatus();
    void assignStaticIP();
    void wait(int seconds, String reason);
  private:
    int _port;
    IPAddress *_ip;
    IPAddress *_myDns;
    IPAddress *_gateway;
    IPAddress *_subnet;
    HttpClient *_client;
    byte *_mac;
    char *_serverAddress;
};

#endif