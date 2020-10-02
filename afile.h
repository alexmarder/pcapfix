//
// Created by amarder on 9/17/20.
//

#ifndef PCAPFIX_AFILE_H
#define PCAPFIX_AFILE_H

#include <stdint.h>
#include <stdio.h>

#define COMPRESSION_NONE 0
#define COMPRESSION_GZIP 1
#define COMPRESSION_BZIP 2

const char *get_filename_ext(const char *filename);

struct afile {
    char *filename;
    void *file;
    uint8_t compression;
};
typedef struct afile afile_t;

afile_t *afile_open(const char *filename, const char *mode);
size_t afile_read(void *ptr, size_t size, size_t nmemb, afile_t *stream);
size_t afile_write(void *ptr, size_t size, size_t nmemb, afile_t *stream);
int afile_close(afile_t *stream);

#endif //PCAPFIX_AFILE_H
