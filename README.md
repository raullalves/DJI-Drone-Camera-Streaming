# DJI camera Streaming with UDP
(for Linux)
Stream and decode the DJI Drone Camera to a C/C++ app.

Developed based on DJI code Samples 
Tested on Dji Phantom 3 Professional
Make your 4G mobile a Router, because of UDP issues. Your computer and mobile should be at the same network

# 1- DOWNLOAD
  * Install git LFS: https://github.com/git-lfs/git-lfs/wiki/Installation
  * Clone: ``` git lfs clone https://github.com/raullalves/DJI-Drone-Camera-Streaming.git ```
  * Install OpenCV (used for decoding and showing. TODO: Use FFmpeg and SDL instead of OpenCV)

# 2-BUILD
  * Open droneControleFaceDetection folder
  * ``` make ```
  * Open android-videostreamdecodingsample (preferably using AndroidStudio)
  * Go to AndroidManifest.xml, insert your DJI App Key
  * Build and deploy on your Android mobile

# 3- RUN
  * Connect your mobile to the Drone's Remote Controller
  * Run the app
  * Click "Iniciar"
  * Run the C++ app, with your mobile's ip
    Example: ``` ./drone 192.168.15.5 ```
  * On your mobile, click "start screenshot"
  * The streaming should start now

# Example
![alt text](https://github.com/raullalves/DJI-Drone-Camera-Streaming/blob/master/imagens/img1.png)

![alt text](https://github.com/raullalves/DJI-Drone-Camera-Streaming/blob/master/imagens/img2.png)