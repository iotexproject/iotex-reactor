#include <stdio.h>
#include <stdlib.h>

#include "ota.h"

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
    return fwrite(ptr, size, nmemb, (FILE *)stream);
}

static void update_progress(uint8_t percentage) {
  printf("percentage: %d\n", percentage);
}

int main(int argc, char **argv) {
    pthread_t tid;
    void *ret;
    printf("Download OTA: %d\n", ioecr_ota(
        "https://ipfs.io/ipfs/QmSXD9yDTbqvhCW7A7T4LyQynu47nJ2tgivzBek3Zfk61i/V2.0.4.7/update.bin",
        "./update.bin",
        write_data,
        update_progress,
        &tid));
    pthread_join(tid, &ret);
    printf("joined: %d\n", (uint32_t)ret);
}

