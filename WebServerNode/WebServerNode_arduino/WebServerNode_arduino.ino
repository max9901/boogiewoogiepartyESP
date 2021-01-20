#include "painlessMesh.h"
#include "PubSubClient.h"
#include <ESPAsyncWebServer.h>
//Change to your full Path
#include "/home/max/boogiewoogiepartyESP/passwords.h"
#ifdef ESP8266
  #include "Hash.h"
  #include <FS.h>
  #include <ESPAsyncTCP.h>
#else
  #include <AsyncTCP.h>
  #include <SPIFFS.h>
#endif

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
AsyncWebSocket ws_ColorPicker("/wsColorPicker");  //infoChannel
AsyncWebSocket ws_Animation("/wsa");              //AnimationChannel


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
  bool newcomma = true;
  SimpleList<uint32_t>::iterator node = nodes.begin();

  while (node != nodes.end()){
    Serial.printf(" %u", *node);
    listsend += *node;
    node++;
    newcomma = false;
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
  ws_ColorPicker.textAll(listsend);
}

//Mesh functions
void receivedCallback( const uint32_t &from, const String &msg ) {
    // Channels inbouwen voor de websockets ??
    Serial.printf("bridge: Received from %u msg=%s\n", from, msg.c_str());
    ws_Animation.textAll(msg);
    char buf[msg.length() + 1];
    msg.toCharArray(buf, sizeof(buf));
    Serial.println(buf);
    char *p = buf;
    char *str;
    // TODO
    ws_Animation.textAll(msg);
    while ((str = strtok_r(p, "&", &p)) != NULL){
      Serial.println(str);
      Serial.println("TODO");
  }
}
void newConnectionCallback(uint32_t nodeId){
    Serial.printf("--> Start: New Connection, nodeId = %u\n", nodeId);
    Serial.printf("--> Start: New Connection, %s\n", mesh.subConnectionJson(true).c_str());
  }
void changedConnectionCallback(){
  Serial.printf("Changed connections\n");
  nodes = mesh.getNodeList();
  sendoverclientlist();
  _mesh_calc_delay = true;
}
void nodeTimeAdjustedCallback(int32_t offset){
  Serial.printf("Adjusted time %u Offset = %d\n", mesh.getNodeTime(),offset);
  }
void onNodeDelayReceived(uint32_t nodeId, int32_t delay){
  Serial.printf("Delay from node:%u delay = %d\n", nodeId,delay);
  }
void reconnect(){
  //byte mac[6];
  char MAC[9];
  int i;
#ifdef ESP8266
  sprintf(MAC, "%08X",(uint32_t)ESP.getChipId());  // generate unique addresss.
#else
  sprintf(MAC, "%08X",(uint32_t)ESP.getEfuseMac());  // generate unique addresss.
#endif
}
IPAddress getlocalIP(){
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
    request->send(404, "text/html", "<h1>Not found options -> ip/= broadcast -> ip/ColorPicker = ColorPicker -> ip/newcolor = visuals<\h1> <form>Text to Broadcast<br><input type='text' name='BROADCAST'><br><br><input type='submit' value='Submit'></form>");
    if (request->hasArg("BROADCAST")){
      String msg = request->arg("BROADCAST");
      mesh.sendBroadcast(msg);
      ws_ColorPicker.textAll(msg);   //websocket to share ! 
      ws_Animation.textAll(msg);
    }  
}
void on_ws_ColorPicker_Event(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  if(type == WS_EVT_CONNECT){
    Serial.print("Websocket client connection received: ");
    Serial.println(client->id());
    client->text("Hello New information channel I will send over some things to start up :D");
    sendoverclientlist();
  } else if(type == WS_EVT_DISCONNECT){
    Serial.print("Client disconnected: ");
    Serial.println(client->id());
  }
}
void on_ws_Animation_Aevent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
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

