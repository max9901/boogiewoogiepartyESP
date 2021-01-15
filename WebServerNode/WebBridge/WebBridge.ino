//#include <Arduino.h>
#include "painlessMesh.h"
#include "PubSubClient.h"
#include "/home/max/boogiewoogiepartyESP/passwords.h"

#ifdef ESP8266
  #include "Hash.h"
  #include <FS.h>
  #include <ESPAsyncTCP.h>
#else
  #include <AsyncTCP.h>
  #include <SPIFFS.h>
#endif
#include <ESPAsyncWebServer.h>


#define   DEBUG         1
#define   ROLE          "WebBridge"
#define   HOSTNAME      "WebBridge"
#define   STATION_PORT  5555
#define   par1          3662001431
#define   par2          3661949679

painlessMesh  mesh;
WiFiClient wifiClient;
Scheduler userScheduler;   // to control your personal task

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncWebSocket wsA("/wsa");

String red    = "0";
String green  = "0";
String blue   = "0";
String white  = "0";
String strobo = "0";

SimpleList<uint32_t> nodes;
SimpleList<uint32_t> sockets;

void sendHeartBEAT(); //prototype
bool _mesh_calc_delay = false;
Task taskSendMessage( TASK_SECOND * 5, TASK_FOREVER, &sendHeartBEAT ); // start with a one second interval

IPAddress getlocalIP(); //prototype
IPAddress myIP(0,0,0,0);
IPAddress myAPIP(0,0,0,0);

//funtions
void sendoverclientlist(){
  Serial.printf("Num nodes: %d\n", nodes.size());
  Serial.printf("Connection list:");
  String listsend = "L{";
  bool newcomma = false;
  SimpleList<uint32_t>::iterator node = nodes.begin();
  while (node != nodes.end()){
    Serial.printf(" %u", *node);
    listsend += *node;
    node++;
    if(node != nodes.end())
      listsend += ",";
    }
  SimpleList<uint32_t>::iterator socket = sockets.begin();
  while (socket != sockets.end()){
    if(!newcomma){
      listsend += ",";
      newcomma = true;
    }
    Serial.printf(" W%u", *socket);
    listsend += "W";
    listsend += *socket;    
    socket++;
    if(socket != sockets.end())
      listsend += ",";
    }
  listsend += "}";
  Serial.println();
  Serial.println(listsend);
  ws.textAll(listsend);
}

//Mesh funcitons
void receivedCallback( const uint32_t &from, const String &msg ) {
    Serial.printf("bridge: Received from %u msg=%s\n", from, msg.c_str());
    ws.textAll(msg);
    char buf[msg.length() + 1];
    msg.toCharArray(buf, sizeof(buf));
    Serial.println(buf);
    char *p = buf;
    char *str;
    while ((str = strtok_r(p, "&", &p)) != NULL){
      Serial.println(str);
      char collor;
      collor = str[0];
      if(collor == 'R'){
        red = String(&str[1]);  
      }
      if(collor == 'B'){
        blue = String(&str[1]);  
      }
      if(collor == 'G'){
        green = String(&str[1]);  
      }
      if(collor == 'W'){
        white = String(&str[1]);  
      }
      if(collor == 'S'){
        strobo = String(&str[1]);  
      }
    }
  }
void newConnectionCallback(uint32_t nodeId) 
  {
    Serial.printf("--> Start: New Connection, nodeId = %u\n", nodeId);
    Serial.printf("--> Start: New Connection, %s\n", mesh.subConnectionJson(true).c_str());
  }
void changedConnectionCallback(){
  Serial.printf("Changed connections\n");
  nodes = mesh.getNodeList();
  sendoverclientlist();
  _mesh_calc_delay = true;
  }
void nodeTimeAdjustedCallback(int32_t offset) 
  {
  Serial.printf("Adjusted time %u Offset = %d\n", mesh.getNodeTime(),offset);
  }
void onNodeDelayReceived(uint32_t nodeId, int32_t delay)
  {
  Serial.printf("Delay from node:%u delay = %d\n", nodeId,delay);
  }
void reconnect()
{
  //byte mac[6];
  char MAC[9];
  int i;
#ifdef ESP8266
  sprintf(MAC, "%08X",(uint32_t)ESP.getChipId());  // generate unique addresss.
#else
  sprintf(MAC, "%08X",(uint32_t)ESP.getEfuseMac());  // generate unique addresss.
#endif
}
IPAddress getlocalIP() 
  {
  return IPAddress(mesh.getStationIP());
  }
