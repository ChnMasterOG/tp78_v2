
#include "ff.h"

#ifndef PACKAGE_FATFS_PORT_FATFS_USBD_H_
#define PACKAGE_FATFS_PORT_FATFS_USBD_H_

int USB_disk_status(void);
int USB_disk_initialize(void);
int USB_disk_read(BYTE *buff, LBA_t sector, UINT count);
int USB_disk_write(const BYTE *buff, LBA_t sector, UINT count);
int USB_disk_ioctl(BYTE cmd, void *buff);

#endif /* PACKAGE_FATFS_PORT_FATFS_USBD_H_ */
