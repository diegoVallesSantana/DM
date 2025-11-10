#include "config.h"
#include "datamgr.h"

extern dplist_t *sensor_list;

/**
 *  This method holds the core functionality of your datamgr. It takes in 2 file pointers to the sensor files and parses them.
 *  When the method finishes all data should be in the internal pointer list and all log messages should be printed to stderr.
 *  \param fp_sensor_map file pointer to the map file
 *  \param fp_sensor_data file pointer to the binary data file
 */
void datamgr_parse_sensor_files(FILE *fp_sensor_map, FILE *fp_sensor_data){

    if (fp_sensor_map == NULL || fp_sensor_data == NULL) {
        fprintf(stderr, "Error: invalid file pointers to datamgr_parse_sensor_files.\n");
        return;
    }

    // 1. Parse the room-sensor mapping file (text)
    uint16_t room_id, sensor_id;
    while (fscanf(fp_sensor_map, "%hu %hu", &room_id, &sensor_id) == 2) {
        sensor_node_t *node = malloc(sizeof(sensor_node_t));

        if (!node) {
            fprintf(stderr, "Error: memory allocation failed for sensor node.\n");
            exit(EXIT_FAILURE);
        }

        node->id = sensor_id;
        node->room = room_id;
        node->running_avg = 0.0;
        node->value = 0.0;
        node->ts = 0;
        node->history_count = 0;
        node->history_index = 0;

        dpl_insert_at_index(sensor_list, node, dpl_size(sensor_list), false);
    }

    //TODO



}


void datamgr_free(){

    if (sensor_list == NULL) return;

    dpl_free(&sensor_list, true);

    sensor_list = NULL;

}


uint16_t datamgr_get_room_id(sensor_id_t sensor_id){

    if (sensor_list == NULL) {
        ERROR_HANDLER("datamgr_get_room_id: sensor list not initialized");
    }

    int size = dpl_size(sensor_list);
    for (int i = 0; i < size; i++) {
        sensor_data_t *element = dpl_get_element_at_index(sensor_list, i);

        if (element == NULL) continue; // defensive

        if (element->id == sensor_id) {
            return element->room;
        }
    }

    // If we reach here → sensor_id not found → error
    ERROR_HANDLER("datamgr_get_room_id: sensor ID does not exist");

    // Function will never reach here, but avoid compiler warning:
    return 0;
}


sensor_value_t datamgr_get_avg(sensor_id_t sensor_id){
    if (sensor_list == NULL)
    {
        ERROR_HANDLER("datamgr_get_avg: sensor list not initialized");
    }

    dplist_node_t *dummy = sensor_list->head;

    double sum =0.0;

    while (dummy != NULL)
    {
        sensor_data_t *data = (sensor_data_t *) dummy->element;
        if (data->id == sensor_id)
        {
            if(data->history_count < RUN_AVG_LENGTH){return 0;}

            for (int i = 0; i < RUN_AVG_LENGTH; i++)
            {
            sum += data->history[i];
            }
            return sum / RUN_AVG_LENGTH;
        }
        dummy = dummy->next;
    }
    ERROR_HANDLER("datamgr_get_avg: sensor_id not found");
    return 0;
}

/**
 * Returns the time of the last reading for a certain sensor ID
 * Use ERROR_HANDLER() if sensor_id is invalid
 * \param sensor_id the sensor id to look for
 * \return the last modified timestamp for the given sensor
 */
time_t datamgr_get_last_modified(sensor_id_t sensor_id);

/**
 *  Return the total amount of unique sensor ID's recorded by the datamgr
 *  \return the total amount of sensors
 */
int datamgr_get_total_sensors();
