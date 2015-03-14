// Compile and run with:
// arduino --upload --board arduino:avr:uno --port /dev/ttyACM0 xy.ino

#include "Adafruit_WS2801.h"
#include "SPI.h" // Comment out this line if using Trinket or Gemma
#ifdef __AVR_ATtiny85__
#include <avr/power.h>
#endif

#define GRID_SIZE_X 5
#define GRID_SIZE_Y 5

// ---------------------------------------------------------------------------------------------------------------------
//  types
// ---------------------------------------------------------------------------------------------------------------------

struct point
{
   int x,y;
};

struct rgbTriple
{
   int r,g,b;
};

// ---------------------------------------------------------------------------------------------------------------------
//  globals
// ---------------------------------------------------------------------------------------------------------------------

uint8_t dataPin  = 2;    // Yellow wire on Adafruit Pixels
uint8_t clockPin = 3;    // Green wire on Adafruit Pixels

Adafruit_WS2801 strip = Adafruit_WS2801(25, dataPin, clockPin);

const float MIN_LIGHTNESS = 0.004;

struct point org = { 0, 0   }   ;
struct point gridSize = (point) {GRID_SIZE_X, GRID_SIZE_Y };

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

   struct rgbTriple rgb;
   
   clearStrip();
   
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
   
   delay( 10000);
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
   struct rgbTriple rgb;
   int r,g,b;
   float rf, gf, bf;
   int h;
   float s, l;
   uint32_t c;
   int i;
   struct point pt;

   enum gridColorspace 
   {
      RGB = 1,
      HSL
   };

   enum axisColorspace
   {
      RED = 1,
      GREEN,
      BLUE,
      HUE,
      SAT,                      // Saturation
      LIGHT                     // Lightness
   };
   
   typedef struct
   {
      gridColorspace colorspace;  
      axisColorspace xAxisSpace, yAxisSpace; 

      // Axis values may be non-linear (e.g., intensity)
      union
      {
         int axisIntValues[GRID_SIZE_X];
         float axisFloatValues[GRID_SIZE_X];
      } x;
      
      union
      {
         int axisIntValues[GRID_SIZE_Y];
         float axisFloatValues[GRID_SIZE_Y];
      } y;
   } runParamTuple;
   
   // TODO: put the following setup in setup() ----------------------------------------------------------------
   
   // Gonna cheat, since we know we have a square grid:  same values on each axis.

   int ledBrightness[GRID_SIZE_X]; // RGB intensity
   float dLedBrightness = 1.0 / (GRID_SIZE_X - 1);
   float dLightness = 1.0 / (GRID_SIZE_X + 1); // Will drop lightness = 0 (black) and 1 (white), since that results in
                                               // an entire row/column being all the same (fully off or fully on).
   float lightness[GRID_SIZE_X]; // HSL
   float saturation[GRID_SIZE_X]; // HSL
   
   ledBrightness[0]               = 0;
   ledBrightness[GRID_SIZE_X - 1] = 255;

   saturation[0]                  = 0;
   saturation[GRID_SIZE_X - 1]    = 1;

   float v;

   clearStrip();
   
   for (i = 1; i < GRID_SIZE_X - 1; i++)
   {
      v = i * dLedBrightness;   // Linear
      saturation[i] = v;
      v = pow( v, exponent);    // Non-linear
      ledBrightness[i] = v * 256; // Don't need to constrain; won't get close to 255.
   }

   for (i = 0; i < GRID_SIZE_X; i++)
   {
      v = (i + 1) * dLightness; // See dLightness.
      v = pow( v, 3.45);
      lightness[i] = v;
   }
   

   // // Debug:
   // for (i = 0; i < GRID_SIZE_X; i++)
   // {
   //    pt = {i, 4 } ;
   //    rgb = hsl2rgb( 0, saturation[i], 0.5); // red/pink
   //    c = Color( rgb.r, rgb.g, rgb.b);
   //    strip.setPixelColor( point2seq( pt, org, gridSize), c);

   //    pt = {i, 3 } ; 
   //    rgb = hsl2rgb( 120, 0.5, lightness[i]); // greenish
   //    c = Color( rgb.r, rgb.g, rgb.b);
   //    strip.setPixelColor( point2seq( pt, org, gridSize), c);

   //    pt = {i, 2 } ;
   //    c = Color( 0, 0, ledBrightness[i]); // blue
   //    strip.setPixelColor( point2seq( pt, org, gridSize), c);
   // }
   // strip.show();
   // delay( 15000);

   runParamTuple runParams[6] = { { RGB , RED   , GREEN } ,  // 0
                                  { RGB , BLUE  , RED }   ,  // 1
                                  { RGB , GREEN , BLUE }  ,  // 2
                                  { HSL , HUE   , SAT }   ,  // 3
                                  { HSL , HUE   , LIGHT } ,  // 4
                                  { HSL , SAT   , LIGHT } }; // 5
   
   // RGB
   for (i = 0; i < 3; i++)
   {
      copyIntArray( runParams[i].x.axisIntValues, ledBrightness, GRID_SIZE_X);
      copyIntArray( runParams[i].y.axisIntValues, ledBrightness, GRID_SIZE_X);
   }
   
   // HSL
   float dHue = 60.0; // 360.0 / GRID_SIZE_X;
   for (i = 0; i < GRID_SIZE_X; i++)
   {
      runParams[3].x.axisIntValues[i] = (int) (i * dHue + 0.5);
      runParams[4].x.axisIntValues[i] = (int) (i * dHue + 0.5);
   }
   copyFloatArray( runParams[3].y.axisFloatValues, saturation, GRID_SIZE_X);
   copyFloatArray( runParams[4].y.axisFloatValues, lightness, GRID_SIZE_X);
   copyFloatArray( runParams[5].y.axisFloatValues, lightness, GRID_SIZE_X);

   copyFloatArray( runParams[5].x.axisFloatValues, saturation, GRID_SIZE_X);

   // (setup ends) ----------------------------------------------------------------
   
   int run;

   // Label the runs for x and y axes: rg, br, gb
   // 0, 1, 2

   for (run = 0; run < 6; run++)
   {
      runParamTuple rp = runParams[run];
      for (y = 0; y < GRID_SIZE_Y; y++)
         for (x = 0; x < GRID_SIZE_X; x++)
         {
            pt = {x,y };
            // Compute color based on colorspace
            switch (rp.colorspace)
            {
               case RGB:
                  r = (rp.xAxisSpace == RED
                       ? rp.x.axisIntValues[x]
                       : (rp.yAxisSpace == RED
                          ? rp.y.axisIntValues[y]
                          : 0));
                  g = (rp.xAxisSpace == GREEN
                       ? rp.x.axisIntValues[x]
                       : (rp.yAxisSpace == GREEN
                          ? rp.y.axisIntValues[y]
                          : 0));
                  b = (rp.xAxisSpace == BLUE
                       ? rp.x.axisIntValues[x]
                       : (rp.yAxisSpace == BLUE
                          ? rp.y.axisIntValues[y]
                          : 0));
                  c = Color( r, g, b);
                  break;
               case HSL:
                  h = (rp.xAxisSpace == HUE
                       ? rp.x.axisIntValues[x]
                       : (rp.yAxisSpace == HUE
                          ? rp.y.axisIntValues[y]
                          : 120));
                  s = (rp.xAxisSpace == SAT
                       ? rp.x.axisFloatValues[x]
                       : (rp.yAxisSpace == SAT
                          ? rp.y.axisFloatValues[y]
                          : 1.0));
                  l = (rp.xAxisSpace == LIGHT
                       ? rp.x.axisFloatValues[x]
                       : (rp.yAxisSpace == LIGHT
                          ? rp.y.axisFloatValues[y]
                          : 0.5));
                  rgb = hsl2rgb( h, s, l);
                  c = Color( rgb.r, rgb.g, rgb.b);
                  break;
               default:
                  c = 0;
                  break;
            }
            // c = Color( 255, 64, 0); // Debug
            strip.setPixelColor( point2seq( pt, org, gridSize), c);
         }
      // // Debug
      // rgb = hsl2rgb( run * 60, 1.0, 0.05);
      // c = Color( rgb.r, rgb.g, rgb.b);
      // strip.setPixelColor( run, c);
      strip.show();
      delay( run < 3 ? 5000 : 20000); // Delay longer for HSL part of run
   }

   // Since we skipped "black" and "white", show a checkerboard of those.

   clearStrip();
   c = Color( 255, 255, 255);
   for (i = 0; i < 25; i += 2)
      strip.setPixelColor( i, c);
   strip.show();
   delay( 5000);

   // for (run = 0; run < 3; run++)
   // {
   //    for (y = 0; y < 5; y++)
   //       for (x = 0; x < 5; x++)
   //       {
   //          r = (run == 0 ? x : run == 1 ? y : 0);
   //          g = (run == 0 ? y : run == 1 ? 0 : x);
   //          b = (run == 0 ? 0 : run == 1 ? x : y);

   //          // pow(): non-linear brightness increase
   //          rf = 256 * pow( d1 * r, exponent);
   //          gf = 256 * pow( d1 * g, exponent);
   //          bf = 256 * pow( d1 * b, exponent);
            
   //          r = constrain( rf, 0, 255);
   //          g = constrain( gf, 0, 255);
   //          b = constrain( bf, 0, 255);
   //          c = Color( r, g, b);
   //          pt = {x,y } ;
   //          i = point2seq( pt, org, gridSize);
   //          strip.setPixelColor( i, c);
   //          // strip.show();
   //          // delay( 300);
   //       }
   //    strip.show();
   //    delay( 2000);
   //    // clearStrip();
   //    // strip.show();
   //    // delay( 500);
   // }
   
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

