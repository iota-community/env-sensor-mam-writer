#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define DATA_SIZE 10

typedef enum {
    FEATURE_REQUEST_CMD, DATA_REQUEST_CMD, DATA_RESPONSE_CMD,
    FEATURE_RESPONSE_CMD, SETUP_TEST_CMD, NO_CMD,
} sensor_command_t;

typedef struct {
    float temperature;
    float humidity;
    float atmosphericPressure;
    float pm2_5;
} env_sensor_data_t;

typedef struct {
    env_sensor_data_t data[DATA_SIZE];
    int position;
} env_sensor_data_ring_t;

typedef struct {
    bool hasTemperature;
    bool hasHumidity;
    bool hasAtmosphericPressure;
    bool has2_5;
} env_sensor_features_t;

typedef struct {
    struct sockaddr_in6 address;
} sensor_config_t;

typedef struct {
    sensor_config_t config;
    env_sensor_features_t features;
    env_sensor_data_ring_t data_ring;
} sensor_node_t;