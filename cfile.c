//
// Created by amarder on 9/17/20.
//

#include "cfile.h"
#include <stdlib.h>
#include <string.h>

cfile_t *cfile_open(const char *filename, const char *mode) {
    cfile_t *stream = (cfile_t *)calloc(1, sizeof(cfile_t));
    stream->file = afile_open(filename, mode);
    stream->index = 0;
    stream->pos = 0;
    stream->end = 0;
    stream->half = sizeof(stream->buf) / 2;
    stream->quarter = stream->half / 2;
    size_t n = afile_read(stream->buf, 1, sizeof(stream->buf), stream->file);
    if (n < sizeof(stream->buf)) {
        stream->end = n;
        fprintf(stderr, "bad\n");
    }
    return stream;
}

void cfile_shift(cfile_t *stream) {
//    fprintf(stderr, "index=%lu pos=%lu half=%lu\n", stream->index, stream->pos, stream->half);
    memcpy(stream->buf, stream->buf + stream->quarter, sizeof(stream->buf) - stream->quarter);
    size_t n = afile_read(stream->buf + (sizeof(stream->buf) - stream->quarter), 1, stream->quarter, stream->file);
    if (n < stream->quarter) {
//        fprintf(stderr, "end in sight\n");
        stream->end = (sizeof(stream->buf) - stream->quarter) + n;
    }
    stream->index -= stream->quarter;
//    fprintf(stderr, "index=%lu pos=%lu half=%lu\n", stream->index, stream->pos, stream->half);
}

size_t cfile_read(void *ptr, size_t size, size_t nmemb, cfile_t *stream) {
    size_t total = size * nmemb;
    if (stream->end > 0 && stream->index + total > stream->end) {
        total = stream->end - stream->index;
        nmemb = total / size;
        total = nmemb * size;
    }
//    fprintf(stderr, "%lu %d %lu %lu\n", stream->index, stream->end, total, stream->half);
    memcpy(ptr, stream->buf+stream->index, total);
    stream->index += total;
    stream->pos += total;
//    fprintf(stderr, "%lu %d %lu\n", stream->index, stream->end, total);
    if (stream->index > stream->half && stream->end == 0) {
        cfile_shift(stream);
    }
//    printf("%lu\n", nmemb);
    return nmemb;
}

int cfile_seeko(cfile_t *stream, off_t offset, int whence) {
    switch (whence) {
        case SEEK_SET:
            stream->index += offset - stream->pos;
            stream->pos = offset;
            if (stream->index > sizeof(stream->buf) || stream->index > stream->pos) {
                fprintf(stderr, "index=%lu", stream->index);
            }
            break;
        case SEEK_CUR:
            stream->index += offset;
            stream->pos += offset;
            break;
        default:
            fprintf(stderr, "SEEK_END undefined for cfile.");
            return -1;
    }
    return 0;
}

off_t cfile_tello(cfile_t *stream) {
    return stream->pos;
}

int cfile_close(cfile_t *stream) {
    int n = afile_close(stream->file);
    free(stream);
    return n;
}
