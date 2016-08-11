// Libraries
#include <LiquidCrystal.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__ //for neopixels
  #include <avr/power.h>
#endif

//Pin definitions
int greenLED = 6;
int pinkLED  = 7;
int partySwitch = 8;
int bLeftPin = 9;
int bRightPin = 10;
#define pixelsPIN  13

//Neopixel definitions
int NUMPIXELS=5;
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, pixelsPIN, NEO_GRB + NEO_KHZ800);

// LCD definition
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

//define values
double dial1Value = 0;
double dial2Value = 0;

char* modes[] = {"Test", "Color"};
int modesChoice = 0;
int modesChoiceMax = 1;
boolean modeChanged = false;
boolean partyMode = false;

long waitDisplay;
long waitButton;

//define button values
boolean bLeftAction = true;
boolean bRightAction = true;

boolean bLeftValue = true;
boolean bRightValue = true;

boolean bLeftLast = true;
boolean bRightLast = true;

//color values
byte r = 0;
byte g = 0;
byte b = 0;

void setup() {
  pixels.begin(); //initialize neopixels

  //Initialize pins
  pinMode(A0,INPUT); // dial 1
  pinMode(A1,INPUT); // dial 2
  pinMode(greenLED,OUTPUT);
  pinMode(pinkLED,OUTPUT);
  pinMode(partySwitch,INPUT);
  pinMode(bLeftPin,INPUT);
  pinMode(bRightPin,INPUT);
  
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("start");

  //turn on green ON-indicating lED
  digitalWrite(greenLED,HIGH);

}

//Main loop that runs program
void loop() {
  checkButtons();
  updateState();

  if(modesChoice == 0){
    pixeltest();
  }else if(modesChoice ==1){
    color();
  }
  
  modeChanged = false;
}


//////////////////////////////////////////////////////////////////
//
// CONTROL CODE
//
//////////////////////////////////////////////////////////////////
void checkButtons(){
  //Read all input pins
  dial1Value = dial2Range(analogRead(A0),0,100); //fix dial-value from 80-1023 to 0 to 100
  dial2Value = dial2Range(analogRead(A1),1,100);
  bLeftValue = digitalRead(bLeftPin);
  bRightValue = digitalRead(bRightPin);
  partyMode = digitalRead(partySwitch);

  boolean buttonPressed = false;

  //if button-value has changed and some time has passed since last buttonpress, 
  //then make button action true and do something in updateState()
  if(bRightLast != bRightValue){
    bRightLast = bRightValue;
    if(bRightValue==HIGH && millis()>waitButton){
      bRightAction=true;
      buttonPressed = true;
    }else{
      buttonPressed = false;
    }
  }

  if(bLeftLast != bLeftValue){
    bLeftLast = bLeftValue;
    if(bLeftValue==HIGH && millis()>waitButton){
      bLeftAction=true;
      buttonPressed = true;
    }else{
      buttonPressed = false;
    }
  }

  //wait-time to fix double jumping button
  if(buttonPressed){
    waitButton = millis() + 200;
  }
  
}

boolean updateState(){
  if(bLeftAction){
    modeChanged = true;
    if(modesChoice>=modesChoiceMax){
        modesChoice = 0;
      }else{
        modesChoice = modesChoice + 1;
      }
  }

  if(bRightAction){
    modeChanged = true;
    if(modesChoice<=0){
        modesChoice = modesChoiceMax;
      }else{
        modesChoice = modesChoice - 1;
      }
  }

  if(partyMode){
    digitalWrite(pinkLED,HIGH); //turn on party-lED
  }else{
    digitalWrite(pinkLED,LOW);
  }
  
  clearButtonActionState(); 

  if(millis()>waitDisplay){
    
      waitDisplay=millis()+200;
      lcd.clear();
      lcd.print(modes[modesChoice]);
      lcd.setCursor(0,1);
      lcd.print("A: ");
      lcd.print(dial1Value);
      lcd.setCursor(9,1);
      lcd.print("B: ");
      lcd.print(dial2Value);
  }
}



//Fix dial value from ~80 to 1023 to other scale. 
double dial2Range(double dial, double minimum,double maximum){
  dial = dial-200; //subtract fluctuating lower-value from dial 
  double out = ( (dial / 823)  *1.05); //give margin to compensate for fluctuating upper-value from dial
  out = (out*(maximum-minimum))+minimum; //actually change dial-value

  //Change values exeding maximum to maximum, and same for minimum. 
  if(out<minimum){
    out=minimum;
  }
  if(out>maximum){
    out=maximum;
  }
  return out;
}

void clearButtonActionState(){
  bLeftAction = false;
  bRightAction = false; 
}

//Pause while also updating state by checking buttons
boolean pause(int milliSeconds){
  long start = millis();
    checkButtons();
    updateState();
    while(millis()<(start+milliSeconds)){
      if(modeChanged){
        break; //end all pauses so program goes to next mode
      }
      checkButtons();
      updateState();
    }
}

//////////////////////////////////////////////////////////////////
//
// PIXEL CODE
//
//////////////////////////////////////////////////////////////////

//set color that is adjusted by brightness
void setColor(byte red, byte green, byte blue, double brightness){
  r = (double)red*(brightness/100);
  g = (double)green*(brightness/100);
  b = (double)blue*(brightness/100);
}

void pixeltest(){
  setColor(255,0,0,dial2Value);
  for(int i=0;i<NUMPIXELS;i++){
    pixels.setPixelColor(i,pixels.Color(r,g,b));
    pixels.show();
    pause(dial1Value);
  }
  pause(500);

  setColor(0,255,0,dial2Value);
  for(int i=0;i<NUMPIXELS;i++){
    pixels.setPixelColor(i,pixels.Color(r,g,b));
    pixels.show();
    pause(dial1Value);
  }
  pause(500);

  setColor(0,0,255,dial2Value);
  for(int i=0;i<NUMPIXELS;i++){
    pixels.setPixelColor(i,pixels.Color(r,g,b));
    pixels.show();
    pause(dial1Value);
  }
  pause(500);

  setColor(0,255,255,dial2Value);
  for(int i=0;i<NUMPIXELS;i++){
    pixels.setPixelColor(i,pixels.Color(r,g,b));
    pixels.show();
    pause(dial1Value);
  }
  pause(500);

  setColor(255,255,255,dial2Value);
  for(int i=0;i<NUMPIXELS;i++){
    pixels.setPixelColor(i,pixels.Color(r,g,b));
    pixels.show();
    pause(dial1Value);
  }
  pause(500); 
}


//Change the color of the LEDs depending on input from dial
void color(){
  for(int i=0;i<NUMPIXELS;i++){
    pixels.setPixelColor(i,wheel(255*((double)dial1Value/100)));
  }
  pixels.show();
  pause(10);
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}


