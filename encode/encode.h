void print_encode_result(char * func_name, bool encode_status, size_t encode_message_length);

void debug_env_sensor_data_response(char * func_name, environmentSensors_DataResponse *message_ptr);

size_t env_sensor_data_response_encode(uint8_t *buffer, size_t buffer_size, environmentSensors_DataResponse *message_ptr);

void debug_env_sensor_feature_response(char * func_name, environmentSensors_FeatureResponse *message_ptr);

size_t env_sensor_feature_response_encode(uint8_t *buffer, size_t buffer_size,
                                          environmentSensors_FeatureResponse *message_ptr);