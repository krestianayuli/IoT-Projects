#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include "SPIFFS.h"


byte flags = 0b00111110;
byte bpm;
byte soil[4] = {0, 0, 0, 0};
byte hrmPos[1] = {2};

bool _BLEClientConnected = false;

#define soilRateService BLEUUID((uint16_t)0x180D)
BLECharacteristic soilRateMeasurementCharacteristics(BLEUUID((uint16_t)0x2A37), BLECharacteristic::PROPERTY_NOTIFY);
BLECharacteristic sensorPositionCharacteristic(BLEUUID((uint16_t)0x2A38), BLECharacteristic::PROPERTY_READ);
BLEDescriptor soilRateDescriptor(BLEUUID((uint16_t)0x2901));
BLEDescriptor sensorPositionDescriptor(BLEUUID((uint16_t)0x2901));

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      _BLEClientConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      _BLEClientConnected = false;
    }
};

void InitBLE() {
  BLEDevice::init("FT7");
  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pSoil = pServer->createService(soilRateService);

  pSoil->addCharacteristic(&soilRateMeasurementCharacteristics);
  soilRateDescriptor.setValue("Rate from 0 to 200");
  soilRateMeasurementCharacteristics.addDescriptor(&soilRateDescriptor);
  soilRateMeasurementCharacteristics.addDescriptor(new BLE2902());

  pSoil->addCharacteristic(&sensorPositionCharacteristic);
  sensorPositionDescriptor.setValue("Position 0 - 6");
  sensorPositionCharacteristic.addDescriptor(&sensorPositionDescriptor);

  pServer->getAdvertising()->addServiceUUID(soilRateService);

  pSoil->start();
  // Start advertising
  pServer->getAdvertising()->start();
}


void setup() {
  Serial.begin(115200);
  
  if(!SPIFFS.begin(true)){
     Serial.println("An Error has occurred while mounting SPIFFS");
     return;
  }
  
  Serial.println("Start");
  InitBLE();
  bpm = 1;
}

String getValue(String data, char separator, int index){
  int strIndex[] = {0,-1};
  int maxIndex = data.length()-1;
  int found=0;
        
  for(int i = 0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i == maxIndex){
      found++;
      strIndex[0]= strIndex[1]+1;
      strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }
  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}
  
void read(int lokasi){
  File file = SPIFFS.open("/test1.csv");
  bool iya = true;
  int count = 0;

  if(file.available()){
    while(iya){
      String data =file.readStringUntil('\n');
      if(count == lokasi){
          String data1 = getValue(data, ',',1);
          String data2 = getValue(data, ',',2);
          String data3 = getValue(data, ',',3);
          String data4 = getValue(data, ',',4);
          Serial.print(data1+" ");
          Serial.print(data2+" ");
          Serial.print(data3+" ");
          Serial.println(data4);
          soil[0]=byte(data1.toDouble()*100);
          soil[1]=byte(data2.toDouble()*100);
          soil[2]=byte(data3.toDouble()*100);
          soil[3]=byte(data4.toDouble()*100);
          iya = false;
      }
      count =count+1;
    }
  }
}

int a = 0;
void loop() {
  // put your main code here, to run repeatedly:
 
  a= a+1;
  read(a);
  
  soilRateMeasurementCharacteristics.setValue(soil, 4);
  soilRateMeasurementCharacteristics.notify();

  sensorPositionCharacteristic.setValue(hrmPos, 1);

  delay(10);
}
