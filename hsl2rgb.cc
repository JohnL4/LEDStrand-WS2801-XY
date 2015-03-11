#include <stdio.h>

// ---------------------------------------------------------------------------------------------------------------------
//  constrain
// ---------------------------------------------------------------------------------------------------------------------

// Built in to Arduino, reproduced here as scaffolding

int constrain( int aValue, int aFloor, int aCeiling)
{
   int retval;

   if (aValue < aFloor)
      retval = aFloor;
   else if (aValue > aCeiling)
      retval = aCeiling;
   else
      retval = aValue;

   return retval;
}

struct rgbStruct
{
   int r,g,b;
};

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
//  hsl2rgb
// ---------------------------------------------------------------------------------------------------------------------

// CSS3 spec algorithm: http://www.w3.org/TR/2011/REC-css3-color-20110607/#hsl-color
// hue in degrees
// sat, light in range [0..1]

struct rgbStruct hsl2rgb( int aHue, float aSat, float aLight)
{
   rgbStruct retval;
   
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
//  main
// ---------------------------------------------------------------------------------------------------------------------

int main( int argc, char **argv)
{
   rgbStruct rgb;

   int hue;
   float sat, light;

   printf( "hue\tsat\tlight\tr\tg\tb\n");

   for (sat = 0.0; sat <= 1.0; sat += 0.125)
      for (light = 0.0; light <= 1.0; light += 0.125)
         for (hue = 0; hue < 360; hue += 15)
         {
            rgb = hsl2rgb( hue, sat, light);
            printf( "%d\t%6.4g\t%6.4g\t%d\t%d\t%d\n", hue, sat, light, rgb.r, rgb.g, rgb.b);
         }
   
   // hue = 0;
   // sat = 1.0;
   // light = 0.5;
   
   // rgb = hsl2rgb( hue, sat, light);
   // printf( "For hue %d, sat %g, lightness %g, rgb = {%d, %d, %d}\n",
   //         hue, sat, light,
   //         rgb.r, rgb.g, rgb.b);

   // hue = 120;
   // rgb = hsl2rgb( hue, sat, light);
   // printf( "For hue %d, sat %g, lightness %g, rgb = {%d, %d, %d}\n",
   //         hue, sat, light,
   //         rgb.r, rgb.g, rgb.b);

   // light = 1.0;
   // rgb = hsl2rgb( hue, sat, light);
   // printf( "For hue %d, sat %g, lightness %g, rgb = {%d, %d, %d}\n",
   //         hue, sat, light,
   //         rgb.r, rgb.g, rgb.b);

   // hue = 240;
   // rgb = hsl2rgb( hue, sat, light);
   // printf( "For hue %d, sat %g, lightness %g, rgb = {%d, %d, %d}\n",
   //         hue, sat, light,
   //         rgb.r, rgb.g, rgb.b);
}
