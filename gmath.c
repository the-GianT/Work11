#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "gmath.h"
#include "matrix.h"
#include "ml6.h"

//lighting functions
color get_lighting( double *normal, double *view, color alight, double light[2][3], double *areflect, double *dreflect, double *sreflect) {
  color i, a, d, s;
  double dp; // dot product of normalized normal vector and normalized light vector

  normalize(normal);
  normalize(light[LOCATION]);

  dp = dot_product(normal, light[LOCATION]);

  a = calculate_ambient(alight, areflect);
  d = calculate_diffuse(light, dreflect, normal, dp);
  s = calculate_specular(light, sreflect, view, normal, dp);

  i.red = a.red + d.red + s.red;
  i.green = a.green + d.green + s.green;
  i.blue = a.blue + d.blue + s.blue;
  limit_color(&i);
  return i;
}

color calculate_ambient(color alight, double *areflect ) {
  color a;

  a.red = alight.red * areflect[RED];
  a.green = alight.green * areflect[GREEN];
  a.blue = alight.blue * areflect[BLUE];

  /*
  printf("a.red: %d\n", a.red);
  printf("a.green: %d\n", a.green);
  printf("a.blue: %d\n", a.blue);
  */
  
  // limit_color(&a);
  // a = limit85(a);
  return a;
}

color calculate_diffuse(double light[2][3], double *dreflect, double *normal, double dp ) {
  color d;
  
  if (dp > 0) {
    d.red = light[COLOR][RED] * dreflect[RED] * dp;
    d.green = light[COLOR][GREEN] * dreflect[GREEN] * dp;
    d.blue = light[COLOR][BLUE] * dreflect[BLUE] * dp;
  } else {
    d.red = 0;
    d.green = 0;
    d.blue = 0;
  }

  // limit_color(&d);
  // d = limit85(d);
  return d;
}

color calculate_specular(double light[2][3], double *sreflect, double *view, double *normal, double dp ) {

  color s;

  if (dp > 0) {
    double sc; // scalar for normal vector
    double resultant[3]; // vector that results from calculating 2(N dot L)N - L
    double rdotv; // dot product of resultant and view
    double multiplier; // rdotv raised to power of specular exponent

    sc = 2 * dp;
    *resultant = sc * *normal - *(light[LOCATION]);
    resultant[1] = sc * normal[1] - light[LOCATION][1];
    resultant[2] = sc * normal[2] - light[LOCATION][2];

    /*
    printf("resultant[0]: %lf\n", *resultant);
    printf("resultant[1]: %lf\n", resultant[1]);
    printf("resultant[2]: %lf\n", resultant[2]);
    */

    if ((rdotv = dot_product(resultant, view)) < 0) {
      // printf("rdotv: %lf\n", rdotv);
      rdotv = 0;
    }
    
    multiplier = pow(rdotv, SPECULAR_EXP);

    s.red = light[COLOR][RED] * sreflect[RED] * multiplier;
    s.green = light[COLOR][GREEN] * sreflect[GREEN] * multiplier;
    s.blue = light[COLOR][BLUE] * sreflect[BLUE] * multiplier;

    /*
    printf("s.red: %d\n", s.red);
    printf("s.green: %d\n", s.green);
    printf("s.blue: %d\n", s.blue);
    */
    
  } else {
    s.red = 0;
    s.green = 0;
    s.blue = 0;
  }

  // limit_color(&s);
  // s = limit85(s);
  return s;
}


//limit each component of c to a max of 255
void limit_color( color * c ) {
  if (c->red > 255)
    c->red = 255;
  if (c->green > 255)
    c->green = 255;
  if (c->blue > 255)
    c->blue = 255;
}

/*
color limit85(color c)
{
  if (c.red > 85)
    c.red = 85;
  if (c.green > 85)
    c.green = 85;
  if (c.blue > 85)
    c.blue = 85;

  return c;
}
*/

//vector functions
//normalize vetor, should modify the parameter
void normalize( double *vector ) {
  double divisor; // what you're dividing each vector component by

  divisor = sqrt(*vector * *vector + vector[1] * vector[1] + vector[2] * vector[2]);

  *vector /= divisor;
  vector[1] /= divisor;
  vector[2] /= divisor;
}

//Return the dot porduct of a . b
double dot_product( double *a, double *b ) {
  return *a * *b + a[1] * b[1] + a[2] * b[2];
}

double *calculate_normal(struct matrix *polygons, int i) {

  double A[3];
  double B[3];
  double *N = (double *)malloc(3 * sizeof(double));

  A[0] = polygons->m[0][i+1] - polygons->m[0][i];
  A[1] = polygons->m[1][i+1] - polygons->m[1][i];
  A[2] = polygons->m[2][i+1] - polygons->m[2][i];

  B[0] = polygons->m[0][i+2] - polygons->m[0][i];
  B[1] = polygons->m[1][i+2] - polygons->m[1][i];
  B[2] = polygons->m[2][i+2] - polygons->m[2][i];

  N[0] = A[1] * B[2] - A[2] * B[1];
  N[1] = A[2] * B[0] - A[0] * B[2];
  N[2] = A[0] * B[1] - A[1] * B[0];

  return N;
}
