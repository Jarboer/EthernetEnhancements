#include <ArduinoHttpClient.h>
#include <SPI.h>
#include <Ethernet.h>  // Library for the WizNet5100 module, change if using a different one
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <TimeLib.h>
// My custom libraries
#include <VirtualButton.h>
#include <EthernetEnhancements.h>

// ---------------------- BEGIN OF USER CONFIG ----------------------

// --------------------- Server Network Details ---------------------

/* Used to store the Node.js HTTP API server address and port which will
   be connected to.
   This should be configured to be static in the router.*/
const char serverAddress[] = "192.168.1.138";
const int port = 8080;

// --------------------- Client Network Details ---------------------

/* Enter a MAC address for your controller below.
   Newer Ethernet shields have a MAC address printed on a sticker on the shield.*/
byte mac[] = { 0xA8, 0x61, 0x0A, 0xAE, 0x94, 0xCA };  // { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

/* Set the static IP address, DNS, gateway and subnet to use if the DHCP
   fails to be assigned or is bypassed.
   NOTE: This will happen always on a bridged connection*/
IPAddress ip(192, 168, 1, 200);
IPAddress myDns(192, 168, 1, 41);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

/* Used to determine if the DHCP server should be bypassed, this is faster when
   using a bridged connection as it will fail even if not bypassed.
   Disable if not using a bridged connection (this is likely the case when deployed).*/
const bool bypassDHCP = true;

// ------------------ Informative Serial Messages -------------------

/* NOTE: These options toggle the displaying of messages in the serial monitor. This is useful for checking
        for errors, debugging or confirming information. Disabling these options can help free flash memory
        and RAM which can help prevent issues and allow the program to run faster.
*/

// If enabled this will show the config info like the server address, port, variables enabled, etc.
const bool displayConfigInfo = false; // Best left off

// If enabled this will show messages informing you of various details related to the Ethernet sheild
// EXAMPLES: "Attempting to initialize Ethernet without using DHCP...", "Attempting to initialize Ethernet with DHCP...", "Failed to configure Ethernet using DHCP", "DHCP assigned IP: ___" and "Static assigned IP: ____"
const bool displayEthernetInfo = true;

// If enabled this will show messages related to the client class (making GET and POST requests) and JSON data
// EXAMPLES: "POST Data: ___", "Making ___ request to ___", "Encountered an error making the request: ___", "Status code: ___", "Response: ___" and "internal server error"
const bool displayClientAndJSONInfo = true;

// ----------------------- END OF USER CONFIG -----------------------

// Used to store the status code while making the request
int statusCode = 0;

// Used to store the JSON document to be sent
DynamicJsonDocument doc(200);  // Number of bytes in the heap, don't make too small or large

// Configure the HttpClient which is used to make HTTP requests
EthernetClient EthClient;
HttpClient client = HttpClient(EthClient, serverAddress, port);

// Set up the EthernetEnhancements class
// NOTE: You must include & before port, ip, myDns, gateway, subnet and client when using this method
EthernetEnhancements etherEnhance(displayClientAndJSONInfo, displayEthernetInfo, serverAddress, &port, mac, &ip, &myDns, &gateway, &subnet, &client);

// This method is called by the Arduino to do some setup
void setup() {
  // Open serial communications and wait for port to open.
  // Needed for native USB port only
  Serial.begin(9600);

  Serial.println();

  // If enabled call the method to display the config the user set
  if (displayConfigInfo == true) {
    Serial.print(F("Network Details:{serverAddress: "));
    Serial.print(serverAddress);
    Serial.print(F(", port: "));
    Serial.print(port);
    Serial.print(F(", bypassDHCP: "));
    Serial.print(bypassDHCP);
    Serial.println(F("}"));
  }

  // Call the method to initilize the ethernet sheild
  etherEnhance.initializeEthernet(bypassDHCP);
}

// This method is called by the Arduino to loop forver
void loop() {
  // Send the data (request for the timestamp) and store the requestBody; which sets the statusCode
  sendGetReq("/api/utilities/unix-timestamp");
  
  //Serial.print(statusCode );  // The status code of the request
  
  // Set the clock up using a Unix Timestamp from the server
  //setTime(doc[F("Timestamp")]);
  Serial.print(doc[F("Timestamp")]);
  //doc.remove(F("Timestamp"));  // Remove old time if needed

  doc[F("MyData")] = "Hello world";

  // Send the data (doc) and which sets the statusCode
  sendPostReq("/api/send-data");  
  
  //Serial.print(statusCode );  // The status code of the request
 
}

// This method is used to send a HTTP POST request.
void sendPostReq(char path[]) {
  // Used store the POST data to be sent
  String postData = "";

  // Serialize the JSON from doc into postData
  serializeJson(doc, postData);
  if (displayClientAndJSONInfo == true) {
    Serial.print(F("POST Data: "));
    Serial.println(postData);
  }

  // Display the request type being made and the API URL
  etherEnhance.displayReqType("POST", path);
  // Make the request and store the connectionCode
  int connectionCode = client.post(path, "application/json", postData.c_str());

  // Used to store and display the request status
  // (if there was any errors and the status code from the server)
  statusCode = etherEnhance.reqStatus(connectionCode);

  // If an error occured while making the POST request
  if (statusCode == -1) {
    // etherEnhance.wait(10, F(" before resending data...")); // TODO: Readd wait? - Disabled because it will send data sooner
    // Return to prevent the program from restarting fully
    return;
  }
  
  // Display if an internal server error occured
  if (displayClientAndJSONInfo == true && statusCode == 500)
    Serial.println(F("An internal server error occured, check the server logs."));

  // Display the request response
  etherEnhance.reqResponse();
  if (displayEthernetInfo || displayClientAndJSONInfo)  // If any are true
    Serial.println();
}

// This method is used to send a HTTP GET request
void sendGetReq(char path[]) {
  // Display the request type being made and the API URL
  etherEnhance.displayReqType("GET", path);
  // Make the request and store the connectionCode
  int connectionCode = client.get(path);

  /* Used to store the value and then display the request status
     (if there was any errors and the status code from the server)*/
  statusCode = etherEnhance.reqStatus(connectionCode);

  // If an error occured while making the GET request
  if (statusCode == -1) {
    // etherEnhance.wait(10, F(" before requesting data again...")); // TODO: Readd wait? - Disabled because it will send data sooner
    // Return to the loop to prevent program restarting fully
    return;
  }

  // Get and store the the request response message
  String reqResp = etherEnhance.reqResponse();

  // If the reqResp doesn't contain message then deserialize it
  if (reqResp.indexOf(F("message")) == -1) {
    // Deserialize the request body and store it in doc
    deserializeJson(doc, reqResp);
  }
  if (displayEthernetInfo == true)
    Serial.println();
}
