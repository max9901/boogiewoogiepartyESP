/* Welkom!
 * Par-code base written by: Max Engelen
 * 
 * Target platform:
 *    1) ESP 01 with 39 leds attached in circles degrading in size!
 *    2) WC-rol start project 
 * 
 *Time it wil take to udpate the leds! ~onavg 
 * RGB microseconds = pixelcount x 30 + 300 + 100
 * RGBW microseconds = pixelcount x 40 + 300 + 100
 * 
 * TODOlist:
 *    - Build in more Animations
 *    - Build in unit identifier    -> the network needs to know this is a par, send on message received "IDENTIFIER"
 *    - Build in MESH OTA support 
 *    
 */

/*
Channel[0] = Rood led
Channel[1] = Blue led
Channel[2] = Red  led
Channel[3] = white led
channel[4] = strobo_set
channel[5] = Animation interval BPM   (if 255 then look foor K (kick, aka step, aka beat) message!)
channel[6] = Animation select         (0 == default, 1 == random brightness, )
channel[7] = fade_setting
*/

#include "/home/max/boogiewoogiepartyESP/passwords.h"

#include <painlessMesh.h>      // https://gitlab.com/painlessMesh/painlessMesh/-/wikis/home && https://github.com/arkhipenko/TaskScheduler/wiki/API-Task#task
#include <NeoPixelBus.h>       // https://github.com/Makuna/NeoPixelBus/wiki/NeoPixelBus-object-API
#include <NeoPixelAnimator.h>  // https://github.com/Makuna/NeoPixelBus/wiki/NeoPixelAnimator-object-API

#define   LED             LED_BUILTIN       // GPIO number of connected LED, ON ESP-12 IS GPIO2
#define   DEBUG           true
#define   BLINK_PERIOD    3000              //ms
#define   BLINK_DURATION  100  

#define   HOSTNAME        "PAR"

/*** BLACK MAGIC PAR ***/
#define ROLE "paresp01"
const uint16_t PixelCount = 39;
const uint8_t PixelPin = 4;  
NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart1800KbpsMethod> strip(PixelCount, PixelPin);
#define RGB 1
#define RGBW 0

/*** WC DUMP par ***/
// #define ROLE "parwc"
// const uint16_t PixelCount = 12; 
// const uint8_t PixelPin = D4;  
// NeoPixelBus<NeoGrbwFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);
// #define RGB 0
// #define RGBW 1

// class declarations
const uint8_t AnimationChannels = 1;            //we only need one as all the pixels are animated at once
NeoPixelAnimator animations(AnimationChannels); // NeoPixel animation management object
boolean fadeToColor = true;                     // general purpose variable used to store effect state

struct MyColorState
{
    RgbwColor CurrentColor;
    RgbwColor PreviousColor;
};
MyColorState ColorState[AnimationChannels];

Scheduler     userScheduler; // to control your personal task
painlessMesh  mesh;
Task Strobo;
Task blinkNoNodes;
Task Animation;
SimpleList<uint32_t> nodes;


//global declarations nog weg werken in class samen met functies 
const int NUMCHANNELS = 8;
uint8_t Channels[NUMCHANNELS];

bool Strobo_en = true;
bool onFlag = false;
IPAddress getlocalIP();
IPAddress myIP(0,0,0,0);
IPAddress myAPIP(0,0,0,0);
uint32_t nodeID = 0;
// Prototypes
void receivedCallback(uint32_t from, String & msg);
void newConnectionCallback(uint32_t nodeId);
void changedConnectionCallback(); 
void nodeTimeAdjustedCallback(int32_t offset); 
void delayReceivedCallback(uint32_t frm, int32_t delay);
void strobo_func();
void setcolor(uint8_t R,uint8_t G,uint8_t B,uint8_t W);
void updatecolor(uint8_t R,uint8_t G,uint8_t B,uint8_t W);
void BlendAnimUpdate(const AnimationParam& param);