String scanprocessor(const String& var){
  if(var == "SCAN")
    return mesh.subConnectionJson(false);
  return String();
}
void sendHeartBEAT() {
  String msg = "Hello from node ";
  msg += mesh.getNodeId();
  msg += " , " + String(ROLE);
  msg += " myFreeMemory: " + String(ESP.getFreeHeap());
  mesh.sendBroadcast(msg);
  if (_mesh_calc_delay) {
    SimpleList<uint32_t>::iterator node = nodes.begin();
    while (node != nodes.end()) {
      mesh.startDelayMeas(*node);
      node++;
    }
    _mesh_calc_delay = false;
  }
  Serial.printf("Sending message: %s\n", msg.c_str());
  taskSendMessage.setInterval( random(TASK_SECOND * 5, TASK_SECOND * 10)); // between 1 and 5 seconds
}

//Web functions
void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/html", "<h1>Not found options -> ip/= broadcast -> ip/new = colorpicker -> ip/newcolor = visuals<\h1> <form>Text to Broadcast<br><input type='text' name='BROADCAST'><br><br><input type='submit' value='Submit'></form>");
    if (request->hasArg("BROADCAST")){
      String msg = request->arg("BROADCAST");
      mesh.sendBroadcast(msg);
      ws.textAll(msg);   //websocket to share ! 
      wsA.textAll(msg);
    }  
}
void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len)
{
  if(type == WS_EVT_CONNECT){
    Serial.print("Websocket client connection received: ");
    Serial.println(client->id());
    client->text("Hello color picker");
  } else if(type == WS_EVT_DISCONNECT){
    Serial.print("Client disconnected: ");
    Serial.println(client->id());
  }
}
void onWsAevent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len)
{
  if(type == WS_EVT_CONNECT){
    Serial.print("Websocket client connection received: ");
    Serial.println(client->id());
    sockets.push_back(client->id());
    client->text("Hello Animation server");
  } else if(type == WS_EVT_DISCONNECT){
    Serial.print("Client disconnected: ");
    Serial.println(client->id());
    for(SimpleList<uint32_t>::iterator socket = sockets.begin(); socket != sockets.end();){
      Serial.println(*socket);
      if(client->id() == (*socket)){
        socket = sockets.erase(socket);  
        continue;
      }
      ++socket;
    }    
  }
  sendoverclientlist();
}

