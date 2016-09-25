#ifndef JSON_H
#define JSON_H
#endif

#ifdef JSON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_TYPE_STRLEN 7   // maximum length of string expected in object.type
#define MAX_OBJECTS 128     // maximume number of objects supported in json file

// structs to store different types of objects
typedef struct camera_t {
    double width;
    double height;
} camera;

typedef struct sphere_t {
    double *color;
    double *position;
    double radius;
} sphere;

typedef struct plane_t {
    double *color;
    double *position;
    double *normal;
} plane;

// object datatype to store json data
typedef struct object_t {
    char type[MAX_TYPE_STRLEN];
    union {
        camera cam;
        sphere sph;
        plane pln;
    } data;
} object;

// use these to decide which "object" data type to use
typedef enum object_types_t {
    CAMERA,
    SPHERE,
    PLANE
} object_types;

/* global variables */
extern int line;
extern object objects[MAX_OBJECTS];

/* function definitions */
void read_json(FILE *json);
void print_objects(object *obj);

#endif
