/* raycast.c - provides raycasting functionality */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "include/json.h"

int main() {
    FILE *json = fopen("test_01_good.json", "rb");
    read_json(json);
    print_objects(objects);
    return 0;
}