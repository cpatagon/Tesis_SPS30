#ifndef UNIT_TESTING
#ifndef FF_H
#define FF_H
typedef int FRESULT;
typedef int FIL;
typedef int FATFS;
typedef unsigned int DWORD;
typedef unsigned int UINT;
#define FR_OK              0
#define FR_DISK_ERR        1
#define FR_INT_ERR         2
#define FR_NOT_READY       3
#define FR_NO_FILE         4
#define FR_NO_PATH         5
#define FR_INVALID_NAME    6
#define FR_DENIED          7
#define FR_EXIST           8
#define FR_INVALID_OBJECT  9
#define FR_WRITE_PROTECTED 10
#define FR_INVALID_DRIVE   11
#define FR_NOT_ENABLED     12
#define FR_NO_FILESYSTEM   13
#define FR_TIMEOUT         14
#define FR_LOCKED          15
#define FA_OPEN_APPEND     0x30
#define FA_WRITE           0x02
#define FA_OPEN_ALWAYS     0x10
#endif
#endif
