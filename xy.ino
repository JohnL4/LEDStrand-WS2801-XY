#include "Adafruit_WS2801.h"
#include "SPI.h" // Comment out this line if using Trinket or Gemma
#ifdef __AVR_ATtiny85__
#include <avr/power.h>
#endif

// ---------------------------------------------------------------------------------------------------------------------
//  types
// ---------------------------------------------------------------------------------------------------------------------

struct point_s
{
   int x,y;
};

struct rgb_s
{
   int r,g,b;
};

// ---------------------------------------------------------------------------------------------------------------------
//  globals
// ---------------------------------------------------------------------------------------------------------------------

uint8_t dataPin  = 2;    // Yellow wire on Adafruit Pixels
uint8_t clockPin = 3;    // Green wire on Adafruit Pixels

Adafruit_WS2801 strip = Adafruit_WS2801(25, dataPin, clockPin);

float MIN_LIGHTNESS = 0.004;

struct point_s org = (struct point_s) { 0, 0   }   ;
struct point_s gridSize = (struct point_s) {5, 5    };

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

   // cartesianTest();

   rotatingColorAxes();
}

// ---------------------------------------------------------------------------------------------------------------------
//  flashTwoDifferentColors
// ---------------------------------------------------------------------------------------------------------------------

void flashTwoDifferentColors()
{
  struct point_s org = {0, 0};
  struct point_s gridSize = {5, 5};
  struct point_s p = {3,1};
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
   struct point_s prevPt;
   bool prevPtSet = false;
   float lightness;
   float dLightness = 1.0 / gridSize.x;
   float saturation;
   float dSaturation = 1.0 / gridSize.x;
   int i;

   struct point_s pt;
   uint32_t c;

   struct rgb_s rgb;
   
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
      rgb = hsl2rgb( 240, 1.0, lightness);
      c = Color( rgb.r, rgb.g, rgb.b);
      
      strip.setPixelColor( point2seq( pt, org, gridSize), c);
      strip.show();
      prevPt = pt;
      prevPtSet = true;
      delay( 100);
   }
   // strip.setPixelColor( point2seq( prevPt, org, gridSize), 0);

   delay( 2000);
   
   // bottom right to top left
   for (i = 0; i < 5; i++)
   {
      pt = {4-i, i };
      lightness = i * dLightness;
      // lightness *= lightness;
      if (lightness < MIN_LIGHTNESS)
         lightness = MIN_LIGHTNESS;
      rgb = hsl2rgb( 10, 1.0, lightness);
      c = Color( rgb.r, rgb.g, rgb.b);
      strip.setPixelColor( point2seq( pt, org, gridSize), c);
      strip.show();
      prevPt = pt;
      prevPtSet = true;
      delay( 100);
   }

   delay( 2000);

   // up the middle
   for (i = 0; i < 5; i++)
   {
      pt = {2,i };
      saturation = i * 0.25;    // 0.0, 0.25, 0.5, 0.75, 1.0
      // lightness *= lightness;
      rgb = hsl2rgb( 120, saturation, 0.5);
      c = Color( rgb.r, rgb.g, rgb.b);
      strip.setPixelColor( point2seq( pt, org, gridSize), c);
      strip.show();
      prevPt = pt;
      prevPtSet = true;
      delay( 100);
   }
   // pt = {3,4 };
   // c = Color( 0, 255, 0);
   // strip.setPixelColor( point2seq( pt, org, gridSize), c);
   // strip.show();
   
   delay( 6000);
   clearStrip();
   strip.show();
   delay( 600);
}

// ---------------------------------------------------------------------------------------------------------------------
//  rotatingColorAxes
// ---------------------------------------------------------------------------------------------------------------------

void rotatingColorAxes()
{
   int x,y;
   int dColor = 64;
   float d1 = 1.0 / (5-1);      // 0..4 ==> 0..1.0 in 5 steps
   int exponent = 4;            // Non-linear brightness increase as {x^exponent : x in [0.0..1.0]}
   int r,g,b;
   float rf, gf, bf;
   uint32_t c;
   int i;
   struct point_s pt;
   struct point_s org = {0,0 };
   struct point_s gridSize = {5,5 } ;

   int run;

   // Label the runs for x and y axes: rg, br, gb
   // 0, 1, 2

   for (run = 0; run < 3; run++)
   {
      for (y = 0; y < 5; y++)
         for (x = 0; x < 5; x++)
         {
            r = (run == 0 ? x : run == 1 ? y : 0);
            g = (run == 0 ? y : run == 1 ? 0 : x);
            b = (run == 0 ? 0 : run == 1 ? x : y);

            // pow(): non-linear brightness increase
            rf = 256 * pow( d1 * r, exponent);
            gf = 256 * pow( d1 * g, exponent);
            bf = 256 * pow( d1 * b, exponent);
            
            r = constrain( rf, 0, 255);
            g = constrain( gf, 0, 255);
            b = constrain( bf, 0, 255);
            c = Color( r, g, b);
            pt = {x,y } ;
            i = point2seq( pt, org, gridSize);
            strip.setPixelColor( i, c);
            // strip.show();
            // delay( 300);
         }
      strip.show();
      delay( 10000);
      // clearStrip();
      // strip.show();
      // delay( 500);
   }
   
}

