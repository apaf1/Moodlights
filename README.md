# Moodlights
This is the Arduino program for moodlights in the living room. 

The program has three important parts that are separated
 - check buttons          -->     void checkButtons()
 - update state           -->     void updateState()
 - run animations         -->     executed in main loop

Feel free to add more animations!

Check list for adding new animations: 
- Add name for mode to be displayed on LCD in char* modes[]
- Increase the value of int modesChoiceMax
- Add mode in void loop()


Please check your code for mistakes before uploading to github, and atleast make sure that it is compilable :)


