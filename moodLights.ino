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
int NUMPIXELS=132;
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, pixelsPIN, NEO_GRB + NEO_KHZ800);

// LCD definition
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

//define values
float dial1Value = 50;
float dial2Value = 100;
long waitDisplay;
long waitButton;
long waitDial;

//Values for choosing mode
char* modes[] = {"White light","Color","Hjul", "Rainbow","Color snake","Wave"};
int modesChoice = 0;
int modesChoiceMax = 5;
boolean modeChanged = false;
boolean partyMode = false;

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


int wavePixel = 0;


void setup() {
  pixels.begin(); //initialize neopixels
  //NUMPIXELS = pixels.numPixels();

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

  
  if(modesChoice ==0){
    whiteLight();
  }else if(modesChoice == 1){
    color();
  }else if(modesChoice ==2){
    hjul();
  }else if(modesChoice ==3){
    rainbowCycle();
  }else if(modesChoice ==4){
    pixeltest();
  }else if(modesChoice ==5){
    wave();
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
  float newDial1Value = dial2Range(analogRead(A0),0,100); //fix dial-value from 80-1023 to 0 to 100
  float newDial2Value = dial2Range(analogRead(A1),0,100);
  bLeftValue = digitalRead(bLeftPin);
  bRightValue = digitalRead(bRightPin);
  partyMode = digitalRead(partySwitch);

  //Only change dial-read if the change is greater than 5 %
  if(dial1Value<(newDial1Value+10)||dial1Value>(newDial1Value+10)){
    waitDial = millis()+2000;
  }
  if(dial2Value<(newDial2Value+10)||dial2Value>(newDial2Value+10)){
    waitDial = millis()+2000;
  }
  if(waitDial>millis()){
    dial1Value = newDial1Value;
    dial2Value = newDial2Value;
  }

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
  dial = dial-223; //subtract fluctuating lower-value from dial 
  double out = ( (dial / 800)  *1.15); //give margin to compensate for fluctuating upper-value from dial
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
  setColor(255,255,255*(dial1Value/100),dial2Value);
  for(int i=0;i<NUMPIXELS;i++){
      pixels.setPixelColor(i,pixels.Color(r,g,b));
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

//Change color of LEDs, one LED at a time
void pixeltest(){
  for(int i=0;i<NUMPIXELS;i++){
    pixels.setPixelColor(i,pixels.Color(r,g,b));
    pixels.show();
    setColor(255,0,0,dial2Value);
    pause(dial1Value);
    if(modeChanged){
        break; //end all pauses so program goes to next mode
    }
  }
  pause(500);

  for(int i=0;i<NUMPIXELS;i++){
    pixels.setPixelColor(i,pixels.Color(r,g,b));
    pixels.show();
    setColor(255,0,0,dial2Value);
    pause(dial1Value);
    if(modeChanged){
        break; //end all pauses so program goes to next mode
    }
  }
  pause(500);

  for(int i=0;i<NUMPIXELS;i++){
    pixels.setPixelColor(i,pixels.Color(r,g,b));
    pixels.show();
    setColor(0,0,255,dial2Value);
    pause(dial1Value);
    if(modeChanged){
        break; //end all pauses so program goes to next mode
    }
  }
  pause(500);

  for(int i=0;i<NUMPIXELS;i++){
    pixels.setPixelColor(i,pixels.Color(r,g,b));
    pixels.show();
    setColor(0,255,255,dial2Value);
    pause(dial1Value);
    if(modeChanged){
        break; //end all pauses so program goes to next mode
    }
  }
  pause(500);

  for(int i=0;i<NUMPIXELS;i++){
    pixels.setPixelColor(i,pixels.Color(r,g,b));
    pixels.show();
    setColor(255,255,255,dial2Value);
    pause(dial1Value);
    if(modeChanged){
        break; //end all pauses so program goes to next mode
    }
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

// Show the whole and moving rainbow in the LEDs
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


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
void setColor(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    r=255 - WheelPos * 3;
    g=0;
    b=WheelPos * 3;
  }else  if(WheelPos < 170) {
    WheelPos -= 85;
    r=0;
    g=WheelPos * 3;
    b=255 - WheelPos * 3;
  }else{
    WheelPos -= 170;
    r=WheelPos * 3;
    g=255 - WheelPos * 3;
    b=0;
  }
}

//Go trough all the colors of the rainbow with all LEDs in same color
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



void wave(){
  setColor(255*(dial1Value/100));
  for(int i=0;i<NUMPIXELS;i++){
      if(i==wavePixel-5){
        pixels.setPixelColor(i,pixels.Color((int) (r*0.2),(int) (g*0.2),(int) (b*0.2)));
      }else if(i==wavePixel-4){
        pixels.setPixelColor(i,pixels.Color((int) (r*0.4),(int) (g*0.4),(int) (b*0.4)));
      }else if(i==wavePixel-3){
        pixels.setPixelColor(i,pixels.Color((int) (r*0.6),(int) (g*0.6),(int) (b*0.6)));
      }else if(i==wavePixel-2){
        pixels.setPixelColor(i,pixels.Color((int) (r*0.8),(int) (g*0.8),(int) (b*0.8)));
      }else if(i==wavePixel-1){
        pixels.setPixelColor(i,pixels.Color((int) (r*0.9),(int) (g*0.9),(int) (b*0.9)));
      }else if(i==wavePixel){
        pixels.setPixelColor(i,pixels.Color((int) (r),(int) (g),(int) (b)));
      }else if(i==wavePixel+1){
        pixels.setPixelColor(i,pixels.Color((int) (r*0.9),(int) (g*0.9),(int) (b*0.9)));
      }else if(i==wavePixel+2){
        pixels.setPixelColor(i,pixels.Color((int) (r*0.8),(int) (g*0.8),(int) (b*0.8)));
      }else if(i==wavePixel+3){
        pixels.setPixelColor(i,pixels.Color((int) (r*0.6),(int) (g*0.6),(int) (b*0.6)));
      }else if(i==wavePixel+4){
        pixels.setPixelColor(i,pixels.Color((int) (r*0.4),(int) (g*0.4),(int) (b*0.4)));
      }else if(i==wavePixel+5){
        pixels.setPixelColor(i,pixels.Color((int) (r*0.2),(int) (g*0.2),(int) (b*0.2)));
      }else{
        pixels.setPixelColor(i,pixels.Color((int) (0),(int) (0),(int) (0)));
      }
  }
  pixels.show();
  if(wavePixel<(NUMPIXELS)+5){
    wavePixel=wavePixel+1;
  }else{
    wavePixel=-5;
  }
  pause(dial2Value);
  
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

//master party function
void party(){
  int nPartyModes = 1;
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

//Blinks in different colors
void blinking(){
  int color = random(0,7);
  if(color==0){
    oneBlink(250,0,0,3);
  }else if(color==1){
    oneBlink(0,250,0,3);
  }else if(color==2){
    oneBlink(0,0,250,3);
  }else if(color==3){
    oneBlink(150,0,150,3);
  }else if(color==4){
    oneBlink(0,150,150,3);
  }else if(color==5){
    oneBlink(150,150,0,3);
  }else if(color>=6){
    oneBlink(150,150,150,3);
  }
  
}

//blink in input color a choosable amount of times
void oneBlink(int r, int g, int b,int times){
  for(int i=0;i<times;i++){
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

//turns all pixels off. NB! need to use pixels.show() for changes to be shown
void off(){
  for(int i=0;i<NUMPIXELS;i++){
    pixels.setPixelColor(i,pixels.Color(0,0,0));
  }
}


