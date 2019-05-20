#include "sensor/sensor.h"

void log_int(char *level, char *func_name, char *key, int value);

void log_float(char *level, char *func_name, char *key, float value);

void log_string(char *level, char *func_name, char *key, char *value);

void log_hex(char *level, char *func_name, char *key, uint8_t value);

void log_sensor_data(char *level, char *func_name, char *key, env_sensor_data_t *sensor_data);

void log_hex_array(char *level, char *func_name, char *key, uint8_t *value, size_t length);

void log_addr(char *level, char *func_name, char *key, struct sockaddr_in6 *client_addr);