void BlendAnimUpdate(const AnimationParam& param)
{
    RgbwColor updatedColor = RgbwColor::LinearBlend(
        ColorState[param.index].PreviousColor,
        ColorState[param.index].CurrentColor,
        param.progress);
    setcolor(updatedColor.R,updatedColor.G,updatedColor.B,updatedColor.W);
}
void updatecolor(uint8_t R,uint8_t G,uint8_t B,uint8_t W){
  if(Channels[7] == 0){
    //swap colors
    ColorState[0].PreviousColor =  ColorState[0].CurrentColor;
    ColorState[0].CurrentColor  = RgbwColor(R,G,B,W);
    //set color
    setcolor(R,G,B,W);
  }else{
    //fix so that fading etc can never take longer as Animation
    uint16_t time = Channels[7] * 4; 
    uint16_t animation_period = 65536;
    if(Channels[5]){
     animation_period = 60000/Channels[5] - 10;
    }
    if((animation_period) < time){
      time = animation_period;
    }
    //swap colors
    ColorState[0].PreviousColor =  ColorState[0].CurrentColor;
    ColorState[0].CurrentColor  = RgbwColor(R,G,B,W);
    //start FADE
    animations.StartAnimation(0, time, BlendAnimUpdate);
  }
}
void setcolor(uint8_t R,uint8_t G,uint8_t B,uint8_t W){
#if RGBW == 1
    RgbwColor color(R,G,B,W);
    for(uint16_t i=0; i<PixelCount; i++) {
      strip.SetPixelColor(i, color);
    }
#elif RGB == 1
    int Ri = R + W;
    if(Ri > 255) Ri = 255;
    int Gi = G + W;
    if(Gi > 255) Gi = 255;
    int Bi = B + W;
    if(Bi > 255) Bi = 255;
    RgbColor color(Ri,Gi,Bi);
    for(uint16_t i=0; i<PixelCount; i++) {
        strip.SetPixelColor(i, color);
    }
#endif
}
void Animation_func(){
   switch(Channels[6]){
      case 1:{
        uint8_t Red   = (int)((double)Channels[0] * ((double)(random(10) / 10.0)));
        uint8_t Green = (int)((double)Channels[1] * ((double)(random(10) / 10.0)));
        uint8_t Blue  = (int)((double)Channels[2] * ((double)(random(10) / 10.0)));
        uint8_t White = (int)((double)Channels[3] * ((double)(random(10) / 10.0)));
        updatecolor(Red,Green,Blue,White);
        break;
      }
      // case 2:{
        // Red = 100;
        // updatecolor(Red,Green,Blue,White);
      // }
      default:{
        uint8_t Red   = Channels[0];
        uint8_t Green = Channels[1];
        uint8_t Blue  = Channels[2];
        uint8_t White = Channels[3];
        updatecolor(Red,Green,Blue,White);
      }
   }
}
void strobo_func(){
      if(Channels[4] > 0){  
        if (Strobo_en){
          Strobo_en = false;
        }
        else{
          Strobo_en = true;
        }
        Strobo.setInterval((1000)/((long)(Channels[4]) / 2.55));
        uint8_t Red   = Channels[0]   * Strobo_en;
        uint8_t Green = Channels[1]   * Strobo_en;
        uint8_t Blue  = Channels[2]   * Strobo_en;
        uint8_t White = Channels[3]   * Strobo_en;
        updatecolor(Red,Green,Blue,White);
      }
      else{
        //todo we2C30
      }
}

