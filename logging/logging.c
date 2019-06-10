#include <stdio.h>
#include <netinet/in.h>
#include "sensor/sensor.h"

void log_int(char *level, char *func_name, char *key, int value) {
    printf("%s | %s | int | %s: %i\n", level, func_name, key, value);
}

void log_float(char *level, char *func_name, char *key, float value) {
    printf("%s | %s | int | %s: %f\n", level, func_name, key, value);
}

void log_string(char *level, char *func_name, char *key, char *value) {
    printf("%s | %s | string | %s: %s\n", level, func_name, key, value);
}

void log_hex(char *level, char *func_name, char *key, uint8_t value) {
    printf("%s | %s | hex | %s: 0x%x\n", level, func_name, key, value);
}

void log_sensor_features(char *level, char *func_name, char *key, env_sensor_features_t *sensor_features){
    printf(
            "%s | %s | sensor_features | %s | [ hasTemperature: %i, hasHumidity: %i, hasAtmosphericPressure: %i, has2_5: %i ]\n",
            level, func_name, key, sensor_features->hasTemperature,
            sensor_features->hasHumidity, sensor_features->hasAtmosphericPressure,
            sensor_features->has2_5
            );
}

void log_sensor_data(char *level, char *func_name, char *key, env_sensor_data_t *sensor_data){
    printf(
            "%s | %s | sensor_data | %s | [ temperature: %f, humidity: %f, atomicPressure: %f, pm2_5: %f ]\n",
            level, func_name, key, sensor_data->temperature, sensor_data->humidity,
            sensor_data->atmosphericPressure, sensor_data->pm2_5
            );
}

void log_hex_array(char *level, char *func_name, char *key, uint8_t *value, size_t length) {
    printf("%s | %s | hex_array | %s: [ ", level, func_name, key);
    for (unsigned int i = 0; i < length; i++) {
        if (i == 0) {
            printf("0x%x", value[i]);
        } else {
            printf(", 0x%x", value[i]);
        }
    }
    printf(" ]\n");
}

void log_addr(char *level, char *func_name, char *key, struct sockaddr_in6 *client_addr) {
    printf("%s | %s | ipv6 | %s: ", level, func_name, key);
    for (int i = 0; i < 16; i += 2) {
        printf("%x", client_addr->sin6_addr.s6_addr[i]);
        printf("%x:", client_addr->sin6_addr.s6_addr[i + 1]);
    }
    printf("\n");
}