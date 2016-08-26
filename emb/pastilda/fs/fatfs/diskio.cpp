/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2014        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include <fs/fatfs/diskio.h>		/* FatFs lower layer API */

void disk_set_callbacks(int (*access)(MemoryCommand cmd, uint32_t sector, uint32_t count, uint8_t *copy_to, const uint8_t *copy_from))
{
	access_memory = access;
}

DSTATUS disk_status (BYTE pdrv)
{
	return (0);
}

/*-----------------------------------------------------------------------*/
/* Initialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (BYTE pdrv)
{
    if (pdrv != 0) {
        return (STA_NOINIT);
    }

    return (0);
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
    BYTE pdrv,        /* Physical drive nmuber (0..) */
    BYTE *buff,        /* Data buffer to store read data */
    DWORD sector,    /* Sector address (LBA) */
    UINT count        /* Number of sectors to read (1..128) */
)
{
    if (pdrv != 0)
    {
        return (RES_PARERR);
    }

    access_memory(READ, sector, count, buff, 0);
    return (RES_OK);
}

#if _USE_WRITE
DRESULT disk_write (
    BYTE pdrv,            /* Physical drive nmuber (0..) */
    const BYTE *buff,    /* Data to be written */
    DWORD sector,        /* Sector address (LBA) */
    UINT count            /* Number of sectors to write (1..128) */
)
{
    if (pdrv != 0)
    {
        return (RES_PARERR);
    }

    access_memory(WRITE, sector, count, 0, buff);
    return (RES_OK);
}
#endif

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void *buff) {
    if (pdrv != 0)
    	return (RES_PARERR);
    
    switch (cmd)
    {
    case CTRL_SYNC:
        //do nothing. By calling SD_WaitReadOperation and
        //SD_WaitWriteOperation we already ensure that operations
        //complete in the read and write functions.
        return (RES_OK);
        break;
    default:
        return (RES_PARERR);
    }
}
#endif

DWORD get_fattime (void) {
    return (get_counter_ms());
}

