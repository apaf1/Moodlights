# Moodlights

This is the Arduino program for moodlights in the living room. 

NB! Before you upload the code to the arduino, please disconnect all wires to and from the control box. If you don't disconnect the power supply the voltage might leak into your USB port and damage it. If you don't disconnect the LEDs, the LEDs might be damaged because the USB port on your computer does not supply enough amperage to the LEDs. 

The program has three important parts that are separated
 - check buttons          -->     void checkButtons()
 - update state           -->     void updateState()
 - run animations         -->     executed in main loop

The code is divided into to parts that are clearly marked, and can be found in the following order
- Controll code
- Led animation code

Feel free to add more animations!

Check list for adding new animations: 
- Add name for mode to be displayed on LCD in char* modes[]
- Increase the value of int modesChoiceMax
- Add mode in void loop()


Please check your code for mistakes before uploading to github, and atleast make sure that it is compilable :)


