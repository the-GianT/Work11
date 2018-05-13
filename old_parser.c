#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "matrix.h"
#include "parser.h"


/*======== void parse_file () ==========
Inputs:   char * filename 
          struct stack * cs (coordinate system),
          struct matrix * edges,
          screen s
Returns: 

Goes through the file named filename and performs all of the actions listed in that file.
The file follows the following format:
     Every command is a single character that takes up a line
     Any command that requires arguments must have those arguments in the second line.
     The commands are as follows:

     push: push a copy of the curent top of the coordinate system stack to the stack

     pop: remove the current top of the coordinate system stack

     All the shape commands work as follows:
        1) Add the shape to a temporary matrix
        2) Multiply that matrix by the current top of the coordinate system stack
        3) Draw the shape to the screen
        4) Clear the temporary matrix

     sphere: add a sphere -
             takes 4 arguments (cx, cy, cz, r)
     torus: add a torus to the polygon matrix -
            takes 5 arguments (cx, cy, cz, r1, r2)
     box: add a rectangular prism -
          takes 6 arguments (x, y, z, width, height, depth)
     circle: add a circle -
             takes 4 arguments (cx, cy, cz, r)
     hermite: add a hermite curve -
              takes 8 arguments (x0, y0, x1, y1, rx0, ry0, rx1, ry1)
     bezier: add a bezier curve -
             takes 8 arguments (x0, y0, x1, y1, x2, y2, x3, y3)
     line: add a line to the edge matrix -
           takes 6 arguemnts (x0, y0, z0, x1, y1, z1)

     scale: create a scale matrix,
            then multiply the current top of the coordinate system stack -
            takes 3 arguments (sx, sy, sz)
     move: create a translation matrix,
                then multiply the current top of the coordinate system stack -
                takes 3 arguments (tx, ty, tz)
     rotate: create a rotation matrix,
             then multiply the transform matrix by the translation matrix -
             takes 2 arguments (axis, theta) axis should be x, y or z

     display: display the screen

     save: save the screen to a file -
           takes 1 argument (file name)

    quit: end parsing

See the file script for an example of the file format


IMPORTANT MATH NOTE:
the trig functions int math.h use radian mesure, but us normal
humans use degrees, so the file will contain degrees for rotations,
be sure to conver those degrees to radians (M_PI is the constant
for PI)
====================*/
void parse_file ( char * filename,
                  // struct stack * cs,
                  struct matrix * edges,
		  screen s, zbuffer zb,
		  double *view, color ambient, double light[2][3],
		  double *areflect, double *sreflect, double *dreflect) {

  FILE *f;
  char line[256];
  color c;
  struct stack *cs; // coordinate system
  int step;
  int step_3d;

  clear_screen(s);
  clear_zbuffer(zb);
  c.red = 0;
  c.green = 0;
  c.blue = 0;
  step = 100;
  step_3d = 50;
  cs = new_stack();

  if ( strcmp(filename, "stdin") == 0 ) 
    f = stdin;
  else
    f = fopen(filename, "r");
  
  while ( fgets(line, 255, f) != NULL ) {
    line[strlen(line)-1]='\0';
    printf(":%s:\n",line);

    if (*line == '#') {
      /* Do nothing because it's a comment */

    } else if (strncmp(line, "push", strlen(line)) == 0) {
      push(cs);

    } else if (strncmp(line, "pop", strlen(line)) == 0) {
      pop(cs);
      
    } else if (strncmp(line, "sphere", strlen(line)) == 0) {
      double args[4];
      int numinputs;

      /* Read arguments: */
      fgets(line, 255, f);
      line[strlen(line)-1]='\0';
      printf(":%s:\n", line);
      numinputs = sscanf(line, "%lf %lf %lf %lf", args, args+1, args+2, args+3);
      if (numinputs != 4) {
	printf("Error: Invalid arguments for sphere\n");
	return;
      }
      
      add_sphere(edges, *args, args[1], args[2], args[3], step_3d);
      matrix_mult(cs->data[cs->top], edges); // apply transformations
      draw_polygons(edges, s, zb, view, light, ambient, areflect, dreflect, sreflect);
      edges->lastcol = 0; // clear temporary polygon matrix

    } else if (strncmp(line, "torus", strlen(line)) == 0) {
      double args[5];
      int numinputs;

      /* Read arguments: */
      fgets(line, 255, f);
      line[strlen(line)-1]='\0';
      printf(":%s:\n", line);
      numinputs = sscanf(line, "%lf %lf %lf %lf %lf", args, args+1, args+2, args+3, args + 4);
      if (numinputs != 5) {
	printf("Error: Invalid arguments for torus\n");
	return;
      }
      
      add_torus(edges, *args, args[1], args[2], args[3], args[4], step_3d);
      matrix_mult(cs->data[cs->top], edges); // apply transformations
      // draw_polygons(edges, s, zb, c);
      draw_polygons(edges, s, zb, view, light, ambient, areflect, dreflect, sreflect);
      edges->lastcol = 0; // clear temporary polygon matrix

    } else if (strncmp(line, "box", strlen(line)) == 0) {
      double args[6];
      int numinputs;

      /* Read arguments: */
      fgets(line, 255, f);
      line[strlen(line)-1]='\0';
      printf(":%s:\n", line);
      numinputs = sscanf(line, "%lf %lf %lf %lf %lf %lf", args, args+1, args+2, args+3, args + 4, args + 5);
      if (numinputs != 6) {
	printf("Error: Invalid arguments for box\n");
	return;
      }
      
      add_box(edges, *args, args[1], args[2], args[3], args[4], args[5]);
      matrix_mult(cs->data[cs->top], edges); // apply transformations
      // draw_polygons(edges, s, zb, c);
      draw_polygons(edges, s, zb, view, light, ambient, areflect, dreflect, sreflect);
      edges->lastcol = 0; // clear temporary polygon matrix

      /*
    } else if (strncmp(line, "clear", strlen(line)) == 0) {
      edges->lastcol = 0;
      polygons->lastcol = 0;
      */
      
    } else if (strncmp(line, "circle", strlen(line)) == 0) {
      int args[4];
      int numinputs;

      /* Read arguments: */
      fgets(line, 255, f);
      line[strlen(line)-1]='\0';
      printf(":%s:\n", line);
      numinputs = sscanf(line, "%d %d %d %d", args, args+1, args+2, args+3);
      if (numinputs != 4) {
	printf("Error: Invalid arguments for circle\n");
	return;
      }
      
      add_circle(edges, *args, args[1], args[2], args[3], step);
      matrix_mult(cs->data[cs->top], edges); // apply transformations
      draw_lines(edges, s, zb, c);
      edges->lastcol = 0; // clear temporary polygon matrix
      
    } else if (strncmp(line, "hermite", strlen(line)) == 0) {
      int args[8];
      int numinputs;

      /* Read arguments: */
      fgets(line, 255, f);
      line[strlen(line)-1]='\0';
      printf(":%s:\n", line);
      numinputs = sscanf(line, "%d %d %d %d %d %d %d %d", args, args+1, args+2, args+3, args+4, args+5, args+6, args+7);
      if (numinputs != 8) {
	printf("Error: Invalid arguments for hermite\n");
	return;
      }
      
      add_curve(edges, *args, args[1], args[2], args[3], args[4], args[5], args[6], args[7], step, HERMITE);
      matrix_mult(cs->data[cs->top], edges); // apply transformations
      draw_lines(edges, s, zb, c);
      edges->lastcol = 0; // clear temporary polygon matrix
      
    } else if (strncmp(line, "bezier", strlen(line)) == 0) {
      int args[8];
      int numinputs;

      /* Read arguments: */
      fgets(line, 255, f);
      line[strlen(line)-1]='\0';
      printf(":%s:\n", line);
      numinputs = sscanf(line, "%d %d %d %d %d %d %d %d", args, args+1, args+2, args+3, args+4, args+5, args+6, args+7);
      if (numinputs != 8) {
	printf("Error: Invalid arguments for bezier\n");
	return;
      }
      
      add_curve(edges, *args, args[1], args[2], args[3], args[4], args[5], args[6], args[7], step, BEZIER);
      matrix_mult(cs->data[cs->top], edges); // apply transformations
      draw_lines(edges, s, zb, c);
      edges->lastcol = 0; // clear temporary polygon matrix

    } else if (strncmp(line, "line", strlen(line)) == 0) {
      int args[6];
      int numinputs;

      /* Read arguments: */
      fgets(line, 255, f);
      line[strlen(line)-1]='\0';
      printf(":%s:\n", line);
      numinputs = sscanf(line, "%d %d %d %d %d %d", args, args+1, args+2, args+3, args+4, args+5);
      if (numinputs != 6) {
	printf("Error: Invalid arguments for line\n");
	return;
      }
      
      add_edge(edges, *args, args[1], args[2], args[3], args[4], args[5]);
      matrix_mult(cs->data[cs->top], edges); // apply transformations
      draw_lines(edges, s, zb, c);
      edges->lastcol = 0; // clear temporary polygon matrix

      /*
    } else if (strncmp(line, "ident", strlen(line)) == 0) {
      ident(transform);
      */
      
    } else if (strncmp(line, "scale", strlen(line)) == 0) {
      int args[3];
      int numinputs;
      struct matrix * scale_m;

      /* Read arguments: */
      fgets(line, 255, f);
      line[strlen(line)-1]='\0';
      printf(":%s:\n", line);
      numinputs = sscanf(line, "%d %d %d", args, args +1, args + 2);
      if (numinputs != 3) {
	printf("Error: Invalid arguments for scale\n");
	return;
      }
      
      scale_m = make_scale(*args, args[1], args[2]);
      /*
      matrix_mult(scale_m, transform);
      free_matrix(scale_m);
      */
      matrix_mult(cs->data[cs->top], scale_m);

      // Replace current top with new top
      free_matrix(cs->data[cs->top]);
      cs->data[cs->top] = scale_m;
      
    } else if (strncmp(line, "move", strlen(line)) == 0) {
      int args[3];
      int numinputs;
      struct matrix * trans_m;

      /* Read arguments: */
      fgets(line, 255, f);
      line[strlen(line)-1]='\0';
      printf(":%s:\n", line);
      numinputs = sscanf(line, "%d %d %d", args, args +1, args + 2);
      if (numinputs != 3) {
	printf("Error: Invalid arguments for move\n");
	return;
      }
      
      trans_m = make_translate(*args, args[1], args[2]);
      // matrix_mult(trans_m, transform);
      // free_matrix(trans_m);
      matrix_mult(cs->data[cs->top], trans_m);

      // Replace current top with new top
      free_matrix(cs->data[cs->top]);
      cs->data[cs->top] = trans_m;
      
      
    } else if (strncmp(line, "rotate", strlen(line)) == 0) {
      char axis;
      double theta;
      int numinputs;
      struct matrix * rot_m;

      /* Read arguments: */
      fgets(line, 255, f);
      line[strlen(line)-1]='\0';
      printf(":%s:\n", line);
      numinputs = sscanf(line, "%c %lf", &axis, &theta);
      if (numinputs != 2) {
	printf("Error: Invalid arguments for rotate\n");
	return;
      }

      theta = theta * (M_PI / 180);
      if (axis == 'x')
	rot_m = make_rotX(theta);
      else if (axis == 'y')
	rot_m = make_rotY(theta);
      else if (axis == 'z')
	rot_m = make_rotZ(theta);
      else {
	printf("Error: Invalid axis argument for rotate; must be x, y, or z\n");
	return;
      }

      /*
      matrix_mult(rot_m, transform);
      free_matrix(rot_m);
      */
      matrix_mult(cs->data[cs->top], rot_m);

      // Replace current top with new top
      free_matrix(cs->data[cs->top]);
      cs->data[cs->top] = rot_m;

      /*
    } else if (strncmp(line, "apply", strlen(line)) == 0) {
      matrix_mult(transform, edges);
      matrix_mult(transform, polygons);
      */

    } else if ( strncmp(line, "clear", strlen(line)) == 0 ) {
      clear_screen(s);
      clear_zbuffer(zb);
      
    } else if (strncmp(line, "display", strlen(line)) == 0) {

      /*
      clear_screen(s);
      if (edges->lastcol)
	draw_lines(edges, s, c);
      if (polygons->lastcol)
	draw_polygons(polygons, s, c);
      */
      
      display(s);
      
    } else if (strncmp(line, "save", strlen(line)) == 0) {

      /*
      clear_screen(s);
      if (edges->lastcol)
	draw_lines(edges, s, c);
      if (edges->lastcol)
	draw_polygons(polygons, s, c);
      */

      // Read file name argument for save
      fgets(line, 255, f);
      line[strlen(line)-1]='\0';
      printf(":%s:\n", line);
      
      save_extension(s, line);
      
    } else if (strncmp(line, "quit", strlen(line)) == 0) {
      free_stack(cs);
      return;

    } else
      printf("%s: command not found\n", line);
  } // end while loop
  free_stack(cs);
} // end parse_file function
