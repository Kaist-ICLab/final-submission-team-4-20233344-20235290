/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

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

#ifndef TENSORFLOW_LITE_MICRO_EXAMPLES_HELLO_WORLD_CONSTANTS_H_
#define TENSORFLOW_LITE_MICRO_EXAMPLES_HELLO_WORLD_CONSTANTS_H_

// This constant represents the range of x values our model was trained on,
// which is from 0 to (2 * Pi). We approximate Pi to avoid requiring additional
// libraries.
const float kXrange = 2.f * 3.14159265359f;

// This constant determines the number of inferences to perform across the range
// of x values defined above. Since each inference takes time, the higher this
// number, the more time it will take to run through the entire range. The value
// of this constant can be tuned so that one full cycle takes a desired amount
// of time. Since different devices take different amounts of time to perform
// inference, this value should be defined per-device.
extern const int kInferencesPerCycle;


// Sampling frequency
const double F_S_BVP = 64; // Hz
const double F_S_TEMP = 4; // Hz
const double WND = 30; // s
const float OVERLAP = 0.9; // ratio

const float SCALING_FACTOR[9] = {0.025,  0.02,  0.5,   0.11026747 , 3.0, 0.16666, 0.125,  0.5, 5};
const float SCALING_MIN[9] = {-11.375,  -11.2, -255,  -2.85611668 ,  -4.2, -4.6648,  -3.625, -16,  -3};

const int RED = 22;
const int GREEN = 23;
const int BLUE = 24;

const int BUF_SIZE_FOR_COMM = 5; // how many window shifting

#endif  // TENSORFLOW_LITE_MICRO_EXAMPLES_HELLO_WORLD_CONSTANTS_H_
