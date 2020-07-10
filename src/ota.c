#include <curl/curl.h>
#include "ota.h"
#include <stdlib.h>
#include <string.h>

#define CA_CERTIF_FILE "cacert.pem"
const char *HTTPS_PREFIX = "https://";

static int progress(void *p, double dltotal, double dlnow, double ultotal, double ulnow) {
    cb_update_ota_progress uap = (cb_update_ota_progress)p;
    if (dltotal >= 1.0) {
        uap(dlnow * 100 / dltotal);
    }
    return 0;
}

typedef struct {
    const char *url;
    const char *save_to;
    cb_write_ota_data write_data;
    cb_update_ota_progress update_progress;
} st_download_ota_param;

void *download_ota(void *arg) {
    st_download_ota_param *param = arg;
    FILE *fp = NULL;
    CURL *curl = NULL;
    CURLcode ret;
    if ((fp = fopen(param->save_to, "wb")) == NULL) {
        printf("failed to open file %s\n", param->save_to);
        pthread_exit((void *)1);
    }
    if ((curl = curl_easy_init()) == NULL) {
        printf("failed to init curl\n");
        fclose(fp);
        pthread_exit((void *)1);
    }
    curl_easy_setopt(curl, CURLOPT_URL, param->url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, param->write_data);
    if (param->update_progress != NULL) {
        curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress);
        curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, param->update_progress);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    } else {
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
    }
    if (memcmp(param->url, HTTPS_PREFIX, 8) == 0) {
        curl_easy_setopt(curl, CURLOPT_CAINFO, CA_CERTIF_FILE);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    }
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    ret = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    fclose(fp);
    pthread_exit((void *)ret);
}

int ioecr_ota(const char *url, const char *save_to, cb_write_ota_data write_data, cb_update_ota_progress update_progress, pthread_t *pthreadid) {
    st_download_ota_param *param = calloc(1, sizeof(st_download_ota_param));
    param->url = url;
    param->save_to = save_to;
    param->write_data = write_data;
    param->update_progress = update_progress;
    if (pthread_create(pthreadid, NULL, download_ota, (void *)param) != 0) {
        return 1;
    }
    return 0;
}
