# DJI camera Streaming with UDP
Stream and decode the DJI Drone Camera to a C/C++ app
This app was based on the DJI SDK 4.0 samples, and is part of my undergrad thesis
It contains two applications (one mobile for android, and another for desktop (tested on windows))

# Example
![alt text](https://github.com/raullalves/DJI-Drone-Camera-Streaming/blob/master/imagens/img1.png)

![alt text](https://github.com/raullalves/DJI-Drone-Camera-Streaming/blob/master/imagens/img2.png)

Tested on Dji Phantom 3 Professional

It has a face Detection implemented

# Clone
  First, if you don't have git lfs, install it and clone using it: git lfs clone ...

# Set Up
  Put your id code on the manifest file
  Run the Android app first! Click "Iniciar" (it means start =) )
  You should instantly see the live camera on the mobile screen
  Consequently, modify the desktop's code to your desired IP. Then, compile and run it
  Then, click "start screenshot" in your app
  After clicking, the stream sould immediately start and you'll see the live streaming on your desktop app
  
