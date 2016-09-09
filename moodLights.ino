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
int NUMPIXELS=44;
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, pixelsPIN, NEO_GRB + NEO_KHZ800);

// LCD definition
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

//define values
double dial1Value = 0;
double dial2Value = 0;

char* modes[] = {"Color","Color snake", "Rainbow","Hjul","Pulsating"};
int modesChoice = 0;
int modesChoiceMax = 4;
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

//code for pulating() function
float MAXPULSERATE = 0.003;
float MINPULSERATEFACTOR = 10;
float ledPulseRate[44];
float ledPulseState[44];
int LARGE_INT = 3.4028235E+38;


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

  //Initialize array for pulsating() function
  for(int i=0;i<NUMPIXELS;i++){
    ledPulseRate[i]=(( (float) (random((MAXPULSERATE/MINPULSERATEFACTOR)*LARGE_INT,MAXPULSERATE*LARGE_INT*2)))/(float) LARGE_INT )-(MAXPULSERATE/2);
    ledPulseState[i] = (float)  random((MAXPULSERATE*LARGE_INT),(1-MAXPULSERATE)*LARGE_INT)/(float)LARGE_INT;
  }

}

//Main loop that runs program
void loop() {
  checkButtons();
  updateState();

  
  if(modesChoice ==0){
    color();
  }else if(modesChoice == 1){
    pixeltest();
  }else if(modesChoice ==2){
    rainbowCycle();
  }else if(modesChoice ==3){
    hjul();
  }else if(modesChoice ==4){
    pulsating();
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
    initPartyMode();
    party();
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
      lcd.print((int) dial1Value);
      lcd.setCursor(9,1);
      lcd.print("B: ");
      lcd.print((int) dial2Value);
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

void whiteLight(){
  for(int i=0;i<NUMPIXELS;i++){
      pixels.setPixelColor(i,pixels.Color(255,255,255));
    }
  pixels.show();
  pause(20);
}

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
  if(dial1Value==100){
    setColor(255,255,255,dial2Value);
    for(int i=0;i<NUMPIXELS;i++){
      pixels.setPixelColor(i,pixels.Color(r,g,b));
    }
  }else{
    for(int i=0;i<NUMPIXELS;i++){
      pixels.setPixelColor(i,wheel(255*((double)dial1Value/100)));
    }
  }
  pixels.show();
  pause(10);
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle() {
  uint16_t i, j;

  for(j=0; j<256; j++) { 
    for(i=0; i< NUMPIXELS; i++) {
      pixels.setPixelColor(i, wheel(((i * 256 / NUMPIXELS) + j) & 255));
    }
    pixels.show();
    pause(dial1Value);
    if(modeChanged){
      break;
    }
  }
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

void hjul(){
  for(int j=0;j<255;j++){
    for(int i=0;i<NUMPIXELS;i++){
        pixels.setPixelColor(i,wheel(j));
    }
    pause(dial1Value);
    pixels.show();
    if(modeChanged){
      break;
    }
  }
}

void pulsating(){
  for(int i=0;i<NUMPIXELS;i++){
    pixels.setPixelColor(i,pixels.Color((int) (ledPulseState[i]*0),(int) (ledPulseState[i]*0),(int) (ledPulseState[i]*255)));
    //Serial.print((ledPulseState[i]));
    //Serial.print(" ");
  } 
  //Serial.println();
  pixels.show();
  
  for(int i=0;i<NUMPIXELS;i++){
    
      if(ledPulseState[i]>=(1-MAXPULSERATE)){
        ledPulseRate[i]=-ledPulseRate[i];
      
      }else if(ledPulseState[i]<=(0+MAXPULSERATE)){
        ledPulseRate[i]=(( (float) (random((MAXPULSERATE/MINPULSERATEFACTOR)*LARGE_INT,MAXPULSERATE*LARGE_INT*2)))/(float) LARGE_INT )-(MAXPULSERATE/2);
      }
    
      ledPulseState[i]=ledPulseState[i]+ledPulseRate[i];
  } 
}


//////////////////////////////////////////////////////////////////
//
// PARTY MODE (use delay() not pause())
//
//////////////////////////////////////////////////////////////////

void initPartyMode(){
    randomSeed(millis());
    digitalWrite(pinkLED,HIGH); //turn on party-lED
    lcd.clear();
    lcd.write("Paaerty mooode!!");
}


void party(){
  int nPartyModes = 1;
  pause(10);
  int mode=(int)random(0,nPartyModes);
  //while(mode == lastMode){
    //mode=(int)random(0,nPartyModes);
  //}  //uncomment when adding more modes
  int lastMode = mode;
  while(digitalRead(partySwitch)){
    //while(mode == lastMode){
      //mode=(int)random(0,nPartyModes);
    //}//uncomment when adding more modes
    lastMode = mode;
    if(mode==0){
      blinking();
    }
  }
}


void blinking(){
  int color = random(0,7);
  if(color==0){
    oneBlink(250,0,0);
  }else if(color==1){
    oneBlink(0,250,0);
  }else if(color==2){
    oneBlink(0,0,250);
  }else if(color==3){
    oneBlink(150,0,150);
  }else if(color==4){
    oneBlink(0,150,150);
  }else if(color==5){
    oneBlink(150,150,0);
  }else if(color>=6){
    oneBlink(150,150,1500);
  }
  
}

void oneBlink(int r, int g, int b){
  for(int i=0;i<3;i++){
      for(int i=0;i<NUMPIXELS;i++){
        pixels.setPixelColor(i,pixels.Color(r,g,b));
      }
      pixels.show();
      delay(70);
      off();
      pixels.show();
      delay(70);
    }
}

void off(){
  for(int i=0;i<NUMPIXELS;i++){
    pixels.setPixelColor(i,pixels.Color(0,0,0));
  }
}

