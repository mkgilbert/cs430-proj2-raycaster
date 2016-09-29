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


int plane_intersection(ray *R, V3 *C) {
    return -1;
}

int sphere_intersection(V3 *Ro, V3 *Rd, V3 *C, double r) {
    double a, b, c;
    a = sqr(Rd[0]) + sqr(Rd[1]) + sqr(Rd[2]);
    b = 2 * (Rd[0]*(Ro[0]-C[0]) + Rd[1]*(Ro[1]-C[1]) + Rd[2]*(Ro[2]-C[2]));

    return -1;
}

void raycast_scene(image *img, object *objects) {
    // loop over all pixels and test for intesections with objects.
    // store results in pixmap
    int x;  // x coord iterator
    int y;  // y coord iterator
    int i;  // object iterator
    double cx = 0;
    double cy = 0;
    double h = 0.7; // these will be set from the camera object
    double w = 0.7;
    
    int M = 20; // pixels width and height of image
    int N = 20;

    double pixheight = h / M;
    double pixwidth = w / N;
    for (int y = 0; y < M; y++) {
        for (int x = 0; x < N; x++) {
            double Ro[3] = {0, 0, 0}; // vector that represents the ray origin
            // Rd = normalize(Pixel - Ro)
            double Rd[3] = {
                cx - (w/2) + pixwidth * (x + 0.5),
                cy - (h/2) + pixheight * (y + 0.5),
                1
            }; // ray direction
            normalize(Rd);

            double best_t = INFINITY;
            for (int i=0; objects[i].type != 0; i++) {
                // we need to run intersection test on each object
                double t = 0;
                switch(objects[i].type) {
                    case 0:
                        printf("no object found\n");
                        break;
                    case CAMERA:
                        break;
                    case SPHERE:
                        break;
                    case PLANE:
                        break
                    default:
                        // Error
                        exit(1);
                }
                if (t > 0 && t < best_t) best_t = t;

            }
            if (best_t > 0 && best_t != INFINITY) {// there was an intersection
                printf("#");    // ascii ray tracer "hit"
            }
            else {
                printf(".");    // ascii ray tracer "miss"
            }
        }
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    // TODO: error checking for file
    FILE *json = fopen(argv[1], "rb");
    read_json(json);
    print_objects(objects);

    // loop through objects in the scene and do raycasting
    /*int i = 0;
    while (i < MAX_OBJECTS && strlen(objects[i].type) > 0) {
        ;
    }*/
    return 0;
}
