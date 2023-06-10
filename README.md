# Pit-a-Pat
Our team implemented a system called Pit-a-Pat, which can detect one’s stress level and express it to a partner by changing the amplitude of the heartbeat vibration. The system was developed for the mini-proejct of the coursework, which is "CS565 IoT Data Science" at KAIST. 

Authors: Hyesoo Park(hyehye@kaist.ac.kr) and Tae-Hoon Lee(th.lee@kaist.ac.kr)

## Demo video

## Presentation slides
<img width="800" alt="image" src="https://github.com/CS565-Pit-a-Pat/modeling/assets/27489013/d24373b9-1e6a-4c55-a43a-4a6e2d35c1b2">

[Link to the slides](https://docs.google.com/presentation/d/1nr1iNvllwkr2UorulvrPxBd-E-VGa7PmlZvasYQV54c/edit?usp=sharing)

## Introduction
Expressing and sharing biosignals have gained significant attention in the field of Human-Computer Interaction (HCI) as a means to facilitate user understanding and foster empathy. Previous research has explored visual representations of biosignals, such as numbers and charts. Visual and audio expressions have shown to improve feelings of connectedness and intimacy between users. People tend to interpret heart rate information in relation to emotions and stress levels, similar to the effects of facial expressions. Additionally, studies have demonstrated that hearing a heartbeat can have a comparable impact to direct eye contact.

While the expression of biosignals in a group or in a visually public way has been extensively studied, there is a lack of research on whether the expression of biosignals in a private way (e.g., vibration) enhances connectedness. Thus, we proposes a way to communicate biosignals in real time between two devices in a private manner. Our study aims to investigate whether 1) we can use TinyML devices for real-time stress level classification(using PPG, skin temp.) and whether 2) sharing heartbeat through device’s vibration strengthens the connectedness between two partners.


