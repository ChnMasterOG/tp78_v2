/*
 * fftools.c
 *
 *  Created on: 2022年5月11日
 *      Author: Administrator
 */
#include "fftools.h"
#include "string.h"
#include "stdio.h"

FRESULT scan_files( char* path )
{
    FRESULT res;
    DIR dir;
    UINT i;
    static FILINFO fno;

    res = f_opendir( &dir, path );                              // 进入目录
    if (res == FR_OK)
    {
        while( 1 )
        {
            res = f_readdir( &dir, &fno );                      // 读取目录信息
            if( ( res != FR_OK ) || ( fno.fname[0] == 0 ) )
            {
                break;                                          // 出错或者未找到文件
            }
            if( fno.fattrib & AM_DIR )                          // 目标为文件夹
            {
                i = strlen(path);                               // 计算出路径长度
                sprintf( &path[i], "/%s", fno.fname );          // 文件名加到路径后面
                res = scan_files( path );                       // 进入目录
                if (res != FR_OK) break;
                path[i] = 0;
            }
            else
            {
                printf("%s/%s\r\n", path, fno.fname);           // 打印文件
            }
        }
        f_closedir(&dir);
    }

    return res;
}
