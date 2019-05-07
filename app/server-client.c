#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#include "pthread.h"
#include <unistd.h>

#include "encode/decode.h"
#include "encode/encode.h"
#include "logging/logging.h"

#include "pb_common.h"
#include "pb_decode.h"
#include "pb_encode.h"
#include "proto_compiled/DataResponse.pb.h"
#include "proto_compiled/DataRequest.pb.h"
#include "proto_compiled/FeatureResponse.pb.h"

#define DATA_SIZE 100
#define SOCKET_BUFFER_SIZE 1024
#DEBUG_SERVER true

typedef enum {
    FEATURE_REQUEST_CMD, DATA_REQUEST_CMD, DATA_RESPONSE_CMD,
    FEATURE_RESPONSE_CMD, SETUP_TEST_CMD, NO_CMD,
} sensor_command_t;

typedef struct {
    float temperature;
    float humanity;
    float atomicPressure;
    float pm2_5;
} env_sensor_data_t;

typedef struct {
    env_sensor_data_t data[DATA_SIZE];
    int position;
} env_sensor_data_ring_t;

typedef struct {
    bool hasTemperature;
    bool hasHumanity;
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

#define SENSOR_NODES_LENGTH 1

sensor_node_t sensor_nodes[SENSOR_NODES_LENGTH] = {
        {
            .config = {
                    .address = {
                            .sin6_addr = {
                                    0xfe, 0x80, 0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00,
                                    0x02, 0x13, 0xaf, 0xff,
                                    0xfe, 0x94, 0x0d, 0x75
                                    },
                            .sin6_scope_id = 0x20,
                            .sin6_port = 51037
                    }
            },
        }
};

int sock;
bool client_is_running = true;

sensor_command_t get_rpc_sensor_command(uint8_t byte) {
    switch(byte){
        case 33:
            return FEATURE_REQUEST_CMD;
        case 34:
            return DATA_REQUEST_CMD;
        case 35:
            return DATA_RESPONSE_CMD;
        case 36:
            return FEATURE_RESPONSE_CMD;
        case 88:
            return SETUP_TEST_CMD;
        default:
            return NO_CMD;
    }
}

uint8_t get_rpc_command_byte(sensor_command_t command) {
    switch(command){
        case FEATURE_REQUEST_CMD:
            return 33;
        case DATA_REQUEST_CMD:
            return 34;
        case DATA_RESPONSE_CMD:
            return 35;
        case FEATURE_RESPONSE_CMD:
            return 36;
        case SETUP_TEST_CMD:
            return 88;
        default:
            return 0;
    }
}

void client_stop(void) {
    client_is_running = false;
}

void clear_socket_buffer(uint8_t *socket_buffer){
    memset(socket_buffer, 0, SOCKET_BUFFER_SIZE);
}

void send_buffer(
        int sock, struct sockaddr_in6 *server_addr_ptr, uint8_t *encode_buffer, int encode_buffer_length) {
    if (DEBUG_SERVER) {
        log_addr("DEBUG", "send_command", "server_addr", server_addr_ptr);
    }

    sendto(
            sock, encode_buffer, encode_buffer_length, 0,
            (struct sockaddr *) server_addr_ptr, sizeof(struct sockaddr_in6));
}

int get_env_sensor_rpc_command_name(char *result, sensor_command_t command) {
    switch (command) {
        case FEATURE_REQUEST_CMD:
            strcat(result, "FEATURE_REQUEST_CMD");
            break;
        case DATA_REQUEST_CMD:
            strcat(result, "DATA_REQUEST_CMD");
            break;
        case DATA_RESPONSE_CMD:
            strcat(result, "DATA_RESPONSE_CMD");
            break;
        case FEATURE_RESPONSE_CMD:
            strcat(result, "DATA_RESPONSE_CMD");
            break;
        default:
            strcat(result, "NONE_CMD");
    }

    return 0;
}

bool check_ip_address(uint8_t *ip_address, uint8_t *ip_address_to_match) {
    for(int i = 0; i < 16; i++){
        if(ip_address[i] != ip_address_to_match[i]){
            return false;
        }
    }
    return true;
}

int get_data_ring_position(env_sensor_data_ring_t *data_ring) {
    int current_index = data_ring->position;

    if(current_index + 1 == DATA_SIZE){
        sensor_nodes[i].data_ring.position = 0;
        return 0;
    }else{
        data_ring->position = current_index + 1;
        return data_ring->position;
    }
}

void add_to_data_ring(env_sensor_data_ring_t *data_ring, env_sensor_data_t *sensor_data) {
    int position = get_data_ring_position(data_ring);
    memcpy(data_ring->data + position, sensor_data, sizeof(env_sensor_data_t));
}

float get_scaled_value(environmentSensors_SingleDataPoint *data_point) {
    return data_point->value / data_point->scale;
}

void handle_data_response(struct sockaddr_in6 *server_addr_ptr, uint8_t *socket_buffer_ptr, int buffer_length) {
    environmentSensors_DataResponse data_response;

    int decoded_length = env_sensor_data_response_decode(&data_response, socket_buffer_ptr, buffer_length);

    for(int i = 0; i < SENSOR_NODES_LENGTH; i++) {
        if(check_ip_address(sensor_nodes[i].config.address.sin6_addr, server_addr_ptr.sin6_addr)) {
            env_sensor_data_t sensor_data = {
                    .humanity = get_scaled_value(&data_response.humanity),
                    .temperature = get_scaled_value(&data_response.temperature),
                    .pm2_5 = get_scaled_value(&data_response.pm2_5),
                    .atomicPressure = get_scaled_value(&data_response.atmosphericPressure),
                    };
            add_to_data_ring(&sensor_nodes[i].data_ring, &sensor_data);
        }
    }
}

void handle_feature_response(struct sockaddr_in6 *server_addr_ptr, uint8_t *socket_buffer_ptr, int buffer_length) {
    environmentSensors_FeatureResponse feature_response;

    env_sensor_feature_response_decode(&feature_response, socket_buffer_ptr, buffer_length);

    for(int i = 0; i < SENSOR_NODES_LENGTH; i++) {
        if(check_ip_address(sensor_nodes[i].config.address.sin6_addr.s6_addr, server_addr_ptr.sin6_addr.s6_addr)) {
            sensor_nodes[i].features.hasAtmosphericPressure = feature_response.hasAtmosphericPressure;
            sensor_nodes[i].features.hasHumanity = feature_response.hasHumanity;
            sensor_nodes[i].features.hasTemperature = feature_response.hasTemperature;
            sensor_nodes[i].features.has2_5 = feature_response.hasPm2_5;
        }
    }
}

void handle_incoming_message(
        int sock, struct sockaddr_in6 *server_addr_ptr,
        uint8_t *socket_buffer_ptr, int socket_buffer_length) {
    char func_name[] = "handle_incoming_message";

    sensor_command_t command = get_rpc_sensor_command(socket_buffer_ptr[0]);

    if (DEBUG_SERVER) {
        log_hex("DEBUG", func_name, "hex_command", (uint8_t) socket_buffer_ptr[0]);
        char command_name[30];
        get_env_sensor_rpc_command_name(command_name, command);
        log_string("DEBUG", func_name, "command_name", command_name);
        log_int("DEBUG", func_name, "server_port", ntohs(server_addr_ptr->sin6_port));
        log_addr("DEBUG", func_name, "server_addr", server_addr_ptr);
    }

    switch(command) {
        case FEATURE_RESPONSE_CMD:
            handle_feature_response(server_addr_ptr, &socket_buffer_ptr[1], socket_buffer_length - 1);
            break;
        case DATA_RESPONSE_CMD:
            handle_data_response(server_addr_ptr, &socket_buffer_ptr[1], socket_buffer_length - 1);
            break;
        default:
            break;
    }

}

void start_listening(void) {
    unsigned int server_addr_len = sizeof(struct sockaddr_in6);
    struct sockaddr_in6 server_addr;
    int socket_buffer_length;

    uint8_t socket_buffer[SOCKET_BUFFER_SIZE];

    while (client_is_running) {
        int length = recvfrom(sock, socket_buffer, sizeof(socket_buffer) - 1, 0,
                              (struct sockaddr *) &server_addr,
                              &server_addr_len);

        if (length < 0) {
            log_int("ERROR", "server_start_listening", "recvfrom", length);
            break;
        }
        socket_buffer[length] = '\0';
        socket_buffer_length = length;

        if (client_is_running) {
            handle_incoming_message(sock, &server_addr, socket_buffer, socket_buffer_length);
        } else {
            clear_socket_buffer(socket_buffer);
        }
    }
}

void *run_receiver_thread(void *args) {
    (void) args;

    start_listening();
    int value = 0;
    close(sock);
    pthread_exit(&value);
}

void start_sending(void) {
    uint8_t command = get_rpc_command_byte(FEATURE_REQUEST_CMD);
    for(int i = 0; i < SENSOR_NODES_LENGTH; i++) {
        struct sockaddr_in6 * address = &sensor_nodes[i].config.address;
        send_buffer(sock, address, &command, 1);
    }

    command = get_rpc_command_byte(DATA_REQUEST_CMD);
    while(client_is_running){
        sleep(10);
        for(int i = 0; i < SENSOR_NODES_LENGTH; i++) {
            struct sockaddr_in6 *address = &sensor_nodes[i].config.address;
            send_buffer(sock, address, &command, 1);
        }
    }
}

void *run_send_thread(void *args) {
    (void) args;

    start_sending();
    int value = 0;
    close(sock);
    pthread_exit(&value);
}


pthread_t send_thread;
pthread_t listing_thread;
int main(void){
    if (listing_thread > 0) {
        puts("Server is already running.");
    } else {
        listing_thread = pthread_create(&listing_thread, NULL, &run_receiver_thread, NULL);
    }
    return 0;
}