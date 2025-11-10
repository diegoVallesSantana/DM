/**
 * \author {Diego Vallés}
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdint.h>
#include <time.h>
#include "lib/dplist.h"
//#include "datamgr.h"

#ifndef RUN_AVG_LENGTH
#define RUN_AVG_LENGTH 5
#endif

#ifndef SET_MAX_TEMP
#define SET_MAX_TEMP 30
#endif

#ifndef SET_MIN_TEMP
#define SET_MIN_TEMP 0
#endif

typedef uint16_t sensor_id_t;
typedef uint16_t room_id_t;
typedef double running_avg_t;
typedef double sensor_value_t;
typedef time_t sensor_ts_t;         // UTC timestamp as returned by time() - notice that the size of time_t is different on 32/64 bit machine

/**
 * structure to hold sensor data
 */
typedef struct {
    sensor_id_t id;         /** < sensor id */
    room_id_t room;
    running_avg_t running_avg;
    sensor_value_t value;   /** < sensor value */
    sensor_ts_t ts;         /** < sensor timestamp */

    double history[RUN_AVG_LENGTH];
    int history_count;
    int history_index;
} sensor_data_t;


#endif /* _CONFIG_H_ */
