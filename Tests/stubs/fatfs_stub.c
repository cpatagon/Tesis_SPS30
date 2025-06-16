#ifndef UNIT_TESTING
#include "fatfs_stub.h"

FRESULT f_mount(FATFS * fs, const char * path, unsigned char opt) {
    return FR_OK;
}
FRESULT f_open(FIL * fp, const char * path, unsigned char mode) {
    return FR_OK;
}
FRESULT f_write(FIL * fp, const void * buff, unsigned int btw, UINT * bw) {
    if (bw)
        *bw = btw;
    return FR_OK;
}
FRESULT f_sync(FIL * fp) {
    return FR_OK;
}
FRESULT f_close(FIL * fp) {
    return FR_OK;
}
FRESULT f_lseek(FIL * fp, unsigned int ofs) {
    return FR_OK;
}
FRESULT f_stat(const char * path, void * fno) {
    return FR_OK;
}
FRESULT f_mkdir(const char * path) {
    return FR_OK;
}
unsigned int f_size(FIL * fp) {
    return 0;
}

#endif
