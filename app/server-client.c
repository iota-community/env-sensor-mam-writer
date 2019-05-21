#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <net/if.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <math.h>

#include "pthread.h"
#include <unistd.h>

#include "encode/decode.h"
#include "encode/encode.h"
#include "logging/logging.h"

#include "iota/send-msg.h"

#include "pb_common.h"
#include "pb_decode.h"
#include "pb_encode.h"
#include "proto_compiled/DataResponse.pb.h"
#include "proto_compiled/DataRequest.pb.h"
#include "proto_compiled/FeatureResponse.pb.h"

//tmp
#include <errno.h>

#define IOTA_HOST "node05.iotatoken.nl"
#define IOTA_PORT 16265
#define IOTA_SEED "RTYNARMDLBLMOWRFCEEFMJFFLCTAHWEBKSPJQGHMHCSQPVUI9NDZMJITSGAZOGRGHGZRKSPCPWAWTVPXA"


#define SOCKET_BUFFER_SIZE 1024
#define DEBUG_SERVER true
#define PORT 8085

#define MAM_ENCODE_BUFFER 2048

//USB Stick
#define CLIENT_ADDRESS { 0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x1a, 0x7d, 0xff, 0xfe, 0xda, 0x71, 0x13 }

// Integrated BLE
//#define CLIENT_ADDRESS { 0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf2, 0xd5, 0xbf, 0xff, 0xfe, 0x10, 0xf1, 0xb1 };

#define SENSOR_NODES_LENGTH 1

sensor_node_t sensor_nodes[SENSOR_NODES_LENGTH];

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


void init_sensor_config(void) {
    sensor_node_t *node = &sensor_nodes[0];
    uint8_t address[16] = {
            0xfe, 0x80, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x02, 0x13, 0xaf, 0xff,
            0xfe, 0x94, 0x0d, 0x75
    };

    memcpy(&node->config.address.sin6_addr, address, 16);
    node->config.address.sin6_family = AF_INET6;
    node->config.address.sin6_scope_id = 0x00;
    node->config.address.sin6_port = htons(51037);
}

struct sockaddr_in6 client_addr;
void init_client(void) {
    unsigned int client_addr_len = sizeof(client_addr);
    memset(&client_addr, 0, client_addr_len);
    client_addr.sin6_family = AF_INET6;
    client_addr.sin6_port = htons(PORT);
    client_addr.sin6_scope_id = if_nametoindex("bt0");
    uint8_t address[16] = CLIENT_ADDRESS;

    memcpy(&client_addr.sin6_addr, address, 16);

    if (DEBUG_SERVER) {
        log_addr("DEBUG", "client_init", "client_addr", &client_addr);
        log_int("DEBUG", "client_init", "client_port", ntohs(client_addr.sin6_port));
        log_hex("DEBUG", "client_init", "client_scope_id", client_addr.sin6_scope_id);
    }

    if ((sock = socket(AF_INET6, SOCK_DGRAM, 0)) < 0) {
        log_int("ERROR", "client_init", "server_socket", sock);
    }

    if (bind(sock, (struct sockaddr *) &client_addr, client_addr_len) < 0) {
        log_string("ERROR", "client_init", "bind_client_socket", "bind failed");
        fprintf(stderr, "socket() failed: %s\n", strerror(errno));
        exit(1);
    }

    init_sensor_config();
}

void client_stop(void) {
    client_is_running = false;
}

void clear_socket_buffer(uint8_t *socket_buffer){
    memset(socket_buffer, 0, SOCKET_BUFFER_SIZE);
}

void send_buffer(struct sockaddr_in6 *server_addr_ptr, uint8_t *encode_buffer, int encode_buffer_length) {
    if (DEBUG_SERVER) {
        log_int("DEBUG", "send_command", "command", encode_buffer[0]);
        log_int("DEBUG", "send_command", "buffer_length", encode_buffer_length);
        log_addr("DEBUG", "send_command", "server_addr", server_addr_ptr);
        log_int("DEBUG", "send_command", "server_port", ntohs(server_addr_ptr->sin6_port));
    }

    sendto(
            sock, encode_buffer, encode_buffer_length, 0,
            (struct sockaddr *) server_addr_ptr, sizeof(struct sockaddr_in6));
}

