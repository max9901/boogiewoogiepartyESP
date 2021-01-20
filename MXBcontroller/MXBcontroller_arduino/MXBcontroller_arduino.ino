#include <painlessMesh.h>      // https://gitlab.com/painlessMesh/painlessMesh/-/wikis/home && https://github.com/arkhipenko/TaskScheduler/wiki/API-Task#task
#include "SSD1306.h"
#include "ESP32Encoder.h"      //https://github.com/madhephaestus/ESP32Encoder

//Change to your full Path
#include "/home/max/boogiewoogiepartyESP/passwords.h"

#define   HOSTNAME        "MX_board"
#define   ROLE            "MX-controller"
Scheduler     userScheduler; // to control your personal task
painlessMesh  mesh;
SimpleList<uint32_t> nodes;

#define SCL  22
#define SDA  21
SSD1306  display(0x3c, SDA, SCL);

#define BUTTON0  15
#define BUTTON1  2   
#define BUTTON2  4   
#define BUTTON3  16  
#define BUTTON4  17  
#define BUTTON5  5   
#define BUTTON6  19  
#define BUTTON7  18  
#define ENCODERP 33

#define NBUTTONS 9
const int buttons[] = {BUTTON0,BUTTON1,BUTTON2,BUTTON3,BUTTON4,BUTTON5,BUTTON6,BUTTON7,ENCODERP};
uint16_t button_states = 0;

#define ENCODERA 32
#define ENCODERB 25
ESP32Encoder encoder;

#define LED0 12
#define LED1 14
#define LED2 26
#define LED3 13

#define PWM_FREQ       5000
#define PWM_RESOLUTION 8

uint encoder_A = 0;
uint encoder_B = 0;
uint encoder_A_prev = 0;

// Prototypes
void receivedCallback(uint32_t from, String & msg);
void newConnectionCallback(uint32_t nodeId);
void changedConnectionCallback(); 
void nodeTimeAdjustedCallback(int32_t offset); 
void delayReceivedCallback(uint32_t frm, int32_t delay);

//TODO opruimen !
unsigned long currentTime;
unsigned long loopTime;
uint8_t pwm = 0;

//Interrupt handles*+
void IRAM_ATTR ButtonPUSH() {
    for(int button = 0; button<NBUTTONS;button++){
      button_states = button_states | digitalRead(buttons[button]) << button;
    }
}
void Preform_button_Action(uint16_t val){
    Serial.println(val);
    switch (val)
    {
        case 0: //uit
            // Serial.print("case 0: 0,0&1,0&2,0&3,0&4,0&5,0&6,0&7,0");
            mesh.sendBroadcast("0,0&1,0&2,0&3,0&4,0&5,0&6,0&7,0");
            break;

        case 1: //rood
            mesh.sendBroadcast("0,255&1,0&2,0&3,0&4,0&5,0&6,0&7,0");
            break;

        case 2: //green
            mesh.sendBroadcast("0,0&1,255&2,0&3,0&4,0&5,0&6,0&7,0");
            break;

        case 3: //blauw
            mesh.sendBroadcast("0,0&1,0&2,255&3,0&4,0&5,0&6,0&7,0");
            break;

        case 4: //stobo
            mesh.sendBroadcast("4,100");
            break;

        case 5: //KICK
            mesh.sendBroadcast("K");
            break;
            
        case 6: //animation
            mesh.sendBroadcast("0,255&1,255&2,255&3,0&4,0&5,255&6,1&7,10");
            break;

        case 7: //animatie met kick!
            mesh.sendBroadcast("0,255&1,255&2,255&3,0&4,0&5,26&6,1&7,255");
            break;
        
        case 8: //backlight off
            pwm = 0;
            for(int i = 0; i < 4; i++)ledcWrite(i, pwm);
            break;
    
    }
}

void setup()  {
  // Start serial monitor for debugging
  Serial.begin(115200);
  Serial.println("Hello debugger!");
  
  //Set pinmodes 
  for(int button = 0; button < NBUTTONS; button++){
      pinMode(buttons[button],  INPUT_PULLDOWN);
      attachInterrupt(buttons[button], ButtonPUSH, RISING);
  }

  pinMode(ENCODERA, INPUT_PULLUP);
  pinMode(ENCODERB, INPUT_PULLUP);
  

  ESP32Encoder::useInternalWeakPullResistors=UP;
  encoder.attachHalfQuad(ENCODERA, ENCODERB);

  // configure LED PWM functionalitites
  ledcSetup(0, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(1, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(2, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(3, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(LED0, 0);
  ledcAttachPin(LED1, 1);
  ledcAttachPin(LED2, 2);
  ledcAttachPin(LED3, 3);
  for(int i = 0; i < 4; i++)ledcWrite(i, pwm);

  //Init display
  display.init();
  display.setContrast(255);
  delay(1000);
  display.clear();
  display.setFont(ArialMT_Plain_24);
  display.drawString(0, 10, "~Max");
  display.display();
 
  //init mesh
  mesh.setDebugMsgTypes(ERROR | DEBUG);
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT,WIFI_AP_STA,4,0,4);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  mesh.onNodeDelayReceived(&delayReceivedCallback);
  mesh.setRoot(false);
  mesh.setContainsRoot(true);
  mesh.initOTAReceive(ROLE);


  //todo opruimen
  currentTime = millis();
} 
void loop()  {
    mesh.update();
    //todo opruimen
    if(millis() -  currentTime  > 200){
        currentTime = millis();

        //check button pushes
        if(button_states){
            for(int i = 0; i<NBUTTONS; i++){
                if ((button_states >> i) & 1)
                {
                    Serial.print("Button: ");
                    Serial.println(i);
                    Preform_button_Action(i);
                }   
            }
            button_states = 0;
            
        }

        //check encoder 
        if(encoder.getCount()){
            pwm +=  encoder.getCount() * 10;
            for(int i = 0; i < 4; i++)ledcWrite(i, pwm);
            encoder.clearCount();
        }
    }
}


// Mesh functions
void receivedCallback(uint32_t from, String & msg) {
  Serial.printf("--->startHere: Received from %u msg=%s\n", from, msg.c_str());
}
void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
  Serial.printf("--> startHere: New Connection, %s\n", mesh.subConnectionJson(true).c_str());
}
void changedConnectionCallback() {
  Serial.printf("--->startHere: Changed connections\n");
  nodes = mesh.getNodeList();
  Serial.printf("Num nodes: %d\n", nodes.size());
  Serial.printf("Connection list:");
  SimpleList<uint32_t>::iterator node = nodes.begin();
  while (node != nodes.end()) {
    Serial.printf(" %u", *node);
    node++;
  }
  Serial.println();
  display.clear();
  
  int temp = nodes.size();
  Serial.print(temp);
  String deze = "~n: " + String(temp);
  Serial.println(deze);
  display.drawString(0, 10, deze);
  display.display();

}
void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}
void delayReceivedCallback(uint32_t from, int32_t delay) {
  Serial.printf("Delay to node %u is %d us\n", from, delay);
}
