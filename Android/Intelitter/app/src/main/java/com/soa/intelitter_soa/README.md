# InteLitter

This folder contains all the code of our Android App

## Where is implemented?
📄 InteLitter.java
> This file contains only a public class with some constant values that represent all the states of the InteLitter (Arduino). Also provides a function that helps to identify if a code receive is a state value or not.
📄 LitterColors.java
> This file contains only a public class with some constant values that represent all the colors that we use in our Android App.
📄 MainActiviy.java
> This file contains all the logic of the Main Activity of our Android App. Here we set the Bluetooth Connections and also we lauch a thread that will be running in background trying to read Bluetooth messages from the Arduino.
📄 SensorActivity.java
> This file contains all the logic of the Sensor Activity of our Android App. Here we only detect a Shake, and reproduce an audio telling to the user the actual state of the Arduino. The main thing about this Activity is understand how works with Sensors of a Smartphone and make something "useful".
