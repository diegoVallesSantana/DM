#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>

#include "config.h"
#include "lib/dplist.h"
#include "datamgr.h"

int main(){
    printf("Hello World\n");

    FILE * map = fopen("room_sensor.map", "r");
    FILE * data = fopen("sensor_data", "rb");

    if(map == NULL || data == NULL) {
    fprintf(stderr, "Error opening input files.\n");
    return EXIT_FAILURE;
    }

    datamgr_parse_sensor_files(map, data);
    fclose(map);
    fclose(data);
    datamgr_free();
    return 0;

}