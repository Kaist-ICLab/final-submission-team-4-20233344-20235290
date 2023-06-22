# 💓 Pit-a-Pat
Our team implemented a system called Pit-a-Pat, which can detect one’s stress level and express it to a partner by changing the amplitude of the heartbeat vibration. The system was developed for the mini-proejct of the coursework, which is "CS565 IoT Data Science" at KAIST. 

Authors: Hyesoo Park(hyehye@kaist.ac.kr) and Tae-Hoon Lee(th.lee@kaist.ac.kr)

## 🎥 Demo video
[![[CS565] Mini-project: Pit-a-pat (Team 4)](https://img.youtube.com/vi/pnacPsE0Fk8/0.jpg)](https://www.youtube.com/watch?v=pnacPsE0Fk8 "[CS565] Mini-project: Pit-a-pat (Team 4)")


## 📢 Presentation slides
<img width="800" alt="image" src="https://github.com/Kaist-ICLab/final-submission-team-4-20233344-20235290/assets/27489013/ac4f6a0d-8f16-47a4-b0c8-bcbd392e6cf2">

[Link to the slides](https://docs.google.com/presentation/d/1nr1iNvllwkr2UorulvrPxBd-E-VGa7PmlZvasYQV54c/edit?usp=sharing)

## 📄 Final report
[Link to the report](https://github.com/Kaist-ICLab/final-submission-team-4-20233344-20235290/blob/main/Final-report_Pit_a_Pat.pdf)

## ℹ️ Introduction
Expressing and sharing biosignals have gained significant attention in the field of Human-Computer Interaction (HCI) as a means to facilitate user understanding and foster empathy. Previous research has explored visual representations of biosignals, such as numbers and charts. Visual and audio expressions have shown to improve feelings of connectedness and intimacy between users. People tend to interpret heart rate information in relation to emotions and stress levels, similar to the effects of facial expressions. Additionally, studies have demonstrated that hearing a heartbeat can have a comparable impact to direct eye contact.

While the expression of biosignals in a group or in a visually public way has been extensively studied, there is a lack of research on whether the expression of biosignals in a private way (e.g., vibration) enhances connectedness. Thus, we proposes a way to communicate biosignals in real time between two devices in a private manner. Our study aims to investigate whether **1) we can use TinyML devices for real-time stress level classification(using PPG, skin temp.) and whether 2) sharing heartbeat through device’s vibration strengthens the connectedness between two partners**.


## 🔌 Hardware component
### List of Hardware components
[Arduino Nano 33 BLE Sense](https://www.devicemart.co.kr/goods/view?no=10919317) 

[PPG sensor](https://www.devicemart.co.kr/goods/view?no=1327536) 

[Temperature sensor](https://www.devicemart.co.kr/goods/view?no=3183) 

[Vibration motor](https://www.devicemart.co.kr/goods/view?no=12515096) 

### Circuit Diagram for each device
<img width="800" alt="image" src="https://github.com/Kaist-ICLab/final-submission-team-4-20233344-20235290/assets/27489013/3771584c-bd6f-4a6a-b8da-80dc41169a4a">


# 👩🏻‍💻 Modeling
## ⚙️ Environment setting
We performed our modeling inside the Anaconda virtual environment. The version of Anaconda we used is 22.9.0, and to create the same environment as ours, please enter the commands below.

1. `git clone https://github.com/Kaist-ICLab/final-submission-team-4-20233344-20235290.git`
2. `cd final-submission-team-4-20233344-20235290`
3. `conda env create -f conda_requirements.txt`

## 📁 Dataset download
We used the [WESAD dataset](https://dl.acm.org/doi/pdf/10.1145/3242969.3242985) to train and test our model, which is widely used in the field of affective computing. You can download the WESAD dataset from [this link](https://peterhcharlton.github.io/info/datasets/wesad). After downloading it, move 'wesad.zip' to the 'data' directory and unzip it. The directory structure after this process is shown below.

```bash
.
├── conda_requirements.txt
├── modeling/
│   ├── analysis-for-collected-data.ipynb
│   ├── processing.ipynb
│   ├── evaluation.ipynb
│   ├── extract_scaling_factor.ipynb
│   ├── data/
│   │   ├── WESAD/
│   │   ├── arduino_ppg/
│   │   ├── processed/
│   │   └── wesad.zip
│   └── models/
└── README.md
```

## 📝 Data description
WESAD dataset was collected from both a chest-worn device RespiBAN Professional and a wrist-worn device Empatica E4. It contains motion and physiological data such as Respiration, Electrocardiogram, Blood volume pulse, Electrodermal activity, Skin temperature, Accelerometer and so on. In this project, we focus only on data from the wrist-worn device because our prototype is worn on the wrist. The below table shows the used sensors in our project and their sampling rate.

<img width="600" alt="image" src="https://github.com/Kaist-ICLab/final-submission-team-4-20233344-20235290/assets/27489013/e0aa672f-516e-4a3b-afd5-955c3480fc66">


During the data collection, subjects were asked to go through various condition such as baseline condition, amusement condition, stress condition. Previous studies used the study protocol conditions as labels that were used for modeling. Since our interest is how stressed the subjects were, we treated the labels as stress condition and non-stress condition (i.e., baseline condition). 

There are 15 subject in WESAD dataset, however, we used data from four subjects (S14, S15, S16, and S17) only. This is because the data from them is similar to our expectation according to our analysis.



## 💡 Feature extraction
We used [window_slider](https://pypi.org/project/window-slider/) package to run sliding window with overlapping. The window size is 30s and overlapped rate is 0.9 (i.e., 27s between the previous and the current window is overlapped.). We extracted features from each sliding window. The below table organizes the definition of the extracted features and which features are used in the project.

<img width="700" alt="image" src="https://github.com/Kaist-ICLab/final-submission-team-4-20233344-20235290/assets/27489013/1c96a7dc-d6dc-4379-bc74-73712d19a624">


## 💬 Model description
<img width="800" alt="image" src="https://github.com/Kaist-ICLab/final-submission-team-4-20233344-20235290/assets/27489013/f0cde1f5-ab33-4e33-b8a6-89e250792d88">


We adopted an ANN-based neural network with a very simple structure. The structure of the model we used follows the model proposed by a [previous study](https://ieeexplore.ieee.org/abstract/document/9183244). They achieved 95.21% of accuracy and 94.24% of f1-score using the WESAD dataset for stress detection. We follow the structure of their model because it performs well despite being a lightweight model.

The input to the model is 9 features as described in the [section](#-Feature-extraction) above. The model performs the binary classification task of detecting stress vs non-stress. Therefore, the output of the model is 0 (non-stress) and 1 (stress). The figure below shows the summaries of deep learning architecture for the stress classification. As shown, our model requires 126 parameters.

<img width="500" alt="image" src="https://github.com/Kaist-ICLab/final-submission-team-4-20233344-20235290/assets/27489013/90416a06-9fc9-4759-a90c-5bb053f5eea3">


## 💯 Evaluation
<img width="735" alt="image" src="https://github.com/Kaist-ICLab/final-submission-team-4-20233344-20235290/assets/27489013/15dea6b1-e5ea-40a4-ab69-2fdff5e7fe83">


This figure shows the overview of the evaluation process.
For evaluation, we used two datasets which are WESAD dataset and the dataset we collected from Arduino.
WESAD dataset is used to train and evaluate the model by splitting the entire data into k folds. 
Once we find the optimal hyperparameter of the model based on the final performance metric, we select a model that achieve the best performance among all k models, and evaluate it with the data from arduino.

Humans differ in their individual reactions to affective stimuli, resulting in generalizibility of the model for affective computing become an important issue. By using leave-one-subject-out (LOSO) cross-validation, the models are trained on a set of subjects while leaving out one subject for testing. This ensures that the model's performance is assessed on data from previously unseen subjects, making it subject independent and enabling it to generalize better. By employing LOSO evaluation, we can obtain more reliable and robust results. Therefore, we used LOSO cross-validation in this project.

In our experiments, the accruacy and f1-score were 88.65% and 83.11% obtained through LOSO validation, respectively. Among the 4 models trained with the LOSO validation procedure, we selected the best performing model for evaluation with the dataset we collected from Arduino. The accuracy and f1-score of the best model on the dataset from Arduino are 81.11% and 84.62%, respectively.


## 📄 File description
### processing.ipynb
This file removes unnecessary parts (i.e., data from chest sensors) of the WESAD dataset, and performs windowing and feature extraction. After running all the code in this file, subject-specific data is stored in the form of pickle files in the `data/wrist-only` folder. The data from subjects is combined into one csv file and saved as `processed_*.csv` in the `data` folder.

### evaluation.ipynb
Once you run the code of processing.ipynb, you would get `processed_*.csv` file that is used for modeling. In this file, the csv file will be loaded, and the features will be scaled by using MinMaxScaler from the sklearn package. After the evaluation process is done, the model that achieves the best accuracy will be converted into C source. The C source will be stored in `model` folder. The size of the converted model formed as C source file is 2.38 KB.

### analysis-for-collected-data.ipynb
This file generates the features of the collected data from Arduino. The extracted featurs will be stored in `data/processed` as csv format. Data from stress and baseline condition should be processed through the code in this file, and the name of files will be `stress_*.csv` and `baseline_*.csv`, respectively. The csv files will be used in `evaluation.ipynb`.


### extract_scaling_factor.ipynb
The code in this file is for extracting scaling factors that are used for real-time scaling on Arduino board. 


# 🤖 Arduino
## ⚙️ Library setting

You need to install several libraries to run the arduino files. The list is below.

1. `ArduinoBLE` ([LINK](https://www.arduino.cc/reference/en/libraries/arduinoble/))
1. `KAIST_IoTDataScience` ([LINK](https://docs.google.com/document/d/1RkVzO9BRBFxpABKsdp_7ik8IUWONbQ7sJxS0z6xoUu4/edit))
1. `CircularBuffer` ([LINK](https://github.com/rlogiacco/CircularBuffer))
1. `EmotiBit_ArduinoFilters` ([LINK](https://github.com/EmotiBit/EmotiBit_ArduinoFilters))
1. `PeakDetection` ([LINK](https://github.com/leandcesar/PeakDetection/tree/master))


For the library `PeakDetection`, it is not found in Arduino IDE unlike other libraries, so please download from its [github page](https://github.com/leandcesar/PeakDetection/tree/master) and unzip on the library of your **Arduino library directory** similar as below.

```bash
/Users/[user_name]/Documents/Arduino/libraries/PeakDetection/
```

Also, you should modify the first line of `peripheral.ino` using your Arduino library directory to run the code correctly.

```cpp
#include </[user_arduino_libraries_folder]/CircularBuffer/CircularBuffer.h>
```

If the setting is finished, you can **upload** the `central.ino` into the central device, and the `peripheral.ino` into the peripheral device.


## 📁 Directory Structure
You should download both `central`, `peripharal` folder. The main implementation & run file is `central/central.ino` and `peripheral/peripheral.ino`. The directory structure is shown below.

```bash
.
├── central/
│   ├── central.ino *
└── peripheral/
    ├── arduino_constants.cpp 
    ├── arduino_main.cpp
    ├── arduino_output_handler.cpp
    ├── constants.h *
    ├── main_functions.h
    ├── model.cpp *
    ├── model.h 
    ├── output_handler.h
    └── peripheral.ino *
```

## 💡 Real-time feature extraction

In the `peripheral.ino`, feature extraction for stress inference is implemented. The descripation of 9 features is explained above [section](#-Feature-extraction). Simple array type can be used, however the memory and runtime for re-allocating will be exceed the capacity of microcontroller. 

Therefore, we used circular buffer data type to handle this problem. It is a data structure that uses a **single, fixed-size buffer as if it were connected end-to-end**. This structure lends itself easily to buffering data streams ([source](https://en.wikipedia.org/wiki/Circular_buffer)).

If you have any question on the library used in this project, please visit the source [githup page](https://github.com/rlogiacco/CircularBuffer).

Using this buffer, it collects the data from the sensor. The below code snippet is from the `void loop()` in the `peripheral.ino`.

```cpp
CircularBuffer<int, arr_size_bvp> bvp;

if (timer_bvp_sample) {
    bvp.push(analogRead(A1));
    ...
}    
```
Especially for the peak detection, collecting data is not simple as the PPG or temperature sensor data. Using the [`peakDetection`](https://github.com/leandcesar/PeakDetection/tree/master) library, we implemented the real-time peak detection.

```cpp
...
int peak = peakDetection.getPeak();
if (peak > 0 && prev_peak < peak) {
    peaks.push(peak);
} else peaks.push(0);
prev_peak = peak;
...
```
The `peaks` is buffer that has exactly same length with `bvp`, and it has binary values `0` or `1`. It is difficult to use the value `peakDetection.getPeak()` returns directly as the number of peak. The example is below.

```cpp
>> 0 0 0 1 1 1 1 0 0 0 // peakDetection.getPeak() by time
>> 0 0 0 1 0 0 0 0 0 0 // Only detecting 0 to 1 change
```

When the buffer is full, feature extraction started.

```cpp
if (bvp.isFull()) {
    int min_bvp = 1500;
    int max_bvp = -100;
    float mean_bvp = 0;
    ...
}
```
For each feature, the way of calculating the feature is described in the file `peripheral.ino`.


## 📄 File description
Main structure for peripheral device (`peripheral/`) follows the example of `KAIST_IoTDataScience/Lab12_TinyML_Hello_World/`. 

`peripheral.ino`, `constans.h`, `model.cpp` has the part that is newly implemented for this project ( * - starred in the [above](#-Directory-Structure)).

### peripheral.ino
In the loop, the first and second chunk is running **to collect the data from sensor** if it is time to sample (by the sampling rate pre-declared). 

When the buffer for the data windowing is full, the feature extraction as explained above [section](#-Real-time-feature-extraction) started. Using the extracted features and the scaling factor/min, normalized values is used in running the inference. The outputs are displayed with the LED. It is transmitted with the bluetooth communication after the several inferences were occured. After feature extraction the process ended, vacate the circular buffer for window shifting (_window size * (1 - overlap ratio)_, which is 3s in this file)

### constants.h

In this file, some constants used in the `peripheral.ino` such as the sampling freq. of sensors are saved. For the `SCALING_FACTOR[9]` and `SCALING_MIN[9]`, it is personalized scale for one of the researcher, so you should change it in order to fit to your own data. The way of using is below.

1. Collect the ppg sensor and temperature sensor data in two different setting
    * **Baseline setting**: Meditation (*3 mins*)
    * **Stress setting**: Mental Arithmetic Task (*3 mins*) [Example Link](https://rankyourbrain.com/mental-math/mental-math-test-advanced/results)  
1. Run the [`extract_scaling_factor.ipynb`](#-extract_scaling_factoripynb) 
1. Get the scaling factor and scaling min, and replace the contants in this file.
### model.cpp

The converted C source model stored in `modeling/models/model.cc` from the best model by the [`evaluation.ipynb`](#-evaluationipynb) is copied in this file. It is not quantized because it's lightweighted enough without quantization.

---

### central/central.ino
If it is connected to peripheral device, it goes into the while loop. In the loop, the timer is periodically called (_PERIOD = window size * (1 - overlap ratio) * number of inference in one cycle_, which is 15s = 30 * (1 - 0.9) * 5 in this file). By the transmitted values from the peripheral(`beat_per_period` and `stress_lv`), calculate the interval between the beat and motor level. When the next beat started by the calculation, motor is activated. 
