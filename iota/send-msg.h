#include <stdio.h>

#include "send-common.h"

int mam_send_message(char *host, int port, char *seed, char *payload, size_t payload_size, bool last_packet);