/* raycast.c - provides raycasting functionality */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "include/json.h"
#include "include/vector_math.h"
#include "include/raycast.h"

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
