#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdint.h>

#include "logging/logging.h"

#include "nanopb/pb_common.h"
#include "nanopb/pb_encode.h"

#include "proto_compiled/DataResponse.pb.h"
#include "proto_compiled/FeatureResponse.pb.h"

//Todo: Use macros to generate debug code.
#define DEBUG_ENCODE true

void print_encode_result(char * func_name, bool encode_status, size_t encode_message_length){
    printf("DEBUG | %s | Encode status: %u\n", func_name, encode_status);
    printf("DEBUG | %s | Bytes written: %u\n", func_name, encode_message_length);
}

void debug_env_sensor_data_response(char * func_name, environmentSensors_DataResponse *message_ptr) {
    printf("DEBUG | %s | HasTemperature: %i\n", func_name, message_ptr->has_temperature);
    printf("DEBUG | %s | Temperature value: %i\n", func_name, (int)message_ptr->temperature.value);
    printf("DEBUG | %s | Temperature scale: %i\n", func_name, (int)message_ptr->temperature.scale);
    printf("DEBUG | %s | HasPm2.5: %i\n", func_name, message_ptr->has_pm2_5);
    printf("DEBUG | %s | Pm2.5 value: %i\n", func_name, (int)message_ptr->pm2_5.value);
    printf("DEBUG | %s | Pm2.5 scale: %i\n", func_name, (int)message_ptr->pm2_5.scale);
    printf("DEBUG | %s | HasHumanity: %i\n", func_name, message_ptr->has_humanity);
    printf("DEBUG | %s | Humanity value: %i\n", func_name, (int)message_ptr->humanity.value);
    printf("DEBUG | %s | Humanity scale: %i\n", func_name, (int)message_ptr->humanity.scale);
    printf("DEBUG | %s | HasAtmosphericPressure: %i\n", func_name, message_ptr->has_atmosphericPressure);
    printf("DEBUG | %s | AtmosphericPressure value: %i\n", func_name, (int)message_ptr->atmosphericPressure.value);
    printf("DEBUG | %s | AtmosphericPressure scale: %i\n", func_name, (int)message_ptr->atmosphericPressure.scale);
}

size_t env_sensor_data_response_encode(uint8_t *buffer, size_t buffer_size, environmentSensors_DataResponse *message_ptr) {
    char func_name[] = "env_sensor_data_response_encode";

    if(DEBUG_ENCODE){
        debug_env_sensor_data_response(func_name, message_ptr);
    }

    pb_ostream_t stream = pb_ostream_from_buffer(buffer, buffer_size);

    bool encode_status = pb_encode(&stream, environmentSensors_DataResponse_fields, message_ptr);
    size_t encode_message_length = stream.bytes_written;

    if(DEBUG_ENCODE){
        print_encode_result(func_name, encode_status, encode_message_length);
    }
    if (!encode_status) {
        printf("ERROR | %s | Encoding failed: %s\n", func_name, PB_GET_ERROR(&stream));
        return 0;
    }

    return encode_message_length;
}

void debug_env_sensor_feature_response(char * func_name, environmentSensors_FeatureResponse *message_ptr) {
    printf("DEBUG | %s | HasTemperature: %i\n", func_name, message_ptr->hasTemperature);
    printf("DEBUG | %s | HasPm2.5: %i\n", func_name, message_ptr->hasPm2_5);
    printf("DEBUG | %s | HasHumanity: %i\n", func_name, message_ptr->hasHumanity);
    printf("DEBUG | %s | HasAtmosphericPressure: %i\n", func_name, message_ptr->hasAtmosphericPressure);
}

size_t env_sensor_feature_response_encode(uint8_t *buffer, size_t buffer_size,
                                       environmentSensors_FeatureResponse *message_ptr) {
    char func_name[] = "env_sensor_feature_response_encode";
    if(DEBUG_ENCODE){
        debug_env_sensor_feature_response(func_name, message_ptr);
    }

    bool encode_status;
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, buffer_size);

    encode_status = pb_encode(&stream, environmentSensors_FeatureResponse_fields, message_ptr);
    size_t encode_message_length = stream.bytes_written;

    if(DEBUG_ENCODE){
        print_encode_result(func_name, encode_status, encode_message_length);
    }
    if (!encode_status) {
        printf("ERROR | %s | Encoding failed: %s\n", func_name, PB_GET_ERROR(&stream));
        return 0;
    }

    return encode_message_length;
}
