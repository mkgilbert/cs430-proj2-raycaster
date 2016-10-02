/* raycast.c - provides raycasting functionality */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "include/raycast.h"

/**
 * Finds and gets the index in objects that has the camera width and height
 * @param objects - array of object types that represent the scene
 * @return int - non-negative if the object was found, -1 otherwise
 */
int get_camera(object *objects) {
    int i = 0;
    while (i < MAX_OBJECTS && objects[i].type != 0) {
        if (objects[i].type == CAMERA) {
            return i;
        }
        i++;
    }
    // no camera found in data
    return -1;
}

/**
 * colors the values of a pixel based on the color array that is passed in
 * @param color - array of 3 color values for r,g,b
 * @param row - which row the pixel is on
 * @param col - which column the pixel is on
 * @param img - image struct that allows for indexing the appropriate spot
 */
void shade_pixel(double *color, int row, int col, image *img) {
    // fill in pixel color values
    // the color vals are stored as values between 0 and 1, so we need to adjust
    img->pixmap[row * img->width + col].r = color[0];
    img->pixmap[row * img->width + col].g = color[1];
    img->pixmap[row * img->width + col].b = color[2];
}

/**
 * Tests for an intersection between a ray and a plane
 * @param Ro - 3d vector of ray origin
 * @param Rd - 3d vector of ray direction
 * @param Pos - 3d vector of the plane's position
 * @param Norm - 3d vector of the normal to the plane
 * @return - distance to the object if intersects, otherwise, -1
 */
double plane_intersect(double *Ro, double *Rd, double *Pos, double *Norm) {
    normalize(Norm);
    // determine if plane is parallel to the ray
    double vd = v3_dot(Norm, Rd);
    
    if (fabs(vd) < 0.0001) return -1;

    double vector[3];
    v3_sub(Pos, Ro, vector);
    double t = v3_dot(vector, Norm) / vd;

    // no intersection
    if (t < 0.0)
        return -1;

    return t;
}

/**
 * Tests for an intersection between a ray and a sphere
 * @param Ro - 3d vector of ray origin
 * @param Rd - 3d vector of ray direction
 * @param C - 3d vector of the center of the sphere
 * @param r - radius of the sphere
 * @return - distance to the object if intersects, otherwise, -1
 */
double sphere_intersect(double *Ro, double *Rd, double *C, double r) {
    double a, b, c;
    // calculate quadratic formula
    // First find a, b, c
    a = sqr(Rd[0]) + sqr(Rd[1]) + sqr(Rd[2]);
    //printf("value of a: %lf\n", a);
    b = 2 * (Rd[0]*(Ro[0]-C[0]) + Rd[1]*(Ro[1]-C[1]) + Rd[2]*(Ro[2]-C[2]));
    //printf("value of b: %lf\n", b);
    c = sqr(Ro[0]-C[0]) + sqr(Ro[1]-C[1]) + sqr(Ro[2]-C[2]) - sqr(r);
    //printf("value of c: %lf\n", c);

    // make sure a is 1 (means the ray direction was normalized)
    if (a > 1.0001 || a < .9999) {
        printf("a = %lf\n", a);
        fprintf(stderr, "Ray direction was not normalized\n");
        exit(1);
    }
    
    // check that discriminant is <, =, or > 0
    double disc = sqr(b) - 4*a*c;
    double t0, t1;  // solutions
    if (disc < 0) {
        //printf("disc was < 0\n");
        return -1; // no solution
    }
    else if (disc == 0) {
        t0 = -1*(b / (2*a)); // single solution
        return t0;
        //printf("t0 = %lf\n", t0);
    }
    else {  // 2 solutions: find the smaller
        t0 = (-1*b - sqrt(sqr(b) - 4*c))/2;
        t1 = (-1*b + sqrt(sqr(b) - 4*c))/2;
        //printf("t0 = %lf\n", t0);
        //printf("t1 = %lf\n", t1);
    }

    if (t0 < 0 && t1 < 0) {
        // no intersection
        return -1;
    }
    else if (t0 < 0 && t1 > 0) {
        return t1;
    }
    else if (t0 > 0 && t1 < 0) {
        return t0;
    }
    else { // they were both positive
        if (t0 <= t1)
            return t0;
        else
            return t1;
    }
    //printf("value of t0: %lf\n", t0);
    //printf("value of t1: %lf\n", t1);
    return -1;
}

/**
 * Shoots out rays over a viewplane of dimensions stored in img and looks through
 * the array of objects for an intersection for each pixel.
 * @param img - image data (width, height, pixmap...)
 * @param cam_width - camera width
 * @param cam_height - camera height
 * @param objects - array of objects in the scene
 */
void raycast_scene(image *img, double cam_width, double cam_height, object *objects) {
    // loop over all pixels and test for intesections with objects.
    // store results in pixmap
    int i;  // x coord iterator
    int j;  // y coord iterator
    int o;  // object iterator
    double vp_pos[3] = {0, 0, 1};   // view plane position
    double Ro[3] = {0, 0, 0};       // camera position (ray origin)
    double point[3] = {0, 0, 0};    // point on viewplane where intersection happens

    double pixheight = cam_height / img->height;
    double pixwidth = cam_width / img->width;

    double Rd[3] = {0, 0, 0};       // direction of Ray
    point[2] = vp_pos[2];    // set intersecting point Z to viewplane Z

    for (i = 0; i < img->height; i++) {
        point[1] = -(vp_pos[1] - cam_height/2.0 + pixheight*(i + 0.5));
        for (j = 0; j < img->width; j++) {
            point[0] = vp_pos[0] - cam_width/2.0 + pixwidth*(j + 0.5);
            normalize(point);   // normalize the point
            // store normalized point as our ray direction
            Rd[0] = point[0];
            Rd[1] = point[1];
            Rd[2] = point[2];

            int best_o = 0;
            double best_t = INFINITY;
            for (o=0; objects[o].type != 0; o++) {
                // we need to run intersection test on each object
                double t = 0;
                switch(objects[o].type) {
                    case 0:
                        printf("no object found\n");
                        break;
                    case CAMERA:
                        break;
                    case SPHERE:
                        t = sphere_intersect(Ro, Rd, objects[o].sph.position,
                                                        objects[o].sph.radius);
                        break;
                    case PLANE:
                        t = plane_intersect(Ro, Rd, objects[o].pln.position,
                                                    objects[o].pln.normal);
                        break;
                    default:
                        // Error
                        exit(1);
                }
                if (t > 0 && t < best_t) {
                    best_t = t;
                    best_o = o;
                }
            }
            if (best_t > 0 && best_t != INFINITY) {// there was an intersection
                //printf("#");    // ascii ray tracer "hit"
                //printf("type: %d\n", objects[best_i].type);
                if (objects[best_o].type == PLANE) {
                    shade_pixel(objects[best_o].pln.color, i, j, img);
                }
                else if (objects[best_o].type == SPHERE) {
                    //printf("shade\n");
                    shade_pixel(objects[best_o].sph.color, i, j, img);
                }
            }
            else {
                //printf(".");    // ascii ray tracer "miss"
            }
        }
        //printf("\n");
    }
}
