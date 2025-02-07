

//Last modification on 18/10/2023
//--------------------THIS COMMENTS ADDED BY YASSER ELHOUJJAJI---------------------------

// LIBRARY FOR A TEMPERATURE AND HUMIDITY SENSOR SHT31

// GLOBAL VARIABLES
#define LED_SEND  PA8
#define LED_RECV  PB2
//#define LED_SYNC  PA0
#define LED_SYNC  PA10
#define VSS_PIN   PB5
#define PWR_ON    LOW

// YOU CAN CHANGE THIS ID FOR YOUR DEVICE TEMP LORA
char NODE_ID = 'B';//id for test C = T1

// The value will quickly become too large for an int to store
unsigned long Previous_time = 0;  // will store last time LED was updated

// constants won't change:
const long interval = 86400000;  // interval at which to blink (milliseconds)

float humd ;    
float temp ;     

#include "DFRobot_SHT20.h"

DFRobot_SHT20 sht20;

// FOR A TEMPERATURE AND HUMIDITY SENSOR SHT31

// START SETTINGS FOR A LORA PHYSICAL LAYER
double myFreq = 868000000;
uint16_t sf = 12, bw = 0, cr = 0, preamble = 8, txPower = 22;
// END SETTINGS FOR A LORA PHYSICAL LAYER

void send_cb(void) {
  Serial.println("send callback");
}

void setup() {
  Previous_time = millis();
  // LED STATUS CONFIQURATION
  pinMode(LED_SEND, OUTPUT);
  pinMode(LED_RECV, OUTPUT);
  pinMode(LED_SYNC, OUTPUT);
  // END LED STATUS CONFIQURATION

  // SERIAL INIT
  Serial.begin(115200);

  
  Serial.println("RAK3172_Canopus LoRaWan P2P Example");
  Serial.println("------------------------------------------------------");
  delay(2000);

  if (api.lora.nwm.get() != 0) {
    Serial.printf("Set Node device work mode %s\r\n",
                  api.lora.nwm.set() ? "Success" : "Fail");
    api.system.reboot();
  }

  Serial.println("P2P Start");
  Serial.printf("Hardware ID: %s\r\n", api.system.chipId.get().c_str());
  Serial.printf("Model ID: %s\r\n", api.system.modelId.get().c_str());
  Serial.printf("RUI API Version: %s\r\n",
                api.system.apiVersion.get().c_str());
  Serial.printf("Firmware Version: %s\r\n",
                api.system.firmwareVersion.get().c_str());
  Serial.printf("AT Command Version: %s\r\n",
                api.system.cliVersion.get().c_str());
  Serial.printf("Set P2P mode frequency %3.3f: %s\r\n", (myFreq / 1e6),
                api.lora.pfreq.set(myFreq) ? "Success" : "Fail");
  Serial.printf("Set P2P mode spreading factor %d: %s\r\n", sf,
                api.lora.psf.set(sf) ? "Success" : "Fail");
  Serial.printf("Set P2P mode bandwidth %d: %s\r\n", bw,
                api.lora.pbw.set(bw) ? "Success" : "Fail");
  Serial.printf("Set P2P mode code rate 4/%d: %s\r\n", (cr + 5),
                api.lora.pcr.set(cr) ? "Success" : "Fail");
  Serial.printf("Set P2P mode preamble length %d: %s\r\n", preamble,
                api.lora.ppl.set(preamble) ? "Success" : "Fail");
  Serial.printf("Set P2P mode tx power %d: %s\r\n", txPower,
                api.lora.ptp.set(txPower) ? "Success" : "Fail");
  api.lora.registerPSendCallback(send_cb);
  // let's kick-start things by waiting 3 seconds.
  pinMode(VSS_PIN, OUTPUT);
  digitalWrite(VSS_PIN, PWR_ON);
  delay(100);
  
  Wire.begin();
  Serial.println("SHT20 ");
  sht20.initSHT20();  // Init SHT20 Sensor
  delay(100);
  sht20.checkSHT20();  // Check SHT20 Sensor
  //sht30.checkSHT20();   // Check SHT20 Sensor
}

void loop() {
  Read_Sensor_Data();
  Send_Data_To_Gateway();
  Mode_Sleep();
  Reboot();
}


void Read_Sensor_Data() {
  humd = sht20.readHumidity();     // Read Humidity
  temp = sht20.readTemperature();  // Read Temperature       
}


void Send_Data_To_Gateway(){
  
    digitalWrite(LED_SEND,HIGH); 
    Serial.print(" Temperature: ");
    Serial.print(temp, 2);
    Serial.print("C");
    Serial.print("\t Humidity: ");
    Serial.print(humd, 2);
    Serial.println("%");
    String msg = "*" + String(NODE_ID) + "," + String(int(temp*100)) + "," + String(int(humd*100)) + "#";
    Serial.printf("\r\nmsg: ");
    Serial.println(msg);
    uint8_t payload[msg.length()+1];
    for (int i = 0; i < msg.length(); i++) {
      payload[i] = msg[i];
      }
    Serial.printf("Lenght: %d\r\n",msg.length());
    bool send_result = api.lora.psend(sizeof(payload), payload);
    Serial.printf("P2P send %s\r\n", send_result ? "Success" : "Fail");
    delay(3000);
    digitalWrite(LED_SEND,LOW); 
  }

void Mode_Sleep(){
  
    //********************** mettre en veille pendant 5 min ******************************
    digitalWrite(LED_SYNC, HIGH);
    //********************** mettre en veille pendant 5 min ******************************
    Serial.print("The timestamp before sleeping: ");
    Serial.print(millis());
    Serial.println(" ms");
    Serial.println("(Wait 10 min or Press any RESET to wakeup)");
    api.system.sleep.all(600000);
    Serial.print("The timestamp after sleeping: ");
    Serial.print(millis());
    Serial.println(" ms");
    //___________________________________________________________________________________
    digitalWrite(LED_SYNC, LOW);
    //___________________________________________________________________________________
}

void Reboot(){
  
    if ((millis() - Previous_time) >= interval){
      api.system.reboot();
    }
  
}
