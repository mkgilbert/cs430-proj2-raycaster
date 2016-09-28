/* raycast.c - provides raycasting functionality */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "include/json.h"
#include "include/vector_math.h"
#include "include/raycast.h"
#include "include/ppmrw.h"

int plane_intersection() {
    return -1;
}

int sphere_intersection() {
    return -1;
}

void raycast_scene(image img, object *objects) {
    // loop over all pixels and test for intesections with objects.
    // store results in pixmap
    int x;  // x coord iterator
    int y;  // y coord iterator
    int i;  // object iterator

}

int main(int argc, char *argv[]) {
    // TODO: error checking for file
    FILE *json = fopen(argv[1], "rb");
    read_json(json);
    //print_objects(objects);

    // loop through objects in the scene and do raycasting
    /*int i = 0;
    while (i < MAX_OBJECTS && strlen(objects[i].type) > 0) {
        ;
    }*/
    return 0;
}
