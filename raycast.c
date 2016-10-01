/* raycast.c - provides raycasting functionality */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "include/json.h"
#include "include/vector_math.h"
#include "include/raycast.h"
#include "include/ppmrw.h"


static inline void normalize(double *v) {
    double len = sqr(v[0]) + sqr(v[1]) + sqr(v[2]);
    len = sqrt(len);
    v[0] /= len;
    v[1] /= len;
    v[2] /= len;
}

/**
 * Finds the camera information in a list of scene objects
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

void shade_pixel(double *color, int row, int col, image *img) {
    // fill in pixel color values
    //printf("color: %lf %lf %lf\n", color[0], color[1], color[2]);
    // need to flip the y axis due to the placement of the viewplane
    //row = (img->height-1) - row;
    img->pixmap[row * img->width + col].r = color[0];
    img->pixmap[row * img->width + col].g = color[1];
    img->pixmap[row * img->width + col].b = color[2];
}

int plane_intersect(double *Ro, double *Rd, double *Pos, double *Norm) {
    normalize(Norm);
    // determine if plane is parallel to the ray
    double den = v3_dot(Norm, Rd);          // denominator
    if (fabs(den) < 0.0001) return -1;                // the plane is parallel
    //double D = v3_len(Pos);                 // distance from origin to the plane
    //printf("D = %lf\n", D);
    //double num = (v3_dot(Ro, Norm) + D); // numerator
    
    /* this works...*/
    // let's try it harrison's way...
    double vector_diff[3];
    v3_sub(Ro, Pos, vector_diff);
    double t = -(v3_dot(vector_diff, Norm) / den);
    /* end */
    

    //double t = num / den;
    //printf("t = %lf\n", t);
    // no intersection
    if (t < 0.0)
        return -1;

    return t;
}

double sphere_intersect(double *Ro, double *Rd, double *C, double r) {
    double a, b, c;
    /*for (int i=0; i<3; i++) {
        printf("Ro[%d] = %lf\n", i, Ro[i]);
        printf("Rd[%d] = %lf\n", i, Rd[i]);
        printf("C[%d] = %lf\n", i, C[i]);
    }*/
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

void raycast_scene(image *img, double cam_width, double cam_height, object *objects) {
    // loop over all pixels and test for intesections with objects.
    // store results in pixmap
    int x;  // x coord iterator
    int y;  // y coord iterator
    int i;  // object iterator
    double cx = 0;
    double cy = 0;
    double h = cam_height; // these will be set from the camera object
    double w = cam_width;
    
    //printf("h: %lf\n", h);
    //printf("w: %lf\n", w);
    int M = img->width; // pixels width and height of image
    int N = img->height;
    //printf("M: %d\n", M);
    //printf("N: %d\n", N);

    double pixheight = h / img->width;
    double pixwidth = w / img->height;

    for (x = 0; x < img->width; x++) {
        for (y = 0; y < img->height; y++) {
            double Ro[3] = {0, 0, 0}; // vector that represents the ray origin
            // Rd = normalize(Pixel - Ro)
            double Rd[3] = {
                cx - (w/2.0) + pixwidth * (y + 0.5),
                -(cy - (h/2.0) + pixheight * (x + 0.5)),
                1
            }; // ray direction
            normalize(Rd);

            int best_i;
            double best_t = INFINITY;
            for (i=0; objects[i].type != 0; i++) {
                // we need to run intersection test on each object
                double t = 0;
                switch(objects[i].type) {
                    case 0:
                        printf("no object found\n");
                        break;
                    case CAMERA:
                        break;
                    case SPHERE:
                        t = sphere_intersect(Ro, Rd, objects[i].sph.position,
                                                        objects[i].sph.radius);
                        //printf("t = %lf\n", t);
                        break;
                    case PLANE:
                        t = plane_intersect(Ro, Rd, objects[i].pln.position,
                                                    objects[i].pln.normal);
                        break;
                    default:
                        // Error
                        exit(1);
                }
                if (t > 0 && t < best_t) {
                    best_t = t;
                    best_i = i;
                }
            }
            if (best_t > 0 && best_t != INFINITY) {// there was an intersection
                //printf("#");    // ascii ray tracer "hit"
                //printf("type: %d\n", objects[best_i].type);
                if (objects[best_i].type == PLANE) {
                    shade_pixel(objects[best_i].pln.color, x, y, img);
                }
                else if (objects[best_i].type == SPHERE) {
                    //printf("shade\n");
                    shade_pixel(objects[best_i].sph.color, x, y, img);
                }
            }
            else {
                //printf(".");    // ascii ray tracer "miss"
            }
        }
        //printf("\n");
    }
}

int main(int argc, char *argv[]) {
    // TODO: error checking for file
    /* testing that we can read json objects */
    FILE *json = fopen(argv[1], "rb");
    read_json(json);
    print_objects(objects);
    image img;
    img.width = 800;
    img.height = 800;
    img.pixmap = malloc(sizeof(RGBPixel)*img.width*img.height);
    int pos = get_camera(objects);
    printf("camera is object %d\n", pos);
    printf("width: %lf\n", objects[pos].cam.width);
    printf("height: %lf\n", objects[pos].cam.height);

    /* intersection testing */
    /*double Ro[3] = {0, 0, 0};
    double Rd[3] = {5, 5, 20};
    normalize(Rd);
    double C[3] = {5, 5, 20};
    double radius = 0.5;*/
    //double red[3] = {255, 1, 25};

    //sphere_intersect(Ro, Rd, C, radius);
    /* shade_pixel test */
    /*for (int i=0; i<100; i++) {
        for (int j=0; j<100; j++) {
            shade_pixel(red, i, j, &img);
        }
    }
    print_pixels(img.pixmap, 100, 100);*/

    /* raycasting a single object test */
    raycast_scene(&img, objects[pos].cam.width, objects[pos].cam.height, objects);
    FILE *out = fopen("image.ppm", "wb");
    create_ppm(out, 6, &img);
    fclose(out);
    // loop through objects in the scene and do raycasting
    /*int i = 0;
    while (i < MAX_OBJECTS && strlen(objects[i].type) > 0) {
        ;
    }*/
    return 0;
}
