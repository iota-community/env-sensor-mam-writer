#include <stdio.h>
#include <stdint.h>

#include "logging/logging.h"

#include "pb_common.h"
#include "pb_decode.h"

#include "proto_compiled/DataRequest.pb.h"
#include "proto_compiled/DataResponse.pb.h"
#include "proto_compiled/FeatureResponse.pb.h"

//Todo: Use macros to generate debug code.
#define DEBUG_DECODE true

void log_env_sensor_data_request(char *level, char *func_name, bool decode_status,
                                 environmentSensors_DataRequest *message_ptr) {
    log_int(level, func_name, "decode_status", decode_status);
    log_int(level, func_name, "message_ptr->atmosphericPressure", message_ptr->atmosphericPressure);
    log_int(level, func_name, "message_ptr->humidity", message_ptr->humidity);
    log_int(level, func_name, "message_ptr->humidity", message_ptr->temperature);
}

void log_env_sensor_data_response(char *level, char *func_name, bool decode_status,
                                  environmentSensors_DataResponse *message_ptr) {
    log_int(level, func_name, "decode_status", decode_status);
    log_int(level, func_name, "message_ptr->has_atmosphericPressure", message_ptr->has_atmosphericPressure);
    log_int(level, func_name, "message_ptr->has_humidity", message_ptr->has_humidity);
    log_int(level, func_name, "message_ptr->hasTemperature", message_ptr->has_temperature);

    if(message_ptr->has_atmosphericPressure){
        log_int(level, func_name, "message_ptr->atmosphericPressure.value", message_ptr->atmosphericPressure.value);
        log_int(level, func_name, "message_ptr->atmosphericPressure.scale", message_ptr->atmosphericPressure.scale);
    }
    if(message_ptr->has_humidity){
        log_int(level, func_name, "message_ptr->humidity.value", message_ptr->humidity.value);
        log_int(level, func_name, "message_ptr->humidity.scale", message_ptr->humidity.scale);
    }
    if(message_ptr->has_temperature){
        log_int(level, func_name, "message_ptr->temperature.value", message_ptr->temperature.value);
        log_int(level, func_name, "message_ptr->temperature.scale", message_ptr->temperature.scale);
    }
}

int env_sensor_feature_response_decode(
        environmentSensors_FeatureResponse *message_ptr,
        uint8_t *encoded_msg_ptr, size_t decoded_msg_size) {
    if (DEBUG_DECODE) {
        log_hex_array("DEBUG", "env_sensor_feature_response_decode", "encoded_message", encoded_msg_ptr, decoded_msg_size);
    }

    pb_istream_t stream = pb_istream_from_buffer(encoded_msg_ptr, decoded_msg_size);

    bool decode_status = pb_decode(&stream, environmentSensors_FeatureResponse_fields, message_ptr);

    if (DEBUG_DECODE) {
        log_env_sensor_data_request("DEBUG", "env_sensor_feature_response_decode", decode_status, message_ptr);
    }
    if (!decode_status) {
        printf("Decoding failed: %s\n", PB_GET_ERROR(&stream));
        return 1;
    }

    return 0;
}

int env_sensor_data_response_decode(
        environmentSensors_DataResponse *message_ptr,
        uint8_t *encoded_msg_ptr, size_t decoded_msg_size) {
    if (DEBUG_DECODE) {
        log_hex_array("DEBUG", "env_sensor_data_response_decode", "encoded_message", encoded_msg_ptr, decoded_msg_size);
    }

    pb_istream_t stream = pb_istream_from_buffer(encoded_msg_ptr, decoded_msg_size);

    bool decode_status = pb_decode(&stream, environmentSensors_DataResponse_fields, message_ptr);

    if (DEBUG_DECODE) {
        log_env_sensor_data_response("DEBUG", "env_sensor_data_response_decode", decode_status, message_ptr);
    }
    if (!decode_status) {
        printf("Decoding failed: %s\n", PB_GET_ERROR(&stream));
        return 1;
    }

    return 0;
}

int env_sensor_data_request_decode(
        environmentSensors_DataRequest *message_ptr,
        uint8_t *encoded_msg_ptr, size_t decoded_msg_size) {
    if (DEBUG_DECODE) {
        log_hex_array("DEBUG", "env_sensor_data_request_decode", "encoded_message", encoded_msg_ptr, decoded_msg_size);
    }

    pb_istream_t stream = pb_istream_from_buffer(encoded_msg_ptr, decoded_msg_size);

    bool decode_status = pb_decode(&stream, environmentSensors_DataRequest_fields, message_ptr);

    if (DEBUG_DECODE) {
        log_env_sensor_data_request("DEBUG", "env_sensor_data_request_decode", decode_status, message_ptr);
    }
    if (!decode_status) {
        printf("Decoding failed: %s\n", PB_GET_ERROR(&stream));
        return 1;
    }

    return 0;
}
