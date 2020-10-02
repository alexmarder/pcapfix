//
// Created by amarder on 9/17/20.
//

#ifndef PCAPFIX_CFILE_H
#define PCAPFIX_CFILE_H
#include <stdint.h>
#include <stdio.h>
#include "afile.h"

struct cfile {
    afile_t *file;
    uint64_t pos;
    uint64_t index;
    uint64_t end;
    char buf[2097152];
    size_t half;
    size_t quarter;
    uint8_t compression;
};
typedef struct cfile cfile_t;

cfile_t *cfile_open(const char *filename, const char *mode);
size_t cfile_read(void *ptr, size_t size, size_t nmemb, cfile_t *stream);
int cfile_seeko(cfile_t *stream, off_t offset, int whence);
off_t cfile_tello(cfile_t *stream);
int cfile_close(cfile_t *stream);
void cfile_shift(cfile_t *stream);

#endif //PCAPFIX_CFILE_H
