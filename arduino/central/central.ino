#include <Filters.h>
#include <AH/Timing/MillisMicrosTimer.hpp>

#include <ArduinoBLE.h>

// constants
const int STRESS_LV_1 = 205;
const int STRESS_LV_2 = 220;
const int STRESS_LV_3 = 235;
const int STRESS_LV_4 = 250;

const int MOTOR_PIN = D2;
const int PERIOD = 15; // in second
const int VIB_DURATION = 150; // in milli second

const int RED = 22; // Red LED
const int GREEN = 23; // Green LED


  // for Bluetooth comm
const char* deviceServiceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214";
const char* deviceServiceCharacteristicUuid = "19b10001-e8f2-537e-4f6c-d104768a1214";
const char* uuid = "19b10001-e8f2-537e-4f6c-d104768a1215";


Timer<micros> TIMER_BEGIN = 1e6 * PERIOD;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial);

  if (!BLE.begin()) {
    Serial.println("* Starting Bluetooth® Low Energy module failed!");
    while (1);
  }
  
  BLE.setLocalName("Nano 33 BLE (Central)"); 
  BLE.advertise();
}


// declaration
int motor_lv;
bool motor_on; 
unsigned long prev_time = 0; // in milli second
int beat_interval = 1e3 * PERIOD; // in milli second
int beat_per_period;
int stress_lv;
float led_val;

void loop() {

  // For bluetooth comm.
  BLEDevice peripheral;
  
  Serial.println("- Discovering peripheral device...");

  do
  {
    BLE.scanForUuid(deviceServiceUuid);
    peripheral = BLE.available();
  } while (!peripheral);
  
  if (peripheral) {
    Serial.println("* Peripheral device found!");
    Serial.print("* Device MAC address: ");
    Serial.println(peripheral.address());
    Serial.print("* Device name: ");
    Serial.println(peripheral.localName());
    Serial.print("* Advertised service UUID: ");
    Serial.println(peripheral.advertisedServiceUuid());
    Serial.println(" ");
    BLE.stopScan();
    Serial.println("- Connecting to peripheral device...");

    if (peripheral.connect()) {
      Serial.println("* Connected to peripheral device!");
      Serial.println(" ");
    } else {
      Serial.println("* Connection to peripheral device failed!");
      Serial.println(" ");
      return;
    }

    Serial.println("- Discovering peripheral device attributes...");
    if (peripheral.discoverAttributes()) {
      Serial.println("* Peripheral device attributes discovered!");
      Serial.println(" ");
    } else {
      Serial.println("* Peripheral device attributes discovery failed!");
      Serial.println(" ");
      peripheral.disconnect();
      return;
    }

    BLECharacteristic stressCharacteristic = peripheral.characteristic(uuid);
      
    if (!stressCharacteristic) {
      Serial.println("no simple key characteristic found!");
      peripheral.disconnect();
      return;
    } else if (!stressCharacteristic.canSubscribe()) {
      Serial.println("simple key characteristic is not subscribable!");
      peripheral.disconnect();
      return;
    } else if (!stressCharacteristic.subscribe()) {
      Serial.println("subscription failed!");
      peripheral.disconnect();
      return;
    } else {
      Serial.println("Subscribed");
    }
    
    // when the bluetooth connection started, start the following steps
    while (peripheral.connected()) {

      unsigned long cur_time = millis();
      
      // When each period begins (15 s in this file)
      if (TIMER_BEGIN) {
        // get the values from the peripheral 
        if (stressCharacteristic.valueUpdated()){
          byte stress[2];
          stressCharacteristic.readValue(stress, 2);

          int beat_per_period = (int) stress[0]; // From the peripheral 
          int stress_lv = (int) stress[1]; // From the peripheral
          
          Serial.println(beat_per_period);
          Serial.println(stress_lv);
        }  

        // calculate the beat interval
        beat_interval = 1e3 * PERIOD / beat_per_period;

        // calculate the motor and LED level
        switch (stress_lv) {
          case 1:
            motor_lv = STRESS_LV_1;
            break;
          case 2:
            motor_lv = STRESS_LV_2;
            break;
          case 3:
            motor_lv = STRESS_LV_3;
            break;
          case 4:
            motor_lv = STRESS_LV_4;
            break;
        }
        
        led_val = 255 * (4.0 - stress_lv)/3.0;

        // analog write
        analogWrite(MOTOR_PIN, motor_lv);
        analogWrite(RED, led_val);
        analogWrite(GREEN, 255 - led_val);

        motor_on = true;
        prev_time = cur_time;
      }
      
      // The vibration will end after the duration (100 ms in this file)
      if (motor_on && cur_time - prev_time > VIB_DURATION) {
        analogWrite(MOTOR_PIN, 0);
        analogWrite(RED, 255);
        analogWrite(GREEN, 255);
        motor_on = false;
      }

      // The vibration will start again after the interval between beats (varied by the beat from the peripheral device)
      if (!motor_on && cur_time - prev_time > beat_interval) {
        analogWrite(MOTOR_PIN, motor_lv);
        analogWrite(RED, led_val);
        analogWrite(GREEN, 255 - led_val);
        motor_on = true;
        prev_time = cur_time;
      }

    }
  }

}
