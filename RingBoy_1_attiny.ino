#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 1
#define LEFT 3
#define RIGHT 4
#define SPEAKER 0

#define BLACK 0
#define BLUE 1
#define GREEN 2
#define RED 3
#define YELLOW 4
#define LIVES 3


// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(24, PIN, NEO_RGBW + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

byte lives;
byte v;
byte pixels[24];
long time;
byte score;

boolean brickActive = false;
int brick;
int brickColor;


int currentPixel[LIVES];
int color;

boolean leftPressed;
boolean rightPressed;


void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code
  
  lives = 3;
  v = 200;
  time = 0;
  score = 0;
  brickActive = false;
  color = RED;
  leftPressed = false;
  rightPressed = false;
  
  strip.begin();
  strip.setBrightness(15);
  strip.clear();

  unsigned int id = EEPROM.read(0);
  randomSeed(id);
  EEPROM.write(0, id+random(10));
  id = random(50,125);

  //Serial.begin(9600);
 
  pinMode(LEFT, INPUT);
  pinMode(RIGHT, INPUT);
  pinMode (SPEAKER,OUTPUT);
  
  for (int i=0;i<lives;i++) currentPixel[i]=8+lives-1-i;

  int intro=0;
  while (digitalRead(RIGHT)==LOW) {
    setPixel(intro,(int)random(1,5),0);
    delay(40);
    strip.show();
    setPixel(intro,BLACK,0);
    intro = nextPixel(intro);
  }
  setPixel(intro,BLACK,0);
}

void loop() {
  
  strip.show();
  for (int i=0;i<lives;i++) setPixel(currentPixel[i],color,i);
  strip.show();
  
  if (millis()-time > v) {
    for (int i=0;i<lives;i++){
      setPixel(currentPixel[i],BLACK,0);
      currentPixel[i] = nextPixel(currentPixel[i]);
    }
    time = millis();
    strip.show();
    
    if (!brickActive) {
    int d = (int)random(8,20);
    //if (score==0 && lives == 3) d = 18;
    brick = currentPixel[0];
    for (int i=0;i<d;i++) brick = nextPixel(brick);
    brickColor=(int)random(1,5);
    setPixel(brick,brickColor,0);
    brickActive = true;
    
    }
    
    if (brick==currentPixel[0]) {
     brickActive = false; 
     if (brickColor != color) {
       lives--;

       analogWrite(SPEAKER, 100); 
       delay(200);
       
       digitalWrite(SPEAKER,LOW);
       
       if (lives == 0) gameOver();
     }
     else {
       v-=3;
       score++;
       digitalWrite(SPEAKER, HIGH); 
       delay(20);
       
       digitalWrite(SPEAKER,LOW);
       
        
     }
     
     }
    
  }
  
  if (digitalRead(RIGHT)==HIGH && rightPressed == false){
    color ++;
    rightPressed = true; 
  }
  if (digitalRead(LEFT)==HIGH && leftPressed == false){
    color --;
    leftPressed = true;
  }
  if (digitalRead(LEFT)==LOW) leftPressed = false;
  if (digitalRead(RIGHT)==LOW) rightPressed = false;
   
  if (color == 5) color = 1;
  if (color == 0) color = 4;
  
}

// sets Pixel to one of the 4 specified colors
void setPixel(int pos, byte col, int dim){
  if (col==BLACK) strip.setPixelColor(pos,0);
  if (col==GREEN) strip.setPixelColor(pos,strip.Color(255,0,0));
  if (col==RED) strip.setPixelColor(pos,strip.Color(0,255,0));
  if (col==BLUE) strip.setPixelColor(pos,strip.Color(0,0,255));
  if (col==YELLOW) strip.setPixelColor(pos,strip.Color(120,255,0)); 
}

//next Pixel
int nextPixel(int p){
  if (p==7) return 13;
  if (p==11) return 0;
  if (p<11) return p+1;
  if (p==14) return 8;
  if (p==12) return 23;
  if (p>11) return p-1;
}

void gameOver(){
  //delay(500);
  //tone(SPEAKER, 600); 
  //delay(200);
  //tone(SPEAKER, 300); 
  //delay(700);
  //noTone(SPEAKER);
  strip.clear();
  strip.show();
  delay(700);
  
  int left = (int)(score/10); //calculate multiples of 10 for the left ring
  int right = score % 10; //calculate multiples of 1 for the right ring
  for (int i=0;i<left;i++) {
    setPixel(i,BLUE,0);
    strip.show();
    delay(300);
  }
  for (int i=0;i<right;i++) {
    setPixel(12+i,GREEN,0);
    strip.show(); 
    delay(300);
  }
  strip.show();

  while (!(digitalRead(LEFT)==HIGH || digitalRead(RIGHT)==HIGH));
  delay(500);
  setup(); 
}