struct rgbTriple hsl2rgb( int aHue, float aSat, float aLight)
{
   struct rgbTriple retval;
   
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

// ---------------------------------------------------------------------------------------------------------------------
//  copyIntArray
// ---------------------------------------------------------------------------------------------------------------------

void copyIntArray( int aDest[], int aSrc[], int n)
{
   int i;
   for (i = 0; i < n; i++)
      aDest[i] = aSrc[i];
}

// ---------------------------------------------------------------------------------------------------------------------
//  copyFloatArray
// ---------------------------------------------------------------------------------------------------------------------

void copyFloatArray( float aDest[], float aSrc[], int n)
{
   int i;
   for (i = 0; i < n; i++)
      aDest[i] = aSrc[i];
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

// ---------------------------------------------------------------------------------------------------------------------
//  Adafruit WS2801 library code
// ---------------------------------------------------------------------------------------------------------------------

// The following copyright applies to all code in this section.

// Example library for driving Adafruit WS2801 pixels!


//   Designed specifically to work with the Adafruit RGB Pixels!
//   12mm Bullet shape ----> https://www.adafruit.com/products/322
//   12mm Flat shape   ----> https://www.adafruit.com/products/738
//   36mm Square shape ----> https://www.adafruit.com/products/683

//   These pixels use SPI to transmit the color data, and have built in
//   high speed PWM drivers for 24 bit color per pixel
//   2 pins are required to interface

//   Adafruit invests time and resources providing this open source code, 
//   please support Adafruit and open-source hardware by purchasing 
//   products from Adafruit!

//   Written by Limor Fried/Ladyada for Adafruit Industries.  
//   BSD license, all text above must be included in any redistribution

// Create a 24 bit color value from R,G,B
// (Code snitched from Adafruit.  Please don't sue me, y'all, for stealing your bit-shifting code.)
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

// ---------------------------------------------------------------------------------------------------------------------
//  Adafruit WS2801 library code ENDS
// ---------------------------------------------------------------------------------------------------------------------
