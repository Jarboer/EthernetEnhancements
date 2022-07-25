/*
  EthernetEnhancements.cpp - A library that adds useful helper functions for the Ethernet and ArduinoHttpClient Libraries
  Created by Jarrett on July 22, 2022
*/
#include "Arduino.h"
#include "EthernetEnhancements.h"
#include <Ethernet.h>  // Library for the WizNet5100 module, change if using a different one
#include <ArduinoHttpClient.h>

/* This is a constructor for EthernetEnhancements which takes a for toggleSwitchesON array.
   NOTE: You must include the & before the serverAddress and port varible when using this method when calling it in a sketch*/
EthernetEnhancements::EthernetEnhancements(char serverAddress[], int port, byte mac[], IPAddress *ip, IPAddress *myDns, IPAddress *gateway, IPAddress *subnet, HttpClient *client) {
   _serverAddress = serverAddress;
  _port = port;
  _mac = mac;
  _ip = ip;
  _myDns = myDns;
  _gateway = gateway;
  _subnet = subnet;
  _client = client;

  /*Serial.print(F("serverAddress: "));
  Serial.println(_serverAddress); // Good
  Serial.print(F("port: "));
  Serial.println(_port); // Good
  Serial.print(F("mac: "));
  Serial.println(*_mac); // Good, HEX?
  Serial.print(F("IP: "));
  Serial.println(*_ip); // Good
  Serial.print(F("myDns: "));
  Serial.println(*_myDns); // Good
  Serial.print(F("gateway: "));
  Serial.println(*_gateway); // Good
  Serial.print(F("subnet: "));
  Serial.println(*_subnet); // Good
  Serial.print(F("client: "));
  Serial.println(*_client); // Bad? */
}

// This method is used to display the request type and API URL
void EthernetEnhancements::displayReqType(char type[], char path[]) {
  Serial.print(F("Making "));
  Serial.print(type);
  Serial.print(F(" request to "));
  Serial.print(_serverAddress);
  Serial.print(F(":"));
  Serial.print(_port);
  Serial.println(path);
}

/*  This method is used to display the request status from the server.
    It returns the connectionCode (a 1 digit value) if there was an error making the request.
    Otherwise, it returns the HTTP response status codes from the server (200, 400, 500, etc.).*/
int EthernetEnhancements::reqStatus(int connectionCode) {
  // If the connectionCode is not 0 there is an error so display it
  if (connectionCode != 0) {
    Serial.print(F("Encountered an error making the request: "));
    Serial.println(connectionCode);
    return connectionCode;  // Return the connection code, -1 is a connection error
  }

  // Read the status code of the response
  int statusCode = _client->responseStatusCode();
  // Display the status code
  Serial.print(F("Status code: "));
  Serial.println(statusCode);
  // Return the status code sent from the server
  return statusCode;
}

/* This method is used to display the request response from the server.
   This should return a JSON message.*/
String EthernetEnhancements::reqResponse() {
  // Read the body of the response
  String response = _client->responseBody();

  // Display the response
  Serial.print(F("Response: "));
  Serial.println(response);

  return response;
}

// This method is used to intilize the Ethernet connection
void EthernetEnhancements::initializeEthernet(bool bypassDHCP) {
  // Bypass getting an IP address from the DHCP server
  if (bypassDHCP) {
    Serial.println(F("Attempting to initialize Ethernet by bypassing DHCP..."));
    // Call the function to assign a static IP address
    assignStaticIP();
    // Call the function to see if there is an issue related to the Ethernet shield/port
    checkEthernetStatus();

    // Must wait at least 10 seconds otherwise the client will get stuck
    // making a request
    wait(10, F(" for Ethernet initialization..."));
  } else {
    // Start the Ethernet connection
    Serial.println(F("Attempting to initialize Ethernet with DHCP..."));
    if (Ethernet.begin(*_mac) == 0) {  // If true then the DHCP was not reachable
      Serial.println(F("Failed to configure Ethernet using DHCP"));
      // Call the function to see if the issue is related to the Ethernet shield/port
      checkEthernetStatus();
      // Call the function to assign a static IP address
      assignStaticIP();
    } else {  // Successfully connected using DHCP
      Serial.print(F("DHCP assigned IP: "));
      Serial.println(Ethernet.localIP());
    }
  }
}

/* This method is used to check the Ethernet status.
   NOTE: This doesn't work on the W5100 sheild.*/
void EthernetEnhancements::checkEthernetStatus() {
  // Check if there's Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println(F("No Ethernet shield was found. Please connect one."));
    while (true) {
      delay(1);  // Do nothing, there's no point in running without Ethernet hardware
    }
  } else if (Ethernet.linkStatus() == LinkOFF) {  // Check if there is an Ethernet cable attatched
    Serial.println(F("Ethernet cable is not connected."));
  }
}

// This method is used to assign a static IP address to the Arduino
void EthernetEnhancements::assignStaticIP() {

  // Try to congifure using IP address instead of DHCP
  Ethernet.begin(*_mac, *_ip, *_myDns, *_gateway, *_subnet);

  // Display IP address
  Serial.print(F("Static assigned IP: "));
  Serial.println(Ethernet.localIP());
}

/* This method is used to wait for the given seconds and
   display why it is waiting and a count down.*/
void EthernetEnhancements::wait(int seconds, String reason) {
  // Display how long to wait and why
  Serial.print(F("\nWaiting "));
  Serial.print(seconds);
  Serial.print(F(" seconds"));
  Serial.print(reason);
  // Countdown for the amount of time
  for (int i = seconds; i > 0; i--) {
    Serial.print(F(" "));
    Serial.print(i);
    delay(1000);
  }
  Serial.println();
}