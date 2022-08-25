/**
 * EthernetEnhancements.h - A library that adds useful helper functions for the Ethernet and ArduinoHttpClient Libraries
 * Created by Jarrett on July 22, 2022
 */
#ifndef EthernetEnhancements_h
#define EthernetEnhancements_h

#include "Arduino.h"
#include <SPI.h>       // May not be required
#include <Ethernet.h>  // Library for the WizNet 5100/5200/5500 modules, change if using a different module
#include <ArduinoHttpClient.h>

class EthernetEnhancements {
 public:
  /**
   * This is a constructor for EthernetEnhancements.
   * NOTE: You must include & before port, ip, myDns, gateway, subnet and client when using this method.
   *
   * @param displayClientInfo Set to true to show infomation about the client
   * @param displayEthernetInfo Set to true to show information about the ethernet
   * @param serverAddress The IP address of the server to send HTTP requests to
   * @param port The port the of the server like 80 or 8080
   * @param mac The MAC address of the Arduino shield
   * @param ip The static IP of the arduino if the DHCP server can't be reached
   * @param myDns The DNS address of your network
   * @param gateway The Gateway address of your network
   * @param subnet The Subnet address of your network
   * @param client The reference to the HttpClient class
   */
  EthernetEnhancements(bool displayClientInfo, bool displayEthernetInfo, char serverAddress[], int *port,
                       byte mac[], IPAddress *ip, IPAddress *myDns, IPAddress *gateway, IPAddress *subnet,
                       HttpClient *client);

  /**
   * This method is used to display the reqest type (GET, POST, etc.) and the path of the request.
   *
   * @param type The request type string (GET, POST, etc.)
   * @param path The path of the request */
  void displayReqType(char type[], char path[]);

  /**
   * This method is used to display the request status from the server.
   *
   * @param connectionCode The value retrieved from client.*requestMethod* (get, post, ect.)
   * @return Returns the connectionCode (a 1 digit value) if there was an error making the request.
   * Otherwise, it returns the HTTP response status codes from the server (200, 400, 500, etc.).*/
  int reqStatus(int connectionCode);

  /**
   * This method is used to display the request response from the server.
   *
   * @return This should return a JSON message.
   */
  String reqResponse();

  /**
   * This method is used to intilize the Ethernet connection.
   *
   * @param bypassDHCP Set to true to bypass DHCP and use the static assigned IP
   */
  void initializeEthernet(bool bypassDHCP);

  /**
   * This method is used to check the Ethernet status.
   * NOTE: This doesn't work on the W5100 sheild.
   */
  void checkEthernetStatus();

  /**
   * This method is used to assign a static IP address to the Arduino
   */
  void assignStaticIP();

  /**
   * This method is used to wait for the given seconds,
   * display why it is waiting and a count down.
   */
  void wait(int seconds, const __FlashStringHelper *reason);

 private:
  bool _displayClientInfo;
  bool _displayEthernetInfo;
  int *_port;
  IPAddress *_ip;
  IPAddress *_myDns;
  IPAddress *_gateway;
  IPAddress *_subnet;
  HttpClient *_client;
  byte *_mac;
  char *_serverAddress;
};

#endif