//arduino functions
void setup(){
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
  ws_ColorPicker.onEvent(on_ws_ColorPicker_Event);
  ws_Animation.onEvent(on_ws_Animation_Aevent);
  server.addHandler(&ws_ColorPicker);
  server.addHandler(&ws_Animation);

  // Main pages
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/main.html", String(), false);
        if (request->hasArg("BROADCAST")){
          String msg = request->arg("BROADCAST");
          mesh.sendBroadcast(msg);
          if(DEBUG) Serial.println(msg);
          ws_ColorPicker.textAll(msg);   //websocket to share ! 
          ws_Animation.textAll(msg);
        }  
    });  
  server.on("/ColorPicker", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/ColorPicker.html", String(), false);
    });
  server.on("/newcolor", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/newcolor.html", String(), false);
    });
  
  // Helper pages
  server.on("/libs/buttonboard.js", HTTP_GET, [](AsyncWebServerRequest *request){
      Serial.println("INFO: buttonboard.js requested");
      request->send(SPIFFS, "/libs/buttonboard.js", String(), false);
    });
  server.on("/libs/ClientList.js", HTTP_GET, [](AsyncWebServerRequest *request){
      Serial.println("INFO: Clientlist.js requested"); 
      request->send(SPIFFS, "/libs/ClientList.js", String(), false);
    });
  server.on("/libs/p5_Phantom_town.js", HTTP_GET, [](AsyncWebServerRequest *request){
      Serial.println("INFO: p5_Phantom_town.js requested"); 
      request->send(SPIFFS, "/libs/p5_Phantom_town.js", String(), false);
    });
  

  // Callback functions
  server.on("/start", HTTP_GET, [](AsyncWebServerRequest *request){
      Serial.println("Startup callback -> FI");
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
      // AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", ("R" + red + "&G" + green + "&B" + blue + "&W" + white + "&S"  + strobo + "&" + listsend + "&"));
       AsyncWebServerResponse *response = request->beginResponse(200);
      request->send(response);
  });
      //color picker TODO
  server.on("/setRGB", HTTP_GET, [](AsyncWebServerRequest *request)
    {
      //Saperate Colors are sent through javascript
      String red = request->arg("r");
      String green = request->arg("g");
      String blue = request->arg("b");
      String white = request->arg("w");
      String strobo = request->arg("s");
      String clist = request->arg("l");

      String temp; 
      String msg = "0," + red + "&1," + green + "&2," + blue + "&3," + white + "&4,"  + strobo + "&";
      Serial.println(msg);

      for(int i = 1; i < clist.length()-1; i++){
          if(clist[i] == ','){
            Serial.println(temp);
            if(temp == "All"){
                mesh.sendBroadcast(msg);
                //todo
                ws_Animation.textAll(msg);
            }
            else{
              //todo error afhandeling.
              if(temp[0] == 'W'){
                String Temp2 = temp.substring(1);
                Serial.println(Temp2);
                //todo
                ws_Animation.text(strtoul(Temp2.c_str(),NULL,10), msg);
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
                ws_Animation.textAll(msg);
            }
            else{
              if(temp[0] == 'W'){
                String Temp2 = temp.substring(1);
                Serial.println(Temp2);
                ws_Animation.text(strtoul(Temp2.c_str(),NULL,10), msg);
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
      //main
  server.on("/BROADCAST", HTTP_GET, [](AsyncWebServerRequest *request){   
      if (request->hasArg("BROADCAST")){
        String msg = request->arg("BROADCAST");
    
        mesh.sendBroadcast(msg);
        ws_Animation.textAll(msg);
        
        char buf[msg.length() + 1];
        msg.toCharArray(buf, sizeof(buf));
        char *p = buf;
        char *str;
        while ((str = strtok_r(p, "&", &p)) != NULL){ // delimiter is the semicolon
          Serial.println(str);
          int channelnumber;
          int value = 0;
          if(sscanf(str, "%d,%d",&channelnumber,&value) == 2){
            Serial.println(str);
            //Vanaf hier dingen regelen !
          }
        }
      }

    });

  //painlessmesh functions for network analysis
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
    ws_ColorPicker.cleanupClients();
    ws_Animation.cleanupClients();
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
