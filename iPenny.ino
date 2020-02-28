#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define SERVICE_UUID        "2df15924-0c57-11ea-8d71-362b9e155667"
#define CHARACTERISTIC_UUID "2df15ba4-0c57-11ea-8d71-362b9e155667"

BLEServer *pServer;
BLEService *pService;
BLECharacteristic *pCharacteristic;
BLEAdvertising *pAdvertising;

bool deviceConnected = false;

bool coinDetected = false;

int potiPin = 34;
int i = 10;
int counter = 0;
int max_coin = 0;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

void setup() {
  Serial.begin(115200);

  Serial.println("Starting BLE work!");

  BLEDevice::init("iPenny");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE|
                                         BLECharacteristic::PROPERTY_NOTIFY
                                       );
  
  pCharacteristic->setValue("");
  pService->start();
  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined!");
  delay(1000);
}

void loop() {
  int muenze = 0;
  int sensorValue1 = analogRead(potiPin);
  delay(i);
  int sensorValue2 = analogRead(potiPin);
  if(sensorValue1+100 < sensorValue2) coinDetected = true;
  if(coinDetected){
    if(max_coin < sensorValue1){
        max_coin = sensorValue1;
        counter = 0;
    }
    counter++;
    if(counter > 5){
      counter = 0;
      coinDetected = false;
      switch(max_coin){
        case 0 ... 1647:
            muenze = 1;
            break;
        case 1648 ... 1858:
            muenze = 2;
            break;
        case 1859 ... 2012:
            muenze = 4;
            break;
        case 2013 ... 2190:
            muenze = 3;
            break;
        case 2191 ... 2320:
            muenze = 5;
            break;
        case 2321 ... 2432:
            muenze = 7;
            break;
        case 2433 ... 2589:
            muenze = 6;
            break;
        case 2590 ... 4095:
            muenze = 8;
            break;
        default:
            muenze = 0;
            break;
        }

      if(deviceConnected){
        Serial.println("Connected");
        pCharacteristic->setValue(muenze);
        pCharacteristic->notify(); // Send the value to the app!
        Serial.print("*** Sent Value: ");
        Serial.print("Value: ");
        Serial.println(max_coin);
        Serial.print(muenze);
        Serial.println(" ***");
    }else{
        Serial.println("Not Connected");
        Serial.print(muenze);
    }
    max_coin = 0;
    }
  }

}
