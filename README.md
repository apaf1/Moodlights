# Moodlights

This is the Arduino program for moodlights in the living room. 

NB! Before you upload the code to the arduino, please disconnect all wires to and from the control box. If you don't disconnect the power supply the voltage might leak into your USB port and damage it. If you don't disconnect the LEDs, the LEDs might be damaged because the USB port on your computer does not supply enough amperage to the LEDs. 

The program has three important parts that are separated
 - check buttons          -->     void checkButtons()
 - update state           -->     void updateState()
 - run animations         -->     executed in main loop, void loop()

The code is divided into to parts that are clearly marked, and can be found in the following order
- Controll code
- Led animation code

Feel free to add more animations!


Check list for adding new animations: 
- Add name for mode to be displayed on LCD in char* modes[]
- Increase the value of int modesChoiceMax
- Add mode in void loop()

Use pause() instead of delay() as pause() checks buttons while pausing. 


Party mode:

When the party switch is set to HIGH, the party mode is initiated. In this mode all button input is ignored and predefined animations are played in random order. Remember to update nPartyModes when adding new animations. Use delay() instead of pause() when writing party mode animations as button input is ignored.  

Please check your code for mistakes before uploading to github, and atleast make sure that it is compilable :)


