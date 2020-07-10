/* Generic */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <assert.h>
#include <curl/curl.h>

#include "ota.h"

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
    return fwrite(ptr, size, nmemb, (FILE *)stream);
}

static void update_progress(uint8_t percentage) {
  //printf("percentage: %d\n", percentage);
}

void download_uri(const char *uri) {
  void *ret;
  double time_spent = 0.0;
  pthread_t otaThreadId;
  char save_to[20];
  printf("Download %s\n", uri);
  for (int i = 0; i < 100; i++) {
    printf("iteration %d\t", i);
    sprintf(save_to, "/tmp/update%d.bin", i);
    clock_t begin = clock();
    if (ioecr_ota(
      uri,
      save_to,
      write_data,
      update_progress,
      &otaThreadId
    ) != 0) {
      printf("failed ot create thread\n");
      return;
    };
    pthread_join(otaThreadId, &ret);
    if (((uint32_t)ret) != 0) {
      printf("failed to download file\n");
      return;
    }
    clock_t end = clock();
    if (i == 0) {
      printf("sleep for 60 seconds\n");
      nanosleep((const struct timespec[]){{60, 0L}}, NULL);
    } else {
      printf("sleep for 1 seconds\n");
      nanosleep((const struct timespec[]){{1, 0L}}, NULL);
    }
    time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
  }
  printf("Download time is %f seconds for %s\n", time_spent, uri);
}

int main(int argc, char **argv) {
  download_uri("https://ucam-cloud-japan.s3-ap-northeast-1.amazonaws.com/update.bin");
  download_uri("https://ipfs.io/ipfs/QmcGRvLptN7pTgPZ5zoD68nHhvNLVAfmnCqo1UuDW2iara/V2.0.5.1/update.bin");

  return 0;
}
