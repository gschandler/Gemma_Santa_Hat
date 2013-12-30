#include <EEPROM.h>

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define  NEO_PIXEL_STRIP_PIN  1
#define  LED_BUILTIN  1
#define  NUM_PIXELS  4


Adafruit_NeoPixel  strip(NUM_PIXELS,NEO_PIXEL_STRIP_PIN,NEO_GRB + NEO_KHZ800);
static int  lightProgram = 0;

// convenience definitions
uint32_t  kRedColor = Adafruit_NeoPixel::Color(255,0,0);
uint32_t  kGreenColor = Adafruit_NeoPixel::Color(0,255,0);
uint32_t  kBlueColor = Adafruit_NeoPixel::Color(0,0,255);
uint32_t  kWhiteColor = Adafruit_NeoPixel::Color(255,255,255);
uint32_t  kBlackColor = Adafruit_NeoPixel::Color(0,0,0);
uint32_t  kMagentaColor = Adafruit_NeoPixel::Color(255,0,255);
uint32_t  kYellowColor = Adafruit_NeoPixel::Color(255,255,0);
uint32_t  kCyanColor = Adafruit_NeoPixel::Color(0,255,255);


enum {
  kWhiteLights = 0,
  kRainbowLights,
  kRedAndGreenLights,
  
  kLightTypeCount
};


//
//  Standard Arduino setup
//

void setup() {  
  // read from EEPROM to see what lightProgram we are running
  uint8_t val = EEPROM.read(0);
  if ( val == 255 ) {  // initial state
    val = 0;
  }
  lightProgram = val;
  val = (val+1) % kLightTypeCount;
  EEPROM.write(0,val);
  
   randomSeed( long( millis() * lightProgram ) );
   
   strip.begin();
   strip.show();
   
    colorWipe(kRedColor,20);
    colorWipe(kGreenColor,20);
    colorWipe(kBlueColor,20);
    rainbow(10);
    rainbowCycle(10);

    clearAllLights();
}

//
//  Standard Arduino loop
//
#define  MIN_LIGHT_VALUE  20
#define  MAX_LIGHT_VALUE  150
uint8_t  randomColorValue()
{
  return byte(random(MIN_LIGHT_VALUE,MAX_LIGHT_VALUE));
}

uint32_t  randomWhiteLight()
{
  uint8_t c = randomColorValue(); 
  uint32_t color = Adafruit_NeoPixel::Color(c,c,c);
  return color;
}

uint32_t  randomRainbowColorLight()
{
  uint32_t  color = Adafruit_NeoPixel::Color(randomColorValue(),randomColorValue(),randomColorValue());
  return color;
}

uint32_t  randomRedGreenLight()
{
  uint32_t  color = kWhiteColor;
  if ( random(2) ) {
    color = Adafruit_NeoPixel::Color(0,randomColorValue(),0);
  }
  else {
    color = Adafruit_NeoPixel::Color(randomColorValue(),0,0);
  }
  return color;
}

void loop() {
    for ( int i=0;i<strip.numPixels();++i ) {
      uint32_t color = kWhiteColor;
     
      switch ( lightProgram ) {
        case kWhiteLights : color = randomWhiteLight(); break;
        case kRainbowLights : color = randomRainbowColorLight(); break;
        case kRedAndGreenLights : color = randomRedGreenLight(); break;
        }
      
      strip.setPixelColor(i,color);
      strip.show();
    }
    delay(200);
}

//
//
//

//
//
//
void  flashLights( uint32_t color, int count, int interval )
{
    if ( color == 0 ) return;
    if ( count == 0 ) return;
    
    uint32_t  c = 0;
    for ( int loop = 0; loop<count*2; ++loop ) {
      setAllLightsColor(c);
      delay(interval);
      c = (c==0) ? color : 0;
    }
    
    clearAllLights();
}

//
//
//
void  flashLight( int index, uint32_t color, int count, int interval ) {

  if ( index < 0 ) index = 0;
  else if (index >= strip.numPixels()) index = strip.numPixels() - 1;
  
  if ( count < 0 ) return;
  if ( color == 0 ) return;
  
  uint32_t c = 0;
  for ( int i=0; i<count*2; ++i ) {
    strip.setPixelColor(index,c);
    strip.show();
    c = (c==0) ? color : 0;
    delay(interval);
  }
  
  // turn it off at the end
  clearLight(index);
}

//
//
//
void  setTopLightColor( uint32_t color )
{
  setLightColor( strip.numPixels()-1, color);
}

//
//
//
void  setLightColor( int index, uint32_t color )
{
    if (index<0) index = 0;
    if (index>=strip.numPixels()) index = strip.numPixels()-1;
    
    strip.setPixelColor(index,color);
    strip.show();
}

//
//
//
void  clearLight( int index )
{
    setLightColor(index,0);
}

//
//
//
void  setAllLightsColor( uint32_t color )
{
  for (int i=0;i<strip.numPixels();++i) {
    strip.setPixelColor(i,color);
    strip.show();
  }
}

//
//
//
void  clearAllLights()
{
  setAllLightsColor(0);
}


// Fill the dots one after the other with a color
void colorWipe(uint32_t c, int wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

void rainbow(int wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(int wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return Adafruit_NeoPixel::Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return Adafruit_NeoPixel::Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return Adafruit_NeoPixel::Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
