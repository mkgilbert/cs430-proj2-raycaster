/* main program entry point */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "include/json.h"
#include "include/vector_math.h"
#include "include/raycast.h"
#include "include/ppmrw.h"

/* example usage: raycast width height input.json out.ppm */
int main(int argc, char *argv[]) {
    // TODO: error checking for file
    /* testing that we can read json objects */
    if (argc != 5) {
        fprintf(stderr, "Error: main: You must have 4 arguments\n");
        exit(1);
    }
    if (atoi(argv[1]) <= 0 || atoi(argv[2]) <= 0) {
        fprintf(stderr, "Error: main: width and height parameters must be > 0\n");
        exit(1);
    }

    FILE *json = fopen(argv[3], "rb");
    if (json == NULL) {
        fprintf(stderr, "Error: main: Failed to open input file '%s'\n", argv[3]);
        exit(1);
    }

    read_json(json); // this sends info to a global array of objects

    /* create image */
    image img;
    img.width = atoi(argv[1]);
    img.height = atoi(argv[2]);
    img.pixmap = (RGBPixel*) malloc(sizeof(RGBPixel)*img.width*img.height);
    int pos = get_camera(objects);

    raycast_scene(&img, objects[pos].cam.width, objects[pos].cam.height, objects);

    FILE *out = fopen(argv[4], "wb");
    if (out == NULL) {
        fprintf(stderr, "Error: main: Failed to create output file '%s'\n", argv[4]);
        exit(1);
    }

    create_ppm(out, 6, &img);
    /* cleanup */
    //fclose(out);
    
    return 0;
}
