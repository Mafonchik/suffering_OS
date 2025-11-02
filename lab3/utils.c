#include "header.h"

void generate_name(char *buf, size_t len, const char *prefix) {
    snprintf(buf, len, "/%s_%ld", prefix, (long)getpid());
}
