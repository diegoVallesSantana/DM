#include "lib/dplist.h"
#include "config.h"
#include "datamgr.h"

static void element_free(void **element) {
    free(*element);
    *element = NULL;
}

dplist_t *sensor_list = NULL;

void datamgr_parse_sensor_files(FILE *fp_sensor_map, FILE *fp_sensor_data){
    if (sensor_list == NULL)
    {
        sensor_list = dpl_create(NULL, element_free, NULL);
    }


    if (fp_sensor_map == NULL || fp_sensor_data == NULL) {
        fprintf(stderr, "Error: invalid file pointers to datamgr_parse_sensor_files.\n");
        return;
    }

    uint16_t room_id, sensor_id;
    while (fscanf(fp_sensor_map, "%hu %hu", &room_id, &sensor_id) == 2) {
        sensor_data_t *node = malloc(sizeof(sensor_data_t));

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

    uint16_t id;
    double temp;
    time_t ts;


    while ( fread(&id, sizeof(uint16_t), 1, fp_sensor_data) == 1 &&
        fread(&temp, sizeof(double),   1, fp_sensor_data) == 1 &&
        fread(&ts, sizeof(time_t),     1, fp_sensor_data) == 1 ) {

        dplist_node_t *dummy = sensor_list->head;
        bool found = false;
        while (dummy != NULL)
        {
            sensor_data_t *data = (sensor_data_t *) dummy->element;
            if (data->id == id)
            {
                found = true;
                data->value = temp;
                data->ts = ts;
                data->history[data->history_index] = temp;
                data->history_index = (data->history_index + 1) % RUN_AVG_LENGTH;
                if (data->history_count < RUN_AVG_LENGTH) data->history_count++;

                double sum = 0;
                if (data->history_count == RUN_AVG_LENGTH) {
                    for (int i = 0; i < RUN_AVG_LENGTH; i++) {
                        sum += data->history[i];
                    }
                    data->running_avg = sum / RUN_AVG_LENGTH;

                    if (data->running_avg <SET_MIN_TEMP){
                    fprintf(stderr, "Sensor %hu in room %hu is too cold\n", id, data->room);}
                    if (data->running_avg >SET_MAX_TEMP){
                    fprintf(stderr, "Sensor %hu in room %hu is too hot\n", id, data->room);}
                }
                else{data->running_avg = 0.0;}
                break;
            }
            dummy = dummy->next;
        }
        if (!found) {fprintf(stderr, "Warning: sensor %hu not in map\n", id);}
    }

}


void datamgr_free(){
    if (sensor_list == NULL) return;

    dpl_free(&sensor_list, true);

    sensor_list = NULL;
}


uint16_t datamgr_get_room_id(sensor_id_t sensor_id){

    ERROR_HANDLER(sensor_list == NULL,"datamgr_get_room_id: sensor list not initialized");

    int size = dpl_size(sensor_list);
    for (int i = 0; i < size; i++) {
        sensor_data_t *element = dpl_get_element_at_index(sensor_list, i);

        if (element == NULL) continue; // defensive

        if (element->id == sensor_id) {
            return element->room;
        }
    }

    ERROR_HANDLER(1,"datamgr_get_room_id: sensor ID does not exist");

    return 0;
}


sensor_value_t datamgr_get_avg(sensor_id_t sensor_id){
    if (sensor_list == NULL)
    {
        ERROR_HANDLER(1,"datamgr_get_avg: sensor list not initialized");
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
    ERROR_HANDLER(1,"datamgr_get_avg: sensor_id not found");
    return 0;
}

time_t datamgr_get_last_modified(sensor_id_t sensor_id)
{
    if (sensor_list == NULL)
    {
        ERROR_HANDLER(1,"datamgr_get_last_modified: sensor list not initialized");
    }

    dplist_node_t *dummy = sensor_list->head;

    while (dummy != NULL)
    {
        sensor_data_t *data = (sensor_data_t *) dummy->element;
        if (data->id == sensor_id)
        {
            return data->ts;
        }
        dummy = dummy->next;
    }
    ERROR_HANDLER(1,"datamgr_get_last_modified: sensor_id not found");
    return 0;
}

int datamgr_get_total_sensors(){
    if (sensor_list == NULL)
    {
        ERROR_HANDLER(1,"datamgr_get_total_sensors: sensor list not initialized");
    }
    return dpl_size(sensor_list);
}

