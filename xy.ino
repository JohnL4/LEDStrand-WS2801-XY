#include "Adafruit_WS2801.h"
#include "SPI.h" // Comment out this line if using Trinket or Gemma
#ifdef __AVR_ATtiny85__
#include <avr/power.h>
#endif

// ---------------------------------------------------------------------------------------------------------------------
//  types
// ---------------------------------------------------------------------------------------------------------------------

struct point
{
   int x,y;
};

// ---------------------------------------------------------------------------------------------------------------------
//  globals
// ---------------------------------------------------------------------------------------------------------------------

uint8_t dataPin  = 2;    // Yellow wire on Adafruit Pixels
uint8_t clockPin = 3;    // Green wire on Adafruit Pixels

Adafruit_WS2801 strip = Adafruit_WS2801(25, dataPin, clockPin);

float MIN_LIGHTNESS = 0.004;

struct point org = (struct point) { 0, 0   }   ;
struct point gridSize = (struct point) {5, 5    };

// ---------------------------------------------------------------------------------------------------------------------
//  setup
// ---------------------------------------------------------------------------------------------------------------------

void setup() {
  strip.begin();
  // clearStrip();
  strip.show();
}

// ---------------------------------------------------------------------------------------------------------------------
//  loop
// ---------------------------------------------------------------------------------------------------------------------

void loop() {
  // put your main code here, to run repeatedly:
  
   // flashTwoDifferentColors();

   cartesianTest();

}

// ---------------------------------------------------------------------------------------------------------------------
//  flashTwoDifferentColors
// ---------------------------------------------------------------------------------------------------------------------

void flashTwoDifferentColors()
{
  struct point org = {0, 0};
  struct point gridSize = {5, 5};
  struct point p = {3,1};
  int i = point2seq( p, org, gridSize);
  
  strip.setPixelColor( i, Color( 0, 30, 30 ));
  strip.show();
  delay( 200);

  clearStrip();
  strip.show();
  delay( 200);

  p = {2,2 };
  i = point2seq( p, org, gridSize);
  strip.setPixelColor( i, Color( 128, 10, 0));
  strip.show();
  delay( 200);

  clearStrip();
  strip.show();
  delay( 200);
}

// ---------------------------------------------------------------------------------------------------------------------
//  cartesianTest
// ---------------------------------------------------------------------------------------------------------------------

// Draw an X.

void cartesianTest()
{
   struct point prevPt;
   bool prevPtSet = false;
   float lightness;
   float dLightness = 1.0 / gridSize.x;
   float saturation;
   float dSaturation = 1.0 / gridSize.x;
   int i;

   struct point pt;
   uint32_t c;

   float rf, gf, bf;
   int ri, gi, bi;
   
   // bottom left to top right
   for (i = 0; i < 5; i++)
   {
      // if (prevPtSet)
      //    // Turn previous point off
      //    strip.setPixelColor( point2seq( prevPt, org, gridSize), 0);
      pt = { i, i };
      lightness = i * dLightness;
      // lightness *= lightness;
      if (lightness < MIN_LIGHTNESS)
         lightness = MIN_LIGHTNESS;
      hsl2rgb( 0.666667, 1.0, lightness, &rf, &gf, &bf);
      ri = constrain( 256 * rf, 0, 255);
      gi = constrain( 256 * gf, 0, 255);
      bi = constrain( 256 * bf, 0, 255);
      c = Color( ri, gi, bi);
      
      strip.setPixelColor( point2seq( pt, org, gridSize), c);
      strip.show();
      prevPt = pt;
      prevPtSet = true;
      delay( 100);
   }
   // strip.setPixelColor( point2seq( prevPt, org, gridSize), 0);

   delay( 1000);
   
   // bottom right to top left
   for (i = 0; i < 5; i++)
   {
      pt = {4-i, i };
      lightness = i * dLightness;
      // lightness *= lightness;
      if (lightness < MIN_LIGHTNESS)
         lightness = MIN_LIGHTNESS;
      hsl2rgb( 10.0/360.0, 1.0, lightness, &rf, &gf, &bf);
      ri = constrain( 256 * rf, 0, 255);
      gi = constrain( 256 * gf, 0, 255);
      bi = constrain( 256 * bf, 0, 255);
      c = Color( ri, gi, bi);
      strip.setPixelColor( point2seq( pt, org, gridSize), c);
      strip.show();
      prevPt = pt;
      prevPtSet = true;
      delay( 100);
   }

   // delay( 1000);

   // // up the middle
   // for (i = 0; i < 5; i++)
   // {
   //    pt = {2,i };
   //    saturation = i * dSaturation;
   //    // lightness *= lightness;
   //    hsl2rgb( 120.0/360.0, saturation, 0.5, &rf, &gf, &bf);
   //    ri = constrain( 256 * rf, 0, 255);
   //    gi = constrain( 256 * gf, 0, 255);
   //    bi = constrain( 256 * bf, 0, 255);
   //    c = Color( ri, gi, bi);
   //    strip.setPixelColor( point2seq( pt, org, gridSize), c);
   //    strip.show();
   //    prevPt = pt;
   //    prevPtSet = true;
   //    delay( 100);
   // }
   // pt = {3,4 };
   // c = Color( 0, 255, 0);
   // strip.setPixelColor( point2seq( pt, org, gridSize), c);
   // strip.show();
   
   delay( 1500);
   clearStrip();
   strip.show();
   delay( 600);
}