void setup() 
  {
  Serial.begin(115200);
  delay(100);
  
  //SPIFFSinit
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  
  // mesh init
  // void init(TSTRING ssid, TSTRING password, Scheduler *baseScheduler,
  //           uint16_t port = 5555, WiFiMode_t connectMode = WIFI_AP_STA,
  //           uint8_t channel = 1, uint8_t hidden = 0,
  //           uint8_t maxconn = MAX_CONN) 
  mesh.setDebugMsgTypes(ERROR | DEBUG);
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT, WIFI_AP_STA, 4, 0 ,4);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  mesh.stationManual(STATION_SSID, STATION_PASSWORD);
  mesh.setHostname(HOSTNAME);
  mesh.setRoot(true);
  mesh.setContainsRoot(true);
  mesh.initOTAReceive(ROLE);

  //WEBinit
  ws.onEvent(onWsEvent);
  wsA.onEvent(onWsAevent);
  server.addHandler(&ws);
  server.addHandler(&wsA);

  // Main pages
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/main.html", String(), false);
        if (request->hasArg("BROADCAST")){
          String msg = request->arg("BROADCAST");
          mesh.sendBroadcast(msg);
          if(DEBUG) Serial.println(msg);
          ws.textAll(msg);   //websocket to share ! 
          wsA.textAll(msg);
        }  
    });  
  server.on("/new", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/new.html", String(), false);
    });
  server.on("/newcolor", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/newcolor.html", String(), false);
    });
  
  // Helper pages
  server.on("/buttonboard.js", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/buttonboard.js", String(), false);
    });
  
  // Callback functions
  server.on("/start", HTTP_GET, [](AsyncWebServerRequest *request){
      nodes = mesh.getNodeList();
      Serial.printf("Num nodes: %d\n", nodes.size());
      Serial.printf("Connection list:");
      String listsend = "L{";
      bool newcomma = false;
      SimpleList<uint32_t>::iterator node = nodes.begin();
      while (node != nodes.end()){
        Serial.printf(" %u", *node);
        listsend += *node;
        node++;
        if(node != nodes.end())
          listsend += ",";
        }
      SimpleList<uint32_t>::iterator socket = sockets.begin();
      while (socket != sockets.end()){
        if(!newcomma){
          listsend += ",";
          newcomma = true;
        }
        Serial.printf(" W%u", *socket);
        listsend += "W";
        listsend += *socket;
        socket++;
        if(socket != sockets.end())
          listsend += ",";
        }
      listsend += "}";
      AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", ("R" + red + "&G" + green + "&B" + blue + "&W" + white + "&S"  + strobo + "&" + listsend + "&"));
      request->send(response);
      Serial.println("hiiiiieer werkt");
  });
  server.on("/setRGB", HTTP_GET, [](AsyncWebServerRequest *request)
    {
    //Saperate Colors are sent through javascript
      red = request->arg("r");
      green = request->arg("g");
      blue = request->arg("b");
      white = request->arg("w");
      strobo = request->arg("s");
      String clist = request->arg("l");
      String temp; 
      String msg = "0," + red + "&1," + green + "&2," + blue + "&3," + white + "&4,"  + strobo + "&";
      Serial.println(msg);

      for(int i = 1; i < clist.length()-1; i++){
          if(clist[i] == ','){
            Serial.println(temp);
            if(temp == "All"){
                mesh.sendBroadcast(msg);
                wsA.textAll("R" + red + "&G" + green + "&B" + blue + "&W" + white + "&S"  + strobo + "&");
            }
            else{
              //todo error afhandeling.
              if(temp[0] == 'W'){
                String Temp2 = temp.substring(1);
                Serial.println(Temp2);
                wsA.text(strtoul(Temp2.c_str(),NULL,10), msg);
              }else{
                mesh.sendSingle(strtoul(temp.c_str(),NULL,10), msg); 
              }
            }
            temp = "";
            continue;
          }
          temp += clist[i];
          if(i == clist.length()-2){
            Serial.println(temp);
            if(temp == "All"){
                Serial.print("broadcast");
                mesh.sendBroadcast(msg);
                wsA.textAll("R" + red + "&G" + green + "&B" + blue + "&W" + white + "&S"  + strobo + "&");
            }
            else{
              if(temp[0] == 'W'){
                String Temp2 = temp.substring(1);
                Serial.println(Temp2);
                wsA.text(strtoul(Temp2.c_str(),NULL,10), msg);
              }else{
                mesh.sendSingle(strtoul(temp.c_str(),NULL,10), msg);
              }
            }
            temp = "";
          }
      }
      AsyncWebServerResponse *response = request->beginResponse(302, "text/plain", "Updated-- Press Back Button");
      response->addHeader("Test-Header", "My header value");
      request->send(response);
    });
  server.on("/map", HTTP_GET, [](AsyncWebServerRequest *request)
    {
    request->send_P(200, "text/html", "<html><head><script type='text/javascript' src='https://cdnjs.cloudflare.com/ajax/libs/vis/4.21.0/vis.js'></script><link href='https://cdnjs.cloudflare.com/ajax/libs/vis/4.21.0/vis-network.min.css' rel='stylesheet' type='text/css' /><style type='text/css'>#mynetwork {width: 1024px;height: 768px;border: 1px solid lightgray;}</style></head><body><h1>PainlessMesh Network Map</h1><div id='mynetwork'></div><a href=https://visjs.org>Made with vis.js<img src='http://www.davidefabbri.net/files/visjs_logo.png' width=40 height=40></a><script>var txt = '%SCAN%';</script><script type='text/javascript' src='http://www.davidefabbri.net/files/painlessmeshmap.js'></script></body></html>", scanprocessor );
    });
  server.on("/scan", HTTP_GET, [](AsyncWebServerRequest *request)
    {
    request->send(200, "application/json", mesh.subConnectionJson(false) );
    });

  server.onNotFound(notFound);
  server.begin();

  userScheduler.addTask(taskSendMessage);

  }
void loop(){
    ws.cleanupClients();
    wsA.cleanupClients();
    mesh.update();
    if(myIP != getlocalIP()){
      myIP = getlocalIP();
      Serial.println("My IP is " + myIP.toString());
    }
    if(myAPIP != IPAddress(mesh.getAPIP())){
      myAPIP = IPAddress(mesh.getAPIP());
      Serial.println("My AP IP is " + myAPIP.toString());
    }
}