## Hardware component
### List of Hardware components
[Arduino Nano 33 BLE Sense](https://www.devicemart.co.kr/goods/view?no=10919317) 

[PPG sensor](https://www.devicemart.co.kr/goods/view?no=1327536) 

[Temperature sensor](https://www.devicemart.co.kr/goods/view?no=3183) 

[Vibration motor](https://www.devicemart.co.kr/goods/view?no=12515096) 

### Circuit Diagram for each device
// 채워넣을 예정



# Modeling
## Environment setting
We performed our modeling inside the Anaconda virtual environment. The version of Anaconda we used is 22.9.0, and to create the same environment as ours, please enter the commands below.

1. `git clone https://github.com/Kaist-ICLab/final-submission-team-4-20233344-20235290.git`
2. `cd final-submission-team-4-20233344-20235290`
3. `conda env create -f conda_requirements.txt`

## Dataset download
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

## Data description
WESAD dataset was collected from both a chest-worn device RespiBAN Professional and a wrist-worn device Empatica E4. It contains motion and physiological data such as Respiration, Electrocardiogram, Blood volume pulse, Electrodermal activity, Skin temperature, Accelerometer and so on. In this project, we focus only on data from the wrist-worn device because our prototype is worn on the wrist. The below table shows the used sensors in our project and their sampling rate.

<img width="600" alt="image" src="https://github.com/CS565-Pit-a-Pat/modeling/assets/27489013/232ccbe7-a71c-4897-a919-45a044a295e8">

During the data collection, subjects were asked to go through various condition such as baseline condition, amusement condition, stress condition. Previous studies used the study protocol conditions as labels that were used for modeling. Since our interest is how stressed the subjects were, we treated the labels as stress condition and non-stress condition (i.e., baseline condition). 

There are 15 subject in WESAD dataset, however, we used data from four subjects (S14, S15, S16, and S17) only. This is because the data from them is similar to our expectation according to our analysis.



## Feature extraction
We used [window_slider](https://pypi.org/project/window-slider/) package to run sliding window with overlapping. The window size is 30s and overlapped rate is 0.9 (i.e., 27s between the previous and the current window is overlapped.). We extracted features from each sliding window. The below table organizes the definition of the extracted features and which features are used in the project.
<img width="700" alt="image" src="https://github.com/CS565-Pit-a-Pat/modeling/assets/27489013/2af776c3-50e2-4408-8697-5d1b7331e430">


## Model description
<img width="800" alt="image" src="https://github.com/CS565-Pit-a-Pat/modeling/assets/27489013/81eaf87f-8b82-46ee-9f19-b7f5a7b15e5f">


We adopted an ANN-based neural network with a very simple structure. The structure of the model we used follows the model proposed by a [previous study](https://ieeexplore.ieee.org/abstract/document/9183244). They achieved 95.21% of accuracy and 94.24% of f1-score using the WESAD dataset for stress detection. We follow the structure of their model because it performs well despite being a lightweight model.

The input to the model is 9 features as described in the [section](#feature-extraction) above. The model performs the binary classification task of detecting stress vs non-stress. Therefore, the output of the model is 0 (non-stress) and 1 (stress). The figure below shows the summaries of deep learning architecture for the stress classification. As shown, our model requires 126 parameters.

<img width="500" alt="image" src="https://github.com/CS565-Pit-a-Pat/modeling/assets/27489013/1be3d226-5170-4661-9f21-3023e84d8533">


## Evaluation
<img width="735" alt="image" src="https://github.com/CS565-Pit-a-Pat/modeling/assets/27489013/c986f635-c8ac-4e70-9508-f6771b64689e">

This figure shows the overview of the evaluation process.
For evaluation, we used two datasets which are WESAD dataset and the dataset we collected from Arduino.
WESAD dataset is used to train and evaluate the model by splitting the entire data into k folds. 
Once we find the optimal hyperparameter of the model based on the final performance metric, we select a model that achieve the best performance among all k models, and evaluate it with the data from arduino.

Humans differ in their individual reactions to affective stimuli, resulting in generalizibility of the model for affective computing become an important issue. By using leave-one-subject-out (LOSO) cross-validation, the models are trained on a set of subjects while leaving out one subject for testing. This ensures that the model's performance is assessed on data from previously unseen subjects, making it subject independent and enabling it to generalize better. By employing LOSO evaluation, we can obtain more reliable and robust results. Therefore, we used LOSO cross-validation in this project.

In our experiments, the accruacy and f1-score were 88.65% and 83.11% obtained through LOSO validation, respectively. Among the 4 models trained with the LOSO validation procedure, we selected the best performing model for evaluation with the dataset we collected from Arduino. The accuracy and f1-score of the best model on the dataset from Arduino are 81.11% and 84.62%, respectively.


## File description
### processing.ipynb
This file removes unnecessary parts (i.e., data from chest sensors) of the WESAD dataset, and performs windowing and feature extraction. After running all the code in this file, subject-specific data is stored in the form of pickle files in the `data/wrist-only` folder. The data from subjects is combined into one csv file and saved as `processed_*.csv` in the `data` folder.

### evaluation.ipynb
Once you run the code of processing.ipynb, you would get `processed_*.csv` file that is used for modeling. In this file, the csv file will be loaded, and the features will be scaled by using MinMaxScaler from the sklearn package. After the evaluation process is done, the model that achieves the best accuracy will be converted into C source. The C source will be stored in `model` folder. The size of the converted model formed as C source file is 2.38 KB.

### analysis-for-collected-data.ipynb
This file generates the features of the collected data from Arduino. The extracted featurs will be stored in `data/processed` as csv format. Data from stress and baseline condition should be processed through the code in this file, and the name of files will be `stress_*.csv` and `baseline_*.csv`, respectively. The csv files will be used in `evaluation.ipynb`.


### extract_scaling_factor.ipynb
The code in this file is for extracting scaling factors that are used for real-time scaling on Arduino board. 


# Arduino
## Library setting

You need to install several libraries to run the arduino files. The list is below.

1. `ArduinoBLE` ([LINK](https://www.arduino.cc/reference/en/libraries/arduinoble/))
1. `CircularBuffer` ([LINK](https://github.com/rlogiacco/CircularBuffer))
1. `EmotiBit_ArduinoFilters` ([LINK](https://github.com/EmotiBit/EmotiBit_ArduinoFilters))
1. `PeakDetection` ([LINK](https://github.com/leandcesar/PeakDetection/tree/master))

For the library `PeakDetection`, it is not found in Arduino IDE unlike other libraries, so please download from its github page and unzip on the library of your Arduino library directory similar as below.

```bash
/Users/[user_name]/Documents/Arduino/libraries/PeakDetection/PeakDetection.h
```




## Directory Structure
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

## Real-time feature extraction



## File description
Main structure for peripheral device follows the example of `KAIST_IoTDataScience/Lab12_TinyML_Hello_World/`. `peripheral.ino`, `constans.h`, `model.cpp` is newly implemented for this preoject ( * - starred in the [above](##-Directory-Structure)).

### central.ino

### peripheral.ino



### constants.h

`extract_scaling_factor.ipynb` 로부터 scaling factor를 받아와서 바꿔야한다

### model.cpp

The converted C source model stored in `modeling/models/model.cc` 를 복사해서 바꿔두었다.