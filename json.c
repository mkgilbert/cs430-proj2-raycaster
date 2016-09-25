/** json.c parses json files for view objects
// Use railroad diagram to determine how to parse
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "include/json.h"


int line = 1;                   // global var for line numbers as we parse
object objects[MAX_OBJECTS];    // allocate space for all objects in json file

// next_c wraps the getc function that provides error checking and line #
// Problem: if we do ungetc, it could screw us up on the line #
int next_c(FILE* json) {
    int c = fgetc(json);
#ifdef DEBUG
    printf("next_c: '%c'\n", c);
#endif
    if (c == '\n') {
        line++;;
    }
    if (c == EOF) {
        fprintf(stderr, "Error: next_c: Unexpected EOF: %d\n", line);
        exit(1);
    }
    return c;
}

void skip_ws(FILE *json) {
    int c = next_c(json);
    while (isspace(c)) {
        c = next_c(json);
    }
    if (c == '\n')
        line--;         // we backed up to the previous line
    ungetc(c, json);    // move back one character (instead of fseek)
}

void expect_c(FILE* json, int d) {
    int c = next_c(json);
    if (c == d) return;
    fprintf(stderr, "Error: Expected '%c': %d\n", d, line);
    exit(1);
}

double next_number(FILE* json) {
    double val;
    fscanf(json, "%lf", &val);
    if (val == EOF) {
        fprintf(stderr, "Error: Expected a number but found EOF: %d\n", line);
        return -1;;
    }
    printf("next_number: %lf\n", val);
    return val;
}

double* next_vector(FILE* json) {
    double* v = malloc(sizeof(double)*3);
    expect_c(json, '[');
    skip_ws(json);
    v[0] = next_number(json);
    skip_ws(json);
    expect_c(json, ',');
    skip_ws(json);
    v[1] = next_number(json);
    skip_ws(json);
    expect_c(json, ',');
    skip_ws(json);
    v[2] = next_number(json);
    skip_ws(json);
    expect_c(json, ']');
    return v;
}
// end


char* parse_string(FILE *json) {
    skip_ws(json);
    int c = next_c(json);
    if (c != '"') {
        fprintf(stderr, "Error: Expected beginning of string but found '%c': %d\n", c, line);
        exit(1); // not a string
    }
    c = next_c(json); // should be first char in the string
    char buffer[128]; // strings are gauranteed to be 128 or less
    int i = 0;
    while (c != '"') {
        if (isspace(c)) {
            continue;
        }
        buffer[i] = c;
        i++;
        c = next_c(json);
    }
    buffer[i] = 0;
    // gonna need more error checking for invalid strings!!!
    //
    return strdup(buffer); // returns a malloc'd version of buffer
}

void read_json(FILE *json) {
    printf("reading json file...\n");
    //read in data from file
    // expecting square bracket but we need to get rid of whitespace
    skip_ws(json);
    
    // find beginning of the list
    int c  = next_c(json);
    if (c != '[') {
        fprintf(stderr, "Error: read_json: JSON file must begin with [\n");
        exit(1);
    }
    skip_ws(json);
    c = next_c(json);

    // check if file empty
    if (c == ']' || c == EOF) {
        fprintf(stderr, "Error: read_json: Empty json file\n");
        exit(1);
    }
    skip_ws(json);

    int counter = 0;

    // find the objects
    while (1) {
        //c  = next_c(json);
        if (counter > MAX_OBJECTS) {
            fprintf(stderr, "Error: read_json: Number of objects is too large: %d\n", line);
            exit(1);
        }
        if (c == ']') {
            fprintf(stderr, "Error: read_json: Unexpected ']': %d\n", line);
            fclose(json);
            return;
        }
        if (c == '{') {     // found an object
            printf("parsing object...\n");
            skip_ws(json);
            char *key = parse_string(json);
            if (strcmp(key, "type") != 0) {
                fprintf(stderr, "Error: read_json: First key of an object must be 'type': %d\n", line); 
                exit(1);
            }
            skip_ws(json);
            // get the colon
            expect_c(json, ':');
            skip_ws(json);

            char *type = parse_string(json);
            object_types obj_type;
            printf("type is '%s'\n", type);
            if (strcmp(type, "camera") == 0) {
                printf("found camera...\n");
                obj_type = CAMERA;
                strcpy(objects[counter].type, "camera");
            }
            else if (strcmp(type, "sphere") == 0) {
                printf("found sphere...\n");
                obj_type = SPHERE;
                strcpy(objects[counter].type, "sphere");
            }
            else if (strcmp(type, "plane") == 0) {
                printf("found plane...\n"); 
                obj_type = PLANE;
                strcpy(objects[counter].type, "plane");
            }
            else {
                exit(1);
            }

            skip_ws(json);
            
            while (1) {
                //  , }
                c = next_c(json);
                if (c == '}') {
                    // stop parsing this object
                    break;
                }
                else if (c == ',') {
                    // read another field
                    skip_ws(json);
                    char* key = parse_string(json);
                    skip_ws(json);
                    expect_c(json, ':');
                    skip_ws(json);
                    if (strcmp(key, "width") == 0) {
                        objects[counter].data.cam.width = next_number(json);
                    }
                    else if (strcmp(key, "height") == 0) {
                        objects[counter].data.cam.height = next_number(json);
                    }
                    else if (strcmp(key, "radius") == 0) {
                        objects[counter].data.sph.radius = next_number(json); 
                    }
                    else if (strcmp(key, "color") == 0) {
                        if (obj_type == SPHERE)
                            objects[counter].data.sph.color = next_vector(json);
                        else if (obj_type == PLANE)
                            objects[counter].data.pln.color = next_vector(json);
                        else {
                            fprintf(stderr, "Error: read_json: Color vector can't be applied here: %d\n", line);
                            exit(1);
                        }
                    }
                    else if (strcmp(key, "position") == 0) {
                        if (obj_type == SPHERE)
                            objects[counter].data.sph.position = next_vector(json);
                        else if (obj_type == PLANE)
                            objects[counter].data.pln.position = next_vector(json);
                        else {
                            fprintf(stderr, "Error: read_json: Position vector can't be applied here: %d\n", line);
                            exit(1);
                        }
                        
                    }
                    else if (strcmp(key, "normal") == 0) {
                        if (obj_type != PLANE) {
                            fprintf(stderr, "Error: read_json: Normal vector can't be applied here: %d\n", line);
                            exit(1);
                        }
                        else
                            objects[counter].data.pln.normal = next_vector(json);
                    }
                    else {
                        fprintf(stderr, "Error: read_json: '%s' not a valid object: %d\n", key, line); 
                        exit(1);
                    }
                    
                    // do something with key and value
                    skip_ws(json);
                }
                else {
                    fprintf(stderr, "Error: read_json: Unexpected value '%c': %d\n", c, line);
                    exit(1);
                }
            }
            skip_ws(json);
            c = next_c(json);
            if (c == ',') {
                // noop
                skip_ws(json);
            }
            else if (c == ']') {
                printf("end of file\n");
                fclose(json);
                return;
            }
            else {
                fprintf(stderr, "Error: read_json: Expecting comma or ]: %d\n", line);
                exit(1);
            }
        }
        c = next_c(json);
        counter++;
    }
    fclose(json);
}

void print_objects(object *obj) {
    int i = 0;
    while (i < MAX_OBJECTS && strlen(obj[i].type) > 0) {
        printf("object type: %s\n", obj[i].type);
        if (strcmp(obj[i].type, "camera") == 0) {
            printf("height: %lf\n", obj[i].data.cam.height);
            printf("width: %lf\n", obj[i].data.cam.width);
        }
        else if (strcmp(obj[i].type, "sphere") == 0) {
            printf("color: %lf %lf %lf\n", obj[i].data.sph.color[0],
                                         obj[i].data.sph.color[1],
                                         obj[i].data.sph.color[2]);
            printf("position: %lf %lf %lf\n", obj[i].data.sph.position[0],
                                            obj[i].data.sph.position[1],
                                            obj[i].data.sph.position[2]);
            printf("radius: %lf\n", obj[i].data.sph.radius);
        }
        else if (strcmp(obj[i].type, "plane") == 0) {
            printf("color: %lf %lf %lf\n", obj[i].data.pln.color[0],
                                         obj[i].data.pln.color[1],
                                         obj[i].data.pln.color[2]);
            printf("position: %lf %lf %lf\n", obj[i].data.pln.position[0],
                                            obj[i].data.pln.position[1],
                                            obj[i].data.pln.position[2]);
            printf("normal: %lf %lf %lf\n", obj[i].data.pln.normal[0],
                                         obj[i].data.pln.normal[1],
                                         obj[i].data.pln.normal[2]);
        }
        else {
            printf("unsupported value\n");
        }
        i++;
    }
    printf("end at i=%d\n", i);
}


int main(int argc, char *argv[]) {
    // testing code
    object c;
    strcpy(c.type, "camera");
    c.data.cam.height = 0.5;
    c.data.cam.width = 0.4;
    printf("object type: %s\n", c.type);
    printf("object height: %lf\n", c.data.cam.height);
    printf("object width: %lf\n", c.data.cam.width);

    FILE *json = fopen(argv[1], "rb");
    read_json(json);
    print_objects(objects);
    return 0;
}