// ---------------------------------------------------------------------------------------------------------------------
//  point2seq
// ---------------------------------------------------------------------------------------------------------------------

// Convert a point to a sequence number.  Point is relative to anOrigin.  aGridSize specifies width (x) and height (y)
// of grid.  Grid is assumed to be laid out in a zigzag pattern starting at the lower left corner, continuing
// horizontally to the lower right corner, going up a row and continuing back to the left, going up a row and continuing
// to the right, etc.

int point2seq( struct point_s aPt, struct point_s anOrigin, struct point_s aGridSize)
{
   int retval;
   struct point_s pt0 = aPt;

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
//  hue2rgb (internal helper function)
// ---------------------------------------------------------------------------------------------------------------------

// CSS3 spec algorithm: http://www.w3.org/TR/2011/REC-css3-color-20110607/#hsl-color

float hue2rgb( float m1, float m2, float hue )
{
   float retval;
   float h;
   
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
//  hsl2rgb
// ---------------------------------------------------------------------------------------------------------------------

// CSS3 spec algorithm: http://www.w3.org/TR/2011/REC-css3-color-20110607/#hsl-color
// hue in degrees
// sat, light in range [0..1]

struct rgb_s hsl2rgb( int aHue, float aSat, float aLight)
{
   rgb_s retval;
   
   float m1, m2, r, g, b;
   
   int hue = aHue;
   while (hue < 0)
      hue += 360;
   while (hue > 360)
      hue -= 360;

   float h = (float) hue / 360.0;
   
   if (aLight < 0.5)
      m2 = aLight * (aSat + 1.0);
   else
      m2 = aLight + aSat - aLight * aSat;
   m1 = aLight * 2.0 - m2;
   r = hue2rgb( m1, m2, h + 1.0/3.0);
   g = hue2rgb( m1, m2, h);
   b = hue2rgb( m1, m2, h - 1.0/3.0);
   retval.r = constrain( (int) (r * 256), 0, 255);
   retval.g = constrain( (int) (g * 256), 0, 255);
   retval.b = constrain( (int) (b * 256), 0, 255);

   return retval;
}

// hsl2rgb, hue2rgb commented out in favor of new versions

// // ---------------------------------------------------------------------------------------------------------------------
// //  hsl2rgb
// // ---------------------------------------------------------------------------------------------------------------------

// // CSS3 spec algorithm: http://www.w3.org/TR/2011/REC-css3-color-20110607/#hsl-color
// // All params in range [0.0..1.0]

// void hsl2rgb( float hue, float sat, float light, float *r, float *g, float *b)
// {
//    float m1, m2;
   
//    if (light < 0.5)
//       m2 = light * (sat + 1.0);
//    else
//       m2 = light + sat - light * sat;
//    m1 = light * 2.0 - m2;
//    *r = hue2rgb( m1, m2, hue + 1.0/3.0);
//    *g = hue2rgb( m1, m2, hue);
//    *b = hue2rgb( m1, m2, hue - 1.0/3.0);
// }

// // ---------------------------------------------------------------------------------------------------------------------
// //  hue2rgb (internal helper function)
// // ---------------------------------------------------------------------------------------------------------------------

// // CSS3 spec algorithm: http://www.w3.org/TR/2011/REC-css3-color-20110607/#hsl-color

// float hue2rgb( float m1, float m2, float hue )
// {
//    float retval;
//    float h = hue;
   
//    if (hue < 0.0)
//       h = hue + 1.0;
//    else if (hue > 1.0)
//       h = hue - 1.0;
//    else
//       h = hue;

//    if (h * 6.0 < 1.0)
//       retval = m1 + (m2 - m1) * h * 6.0;
//    else if (h * 2.0 < 1.0)
//       retval = m2;
//    else if (h * 3.0 < 2.0)
//       retval = m1 + (m2 - m1) * (2.0 / 3.0 - h) * 6.0;
//    else
//       retval = m1;

//    return retval;
// }

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

