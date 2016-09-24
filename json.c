/** Example
// Use railroad diagram to determine how to parse

//#define DEBUG
**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int line = 1; // global var for line numbers as we parse

// Palmer's refactored parts
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
        fprintf(stderr, "Error: unepected EOF: %d\n", line);
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
    if (val == 0 || val == EOF) {
        fprintf(stderr, "Error: Expected a number: %d\n", line);
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
    skip_ws(json);
    return v;
}
// end


char* parse_string(FILE *json) {
    skip_ws(json);
    int c = next_c(json);
    if (c != '"') {
        fprintf(stderr, "Error: Expected string but found something else: %d\n", line);
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

    // find the objects
    while (1) {
        //c  = next_c(json);
        if (c == ']') {
            fprintf(stderr, "Error: read_json: Unexpected ']': %d\n", line);
            fclose(json);
            return;
        }
        if (c == '{') {
           // Parse the object... 
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
            printf("type is '%s'\n", type);
            if (strcmp(type, "camera") == 0) {
                printf("found camera...\n");
                // TODO: allocate camera space
                // TODO: store data
            }
            else if (strcmp(type, "sphere") == 0) {
                printf("found sphere...\n");
            }
            else if (strcmp(type, "plane") == 0) {
                printf("found plane...\n"); 
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
                    if ((strcmp(key, "width") == 0) ||
                            (strcmp(key, "height") == 0) ||
                            (strcmp(key, "radius") == 0)) {
                        double value = next_number(json); 
                    }
                    else if ((strcmp(key, "color") == 0) ||
                            (strcmp(key, "position") == 0) ||
                            (strcmp(key, "normal") == 0)) {
                        // need to return multiple values
                        double* value = next_vector(json); 
                    }
                    else {
                        // error
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
    }
    fclose(json);
}

int main(int argc, char *argv[]) {
    FILE *json = fopen(argv[1], "rb");
    read_json(json);
    return 0;
}
