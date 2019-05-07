void log_env_sensor_data_request(char *level, char *func_name, bool decode_status,
                                 environmentSensors_DataRequest *message_ptr);

int env_sensor_feature_response_decode(
        environmentSensors_FeatureResponse *message_ptr,
        uint8_t *encoded_msg_ptr, size_t decoded_msg_size);

int env_sensor_data_response_decode(
        environmentSensors_DataResponse *message_ptr,
        uint8_t *encoded_msg_ptr, size_t decoded_msg_size);

int env_sensor_data_request_decode(
        environmentSensors_DataRequest *message_ptr,
        uint8_t *encoded_msg_ptr, size_t decoded_msg_size);