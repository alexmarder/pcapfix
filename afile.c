//
// Created by amarder on 9/17/20.
//

#include "afile.h"
#include <stdlib.h>
#include <string.h>
#include "zlib.h"
#include "bzlib.h"

const char *get_filename_ext(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if (dot) return dot + 1;
    return "";
}

afile_t *afile_open(const char *filename, const char *mode) {
    afile_t *stream = (afile_t *)calloc(1, sizeof(afile_t));
    if (strcmp(filename, "-") == 0) {
        if (mode[0] == 'r') {
            stream->file = stdin;
        } else if (mode[0] == 'w') {
            stream->file = stdout;
        } else {
            fprintf(stderr, "Cannot read/write to stdin/stdout with append.\n");
            exit(1);
        }
        stream->compression = COMPRESSION_NONE;
    } else {
        const char *ext = get_filename_ext(filename);
        if (strcmp(ext, "gz") == 0) {
            stream->file = gzopen(filename, mode);
            stream->compression = COMPRESSION_GZIP;
        } else if (strcmp(ext, "bz2") == 0) {
            stream->file = BZ2_bzopen(filename, mode);
            stream->compression = COMPRESSION_BZIP;
        } else {
            stream->file = fopen(filename, mode);
            stream->compression = COMPRESSION_NONE;
        }
    }
    return stream;
}

int afile_close(afile_t *stream) {
    switch (stream->compression) {
        case COMPRESSION_GZIP:
            gzclose((gzFile)stream->file);
            break;
        case COMPRESSION_BZIP:
            BZ2_bzclose((BZFILE *)stream->file);
            break;
        default:
            fclose((FILE *)stream->file);
            break;
    }
    free(stream);
    return 0;
}

size_t afile_read(void *ptr, size_t size, size_t nmemb, afile_t *stream) {
    switch (stream->compression) {
        case COMPRESSION_GZIP:
            return gzfread(ptr, size, nmemb, (gzFile)stream->file);
        case COMPRESSION_BZIP:
            return BZ2_bzread((BZFILE *)stream->file, ptr, size * nmemb) / size;
        default:
            return fread(ptr, size, nmemb, (FILE*)stream->file);
    }
}

size_t afile_write(void *ptr, size_t size, size_t nmemb, afile_t *stream) {
    switch (stream->compression) {
        case COMPRESSION_GZIP:
            return gzfwrite(ptr, size, nmemb, (gzFile)stream->file);
        case COMPRESSION_BZIP:
            return BZ2_bzwrite((BZFILE *)stream->file, ptr, size * nmemb) / size;
        default:
//            printf("No compression.\n");
            return fwrite(ptr, size, nmemb, (FILE *)stream->file);
    }
}
