#ifndef UNIT_TESTING
#ifndef FATFS_STUB_H
#define FATFS_STUB_H

typedef int FRESULT;
typedef int FIL;
typedef int FATFS;
typedef unsigned int DWORD;
typedef unsigned int UINT;

#define FR_OK 0

FRESULT f_mount(FATFS * fs, const char * path, unsigned char opt);
FRESULT f_open(FIL * fp, const char * path, unsigned char mode);
FRESULT f_write(FIL * fp, const void * buff, unsigned int btw, UINT * bw);
FRESULT f_sync(FIL * fp);
FRESULT f_close(FIL * fp);
FRESULT f_lseek(FIL * fp, unsigned int ofs);
FRESULT f_stat(const char * path, void * fno);
FRESULT f_mkdir(const char * path);
unsigned int f_size(FIL * fp);

#endif
#endif
