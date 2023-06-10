#include </Users/lab/Documents/Arduino/libraries/CircularBuffer/CircularBuffer.h>
#include <Filters.h>
#include <PeakDetection.h> 
#include <AH/Timing/MillisMicrosTimer.hpp>
#include <Filters/Butterworth.hpp>

PeakDetection peakDetection;  

/* Copyright 2020 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include <TensorFlowLite.h>

#include "main_functions.h"

#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "constants.h"
#include "model.h"
#include "output_handler.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"


#include <ArduinoBLE.h>

// Globals, used for compatibility with Arduino-style sketches.
namespace {
  tflite::ErrorReporter* error_reporter = nullptr;
  const tflite::Model* model = nullptr;
  tflite::MicroInterpreter* interpreter = nullptr;
  TfLiteTensor* input = nullptr;
  TfLiteTensor* output = nullptr;
  int inference_count = 0;
  int num_inferences_per_cycle = BUF_SIZE_FOR_COMM;

  constexpr int kTensorArenaSize = 2000;
  uint8_t tensor_arena[kTensorArenaSize];
}  // namespace

const char* deviceServiceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214";
const char* deviceServiceCharacteristicUuid_test = "19b10001-e8f2-537e-4f6c-d104768a1215";

BLEService stressService(deviceServiceUuid); 
BLECharacteristic stressCharacteristic(deviceServiceCharacteristicUuid_test, BLERead | BLEWrite | BLENotify, 2);

void setup() {
  pinMode(GREEN, OUTPUT);
  pinMode(RED, OUTPUT);
  peakDetection.begin();
  Serial.begin(9600);
  while (!Serial);  
  // Set up logging. Google style is to avoid globals or statics because of
  // lifetime uncertainty, but since this has a trivial destructor it's okay.
  // NOLINTNEXTLINE(runtime-global-variables)

  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;

  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  model = tflite::GetModel(g_model);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    TF_LITE_REPORT_ERROR(error_reporter,
                         "Model provided is schema version %d not equal "
                         "to supported version %d.",
                         model->version(), TFLITE_SCHEMA_VERSION);
    return;
  }

  // This pulls in all the operation implementations we need.
  // NOLINTNEXTLINE(runtime-global-variables)
  static tflite::AllOpsResolver resolver;

  // Build an interpreter to run the model with.
  static tflite::MicroInterpreter static_interpreter(
      model, resolver, tensor_arena, kTensorArenaSize, error_reporter);
  interpreter = &static_interpreter;

  // Allocate memory from the tensor_arena for the model's tensors.
  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if (allocate_status != kTfLiteOk) {
    TF_LITE_REPORT_ERROR(error_reporter, "AllocateTensors() failed");
    return;
  }

  // Obtain pointers to the model's input and output tensors.
  input = interpreter->input(0);
  output = interpreter->output(0);


  // Keep track of how many inferences we have performed.
  inference_count = 0;


  if (!BLE.begin()) {
    Serial.println("- Starting Bluetooth® Low Energy module failed!");
    while (1);
  }
  BLE.setLocalName("Arduino Nano 33 BLE (Peripheral)");
  BLE.setAdvertisedService(stressService);
  stressService.addCharacteristic(stressCharacteristic);
  BLE.addService(stressService);
  byte val[2];
  val[0] = 15;
  val[1] = 2;
  stressCharacteristic.writeValue(val, 2);
  BLE.advertise();
  Serial.println("Nano 33 BLE (Peripheral Device)");
  Serial.println(" ");
}

// constants from contants.h
const int arr_size_bvp = WND * F_S_BVP;
const int arr_size_temp = WND * F_S_TEMP;
const int sliding_lag_bvp = WND * (1-OVERLAP) * F_S_BVP;
const int sliding_lag_temp = WND * (1-OVERLAP) * F_S_TEMP;


const byte NUMBER_OF_FEATURES = 9;

CircularBuffer<int, arr_size_bvp> bvp;
CircularBuffer<byte, arr_size_bvp> peaks;
CircularBuffer<float, arr_size_temp> temp;

int peak_cnt = 0;
int prev_peak ;
float stress_inference[BUF_SIZE_FOR_COMM];
float beat_per_period;
byte stress_lv;

// Sample timer
Timer<micros> timer_bvp_sample = std::round(1e6 / F_S_BVP);
Timer<micros> timer_temp_sample = std::round(1e6 / F_S_TEMP);

void loop() {
  BLEDevice central = BLE.central();
  
  if (timer_bvp_sample) {  // Stores a BVP value and peak at every sampling timing
    bvp.push(analogRead(A1));
    peakDetection.add(analogRead(A1));
    int peak = peakDetection.getPeak();
    if (peak > 0 && prev_peak < peak) {
      peaks.push(peak);
      // peak_cnt = peak_cnt + peak; 
    } else peaks.push(0);
    prev_peak = peak;
  }

  if (timer_temp_sample) {  // Stores a temperature value at every sampling timing
    float temp_val = 3.3 * analogRead(A2) * 100 / 1024.0;
    temp.push(temp_val);
  }


  if (bvp.isFull()) {  // Extract features when the circular buffer is full
    // calculating features using the full bf
    int min_bvp = 1500;
    int max_bvp = -100;
    float mean_bvp = 0;
    float total_bvp = 0;
    int peak_cnt = 0;
    float min_temp = 50;
    float max_temp = -10;
    float mean_temp = 0;
    float total_temp = 0;

    // Features related to BVP
    for (int i = 0; i < arr_size_bvp; i++) {
      if (min_bvp > bvp[i]) {
        min_bvp = bvp[i];
      }
      if (max_bvp < bvp[i]) {
        max_bvp = bvp[i];
      }
      mean_bvp += (float) bvp[i] / (float) arr_size_bvp;
      if (peaks[i] == 1) {
        peak_cnt++;
      }
    }

    for (int i = 0; i < arr_size_bvp; i++) {
      total_bvp = total_bvp + (bvp[i] - mean_bvp) * (bvp[i] - mean_bvp);
    }

    float variance_bvp = total_bvp / (float) arr_size_bvp;
    float std_bvp = sqrt(variance_bvp);
    float peak_freq = (float) peak_cnt / (float) WND;
    
    // Features related to temperature
    for (int i = 0; i < arr_size_temp; i++) {
      if (min_temp > temp[i]) {
        min_temp = temp[i];
      }
      if (max_temp < temp[i]) {
        max_temp = temp[i];
      }
      mean_temp += (float) temp[i] / (float) arr_size_temp;
    }

    for (int i = 0; i < arr_size_temp; i++) {
      total_temp = total_temp + (temp[i] - mean_temp) * (temp[i] - mean_temp);
    }

    float variance_temp = total_temp / (float) arr_size_temp;
    float std_temp = sqrt(variance_temp);


    // After feature extraction, vacate the circular buffer
    peak_cnt = 0;
    for (int i = 0; i < sliding_lag_bvp; i++){
      bvp.shift();
      peaks.shift();
    }
    for (int i = 0; i < sliding_lag_temp; i++){
      temp.shift();
    }

    float features[9] = {min_bvp, max_bvp, mean_bvp, std_bvp, peak_freq, min_temp, max_temp, mean_temp, std_temp};  // Assign the extracted features to a float array

    beat_per_period += features[4] * WND * (1-OVERLAP);

    for (int i = 0; i < NUMBER_OF_FEATURES; i++) {  // Feature scaling to make the feature values ranging from 0 to 1
      float raw_val = features[i];
      float val = features[i]* SCALING_FACTOR[i] + SCALING_MIN[i];

      // For the outliers, make the scaled features 0 or 1
      if  (val > 1) {
        val = 1;
      } else if (val < 0) {
        val = 0;
      }
      features[i] = val;
    }

    for (int i = 0; i< NUMBER_OF_FEATURES; i++) {  // Put the scaled features to input plate
      input->data.f[i] = features[i];
    }

    // Run inference, and report any error
    TfLiteStatus invoke_status = interpreter->Invoke();


    // Obtain the quantized output from model's output tensor
    float y_0 = output->data.f[0];  // Probability of non-stress  
    float y_1 = output->data.f[1];  // Probability of stress

    Serial.print("Probability of stress: ");
    Serial.println(y_1);
    
    // // Using output, show with LED
    analogWrite(RED, 255 * y_0);
    analogWrite(GREEN, 255 * y_1);

    stress_inference[inference_count] = y_1;
    
    // Increment the inference_counter, and reset it if we have reached
    // the total number per cycle
    inference_count++;
    if (inference_count >= num_inferences_per_cycle) {
      // calcualte the overall stress level
      float mean = 0;
      for (int i = 0; i < num_inferences_per_cycle; i++) {
        mean += stress_inference[i] / num_inferences_per_cycle;
      }

      if (mean >= 0 && mean < 0.25) {  // Depending on the probability of the stress, we define the level of stress
        stress_lv = 0;
      } else if (mean >= 0.25 && mean < 0.5) {
        stress_lv = 1;
      } else if (mean >= 0.5 && mean < 0.75) {
        stress_lv = 2;
      } else {
        stress_lv = 3;
      }

      byte stress_transmission[2];
      stress_transmission[0] = (byte) beat_per_period;
      stress_transmission[1] = stress_lv;
      Serial.print("* heartrate (bpm): ");
      Serial.println(stress_transmission[0] * 60 / (num_inferences_per_cycle * WND * (1-OVERLAP))); // To calculate the BPM(per min)
      Serial.print("* stress level: ");
      Serial.println(stress_transmission[1]);

      if (central && central.connected()) {  // send the values of beat_per_period and the stress level to the central device
        stressCharacteristic.writeValue(stress_transmission, 2);
        Serial.println("* stress_transmission!");
      }
      else {
        Serial.println("* Disconnected to central device!");
      }
      
      // clearing vals for bluetooth
      inference_count = 0;
      for (int i = 0; i < num_inferences_per_cycle; i++) {
        stress_inference[i] = 0;
      }
      beat_per_period = 0;
    }
  }
}
