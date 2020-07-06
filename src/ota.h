#ifndef _OTA_H_
#define _OTA_H_
#include <pthread.h>
#include <stdint.h>

/**
 * @brief Type of the callback processing ota write data
 */
typedef size_t (*cb_write_ota_data)(void *ptr, size_t size, size_t nmemb, void *stream);

/**
 * @brief Type of the callback updating ota progress
 */
typedef void (*cb_update_ota_progress)(uint8_t percentage);

/**
 * @brief Start a thread to download new firmware
 *
 * @param url new version firmware url
 * @param save_to local path to save new firmware
 * @param write_data callback function to write ota data
 * @param update_progress callback function to update ota download progress
 * @param pthreadid thread id
 * @return int
 *          0: success
 *          1: failed
 */
int ioecr_ota(const char *url, const char *save_to, cb_write_ota_data write_data, cb_update_ota_progress update_progress, pthread_t *pthreadid);

#endif