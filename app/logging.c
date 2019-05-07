#include <stdio.h>
#include <netinet/in.h>

void log_int(char *level, char *func_name, char *key, int value) {
    printf("%s | %s | int | %s: %i\n", level, func_name, key, value);
}

void log_string(char *level, char *func_name, char *key, char *value) {
    printf("%s | %s | string | %s: %s\n", level, func_name, key, value);
}

void log_hex(char *level, char *func_name, char *key, uint8_t value) {
    printf("%s | %s | hex | %s: 0x%x\n", level, func_name, key, value);
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