//Arduino routine
void setup() {
  Serial.begin(115200);
  for(int i =0; i < NUMCHANNELS; i++){
    Channels[i] = 0;
  }

  if(DEBUG){
    Channels[0] = 255;
    Channels[1] = 0;
    Channels[2] = 0;
    Channels[3] = 0;
    Channels[4] = 0;
    Channels[5] = 100;
    Channels[6] = 1;
    Channels[7] = 1;
  }

  mesh.setDebugMsgTypes(ERROR | DEBUG);
  
  // void init(TSTRING ssid, TSTRING password, Scheduler *baseScheduler,
  //           uint16_t port = 5555, WiFiMode_t connectMode = WIFI_AP_STA,
  //           uint8_t channel = 1, uint8_t hidden = 0,
  //           uint8_t maxconn = MAX_CONN) 

  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT,WIFI_AP_STA,4,0,4);

  mesh.onReceive(&receivedCallback);
  mesh.setRoot(false);
  mesh.setContainsRoot(true);
  mesh.initOTAReceive(ROLE);

  nodeID = mesh.getNodeId();
  mesh.setRoot(false);
  mesh.setContainsRoot(true);
  mesh.initOTAReceive(ROLE);

  Strobo.set(1000,-1,[](){
      strobo_func();
  });
  userScheduler.addTask(Strobo);

  blinkNoNodes.set(BLINK_PERIOD, (mesh.getNodeList().size() + 1) * 2, []() {
      if (onFlag)
        onFlag = false;
      else
        onFlag = true;
      blinkNoNodes.delay(BLINK_DURATION);
      if (blinkNoNodes.isLastIteration()) {
        blinkNoNodes.setIterations((mesh.getNodeList().size() + 1) * 2);
        blinkNoNodes.enableDelayed(BLINK_PERIOD - 
            (mesh.getNodeTime() % (BLINK_PERIOD*1000))/1000);
      }
  });
  userScheduler.addTask(blinkNoNodes);
  blinkNoNodes.enable();

  Animation.set(60000/Channels[5],-1,[](){
      Animation_func();
  });
  userScheduler.addTask(Animation);
  Animation.enable();

  randomSeed(analogRead(A0));
  myAPIP = IPAddress(mesh.getAPIP());
  Serial.println("My AP IP is " + myAPIP.toString());
  strip.Begin();
  strip.Show();
}


void loop() {
  mesh.update();
  if (animations.IsAnimating()){
    animations.UpdateAnimations();
  }
  strip.Show();

  //blink amount of nodes to debug
  if(1) digitalWrite(LED_BUILTIN, !onFlag);
  //update IP
  if(myIP != getlocalIP()){
    myIP = getlocalIP();
    Serial.println("My IP is " + myIP.toString());
  }
}



// Mesh functions
void receivedCallback(uint32_t from, String & msg) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
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
      if(channelnumber >= NUMCHANNELS){
        Serial.println("ERROR: Channel number is to high numberofchannels");
        continue;
      }
      if(value > 255){
        Serial.print("ERROR: value is bigger then 255: ");
        Serial.println(value);
        continue;
      }
      Channels[channelnumber] = value;   
      if(channelnumber == 4){
          if(!Strobo.isEnabled()){
            Strobo.enable();
          }
      }
      if(channelnumber == 5){
        if((Channels[5] == 0 ) || (Channels[5] == 255)){
          Animation.disable();
          Animation_func();
        }
        else{
         Animation.setInterval(60000/Channels[5]);
          if(!Animation.isEnabled()){
            Animation.enable();
          }
        }
      }
    }
    else if(str[0] == 'K'){
      Serial.println("KICK");
      if(Channels[5] == 255){
          Animation_func();
      }
    }
    else{
       
       Serial.print("ERROR: Misformed message :");
       Serial.println(str);
    }
  } //end receive!

  if((Channels[6] == 0)){   //if no animation is running we need to update the color!
      Animation_func();
  }
}
void newConnectionCallback(uint32_t nodeId) {
  // Reset blink task
  onFlag = false;
  blinkNoNodes.setIterations((mesh.getNodeList().size() + 1) * 2);
  blinkNoNodes.enableDelayed(BLINK_PERIOD - (mesh.getNodeTime() % (BLINK_PERIOD*1000))/1000);
 
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
  Serial.printf("--> startHere: New Connection, %s\n", mesh.subConnectionJson(true).c_str());
}
void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
  // Reset blink task
  onFlag = false;
  blinkNoNodes.setIterations((mesh.getNodeList().size() + 1) * 2);
  blinkNoNodes.enableDelayed(BLINK_PERIOD - (mesh.getNodeTime() % (BLINK_PERIOD*1000))/1000);
 
  nodes = mesh.getNodeList();

  Serial.printf("Num nodes: %d\n", nodes.size());
  Serial.printf("Connection list:");

  SimpleList<uint32_t>::iterator node = nodes.begin();
  while (node != nodes.end()) {
    Serial.printf(" %u", *node);
    node++;
  }
  Serial.println();
}
void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}
void delayReceivedCallback(uint32_t from, int32_t delay) {
  Serial.printf("Delay to node %u is %d us\n", from, delay);
}
IPAddress getlocalIP() {
  return IPAddress(mesh.getStationIP());
}