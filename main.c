/**
 * \author {Diego Vallés}
 */
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

    // PRINT SUMMARY
    int count = datamgr_get_total_sensors();
    printf("Total sensors: %d\n", count);

    for (int i = 0; i < count; i++) {
    sensor_id_t id = datamgr_get_sensor_id_at_index(i);

    printf("Sensor %hu in room %hu has avg %.2f (last updated: %ld)\n",
           id,
           datamgr_get_room_id(id),
           datamgr_get_avg(id),
           datamgr_get_last_modified(id));
    }
    datamgr_free();
    return 0;

}