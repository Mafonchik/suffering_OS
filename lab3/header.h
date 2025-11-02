#ifndef HEADER_H
#define HEADER_H

#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 4096
#define MAX_LINE 4096

typedef struct {
    char data[BUFFER_SIZE];
    size_t size; 
    int ready; 
} SharedBuffer;


void generate_name(char *buf, size_t len, const char *prefix);

#endif