// ---------------------------------------------------------------------------------------------------------------------
//  point2seq
// ---------------------------------------------------------------------------------------------------------------------

// Convert a point to a sequence number.  Point is relative to anOrigin.  aGridSize specifies width (x) and height (y)
// of grid.  Grid is assumed to be laid out in a zigzag pattern starting at the lower left corner, continuing
// horizontally to the lower right corner, going up a row and continuing back to the left, going up a row and continuing
// to the right, etc.

int point2seq( struct point aPt, struct point anOrigin, struct point aGridSize)
{
   int retval;
   struct point pt0 = aPt;

   // Transform to zero-based point.
   pt0.x -= anOrigin.x;
   pt0.y -= anOrigin.y;

   // y-coord * number of rows in grid.  For 5x5 grid,
   //    0 ==> 0
   //    1 ==> 5
   //    2 ==> 10
   //    etc.
   retval = pt0.y * aGridSize.y;

   // x-coord.  For 5x5 grid,
   //    (0,0) ==> 0 + 0         = 0
   //    (0,1) ==> 0 + (4 - 0)   = 4
   //    (1,0) ==> 0 + 1         = 1
   //    (1,1) ==> 5 + (4 - 1)   = 8
   //    (2,2) ==> 10 + 2        = 12
   //    (2,3) ==> 15 + (4 - 2)  = 17
   //    (3,2) ==> 10 + 3        = 13
   if (pt0.y % 2 == 0)
      // Even-numbered row
      retval += pt0.x;
   else
      // Odd-numbered row
      retval += (aGridSize.x - 1 - pt0.x);
   
   return retval;
}

// ---------------------------------------------------------------------------------------------------------------------
//  hsl2rgb
// ---------------------------------------------------------------------------------------------------------------------

// CSS3 spec algorithm: http://www.w3.org/TR/2011/REC-css3-color-20110607/#hsl-color
// All params in range [0.0..1.0]

void hsl2rgb( float hue, float sat, float light, float *r, float *g, float *b)
{
   float m1, m2;
   
   if (light < 0.5)
      m2 = light * (sat + 1.0);
   else
      m2 = light + sat - light * sat;
   m1 = light * 2.0 - m2;
   *r = hue2rgb( m1, m2, hue + 1.0/3.0);
   *g = hue2rgb( m1, m2, hue);
   *b = hue2rgb( m1, m2, hue - 1.0/3.0);
}

// ---------------------------------------------------------------------------------------------------------------------
//  hue2rgb (internal helper function)
// ---------------------------------------------------------------------------------------------------------------------

// CSS3 spec algorithm: http://www.w3.org/TR/2011/REC-css3-color-20110607/#hsl-color

float hue2rgb( float m1, float m2, float hue )
{
   float retval;
   float h = hue;
   
   if (hue < 0.0)
      h = hue + 1.0;
   else if (hue > 1.0)
      h = hue - 1.0;
   else
      h = hue;

   if (h * 6.0 < 1.0)
      retval = m1 + (m2 - m1) * h * 6.0;
   else if (h * 2.0 < 1.0)
      retval = m2;
   else if (h * 3.0 < 2.0)
      retval = m1 + (m2 - m1) * (2.0 / 3.0 - h) * 6.0;
   else
      retval = m1;

   return retval;
}

// ---------------------------------------------------------------------------------------------------------------------
//  Helper functions
// ---------------------------------------------------------------------------------------------------------------------

void clearStrip()
{
   int i;
   for (i = 0; i < 25; i++)
      strip.setPixelColor( i, 0);
}

// Create a 24 bit color value from R,G,B
uint32_t Color(byte r, byte g, byte b)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}

