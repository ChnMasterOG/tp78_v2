#include "diskio.h"
#include "string.h"
#include "CH58x_common.h"

#define BLOCK_SIZE  512
#define BLOCK_COUNT 64

int USB_disk_status(void)
{
    return RES_OK;
}

int USB_disk_initialize(void)
{
    return RES_OK;
}

int USB_disk_read(BYTE *buff, LBA_t sector, UINT count)
{
    EEPROM_READ(sector * BLOCK_SIZE, buff, count);
    return 0;
}

int USB_disk_write(const BYTE *buff, LBA_t sector, UINT count)
{
    EEPROM_ERASE(sector * BLOCK_SIZE, count);
    EEPROM_WRITE(sector * BLOCK_SIZE, (BYTE*)buff, count);
    return 0;
}

int USB_disk_ioctl(BYTE cmd, void *buff)
{
    int result = 0;

    switch (cmd) {
        case CTRL_SYNC:
            result = RES_OK;
            break;

        case GET_SECTOR_SIZE:
            *(WORD *)buff = BLOCK_SIZE;
            result = RES_OK;
            break;

        case GET_BLOCK_SIZE:
            *(DWORD *)buff = 1;
            result = RES_OK;
            break;

        case GET_SECTOR_COUNT:
            *(DWORD *)buff = BLOCK_COUNT;
            result = RES_OK;
            break;

        default:
            result = RES_PARERR;
            break;
    }

    return result;
}