int get_env_sensor_rpc_command_name(char *result, sensor_command_t command) {
    switch (command) {
        case FEATURE_REQUEST_CMD:
            strcat(result, "FEATURE_REQUEST_CMD\0");
            break;
        case DATA_REQUEST_CMD:
            strcat(result, "DATA_REQUEST_CMD\0");
            break;
        case DATA_RESPONSE_CMD:
            strcat(result, "DATA_RESPONSE_CMD\0");
            break;
        case FEATURE_RESPONSE_CMD:
            strcat(result, "DATA_RESPONSE_CMD\0");
            break;
        default:
            strcat(result, "NONE_CMD\0");
            break;
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

float get_scaled_value(environmentSensors_SingleDataPoint *data_point) {
    if(data_point->value == 0){
        return 0;
    }else{
        return data_point->value / pow(10, -data_point->scale);
    }
}

int get_data_ring_position(env_sensor_data_ring_t *data_ring) {
    int current_index = data_ring->position;

    if(current_index == DATA_SIZE){
        data_ring->position = 0;
        return 0;
    }else{
        data_ring->position = current_index + 1;
        return current_index;
    }
}

void add_to_data_ring(env_sensor_data_ring_t *data_ring, env_sensor_data_t *sensor_data) {
    int position = get_data_ring_position(data_ring);
    if(DEBUG_SERVER){
        log_int("DEBUG", "add_to_data_ring", "position", position);
        log_sensor_data("DEBUG", "add_to_data_ring", "sensor_data", sensor_data);
    }
    //log_int("DEBUG", "add_", "temperature", get_scaled_value(&data_ring->data[0]));
    memcpy(&data_ring->data[position], sensor_data, sizeof(env_sensor_data_t));
}

uint8_t mam_encode_buffer[MAM_ENCODE_BUFFER];
void clear_mam_buffer(){
    memset(mam_encode_buffer, NULL, MAM_ENCODE_BUFFER);
}

void send_to_tangle(environmentSensors_DataResponse *data_response) {
    clear_mam_buffer();
    size_t encoded_size = env_sensor_data_response_encode(&mam_encode_buffer, MAM_ENCODE_BUFFER, data_response);
    mam_encode_buffer[encoded_size + 1] = '\0';
    log_hex_array("DEBUG", "send_to_tangle", "mam_buffer", &mam_encode_buffer, encoded_size +1);
    mam_send_message(IOTA_HOST, IOTA_PORT, IOTA_SEED, &mam_encode_buffer, encoded_size +1, true);
}

void handle_data_response(struct sockaddr_in6 *server_addr_ptr, uint8_t *socket_buffer_ptr, int buffer_length) {
    environmentSensors_DataResponse data_response;

    env_sensor_data_response_decode(&data_response, socket_buffer_ptr, buffer_length);

    for(int i = 0; i < SENSOR_NODES_LENGTH; i++) {
        if(check_ip_address(sensor_nodes[i].config.address.sin6_addr.s6_addr, server_addr_ptr->sin6_addr.s6_addr)) {
            if(DEBUG_SERVER){
                log_float("DEBUG", "handle_data_response", "temperature", get_scaled_value(&data_response.temperature));
            }

            env_sensor_data_t sensor_data = {
                    .humanity = get_scaled_value(&data_response.humanity),
                    .temperature = get_scaled_value(&data_response.temperature),
                    .pm2_5 = get_scaled_value(&data_response.pm2_5),
                    .atmosphericPressure = get_scaled_value(&data_response.atmosphericPressure),
                    };
            send_to_tangle(&data_response);
            add_to_data_ring(&sensor_nodes[i].data_ring, &sensor_data);
        }
    }
}

void handle_feature_response(struct sockaddr_in6 *server_addr_ptr, uint8_t *socket_buffer_ptr, int buffer_length) {
    environmentSensors_FeatureResponse feature_response;

    env_sensor_feature_response_decode(&feature_response, socket_buffer_ptr, buffer_length);

    for(int i = 0; i < SENSOR_NODES_LENGTH; i++) {
        if(check_ip_address(sensor_nodes[i].config.address.sin6_addr.s6_addr, server_addr_ptr->sin6_addr.s6_addr)) {
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
        char command_name[20];
        memset(command_name, 0, 20);
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
    unsigned int client_addr_len = sizeof(struct sockaddr_in6);
    int socket_buffer_length;

    uint8_t socket_buffer[SOCKET_BUFFER_SIZE];

    while (client_is_running) {
        int length = recvfrom(sock, socket_buffer, SOCKET_BUFFER_SIZE - 1, 0,
                              (struct sockaddr *) &client_addr,
                              &client_addr_len);

        if (length < 0) {
            log_int("ERROR", "server_start_listening", "recvfrom", length);
            break;
        }
        socket_buffer[length] = '\0';
        socket_buffer_length = length;

        if (client_is_running) {
            handle_incoming_message(sock, &client_addr, socket_buffer, socket_buffer_length);
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
        send_buffer(address, &command, 1);
    }

    command = get_rpc_command_byte(DATA_REQUEST_CMD);
    while(client_is_running){
        sleep(5);
        for(int i = 0; i < SENSOR_NODES_LENGTH; i++) {
            struct sockaddr_in6 *address = &sensor_nodes[i].config.address;
            send_buffer(address, &command, 1);
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

void node_status_report(sensor_node_t *node){
    char func_name[] = "node_status_report";
    char level[] = "DEBUG";
    if(DEBUG_SERVER){
        log_addr(level, func_name, "sensor_address", &node->config.address);
    }

    for(int i = 0; i < DATA_SIZE;i++){
        log_sensor_data(level, func_name, "sensor_data", &node->data_ring.data[i]);
    }
}

void start_status_report(void) {
    while(client_is_running){
        sleep(10);
        for(int i = 0; i < SENSOR_NODES_LENGTH; i++) {
            node_status_report(&sensor_nodes[i]);
        }
    }
}

void *run_status_thread(void *args) {
    (void) args;

    start_status_report();
    int value = 0;
    close(sock);
    pthread_exit(&value);
}

pthread_t status_thread;
pthread_t send_thread;
pthread_t listing_thread;
int main(void){
    init_client();
    if (listing_thread > 0) {
        puts("Server is already running.");
    } else {
        listing_thread = pthread_create(&listing_thread, NULL, &run_receiver_thread, NULL);
        send_thread = pthread_create(&send_thread, NULL, &run_send_thread, NULL);
        status_thread = pthread_create(&status_thread, NULL, &run_status_thread, NULL);
    }
    while(client_is_running){}
    return 0;
}