/**
 * EthernetEnhancements.cpp - A library that adds useful helper functions for the Ethernet and ArduinoHttpClient Libraries
 * Created by Jarrett on July 22, 2022
 */
#include "EthernetEnhancements.h"

EthernetEnhancements::EthernetEnhancements(bool displayClientInfo, bool displayEthernetInfo, char serverAddress[], int *port,
                                           byte mac[], IPAddress *ip, IPAddress *myDns, IPAddress *gateway, IPAddress *subnet,
                                           HttpClient *client) {
  _displayClientInfo = displayClientInfo;
  _displayEthernetInfo = displayEthernetInfo;
  _serverAddress = serverAddress;
  _port = port;
  _mac = mac;
  _ip = ip;
  _myDns = myDns;
  _gateway = gateway;
  _subnet = subnet;
  _client = client;
}

void EthernetEnhancements::displayReqType(char type[], char path[]) {
  if (_displayClientInfo == true) {
    Serial.print(F("Making "));
    Serial.print(type);
    Serial.print(F(" request to "));
    Serial.print(_serverAddress);
    Serial.print(F(":"));
    Serial.print(*_port);
    Serial.println(path);
  }
}

int EthernetEnhancements::reqStatus(int connectionCode) {
  // If the connectionCode is not 0 there is an error so display it
  if (connectionCode != 0) {
    if (_displayClientInfo == true) {
      Serial.print(F("Encountered an error making the request: "));
      Serial.println(connectionCode);
    }
    return connectionCode;  // Return the connection code, -1 is a connection error
  }

  // Read the status code of the response
  int statusCode = _client->responseStatusCode();
  // Display the status code
  if (_displayClientInfo == true) {
    Serial.print(F("Status code: "));
    Serial.println(statusCode);
  }
  // Return the status code sent from the server
  return statusCode;
}

String EthernetEnhancements::reqResponse() {
  // Read the body of the response
  String response = _client->responseBody();

  // Display the response
  if (_displayClientInfo == true) {
    Serial.print(F("Response: "));
    Serial.println(response);
  }
  // Return the response sent from the server
  return response;
}

void EthernetEnhancements::initializeEthernet(bool bypassDHCP) {
  // Bypass getting an IP address from the DHCP server
  if (bypassDHCP) {
    if (_displayEthernetInfo == true)
      Serial.println(F("Attempting to initialize Ethernet without using DHCP..."));
    // Call the function to assign a static IP address
    assignStaticIP();
    // Call the function to see if there is an issue related to the Ethernet shield/port
    checkEthernetStatus();

    // Must wait at least 10 seconds otherwise the client will get stuck
    // making a request
    wait(10, F(" for Ethernet initialization..."));
  } else {
    // Start the Ethernet connection
    if (_displayEthernetInfo == true)
      Serial.println(F("Attempting to initialize Ethernet with DHCP..."));
    if (Ethernet.begin(*_mac) == 0) {  // If true then the DHCP was not reachable
      if (_displayEthernetInfo == true)
        Serial.println(F("Failed to configure Ethernet using DHCP"));
      // Call the function to see if the issue is related to the Ethernet shield/port
      checkEthernetStatus();
      // Call the function to assign a static IP address
      assignStaticIP();
    } else if (_displayEthernetInfo == true) {  // Successfully connected using DHCP
      Serial.print(F("DHCP assigned IP: "));
      Serial.println(Ethernet.localIP());
    }
  }
}

void EthernetEnhancements::checkEthernetStatus() {
  // Check if there's Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println(F("No Ethernet shield was found. Please connect one."));
    while (true) {
      /* Do nothing, there's no point in running without Ethernet hardware */
    }
  } else if (Ethernet.linkStatus() == LinkOFF) {  // Check if there is an Ethernet cable attatched
    Serial.println(F("No Ethernet cable is connected."));
  }
}

void EthernetEnhancements::assignStaticIP() {
  // Try to congifure using IP address instead of DHCP
  Ethernet.begin(*_mac, *_ip, *_myDns, *_gateway, *_subnet);

  // Display IP address
  if (_displayEthernetInfo == true) {
    Serial.print(F("Static assigned IP: "));
    Serial.println(Ethernet.localIP());
  }
}

void EthernetEnhancements::wait(int seconds, const __FlashStringHelper *reason) {
  // Display how long to wait and why
  Serial.print(F("Waiting "));
  Serial.print(seconds);
  Serial.print(F(" seconds"));
  Serial.print(reason);
  // Countdown for the amount of time
  for (int i = seconds; i > 0; i--) {
    Serial.print(F(" "));
    Serial.print(i);
    delay(1000);
  }
  Serial.println("\n");
}