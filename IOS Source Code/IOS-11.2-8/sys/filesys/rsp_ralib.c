/* $Id: rsp_ralib.c,v 3.2.60.4 1996/08/28 12:42:45 thille Exp $
 * $Source: /release/112/cvs/Xsys/filesys/rsp_ralib.c,v $
 *------------------------------------------------------------------
 * Raw Access LIBrary for the flash file system on RSP
 *
 * January 1995, Jack Jenney
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp_ralib.c,v $
 * Revision 3.2.60.4  1996/08/28  12:42:45  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.60.3  1996/04/29  19:22:43  ssangiah
 * CSCdi55648:  The boot flash command does not work.
 *   o Get the "boot flash" command to work again on RSP and c7200.
 *   o Fix an erroneous log message.
 * Branch: California_branch
 *
 * Revision 3.2.60.2  1996/04/22  18:58:25  ssangiah
 * CSCdi54920:  Need to add support for the flash MIB on c7200.
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/03/18  19:36:14  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  09:36:39  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  13:50:43  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:09:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:32:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  20:57:36  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/07/07  22:57:13  yanke
 * Detect device write protect switch, update it in device structure.
 * Check the device for protection in all write-related operations.
 * Reduce the time to wait for vpp on valid bit.
 *
 * CSCdi36427:  write protecting flash credit card hangs the router
 *
 * Revision 2.3  1995/06/16  04:26:49  dkatz
 * CSCdi35882:  Time-of-day code needs cleanup
 *
 * Revision 2.2  1995/06/09  13:04:04  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:33:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include <string.h>
#include <ciscolib.h>

#if !defined(RSP_ROM)

#include "../os/clock.h"
#include "stacks.h"
#include "logger.h"
#include "../filesys/msg_filesys.c"	/* Not a typo, see logger.h */

#if !defined(FSLIB)

#define strncpy sstrncpy

#endif /* !FSLIB */

#endif /* !RSP_ROM */

#include "filesys.h"
#include "ralib.h"
#include "fslib.h"
#include "clpd6720.h"
#include "fslib_print.h"
#include "../src-rsp/rsp_pcmap.h"

/* these need to be replaced */
#define STATIC static

#if defined(RSP_ROM)
#include "mon_ralib.h"
#endif

/*----------------------------------------------------------------------------*
 * defines used internally that have to do with windows and other PCMCIA
 * stuff. 
 *----------------------------------------------------------------------------*/
#define WINDOW_SIZE                  0x00400000  /* 4 MBytes */
#define ATTRIB_WINDOW_SIZE           0x00100000  /* 1 MBytes */
#define PCMCIA_BASE                  0xb7000000
#define PCMCIA_WINDOW_0_BASE         0xb7400000
#define PCMCIA_WINDOW_1_BASE         0xb7c00000
#define PCMCIA_ATTRIB_WINDOW_0_BASE  0xb7300000
#define PCMCIA_ATTRIB_WINDOW_1_BASE  0xb7b00000
#define INTERNAL_FLASH_BASE          0xb8000000

#define FAST_WINDOW_TIMING_SET     CLPD6720_TIMING_0
#define FAST_WINDOW_SETUP_TIME     0x04
#define FAST_WINDOW_COMMAND_TIME   0x0a
#define FAST_WINDOW_RECOVERY_TIME  0x04
#define SLOW_WINDOW_TIMING_SET     CLPD6720_TIMING_1
#define SLOW_WINDOW_SETUP_TIME     0x20
#define SLOW_WINDOW_COMMAND_TIME   0x48
#define SLOW_WINDOW_RECOVERY_TIME  0x20

/*----------------------------------------------------------------------------*
 * RALIB vector table to return when initializing.
 *----------------------------------------------------------------------------*/
ralib_vector_table_t ralib_vector_table;

/*----------------------------------------------------------------------------*
 * DEVICE numbers
 *----------------------------------------------------------------------------*/
#define RALIB_MAX_DEV_NUM 2  /* only devices 0, 1 and 2 */
#define RALIB_DEV_NUM_PCMCIA_SOCKET_0  0
#define RALIB_DEV_NUM_PCMCIA_SOCKET_1  1
#define RALIB_DEV_NUM_INTERNAL_FLASH   2

/*----------------------------------------------------------------------------*
 * socket_to_dev_num -- used to convert the socket numbers (0 and 1)
 * to the device numbers for the sockets.  This conversion is done in
 * an unprotected manner (i.e. we don't check for invalid socket
 * numbers) because the only provider of socket numbers is the
 * interrupt handler for the PCMCIA controller which we trust since it
 * is part of the RALIB.  (Heck, at some point you have to trust
 * someone.)
 * dev_num_to_socket -- used to convert the dev numbers to socket numbers.
 * returns a -1 if the dev num does not correspond to a socket number.
 *----------------------------------------------------------------------------*/
STATIC int socket_to_dev_num[CLPD6720_SOCKET_1 + 1] = 
    { RALIB_DEV_NUM_PCMCIA_SOCKET_0,
      RALIB_DEV_NUM_PCMCIA_SOCKET_1 };

STATIC int dev_num_to_socket[RALIB_MAX_DEV_NUM + 1] = 
    { CLPD6720_SOCKET_0,
      CLPD6720_SOCKET_1,
      2};                  /* socket number of internal flash --
			      bogus, but needed for insert/removeal to
			      work right */

/*----------------------------------------------------------------------------*
 * RALIB DEVICE TABLE
 * device_id -- the text of the device (without the trailing ":")
 * device_type -- PCMCIA or INTERNALFLASH
 * socket -- PCMCIA socket number (PCMCIA only)
 * window -- PCMCIA window number to use to access device (PCMCIA only)
 * base_addr -- (for INTERNALFLASH) the address of the start of the flash bank
 *   (for PCMCIA) the address of the start of the 4MB window to use
 *   for the socket.
 * window_base_ptr -- the base address of PCMCIA memory that is mapped to the
 *   window (PCMCIA only) 
 * attrib_window -- PCMCIA window number to use to access attrib memory
 *   (PCMCIA only)
 * attrib_base_addr -- the address of the start of the 1MB window to use for
 *   attribute memory for the socket. (PCMCIA only).
 * attrib_window_base_ptr -- the base address of PCMCIA attrib memory that is
 *   mapped to the window (PCMCIA only) (this is always 0 -- for now)
 * physical_status -- is device physically present or removed (PCMCIA only)
 *   (for INTERNALFLASH this is always set to TRUE)
 * logical_status -- is the device logically present or removed (PCMCIA only)
 *   (for INTERNALFLASH this is always set to TRUE).  This flag is used to 
 *   simulate removing a PCMCIA card without having to actually physically
 *   remove the card.
 * valid -- is this entry in the device table valid (TRUE = VALID)
 * present -- the combination of physical and logical status.  If both
 *   physical_status and logical_status are TRUE, this is set TRUE.
 *   (used by all devices, but really only applies to PCMCIA)
 * initialized -- indicates whether the device has been initialized (by
 *   calling ralib_init_device()) since the last time it was removed.
 *   (used by all devices, but really only applies to PCMCIA)
 * ready -- indicates if the device is ready.
 *   (used by all devices, but really only applies to PCMCIA)
 *----------------------------------------------------------------------------*/
typedef struct ralib_dev_rec_ {
    char  device_id[32];
    int   device_type;
    int   socket;
    int   window;
    ulong base_addr;
    ulong window_base_ptr;
    int   attrib_window;
    ulong attrib_base_addr;
    ulong attrib_window_base_ptr;
    int   physical_status;
    int   logical_status;
    int   valid;
    int   present;
    int   initialized;
    int   ready;
    int   write_protect;
} ralib_dev_rec_t;

STATIC ralib_dev_rec_t  ralib_dev_table[RALIB_MAX_DEV_NUM + 1];

#define DEVICE_TYPE_UNKNOWN          0
#define DEVICE_TYPE_PCMCIA           1
#define DEVICE_TYPE_INTERNAL_FLASH   2


/******************************************************************************
 * Name: get_dev_num_from_socket
 *
 * Description: macro to convert the socket number to the device number
 *
 *   This function is called from an interrupt handler
 *
 * Input: 
 *   socket -- the socket number to convert to a device number
 *
 * Return Value:
 *   dev_num -- the device number that corresponds to that socket number
 *
 *******************************************************************************/
#define get_dev_num_from_socket(_socket_)   socket_to_dev_num[(_socket_)]


/******************************************************************************
 * Name: get_socket_from_dev_num
 *
 * Description: macro to convert the device number to the socket number 
 *
 *   This function is called from an interrupt handler
 *
 * Input: 
 *   dev_num -- the device number to convert to socket number
 *
 * Return Value:
 *   socket -- the socket number that corresponds to that device number
 *
 *******************************************************************************/
#define get_socket_from_dev_num(_dev_num_)   dev_num_to_socket[(_dev_num_)]



/******************************************************************************
 * Name: ralib_event_card_ready
 *
 * Description: called when a card ready interrupt is received (BUSY-->READY)
 *
 *   This function is called from an interrupt handler
 *
 * Input: 
 *   socket -- the socket number of the card that just became ready
 *
 * Return Value:
 *
 *******************************************************************************/
void ralib_event_card_ready (clpd6720_socket_t socket)
{
    int dev_num;
    ralib_dev_rec_t *dr;

    /*
     * Find the device record and set the ready flag to TRUE
     */
    dev_num = get_dev_num_from_socket(socket);
    dr = &ralib_dev_table[dev_num];
    if (dr->valid == TRUE) {
#ifdef DEBUG
#if !defined(FSLIB) && !defined(RSP_ROM)
	if (dr->ready == FALSE) {
	    buginf("\nDevice %d change to ready state", dev_num);
	    if (onintstack())
	    	buginf("\nCalled from interrupt handler");
	}
#endif
#endif
        dr->ready = TRUE;
    }
    PRT(P15INT,("ralib_event_card_ready(%d)\n",socket));
}


/******************************************************************************
 * Name: ralib_event_card_busy
 *
 * Description: called when a card busy interrupt is received (READY-->BUSY)
 *
 *   This function is called from an interrupt handler
 *
 * Input: 
 *   socket -- the socket number of the card that just became busy
 *
 * Return Value:
 *
 *******************************************************************************/
void ralib_event_card_busy (clpd6720_socket_t socket)
{
    int dev_num;
    ralib_dev_rec_t *dr;

    /*
     * Find the device record and set the ready flag to FALSE
     */
    dev_num = get_dev_num_from_socket(socket);
    dr = &ralib_dev_table[dev_num];
    if (dr->valid == TRUE) {
	dr->ready = FALSE;
#ifdef DEBUG
#if !defined(FSLIB) && !defined(RSP_ROM)
	buginf("\nDevice %d in busy state", dev_num);
	if (onintstack())
	    buginf("\nCalled from interrupt handler");
#endif
#endif
    }
    PRT(P15INT,("ralib_event_card_busy(%d)\n",socket));
}


void ralib_event_card_protected (clpd6720_socket_t socket)
{
    int dev_num;
    ralib_dev_rec_t *dr;

    /*
     * Find the device record and set the ready flag to FALSE
     */
    dev_num = get_dev_num_from_socket(socket);
    dr = &ralib_dev_table[dev_num];
    if (dr->valid == TRUE) {
	dr->write_protect = 1;
    }
    PRT(P15INT,("ralib_event_card_protected(%d)\n",socket));
}


void ralib_event_card_writable (clpd6720_socket_t socket)
{
    int dev_num;
    ralib_dev_rec_t *dr;

    /*
     * Find the device record and set the ready flag to FALSE
     */
    dev_num = get_dev_num_from_socket(socket);
    dr = &ralib_dev_table[dev_num];
    if (dr->valid == TRUE) {
	dr->write_protect = 0;
    }
    PRT(P15INT,("ralib_event_card_writable(%d)\n",socket));
}



/******************************************************************************
 * Name: ralib_event_card_inserted
 *
 * Description: called when a card inserted interrupt is received
 *   This is called when the card is inserted, and the hardware
 *   detects the card is present and both CD signals are active.
 *
 *   This function is called from an interrupt handler
 *
 * Input: 
 *   socket -- the socket number of the card that was just inserted
 *
 * Return Value:
 *
 *******************************************************************************/
void ralib_event_card_inserted (clpd6720_socket_t socket)
{
    int dev_num;
    ralib_dev_rec_t *dr;

    /*
     * Find the device record and set the PHYSICAL status to TRUE
     * (i.e. inserted).  Set the present flag to the combination of
     * the physical and logical status.
     */
    dev_num = get_dev_num_from_socket(socket);
    dr = &ralib_dev_table[dev_num];
    if ( (dr->valid == TRUE) && (dr->device_type == DEVICE_TYPE_PCMCIA) ) {
	dr->physical_status = TRUE;
#if !defined(FSLIB) && !defined(RSP_ROM)
	if (dr->present == FALSE) {
#ifdef DEBUG
	    buginf("\nDevice %d changes to inserted status", dev_num);
#endif
	    if (onintstack())
	    	errmsg(&msgsym(DEV, FILESYS), "inserted into", dr->device_id);
	}
#endif
	dr->present = dr->physical_status && dr->logical_status;
    }
    PRT(P15INT,("ralib_event_card_inserted(%d)\n",socket));
}


/******************************************************************************
 * Name: ralib_event_card_removed
 *
 * Description: called when a card removed interrupt is received
 *   This is called when the card detect lines change and the
 *   resulting state is either removed or partially present (i.e. one
 *   or both of the CD pins are inactive).  This could be called when
 *   a card is already removed (e.g. if the pins go from having one CD
 *   pin active to having no CD pins active).
 *
 *   This function is called from an interrupt handler
 *
 * Input: 
 *   socket -- the socket number of the card that was just inserted
 *
 * Return Value:
 *
 *******************************************************************************/
void ralib_event_card_removed (clpd6720_socket_t socket)
{
    int dev_num;
    ralib_dev_rec_t *dr;

    /*
     * Find the device record and set the PHYSICAL status to TRUE
     * (i.e. inserted).  Set the present flag to FALSE since it cannot
     * be true if the physical status is FALSE
     */
    dev_num = get_dev_num_from_socket(socket);
    dr = &ralib_dev_table[dev_num];
    if ( (dr->valid == TRUE) && (dr->device_type == DEVICE_TYPE_PCMCIA) ) {
        dr->physical_status = FALSE;
#if !defined(FSLIB) && !defined(RSP_ROM)
	if (dr->present == TRUE) {
#ifdef DEBUG
	    buginf("\nDevice %d changes to not present status", dev_num);
#endif
	    if (onintstack())
	    	errmsg(&msgsym(DEV, FILESYS), "removed from", dr->device_id);
	}
#endif
	dr->present = FALSE;
#ifdef DEBUG
#if !defined(FSLIB) && !defined(RSP_ROM)
	if (dr->initialized == TRUE) {
	    buginf("\nDevice %d changes to uninitialized status", dev_num);
	    if (onintstack())
	    	buginf("\nCalled from interrupt handler");
	}
#endif
#endif
	dr->initialized = FALSE;
    }
    PRT(P15INT,("ralib_event_card_removed(%d)\n",socket));
}


/******************************************************************************
 * Name: ralib_status
 *
 * Description: returns the ready/busy status of the card
 *
 * Input: 
 *   dev_num -- the device number of the file system from which to read
 *
 * Return Value: 
 *   status -- status of the device.  Possible values are
 *     RALIB_OK
 *     RALIB_BUSY
 *     RALIB_ERROR_INVALID_DEV_NUM
 *     RALIB_ERROR_NO_DEVICE
 *     RALIB_ERROR_DEVICE_NEEDS_INIT
 *
 *******************************************************************************/
int ralib_status (int dev_num)
{
    int status;
    ralib_dev_rec_t *dr;

    if (dev_num > RALIB_MAX_DEV_NUM) {
	status = RALIB_ERROR_INVALID_DEV_NUM;
    } else {
	dr = &ralib_dev_table[dev_num];
	if (dr->valid == FALSE) {
	    status = RALIB_ERROR_INVALID_DEV_NUM;
	} else {
	    clpd6720_mgmt_intr_handler(); /* XXX jack, fix this later */
	    if (dr->present == FALSE) {
		status = RALIB_ERROR_NO_DEVICE;
	    } else {
		if (dr->initialized == FALSE) {
		    status = RALIB_ERROR_DEVICE_NEEDS_INIT;
		} else {
		    /*
		     * The actual status mechanism is very device dependant
		     */
		    switch (dev_num) {
		    case RALIB_DEV_NUM_PCMCIA_SOCKET_0:
		    case RALIB_DEV_NUM_PCMCIA_SOCKET_1:
			clpd6720_mgmt_intr_handler();
			if (dr->ready == FALSE) {
			    status = RALIB_BUSY;
			} else {
			    status = RALIB_OK;
			}
			break;
		    case RALIB_DEV_NUM_INTERNAL_FLASH:
			/* jack XXX fill this in later */
			status = RALIB_ERROR;
			break;
		    default:
			status = RALIB_ERROR;
			break;
		    }
		}
	    }
	}
    }

    PRT(P16,("ralib_status(%d) = %d\n",dev_num,status));
    return(status);
}


/******************************************************************************
 * Name: rsp_read_reg8
 *
 * Description: reads a value from an 8-bit hardware register on RSP
 *
 * Input: 
 *   reg_addr -- the address of the register to read
 *
 * Return Value: 
 *   value -- the byte read from the register
 *
 *******************************************************************************/
static uchar rsp_read_reg8(ulong reg_addr)
{

    /* XXX jack should these be volatile? */
    return(*(uchar *)reg_addr);
}


/******************************************************************************
 * Name: rsp_write_reg8
 *
 * Description: writes a value to an 8-bit hardware register on RSP
 *
 * Input: 
 *   reg_addr -- the address of the register to write
 *   value -- the byte to write to the register
 *
 * Return Value: None
 *
 *******************************************************************************/
static void rsp_write_reg8(ulong reg_addr, uchar value)
{

    /* XXX jack should these be volatile? */
    *(uchar *)reg_addr = value;
}


/******************************************************************************
 * Name: ralib_control
 *
 * Description: Misc device control function
 *
 * Input: 
 *   dev_num -- the device number of the file system from which to read
 *   mode -- the control command.  One of the following:
 *     RESET -- reset the device
 *     VPGM_OFF -- turn the programming voltage off
 *     VPGM_VCC -- turn the programming voltage on to Vcc
 *     VPGM_12V -- turn the programming voltage on to 12 Volts
 *
 * Return Value: 
 *   return code -- RALIB_OK = success, otherwise an error code
 *
 *******************************************************************************/
int ralib_control (int dev_num, int mode)
{
    int rc;
    uchar sys_control;
    ralib_dev_rec_t *dr;

    /*
     * the "mode" parameter is essentially a control command
     */
    switch (mode) {
	/*
	 * The command is reset the device
	 */
    case RALIB_RESET_DEVICE:
	switch (dev_num) {
	case RALIB_DEV_NUM_PCMCIA_SOCKET_0:
	    clpd6720_reset_socket(CLPD6720_SOCKET_0);
	    rc = RALIB_OK;
	    break;
	case RALIB_DEV_NUM_PCMCIA_SOCKET_1:
	    clpd6720_reset_socket(CLPD6720_SOCKET_1);
	    rc = RALIB_OK;
	    break;
	case RALIB_DEV_NUM_INTERNAL_FLASH:
	    rc = RALIB_OK;
	    break;
	default:
	    rc = RALIB_ERROR_INVALID_DEV_NUM;
	    break;
	}
	break;

	/*
	 * The command is turn the programming voltage off.
	 */
    case RALIB_VPGM_OFF:
	switch (dev_num) {
	case RALIB_DEV_NUM_PCMCIA_SOCKET_0:
	case RALIB_DEV_NUM_PCMCIA_SOCKET_1:
	    rc = RALIB_ERROR_VPGM_NOT_SUPPORTED;
	    break;
	case RALIB_DEV_NUM_INTERNAL_FLASH:
	    rc = RALIB_OK;
	    sys_control = rsp_read_reg8(ADRSPC_SYSCONTROL);
	    sys_control |= CONTROL_FLASH_VPP;
	    rsp_write_reg8(ADRSPC_SYSCONTROL,sys_control);
	    break;
	default:
	    rc = RALIB_ERROR_INVALID_DEV_NUM;
	    break;
	}
	break;

	/*
	 * The command is turn the programming voltage on to Vcc
	 */
    case RALIB_VPGM_VCC:
	rc = RALIB_ERROR_VPGM_NOT_SUPPORTED;
	break;

	/*
	 * The command is turn the programming voltage on to 12 Volts
	 */
    case RALIB_VPGM_12V:
	switch (dev_num) {
	case RALIB_DEV_NUM_PCMCIA_SOCKET_0:
	case RALIB_DEV_NUM_PCMCIA_SOCKET_1:
	    rc = RALIB_ERROR_VPGM_NOT_SUPPORTED;
	    break;
	case RALIB_DEV_NUM_INTERNAL_FLASH:
	    sys_control = rsp_read_reg8(ADRSPC_SYSCONTROL);
	    sys_control &= ~CONTROL_FLASH_VPP;
	    rsp_write_reg8(ADRSPC_SYSCONTROL,sys_control);
	    rc = RALIB_OK;
	    break;
	default:
	    rc = RALIB_ERROR_INVALID_DEV_NUM;
	    break;
	}
	break;

    case RALIB_WRITE_PROTECT:
	switch (dev_num) {
	case RALIB_DEV_NUM_PCMCIA_SOCKET_0:
	case RALIB_DEV_NUM_PCMCIA_SOCKET_1:
	    dr = &ralib_dev_table[dev_num];
	    rc = dr->write_protect;
	    break;
	case RALIB_DEV_NUM_INTERNAL_FLASH:
	    rc = 0;
	    break;
	default:
	    rc = RALIB_ERROR_INVALID_DEV_NUM;
	    break;
	}
	break;

	/*
	 * The command is not recognized
	 */
    default:
	rc = RALIB_ERROR_INVALID_MODE;
	break;
    }

    PRT(P16,("ralib_control(%d,%d) = %d\n",dev_num,mode,rc));
    return(rc);
}


/******************************************************************************
 * Name: ralib_init_device_table
 *
 * Description: initializes the device table for the RALIB
 *
 * Input: None
 *
 * Return Value: None
 *
 *******************************************************************************/
void ralib_init_device_table (void)
{
    ralib_dev_rec_t *dr;

    /*
     * PCMCIA slot 0
     */
    dr = &ralib_dev_table[0];
    strncpy(dr->device_id,"slot0",sizeof(dr->device_id));
    dr->device_type = DEVICE_TYPE_PCMCIA;
    dr->socket = CLPD6720_SOCKET_0;
    dr->window = CLPD6720_WINDOW_0;
    dr->base_addr = PCMCIA_WINDOW_0_BASE;
    dr->window_base_ptr = 0;
    dr->attrib_window = CLPD6720_WINDOW_4;
    dr->attrib_base_addr = PCMCIA_ATTRIB_WINDOW_0_BASE;
    dr->attrib_window_base_ptr = 0;
    dr->physical_status = FALSE;
    dr->logical_status = TRUE;
    dr->valid = TRUE;
    dr->present = FALSE;
    dr->initialized = FALSE;
    dr->ready = FALSE;

    /*
     * PCMCIA slot 1
     */
    dr = &ralib_dev_table[1];
    strncpy(dr->device_id,"slot1",sizeof(dr->device_id));
    dr->device_type = DEVICE_TYPE_PCMCIA;
    dr->socket = CLPD6720_SOCKET_1;
    dr->window = CLPD6720_WINDOW_0;
    dr->base_addr = PCMCIA_WINDOW_1_BASE;
    dr->attrib_window = CLPD6720_WINDOW_4;
    dr->attrib_base_addr = PCMCIA_ATTRIB_WINDOW_1_BASE;
    dr->attrib_window_base_ptr = 0;
    dr->window_base_ptr = 0;
    dr->physical_status = FALSE;
    dr->logical_status = TRUE;
    dr->valid = TRUE;
    dr->present = FALSE;
    dr->initialized = FALSE;
    dr->ready = FALSE;

    /*
     * Internal Flash Bank
     */
    dr = &ralib_dev_table[2];
    strncpy(dr->device_id,"bootflash",sizeof(dr->device_id));
    dr->device_type = DEVICE_TYPE_INTERNAL_FLASH;
    dr->socket = 0;
    dr->window = 0;
    dr->base_addr = INTERNAL_FLASH_BASE;
    dr->window_base_ptr = 0;
    dr->attrib_window = 0;
    dr->attrib_base_addr = 0;
    dr->attrib_window_base_ptr = 0;
    dr->physical_status = TRUE;
    dr->logical_status = TRUE;
    dr->valid = TRUE;
    dr->present = TRUE;
    dr->initialized = TRUE;
    dr->ready = TRUE;

#if !defined(FSLIB) && !defined(RSP_ROM)
    /* CSCdi49799 - see if bootflash is really installed */
    if (!touch_device((void *)dr->base_addr)) {
	dr->physical_status = FALSE;
	dr->present = FALSE;
	dr->initialized = FALSE;
	dr->ready = FALSE;
    }
#endif

    PRT(P16,("ralib_init_device_table() = done\n"));
}


/******************************************************************************
 * Name: ralib_init
 *
 * Description: initializes the raw access library
 *
 *            ISA Memory Space
 *  0xffffff +----------------+   
 *           | Socket 1       |
 *  0xf00000 + Common Memory  +
 *           | Access         |
 *  0xe00000 +                + ===========>  Mapped to Common memory space
 *           |                |               for PCMCIA socket 1 on even
 *  0xd00000 +                +               4 MByte addresses 
 *           |                |               (i.e. 0, 4MB, 8MB, 16MB, etc.)
 *  0xc00000 +----------------+               (socket 1, window 0)
 *           | Sckt 1 Attrib  | ===========>  Mapped to Attribute memory space
 *  0xb00000 +----------------+               for PCMCIA socket 0 at offset 0
 *           | UNUSED         |               (socket 1 window 4)
 *  0xa00000 +----------------+
 *           | UNUSED         |
 *  0x900000 +----------------+
 *           | UNUSED         |
 *  0x800000 +----------------+
 *           | Socket 0       |
 *  0x700000 + Common Memory  +
 *           | Access         |
 *  0x600000 +                + ===========>  Mapped to Common memory space
 *           |                |               for PCMCIA socket 0 on even
 *  0x500000 +                +               4 MByte addresses 
 *           |                |               (i.e. 0, 4MB, 8MB, 16MB, etc.)
 *  0x400000 +----------------+               (socket 0 window 0)
 *           | Sckt 0 Attrib  | ===========>  Mapped to Attribute memory space
 *  0x300000 +----------------+               for PCMCIA socket 0 at offset 0
 *           | UNUSED         |               (socket 0 window 4)
 *  0x200000 +----------------+
 *           | UNUSED         |
 *  0x100000 +----------------+
 *           | UNUSABLE       |
 *  0x000000 +----------------+
 *
 *   Window usage:  There are 5 windows per socket.  Only 2 windows
 *     are used for each socket -- window 0 is used as a 4MB window to
 *     access common memory, and window 4 is used as a 1MB window to
 *     access attribute memory.  Sockets 0 and 1 are set up similarly.
 *
 *     Socket  Window  Usage
 *     ------  ------  --------------------------------------------------
 *       0       0     4MB window at ISA offset 0x400000, common memory
 *       0       1     Not used
 *       0       2     Not used
 *       0       3     Not used
 *       0       4     1MB window at ISA offset 0x300000, attribute memory
 *       1       0     4MB window at ISA offset 0xc00000, common memory
 *       1       1     Not used
 *       1       2     Not used
 *       1       3     Not used
 *       1       4     1MB window at ISA offset 0xb00000, attribute memory
 *
 * Input: None
 *
 * Return Value: 
 *   a pointer to the RALIB vector table for this RALIB.
 *
 *******************************************************************************/
ralib_vector_table_t *ralib_init (void)
{
    clpd6720_window_t window;
    clpd6720_socket_t socket;
    ralib_vector_table_t *ralib;

    /*
     * set up the ralib vector table with pointers to the functions
     */
    ralib = &ralib_vector_table;
    ralib->read = ralib_read;
    ralib->write = ralib_write;
    ralib->status = ralib_status;
    ralib->control = ralib_control;
    ralib->yield = ralib_yield;
    ralib->time = ralib_time;
    ralib->device_init = ralib_device_init;
    ralib->device_reset = ralib_device_reset;
    ralib->get_dev_num = ralib_get_dev_num;
    ralib->attrib_read = ralib_attrib_read;
    ralib->attrib_write = ralib_attrib_write;

    /*
     * setup the interrupt handler
     */
#if !defined(FSLIB)
    createlevel(LEVEL_PCMCIA, clpd6720_mgmt_intr_handler, "PCMCIA Interrupt");
#endif
    /*
     * set up the platform-dependant device stuff
     */
    ralib_init_device_table();

    /*
     * initialize the PCMCIA controller and set up some default stuff
     */
    clpd6720_init();

    /*
     * Set up the timing parameters for the windows.
     */
    clpd6720_set_timing(CLPD6720_SOCKET_0,
			FAST_WINDOW_TIMING_SET,
			FAST_WINDOW_SETUP_TIME,
			FAST_WINDOW_COMMAND_TIME,
			FAST_WINDOW_RECOVERY_TIME);

    clpd6720_set_timing(CLPD6720_SOCKET_0,
			SLOW_WINDOW_TIMING_SET,
			SLOW_WINDOW_SETUP_TIME,
			SLOW_WINDOW_COMMAND_TIME,
			SLOW_WINDOW_RECOVERY_TIME);
    
    clpd6720_set_timing(CLPD6720_SOCKET_1,
			FAST_WINDOW_TIMING_SET,
			FAST_WINDOW_SETUP_TIME,
			FAST_WINDOW_COMMAND_TIME,
			FAST_WINDOW_RECOVERY_TIME);

    clpd6720_set_timing(CLPD6720_SOCKET_1,
			SLOW_WINDOW_TIMING_SET,
			SLOW_WINDOW_SETUP_TIME,
			SLOW_WINDOW_COMMAND_TIME,
			SLOW_WINDOW_RECOVERY_TIME);

    /*
     * Set all the windows to use the fast timing set.
     */
    for (socket = CLPD6720_SOCKET_0; socket <= CLPD6720_SOCKET_1; socket++) { 
	for (window = CLPD6720_WINDOW_0; window <= CLPD6720_WINDOW_4; window++) { 
	    clpd6720_set_window_timing(socket,window,FAST_WINDOW_TIMING_SET);
	}
    }

    /*
     * set up the attribute memory windows at their default value
     */
    clpd6720_set_window(CLPD6720_SOCKET_0,
			CLPD6720_WINDOW_4,
			PCMCIA_ATTRIB_WINDOW_0_BASE,
			0,
			ATTRIB_WINDOW_SIZE,
			CLPD6720_ATTRIB_MEM,
			CLPD6720_WIDTH_16_BIT,
			CLPD6720_READ_WRITE);
    clpd6720_set_window(CLPD6720_SOCKET_1,
			CLPD6720_WINDOW_4,
			PCMCIA_ATTRIB_WINDOW_1_BASE,
			0,
			ATTRIB_WINDOW_SIZE,
			CLPD6720_ATTRIB_MEM,
			CLPD6720_WIDTH_16_BIT,
			CLPD6720_READ_WRITE);
    
    /*
     * set up the common windows at their default value
     */
    clpd6720_set_window(CLPD6720_SOCKET_0,
			CLPD6720_WINDOW_0,
			PCMCIA_WINDOW_0_BASE,
			0,
			WINDOW_SIZE,
			CLPD6720_COMMON_MEM,
			CLPD6720_WIDTH_16_BIT,
			CLPD6720_READ_WRITE);
    clpd6720_set_window(CLPD6720_SOCKET_1,
			CLPD6720_WINDOW_0,
			PCMCIA_WINDOW_1_BASE,
			0,
			WINDOW_SIZE,
			CLPD6720_COMMON_MEM,
			CLPD6720_WIDTH_16_BIT,
			CLPD6720_READ_WRITE);
    
    /*
     * enable all the windows that we are using
     */
    clpd6720_enable_window(CLPD6720_SOCKET_0,CLPD6720_WINDOW_0);
    clpd6720_enable_window(CLPD6720_SOCKET_0,CLPD6720_WINDOW_4);
    clpd6720_enable_window(CLPD6720_SOCKET_1,CLPD6720_WINDOW_0);
    clpd6720_enable_window(CLPD6720_SOCKET_1,CLPD6720_WINDOW_4);

    PRT(P16,("ralib_init() = done\n"));

    return(ralib);
}


/******************************************************************************
 * Name: ralib_get_dev_num
 *
 * Description: gets a device number, given a device ID
 *
 * Input: 
 *   device_id -- a pointer to the null-terminated character string that is
 *     the device ID
 *
 * Return Value:
 *   dev_num -- the device number for the device, or an error code.  All error
 *     codes are negative, and all valid device numbers are non-negative.
 *
 *******************************************************************************/
int ralib_get_dev_num (const char *device_id)
{
    int i;
    int dev_num;

    PRT(P16E,("ralib_get_dev_num(%s)\n",device_id));

    dev_num = RALIB_ERROR_INVALID_DEVICE_ID;
    for (i = 0; i <= RALIB_MAX_DEV_NUM; i++) {
	if (ralib_dev_table[i].valid == TRUE) {
	    if (strcmp(ralib_dev_table[i].device_id,device_id) == 0) {
		dev_num = i;
		break;
	    }
	}
    }

    PRT(P16X,("ralib_get_dev_num(%s) = %d\n",device_id,dev_num));

    return(dev_num);
}


/******************************************************************************
 * Name: ralib_get_device_names
 *
 * Description: gets a string that contains all the valid device names
 *
 * Input: None
 *
 * Return Value:
 *   device_name_ptr -- a pointer to a character string that contains
 *     the names of the supported device names (i.e the name of the
 *     devices that cause get_dev_num() to return a valid device
 *     number).  The names do not contain the colon (:) and are
 *     concatenated with separating spaces and null terminated.
 *     The device names are in no specific order. 
 *     The character string is a const, and must not be modified.
 *
 *     Example of return value.
 *       "slot0 slot1 bootflash"
 *
 *******************************************************************************/
const char *ralib_get_device_names(void)
{
    /*
     * this method could be improved upon, but this is easiest to implement 
     */
    return("slot0 slot1 bootflash");
}


/******************************************************************************
 * Name: set_window
 *
 * Description: sets the window for the PCMCIA card if it is not already set
 *
 * Input: 
 *   dr -- pointer to the device record for the PCMCIA slot (all the fields
 *     must reflect the current status of the device and the window)
 *   window_base_ptr -- a pointer into PCMCIA memory at which to set the window
 *     (must be a valid window base value)
 *
 * Return Value: None
 *
 *******************************************************************************/
static void set_window (ralib_dev_rec_t *dr, ulong window_base_ptr)
{
    
    if (window_base_ptr != dr->window_base_ptr) {
	dr->window_base_ptr = window_base_ptr;
	clpd6720_set_window(dr->socket,
			    dr->window,
			    dr->base_addr,
			    dr->window_base_ptr,
			    WINDOW_SIZE,
			    CLPD6720_COMMON_MEM,
			    CLPD6720_WIDTH_16_BIT,
			    CLPD6720_READ_WRITE);
    }
}


/******************************************************************************
 * Name: pcmcia_read
 *
 * Description: reads from a pcmcia socket using the window specified
 *
 * NOTES: 
 *   1. assumes the window timings have been set up correctly
 *   2. makes no assumption/restriction on alignment
 *   3. adjusts the windows to reach the PCMCIA address(es)
 *
 * Input: 
 *   dr -- pointer to the device record for the device (PCMCIA socket)
 *   flash_ptr -- offset into the file system device from which to read
 *     (this is the source pointer)
 *   buffer_ptr -- address of the buffer in which to put the data 
 *     (this is the destination pointer)
 *   length -- the number of bytes to read
 *
 * Return Value:
 *   actual -- the number of bytes actually read.
 *
 *******************************************************************************/
static int pcmcia_read (ralib_dev_rec_t *dr, ulong flash_ptr, char *buffer_ptr, int length)
{
    int actual = 0;
    ushort data;
    volatile ushort *data_ptr;
    ulong window_base_ptr;
    ulong window_offset;
    ulong bytes_left_this_window;
    
    PRT(P19E,("pcmcia_read(%s,%lx,%lx,%d)\n",dr->device_id,flash_ptr,buffer_ptr,length));

    /*
     * Handle the first byte copy if it is an unaligned flash address.
     * Read the 16-bit value from the aligned address, and then copy
     * the byte to the buffer and increment all the pointers.
     */
    if (flash_ptr & 1) {
	/*
	 * Set up the window for the first byte (if it is not already set)
	 */
	window_base_ptr = (flash_ptr / WINDOW_SIZE) * WINDOW_SIZE;
	set_window(dr,window_base_ptr);
	data_ptr = (ushort *)(dr->base_addr + ((flash_ptr & 0xfffffffe) - window_base_ptr));
	data = *data_ptr;
	data &= 0x00ff;
	*(uchar *)buffer_ptr = (uchar)data;
	
	PRT(PRDATA,("RD @ %lx, wr @ %lx, DATA=%x\n",data_ptr,buffer_ptr,data));

	flash_ptr += sizeof(uchar);
	buffer_ptr += sizeof(uchar);
	length -= sizeof(uchar);
	actual += sizeof(uchar);
    }
	
    /*
     * Main copy loop: copy 16-bit values until there are 0 or 1 bytes left
     * to copy.  The flash pointer is now aligned to a 16-bit boundary.
     * Remember to adjust the PCMCIA window as we cross 4M byte boundaries.
     */
    while (length >= sizeof(ushort)) {
	/*
	 * Calculate how many bytes are reachable via this window before
	 * we have to adjust the mapping.
	 */
	window_base_ptr = (flash_ptr / WINDOW_SIZE) * WINDOW_SIZE;
	set_window(dr,window_base_ptr);
	window_offset = flash_ptr - window_base_ptr;
	data_ptr = (ushort *)(dr->base_addr + window_offset);
	bytes_left_this_window = WINDOW_SIZE - window_offset;
	if (length < bytes_left_this_window)
	    bytes_left_this_window = length;

	/*
	 * while there is still a 16-bit (2-byte) quantity left to read, read
         * the data from the flash and copy it to the buffer.
	 */
	while (bytes_left_this_window >= sizeof(ushort)) {
	    data = *data_ptr;
	    PUTSHORT((ushort *)buffer_ptr,data);
	
	    PRT(PRDATA,("RD @ %lx, wr @ %lx, DATA=%4x\n",data_ptr,buffer_ptr,data));

	    flash_ptr += sizeof(ushort);
	    data_ptr++;
	    buffer_ptr += sizeof(ushort);
	    length -= sizeof(ushort);
	    bytes_left_this_window -= sizeof(ushort);
	    actual += sizeof(ushort);
	    }
	}

    /*
     * Handle the last byte copy if there is still one byte left.
     * Read the 16 bit value and copy only the MSB to the buffer;
     */
    if (length > 0) {
	/*
	 * Set up the window for the first byte (if it is not already set)
	 */
	window_base_ptr = (flash_ptr / WINDOW_SIZE) * WINDOW_SIZE;
	set_window(dr,window_base_ptr);
	data_ptr = (ushort *)(dr->base_addr + (flash_ptr - window_base_ptr));
	data = *data_ptr;
	data >>= 8;
	*(uchar *)buffer_ptr = (uchar)data;
	
	PRT(PRDATA,("RD @ %lx, wr @ %lx, DATA=%x\n",data_ptr,buffer_ptr,data));

	flash_ptr += sizeof(uchar);
	buffer_ptr += sizeof(uchar);
	length -= sizeof(uchar);
	actual += sizeof(uchar);
    }
	

    PRT(P19X,("pcmcia_read(%s,%lx,%lx,%d) = %d\n",dr->device_id,flash_ptr,buffer_ptr,length,actual));

    return(actual);
}


/******************************************************************************
 * Name: pcmcia_write
 *
 * Description: writes to a pcmcia socket using the window specified
 *
 * NOTES: 
 *   1. assumes the window timings have been set up correctly
 *   2. makes no assumption/restriction on alignment
 *   3. adjusts the windows to reach the PCMCIA address(es)
 *
 * Input: 
 *   dr -- pointer to the device record for the device (PCMCIA socket)
 *   window -- which window to use (CLPD6720_WINDOW_X)
 *   flash_ptr -- offset into the file system device to which to write
 *     (this is the source pointer)
 *   buffer_ptr -- address of the buffer containing the data to write
 *     (this is the destination pointer)
 *   length -- the number of bytes to read
 *
 * Return Value:
 *   actual -- the number of bytes actually read.
 *
 *******************************************************************************/
static int pcmcia_write (ralib_dev_rec_t *dr, char *buffer_ptr, ulong flash_ptr, int length)
{
    int actual = 0;
    ushort data;
    volatile ushort *data_ptr;
    ulong window_base_ptr;
    ulong window_offset;
    ulong bytes_left_this_window;

    PRT(P20E,("pcmcia_write(%s,%lx,%lx,%d)\n",dr->device_id,buffer_ptr,flash_ptr,length));

    /*
     * Handle the first byte copy if it is an unaligned flash address.
     * Read the 16-bit value from the aligned address, and then copy
     * the byte to the buffer and increment all the pointers.
     */
    if (flash_ptr & 1) {
	/*
	 * Set up the window for the first byte (if it is not already set)
	 */
	window_base_ptr = (flash_ptr / WINDOW_SIZE) * WINDOW_SIZE;
	set_window(dr,window_base_ptr);
	data = *(uchar *)buffer_ptr;
	data |= 0xff00; /* XXX jack, this assumes the erased state is ff */
	data_ptr = (ushort *)(dr->base_addr + ((flash_ptr & 0xfffffffe) - window_base_ptr));
	*data_ptr = data;
	
	PRT(PWDATA,("rd @ %lx, WR @ %lx, DATA=%4x\n",buffer_ptr,data_ptr,data));

	flash_ptr += sizeof(uchar);
	buffer_ptr += sizeof(uchar);
	length -= sizeof(uchar);
	actual += sizeof(uchar);
    }
	

    /*
     * Main copy loop: copy 16-bit values until there are 0 or 1 bytes left
     * to copy.  The flash pointer is now aligned to a 16-bit boundary.
     * Remember to adjust the PCMCIA window as we cross 4M byte boundaries.
     */
    while (length >= sizeof(ushort)) {
	/*
	 * Calculate how many bytes are reachable via this window before
	 * we have to adjust the mapping.
	 */
	window_base_ptr = (flash_ptr / WINDOW_SIZE) * WINDOW_SIZE;
	set_window(dr,window_base_ptr);
	window_offset = flash_ptr - window_base_ptr;
	data_ptr = (ushort *)(dr->base_addr + window_offset);
	bytes_left_this_window = WINDOW_SIZE - window_offset;
	if (length < bytes_left_this_window)
	    bytes_left_this_window = length;

	/*
	 * while there is still a 16-bit (2-byte) quantity left to read, read
         * the data from the flash and copy it to the buffer.
	 */
	while (bytes_left_this_window >= sizeof(ushort)) {
	    data = GETSHORT((ushort *)buffer_ptr);
	    *data_ptr = data;
	
	    PRT(PWDATA,("rd @ %lx, WR @ %lx, DATA=%4x\n",buffer_ptr,data_ptr,data));

	    data_ptr++;
	    flash_ptr += sizeof(ushort);
	    window_offset += sizeof(ushort);
	    buffer_ptr += sizeof(ushort);
	    length -= sizeof(ushort);
	    bytes_left_this_window -= sizeof(ushort);
	    actual += sizeof(ushort);
	    }
	}

    /*
     * Handle the last byte copy if there is still one byte left.
     * Read the 16 bit value and copy only the MSB to the buffer;
     */
    if (length > 0) {
	/*
	 * Set up the window for the first byte (if it is not already set)
	 */
	window_base_ptr = (flash_ptr / WINDOW_SIZE) * WINDOW_SIZE;
	set_window(dr,window_base_ptr);
	data = (*(uchar *)buffer_ptr) << 8;
	data |= 0x00ff; /* XXX jack, this assumes the erased state is ff */
	data_ptr = (ushort *)(dr->base_addr + (flash_ptr - window_base_ptr));
	*data_ptr = data;
	
	PRT(PWDATA,("rd @ %lx, WR @ %lx, DATA=%4x\n",buffer_ptr,data_ptr,data));

	flash_ptr += sizeof(uchar);
	buffer_ptr += sizeof(uchar);
	length -= sizeof(uchar);
	actual += sizeof(uchar);
    }
	
    PRT(P20X,("pcmcia_write(%s,%lx,%lx,%d) = %d\n",dr->device_id,buffer_ptr,flash_ptr,length,actual));

    return(actual);
}


/******************************************************************************
 * Name: pcmcia_attrib_read
 *
 * Description: reads from a pcmcia attrib memory
 *
 * NOTES: 
 *   1. assumes the window timings have been set up correctly
 *   2. makes no assumption/restriction on alignment
 *   3. adjusts the windows to reach the PCMCIA address(es)
 *
 * Input: 
 *   dr -- pointer to the device record for the device (PCMCIA socket)
 *   flash_ptr -- offset into the file system device from which to read
 *     (this is the source pointer)
 *     This must be an even offset less than 1MB.
 *   buffer_ptr -- address of the buffer in which to put the data 
 *     (this is the destination pointer)
 *   length -- the number of bytes to read
 *
 * Return Value:
 *   actual -- the number of bytes actually read.
 *
 *******************************************************************************/
static int pcmcia_attrib_read (ralib_dev_rec_t *dr, ulong flash_ptr, char *buffer_ptr, int length)
{
    int actual = 0;
    ushort data;
    volatile ushort *data_ptr;
    
    PRT(P19E,("pcmcia_attrib_read(%s,%lx,%lx,%d)\n",dr->device_id,flash_ptr,buffer_ptr,length));

    /*
     * Since attrib memory only has even bytes represented, it doesn't make
     * any sense to allow odd byte starts.
     */
    if (flash_ptr & 1) {
	return(0);
    }
	
    /*
     * We only allow access of up to the first 1MB of attrib memory
     */
    if ((flash_ptr + length) >= ATTRIB_WINDOW_SIZE) {
	return(0);
    }
	
    /*
     * Main copy loop: copy 16-bit values until there are 0 or 1 bytes left
     * to copy.  The flash pointer is now aligned to a 16-bit boundary.
     * Remember to adjust the PCMCIA window as we cross 4M byte boundaries.
     */
    data_ptr = (ushort *)(dr->attrib_base_addr + flash_ptr);
    while (length >= sizeof(ushort)) {
	data = *data_ptr;
	PUTSHORT((ushort *)buffer_ptr,data);
	
	PRT(PRDATA,("RD @ %lx, wr @ %lx, DATA=%4x\n",data_ptr,buffer_ptr,data));
	
	data_ptr++;
	flash_ptr += sizeof(ushort);
	buffer_ptr += sizeof(ushort);
	length -= sizeof(ushort);
	actual += sizeof(ushort);
    }

    /*
     * Handle the last byte copy if there is still one byte left.
     * Read the 16 bit value and copy only the MSB to the buffer;
     */
    if (length > 0) {
	data_ptr = (ushort *)(dr->attrib_base_addr + flash_ptr);
	data = *data_ptr;
	data >>= 8;
	*(uchar *)buffer_ptr = (uchar)data;
	
	PRT(PRDATA,("RD @ %lx, wr @ %lx, DATA=%2x\n",data_ptr,buffer_ptr,data));

	flash_ptr += sizeof(uchar);
	buffer_ptr += sizeof(uchar);
	length -= sizeof(uchar);
	actual += sizeof(uchar);
    }

    PRT(P19X,("pcmcia_attrib_read(%s,%lx,%lx,%d) = %d\n",dr->device_id,flash_ptr,buffer_ptr,length,actual));

    return(actual);
}


/******************************************************************************
 * Name: pcmcia_attrib_write
 *
 * Description: writes to a pcmcia attrib memory
 *
 * NOTES: 
 *   1. assumes the window timings have been set up correctly
 *   2. makes no assumption/restriction on alignment
 *   3. adjusts the windows to reach the PCMCIA address(es)
 *
 * Input: 
 *   dr -- pointer to the device record for the device (PCMCIA socket)
 *   window -- which window to use (CLPD6720_WINDOW_X)
 *   flash_ptr -- offset into the file system device to which to write
 *     (this is the source pointer)
 *   buffer_ptr -- address of the buffer containing the data to write
 *     (this is the destination pointer)
 *   length -- the number of bytes to read
 *
 * Return Value:
 *   actual -- the number of bytes actually read.
 *
 *******************************************************************************/
static int pcmcia_attrib_write (ralib_dev_rec_t *dr, char *buffer_ptr, ulong flash_ptr, int length)
{
    int actual = 0;
    ushort data;
    volatile ushort *data_ptr;

    PRT(P20E,("pcmcia_attrib_write(%s,%lx,%lx,%d)\n",dr->device_id,buffer_ptr,flash_ptr,length));

    /*
     * Since attrib memory only has even bytes represented, it doesn't make
     * any sense to allow odd byte starts.
     */
    if (flash_ptr & 1) {
	return(0);
    }
	
    /*
     * We only allow access of up to the first 1MB of attrib memory
     */
    if ((flash_ptr + length) >= ATTRIB_WINDOW_SIZE) {
	return(0);
    }
	
    /*
     * Main copy loop: copy 16-bit values until there are 0 or 1 bytes left
     * to copy.  The flash pointer is now aligned to a 16-bit boundary.
     * Remember to adjust the PCMCIA window as we cross 4M byte boundaries.
     */
    data_ptr = (ushort *)(dr->attrib_base_addr + flash_ptr);
    while (length >= sizeof(ushort)) {
	data = GETSHORT((ushort *)buffer_ptr);
	*data_ptr = data;
	
	PRT(PWDATA,("rd @ %lx, WR @ %lx, DATA=%4x\n",buffer_ptr,data_ptr,data));

	data_ptr++;
	flash_ptr += sizeof(ushort);
	buffer_ptr += sizeof(ushort);
	length -= sizeof(ushort);
	actual += sizeof(ushort);
    }

    /*
     * Handle the last byte copy if there is still one byte left.
     * Read the 16 bit value and copy only the MSB to the buffer;
     */
    if (length > 0) {
	data = (*(uchar *)buffer_ptr) << 8;
	data_ptr = (ushort *)(dr->attrib_base_addr + flash_ptr);
	*data_ptr = data;
	
	PRT(PWDATA,("rd @ %lx, WR @ %lx, DATA=%4x\n",buffer_ptr,data_ptr,data));

	flash_ptr += sizeof(uchar);
	buffer_ptr += sizeof(uchar);
	length -= sizeof(uchar);
	actual += sizeof(uchar);
    }
	
    PRT(P20X,("pcmcia_attrib_write(%s,%lx,%lx,%d) = %d\n",dr->device_id,buffer_ptr,flash_ptr,length,actual));

    return(actual);
}


/******************************************************************************
 * Name: flash32_read
 *
 * Description: reads from internal flash bank assuming 32-bit access only
 *
 * NOTES: 
 *   1. makes no assumption/restriction on alignment
 *
 * Input: 
 *   flash_ptr -- offset into the file system device from which to read
 *     (this is the source pointer)
 *   buffer_ptr -- address of the buffer in which to put the data 
 *     (this is the destination pointer)
 *   length -- the number of bytes to read
 *
 * Return Value:
 *   actual -- the number of bytes actually read.
 *
 *******************************************************************************/
static int flash32_read (ralib_dev_rec_t *dr, ulong flash_ptr, char *buffer_ptr, int length)
{
    int actual = 0;
    ulong data;
    ulong *data_ptr;
    ulong flash_ptr_low_bits;
    int first_length;
    int i;
    uchar byte_data;

    PRT(P19E,("flash32_read(%s,%lx,%lx,%d)\n",dr->device_id,flash_ptr,buffer_ptr,length));

    /*
     * Handle the first byte copy if it is an unaligned flash address.
     * Read the 16-bit value from the aligned address, and then copy
     * the byte to the buffer and increment all the pointers.
     */
    data_ptr = (ulong *)((flash_ptr + dr->base_addr) & 0xfffffffc);
    if (flash_ptr & 0x00000003) {
	data = *data_ptr;
	flash_ptr_low_bits = flash_ptr & 0x00000003;
	first_length = 4 - flash_ptr_low_bits;
	data <<= (8 * flash_ptr_low_bits);
	
	/*
	 * make sure we don't transfer more bytes than were requested.
         */
	if (first_length > length) {
	    first_length = length;
	}
	
	/*
	 * now copy the bytes from the data to the buffer and adjust the
	 * pointers 
	 */
	for (i = 0; i < first_length; i++) {
	    byte_data = (uchar)(data >> 24);
	    *(uchar *)buffer_ptr = byte_data;
	
	    PRT(PRDATA,("RD.@ %lx, wr @ %lx, DATA=%x\n",data_ptr,buffer_ptr,byte_data));

	    data <<= 8;
	    buffer_ptr += sizeof(uchar);
	    flash_ptr += sizeof(uchar);
	    length -= sizeof(uchar);
	    actual += sizeof(uchar);
	}
	/*
	 * increment the data pointer here after the debug print
	 * statement so the print message is right
	 */
	data_ptr++;
    }
	

    /*
     * Main copy loop: copy 32-bit values until there are 0, 1, 2 or 3 bytes left
     * to copy.  The flash pointer is now aligned to a 32-bit boundary.
     */
    while (length >= sizeof(ulong)) {
	data = *data_ptr;
	PUTLONG((ulong *)buffer_ptr,data);
	
	PRT(PRDATA,("RD @ %lx, wr @ %lx, DATA=%lx\n",data_ptr,buffer_ptr,data));

	data_ptr++;
	buffer_ptr += sizeof(ulong);
	length -= sizeof(ulong);
	actual += sizeof(ulong);
    }

    /*
     * Handle the last byte copy if there is still one byte left.
     * Read the 16 bit value and copy only the MSB to the buffer;
     *
     * There are three cases:
     *
     *             MSB                               LSB
     *             +--------+--------+--------+--------+
     * length = 1  | data 1 |        |        |        |
     *             +--------+--------+--------+--------+
     *             +--------+--------+--------+--------+
     * length = 2  | data 1 | data 2 |        |        |
     *             +--------+--------+--------+--------+
     *             +--------+--------+--------+--------+
     * length = 3  | data 1 | data 2 | data 3 |        |
     *             +--------+--------+--------+--------+
     */
    if (length > 0) {
	data = *data_ptr;
	switch (length) {
	case 1: 
	case 2: 
	case 3:
	    /*
	     * Do the first byte
             */
	    byte_data = (uchar)(data >> 24);
	    *(uchar *)buffer_ptr = byte_data;
	
	    PRT(PRDATA,("RD @.%lx, wr @ %lx, DATA=%x\n",data_ptr,buffer_ptr,byte_data));

	    length -= sizeof(uchar);
	    actual += sizeof(uchar);
	    buffer_ptr += sizeof(uchar);
	    if (length == 0) {
		break;
	    }

	    /*
	     * Do the second byte if necessary
             */
	    byte_data = (uchar)(data >> 16);
	    *(uchar *)buffer_ptr = byte_data;
	
	    PRT(PRDATA,("RD @.%lx, wr @ %lx, DATA=%x\n",data_ptr,buffer_ptr,byte_data));

	    length -= sizeof(uchar);
	    actual += sizeof(uchar);
	    buffer_ptr += sizeof(uchar);
	    if (length == 0) {
		break;
	    }

	    /*
	     * Do the third byte if necessary
             */
	    byte_data = (uchar)(data >> 8);
	    *(uchar *)buffer_ptr = byte_data;

	    PRT(PRDATA,("RD @.%lx, wr @ %lx, DATA=%x\n",data_ptr,buffer_ptr,byte_data));

	    length -= sizeof(uchar);
	    actual += sizeof(uchar);
	    buffer_ptr += sizeof(uchar);
	    break;
	}
    }
	
    PRT(P19X,("flash32_read(%s,%lx,%lx,%d) = %d\n",dr->device_id,flash_ptr,buffer_ptr,length,actual));

    return(actual);
}


/******************************************************************************
 * Name: flash32_write
 *
 * Description: writes to internal flash bank assuming 32-bit access only
 *
 * NOTES: 
 *   1. makes no assumption/restriction on alignment
 *
 * Input: 
 *   dr -- pointer to the device record for the internal flash bank
 *   buffer_ptr -- address of the buffer containing the data to write
 *     to flash
 *     (this is the source pointer)
 *   flash_ptr -- offset into the file system device to which to write
 *     (this is the destination pointer)
 *   length -- the number of bytes to write
 *
 * Return Value:
 *   actual -- the number of bytes actually written
 *
 *******************************************************************************/
static int flash32_write (ralib_dev_rec_t *dr, char *buffer_ptr, ulong flash_ptr, int length)
{
    int actual = 0;
    ulong data;
    ulong *data_ptr;
    ulong flash_ptr_low_bits;
    int first_length;
    ulong mask;

    PRT(P20E,("flash32_write(%s,%lx,%lx,%d)\n",dr->device_id,buffer_ptr,flash_ptr,length));

    /*
     * Handle the first write if it is an unaligned flash address (and
     * therefore a partial transfer)
     *
     * the following table shows the value written to the flash (assuming the
     * first long word (32-bits) is 0x12345678) based on the alignment and
     * length of the transfer
     *
     *                      +------------+------------+------------+------------+
     *                      |                    L E N G T H                    | 
     *                      +------------+------------+------------+------------+
     * Flash Addr MOD 4     |     1      |     2      |     3      |     >=4    |
     * (flash_ptr_low_bits) +------------+------------+------------+------------+
     *                   0  | no action  | no action  | no action  | no action  |
     *                      +------------+------------+------------+------------+
     *                   1  | 0xff12ffff | 0xff1234ff | 0xff123456 | 0xff123456 |
     *                      +------------+------------+------------+------------+
     *                   2  | 0xffff12ff | 0xffff1234 | 0xffff1234 | 0xffff1234 |
     *                      +------------+------------+------------+------------+
     *                   3  | 0xffffff12 | 0xffffff12 | 0xffffff12 | 0xffffff12 |
     *                      +------------+------------+------------+------------+
     */
    data_ptr = (ulong *)((flash_ptr + dr->base_addr) & 0xfffffffc);
    flash_ptr_low_bits = flash_ptr & 0x00000003;
    if (flash_ptr_low_bits != 0) {
	data = GETLONG((ulong *)buffer_ptr);
        data >>= (8 * flash_ptr_low_bits);
	/*
	 * Calculate the mask for the byte(s) we want based on length
	 * and alignment of the flash_ptr;
	 */
	switch (flash_ptr_low_bits) {
	case 1:
	    if (length == 1) {
		mask = 0x00ff0000;
		first_length = 1;
	    } else if (length == 2) {
		mask = 0x00ffff00;
		first_length = 2;
	    } else {
		mask = 0x00ffffff;
		first_length = 3;
	    }
	    break;
	case 2:
	    if (length == 1) {
		mask = 0x0000ff00;
		first_length = 1;
	    } else {
		mask = 0x0000ffff;
		first_length = 2;
	    }
	    break;
	case 3:
	    first_length = 1;
	    mask = 0x000000ff;
	    break;
	default: /* to keep the compiler quiet */
	    first_length = 0;
	    mask = 0;
	    break;
	}
	/*
	 * Set the byte(s) we don't want to touch to 0xff (this
	 * assumes that the erased state of flash is 0xff (!!!)
	 */
	data |= ~mask; /* XXX jack, this assumes the erased state is ff */
	*data_ptr = data;
	
	PRT(PRDATA,("RD @ %lx, WR.@ %lx, DATA=%lx\n",buffer_ptr,data_ptr,data));

	data_ptr++;
	buffer_ptr += first_length;
	length -= first_length;
	actual += first_length;
	
    }

    /*
     * Main write loop: copy 32-bit values until there are 0, 1, 2 or 3 bytes left
     * to copy.  The flash pointer is now aligned to a 32-bit boundary.
     */
    while (length >= sizeof(ulong)) {
	data = GETLONG((ulong *)buffer_ptr);
	*data_ptr = data;
	
	PRT(PWDATA,("rd @ %lx, WR @ %lx, DATA=%lx\n",buffer_ptr,data_ptr,data));

	data_ptr++;
	buffer_ptr += sizeof(ulong);
	length -= sizeof(ulong);
	actual += sizeof(ulong);
    }

    /*
     * Handle the last write if there is still data left.
     * Read the 32 bit value and write only the remaining number of bytes to
     * flash.
     */
    if (length > 0) {
	data = GETLONG((ulong *)buffer_ptr);
	/*
	 * Calculate the mask for the byte(s) we want based on length
	 */
	mask = ~(0xffffffff >> (8 * length));
	/*
	 * Set the byte(s) we don't want to touch to 0xff (this
	 * assumes that the erased state of flash is 0xff (!!!)
	 */
	data |= ~mask; /* XXX jack, this assumes the erased state is ff */
	*data_ptr = data;
	
	PRT(PWDATA,("rd @ %lx, WR @.%lx, DATA=%lx\n",buffer_ptr,data_ptr,data));

	data_ptr++;
	buffer_ptr += length;
	actual += length;
	length -= length;
    }

    PRT(P20X,("flash32_write(%s,%lx,%lx,%d) = %d\n",dr->device_id,buffer_ptr,flash_ptr,length,actual));

    return(actual);
}


/******************************************************************************
 * Name: ralib_read
 *
 * Description: reads from a file system (issues read cycles)
 *
 * Input: 
 *   dev_num -- the device number of the file system from which to read
 *   flash_ptr -- offset into the file system device from which to read
 *     (this is the source pointer)
 *   buffer_ptr -- address of the buffer in which to put the data 
 *     (this is the destination pointer)
 *   length -- the number of bytes to read
 *
 * Return Value:
 *   actual -- the number of bytes actually read, or an error code.  All
 *     error codes are negative.
 *
 *******************************************************************************/
int ralib_read (int dev_num, ulong flash_ptr, void *buffer_ptr, int length)
{
    int actual;
    ralib_dev_rec_t *dr;

    PRT(P17E,("ralib_read(%d,%lx,%lx,%d)\n",dev_num,flash_ptr,buffer_ptr,length));

    if (dev_num > RALIB_MAX_DEV_NUM) {
	actual = RALIB_ERROR_INVALID_DEV_NUM;
    } else {
	dr = &ralib_dev_table[dev_num];
	if (dr->valid == FALSE) {
	    actual = RALIB_ERROR_INVALID_DEV_NUM;
	} else {
	    clpd6720_mgmt_intr_handler(); /* XXX jack, fix this later */
	    if (dr->present == FALSE) {
		actual = RALIB_ERROR_NO_DEVICE;
	    } else {
		if (dr->initialized == FALSE) {
		    actual = RALIB_ERROR_DEVICE_NEEDS_INIT;
		} else {
		    if (dr->ready == FALSE) {
			actual = RALIB_ERROR_DEVICE_NOT_READY;
		    } else {
			/*
			 * The actual read is very device dependant
			 */
			switch (dev_num) {
			case RALIB_DEV_NUM_PCMCIA_SOCKET_0:
			case RALIB_DEV_NUM_PCMCIA_SOCKET_1:
			    actual = pcmcia_read(dr,flash_ptr,buffer_ptr,length);
			    break;
			case RALIB_DEV_NUM_INTERNAL_FLASH:
			    actual = flash32_read(dr,flash_ptr,buffer_ptr,length);
			    break;
			default:
			    actual = 0;
			    break;
			}
		    }
		}
	    }
	}
    }

    PRT(P17X,("ralib_read(%d,%lx,%lx,%d) = %d\n",dev_num,flash_ptr,buffer_ptr,length,actual));

    return(actual);
}


/******************************************************************************
 * Name: ralib_write
 *
 * Description: writes to a file system (issues write cycles)
 *
 * Input: 
 *   dev_num -- the device number of the file system from which to read
 *   buffer_ptr -- address of the buffer in which to put the data 
 *     (this is the destination pointer)
 *   flash_ptr -- offset into the file system device from which to read
 *     (this is the source pointer)
 *   length -- the number of bytes to read
 *
 * Return Value:
 *   actual -- the number of bytes actually written.  If there was an error while
 *     writing the file, an error code is returned.  All error codes are
 *     negative, and all valid values for actual are positive.
 *
 *******************************************************************************/
int ralib_write (int dev_num, void *buffer_ptr, ulong flash_ptr, int length)
{
    int   actual;
    ralib_dev_rec_t *dr;

    PRT(P18E,("ralib_write(%d,%lx,%lx,%d)\n",dev_num,buffer_ptr,flash_ptr,length));

    actual = 0;
    if (dev_num > RALIB_MAX_DEV_NUM) {
	actual = RALIB_ERROR_INVALID_DEV_NUM;
    } else {
	dr = &ralib_dev_table[dev_num];
	if (dr->valid == FALSE) {
	    actual = RALIB_ERROR_INVALID_DEV_NUM;
	} else {
	    clpd6720_mgmt_intr_handler(); /* XXX jack, fix this later */
	    if (dr->present == FALSE) {
		actual = RALIB_ERROR_NO_DEVICE;
	    } else {
		if (dr->initialized == FALSE) {
		    actual = RALIB_ERROR_DEVICE_NEEDS_INIT;
		} else {
		    if (dr->ready == FALSE) {
			actual = RALIB_ERROR_DEVICE_NOT_READY;
		    } else {
			/*
			 * The actual write is very device dependant
			 */
			switch (dev_num) {
			case RALIB_DEV_NUM_PCMCIA_SOCKET_0:
			case RALIB_DEV_NUM_PCMCIA_SOCKET_1:
			    actual = pcmcia_write(dr,buffer_ptr,flash_ptr,length);
			    break;
			case RALIB_DEV_NUM_INTERNAL_FLASH:
			    actual = flash32_write(dr,buffer_ptr,flash_ptr,length);
			    break;
			default:
			    actual = 0;
			    break;
			}
		    }
		}
	    }
	}
    }

    PRT(P18X,("ralib_write(%d,%lx,%lx,%d) = %d\n",dev_num,buffer_ptr,flash_ptr,length,actual));

    return(actual);
}


/******************************************************************************
 * Name: ralib_attrib_read
 *
 * Description: reads from attribute memory for PCMCIA devices (issues read cycles)
 *
 * Input: 
 *   dev_num -- the device number of the file system from which to read
 *   flash_ptr -- offset into the attribute memory space from which to read
 *     (this is the source pointer)
 *     This is limited to only the first 1MB of the attrib memory space.
 *   buffer_ptr -- address of the buffer in which to put the data 
 *     (this is the destination pointer)
 *   length -- the number of bytes to read
 *
 * Return Value:
 *   actual -- the number of bytes actually read, or an error code.  All
 *     error codes are negative.
 *
 *******************************************************************************/
int ralib_attrib_read (int dev_num, ulong flash_ptr, void *buffer_ptr, int length)
{
    int actual;
    ralib_dev_rec_t *dr;

    PRT(P17E,("ralib_attrib_read(%d,%lx,%lx,%d)\n",dev_num,flash_ptr,buffer_ptr,length));

    if (dev_num > RALIB_MAX_DEV_NUM) {
	actual = RALIB_ERROR_INVALID_DEV_NUM;
    } else {
	dr = &ralib_dev_table[dev_num];
	if (dr->valid == FALSE) {
	    actual = RALIB_ERROR_INVALID_DEV_NUM;
	} else {
	    clpd6720_mgmt_intr_handler(); /* XXX jack, fix this later */
	    if (dr->present == FALSE) {
		actual = RALIB_ERROR_NO_DEVICE;
	    } else {
		if (dr->initialized == FALSE) {
		    actual = RALIB_ERROR_DEVICE_NEEDS_INIT;
		} else {
		    if (dr->ready == FALSE) {
			actual = RALIB_ERROR_DEVICE_NOT_READY;
		    } else {
			/*
			 * The actual read is very device dependant
			 */
			switch (dev_num) {
			case RALIB_DEV_NUM_PCMCIA_SOCKET_0:
			case RALIB_DEV_NUM_PCMCIA_SOCKET_1:
			    actual = pcmcia_attrib_read(dr,flash_ptr,buffer_ptr,length);
			    break;
			case RALIB_DEV_NUM_INTERNAL_FLASH:
			default:
			    actual = 0;
			    break;
			}
		    }
		}
	    }
	}
    }

    PRT(P17X,("ralib_attrib_read(%d,%lx,%lx,%d) = %d\n",dev_num,flash_ptr,buffer_ptr,length,actual));

    return(actual);
}


/******************************************************************************
 * Name: ralib_attrib_write
 *
 * Description: writes to a file system (issues write cycles)
 *
 * Input: 
 *   dev_num -- the device number of the file system from which to read
 *   buffer_ptr -- address of the buffer in which to put the data 
 *     (this is the destination pointer)
 *   flash_ptr -- offset into attrib memory to which to write
 *     (this is the source pointer)
 *     This is limited to the first 1MB of attrib memory.
 *   length -- the number of bytes to read
 *
 * Return Value:
 *   actual -- the number of bytes actually written.  If there was an error while
 *     writing the file, an error code is returned.  All error codes are
 *     negative, and all valid values for actual are positive.
 *
 *******************************************************************************/
int ralib_attrib_write (int dev_num, void *buffer_ptr, ulong flash_ptr, int length)
{
    int   actual;
    ralib_dev_rec_t *dr;

    PRT(P18E,("ralib_attrib_write(%d,%lx,%lx,%d)\n",dev_num,buffer_ptr,flash_ptr,length));

    actual = 0;
    if (dev_num > RALIB_MAX_DEV_NUM) {
	actual = RALIB_ERROR_INVALID_DEV_NUM;
    } else {
	dr = &ralib_dev_table[dev_num];
	if (dr->valid == FALSE) {
	    actual = RALIB_ERROR_INVALID_DEV_NUM;
	} else {
	    clpd6720_mgmt_intr_handler(); /* XXX jack, fix this later */
	    if (dr->present == FALSE) {
		actual = RALIB_ERROR_NO_DEVICE;
	    } else {
		if (dr->initialized == FALSE) {
		    actual = RALIB_ERROR_DEVICE_NEEDS_INIT;
		} else {
		    if (dr->ready == FALSE) {
			actual = RALIB_ERROR_DEVICE_NOT_READY;
		    } else {
			/*
			 * The actual write is very device dependant
			 */
			switch (dev_num) {
			case RALIB_DEV_NUM_PCMCIA_SOCKET_0:
			case RALIB_DEV_NUM_PCMCIA_SOCKET_1:
			    actual = pcmcia_attrib_write(dr,buffer_ptr,flash_ptr,length);
			    break;
			case RALIB_DEV_NUM_INTERNAL_FLASH:
			default:
			    actual = 0;
			    break;
			}
		    }
		}
	    }
	}
    }

    PRT(P18X,("ralib_attrib_write(%d,%lx,%lx,%d) = %d\n",dev_num,buffer_ptr,flash_ptr,length,actual));

    return(actual);
}


/******************************************************************************
 * Name: ralib_device_init
 *
 * Description: initializes a device
 *
 * Input: 
 *   dev_num -- the device number of the device to initialize
 *
 * Return Value:
 *   return code -- returns the state of the device.
 *
 *******************************************************************************/
int ralib_device_init (int dev_num)
{
    int   rc;
    ralib_dev_rec_t *dr;
 
    PRT(P16E,("ralib_device_init(%d)\n",dev_num));

    if (dev_num > RALIB_MAX_DEV_NUM) {
	rc = RALIB_ERROR_INVALID_DEV_NUM;
    } else {
	dr = &ralib_dev_table[dev_num];
	if (dr->valid == FALSE) {
	    rc = RALIB_ERROR_INVALID_DEV_NUM;
	} else {
	    clpd6720_mgmt_intr_handler(); /* XXX jack, fix this later */
	    if (dr->present == FALSE) {
		rc = RALIB_ERROR_NO_DEVICE;
	    } else {
		if (dr->initialized == FALSE) {
		    dr->initialized = TRUE;
		    rc = RALIB_DEVICE_FIRST_OK;
		} else {
		    rc = RALIB_DEVICE_STILL_OK;
		}
	    }
	}
    }

    PRT(P16X,("ralib_device_init(%d) = %d\n",dev_num,rc));

    return(rc);
}


/******************************************************************************
 * Name: ralib_device_reset
 *
 * Description: resets the device status so device_init returns FSLIB_FIRST_OK
 *
 * Input: 
 *   dev_num -- the device number of the device to reset
 *
 * Return Value:
 *   return code -- returns the state of the device.
 *
 *******************************************************************************/
int ralib_device_reset (int dev_num)
{
    int   rc;
    ralib_dev_rec_t *dr;
 
    PRT(P16E,("ralib_device_reset(%d)\n",dev_num));

    if (dev_num > RALIB_MAX_DEV_NUM) {
	rc = RALIB_ERROR_INVALID_DEV_NUM;
    } else {
	dr = &ralib_dev_table[dev_num];
	if (dr->valid == FALSE) {
	    rc = RALIB_ERROR_INVALID_DEV_NUM;
	} else {
	    clpd6720_mgmt_intr_handler(); /* XXX jack, fix this later */
	    if (dr->present == FALSE) {
		rc = RALIB_ERROR_NO_DEVICE;
	    } else {
		dr->initialized = FALSE;
#ifdef DEBUG
		if (dr->initialized == TRUE) {
		    buginf("\nDevice %d reset: initialized set to false\n",
		       dev_num);
		}
#endif
		rc = RALIB_OK;
	    }
	}
    }

    PRT(P16X,("ralib_device_reset(%d) = %d\n",dev_num,rc));

    return(rc);
}


/******************************************************************************
 * Name: ralib_time
 *
 * Description: returns the time of day
 *
 * Input: None
 *
 * Return Value:
 *   time -- the time of day (in Unix time?)
 *
 *******************************************************************************/
ulong ralib_time (void)
{
    int now;

#if defined(FSLIB) || defined(RSP_ROM)
    now = 0;
#else
    now = unix_time();
#endif

    PRT(P16,("ralib_get_time() = %d\n",now));

    return(now);
}


/******************************************************************************
 * Name: ralib_yield
 *
 * Description: allows other processes to run (no-op here)
 *
 * Input: 
 *   microseconds -- number of microseconds to yield for 
 *
 * Return Value: None
 *
 *******************************************************************************/
void ralib_yield (int microseconds)
{
    ulong milliseconds;

#if !defined(FSLIB) && !defined(RSP_ROM)
    /*
     * If non-zero, convert microseconds to milliseconds and round
     * off to the ceiling. Dismiss and let other processes run.
     */
    if (get_interrupt_level() == 0) {
	if (microseconds) {
	    milliseconds = ((microseconds + 999)/1000);
	    process_sleep_for(milliseconds);
	}
	else
	    process_suspend();
    }
#endif

    PRT(P16,("ralib_yield(%d)\n",microseconds));
    return;
}



/******************************************************************************
 * Name: ralib_remove_device
 *
 * Description: simulates removing a device 
 *
 * Input: 
 *   dev_num -- the device number of the device to remove
 *
 * Return Value: None
 *
 *******************************************************************************/
void ralib_remove_device (int dev_num)
{
    ralib_dev_rec_t *dr;
    clpd6720_socket_t socket;

    if (dev_num <= RALIB_MAX_DEV_NUM) {
	dr = &ralib_dev_table[dev_num];
	dr->logical_status = FALSE;
	dr->present = FALSE;
	dr->initialized = FALSE;
#ifdef DEBUG
	buginf("\nCard removed (%d): initialized set to false\n", dev_num);
#endif
	socket = get_socket_from_dev_num(dev_num);
	ralib_event_card_removed(socket);
    }
    PRT(P16,("ralib_remove_device(%d)\n",dev_num));
}


/******************************************************************************
 * Name: ralib_insert_device
 *
 * Description: simulates inserting a device 
 *
 * Input: 
 *   dev_num -- the device number of the device to remove
 *   file_name -- the name of the file to use for this device
 *
 * Return Value: None
 *
 *******************************************************************************/
void ralib_insert_device (int dev_num)
{
    ralib_dev_rec_t *dr;
    clpd6720_socket_t socket;

    if (dev_num <= RALIB_MAX_DEV_NUM) {
	dr = &ralib_dev_table[dev_num];
	if (dr->valid == TRUE) {
	    dr->logical_status = TRUE;
	    dr->present = dr->physical_status && dr->logical_status;
	    clpd6720_mgmt_intr_handler(); /* XXX jack, fix this later */
	    if (dr->present == TRUE) {
		socket = get_socket_from_dev_num(dev_num);
		ralib_event_card_inserted(socket);
	    }
	}
    }
    PRT(P16,("ralib_insert_device(%d)\n",dev_num));
}

/******************************************************************************
 * Name: ralib_get_pcmcia_base_local_addr
 *
 * Description: returns the base local processor address for 
 *              pcmcia devices
 *
 * Input: 
 *   socket -- the device number of the PCI device
 *
 * Return Value: base local processor address for pcmcia
 *
 *******************************************************************************/
ulong ralib_get_pcmcia_base_local_addr (clpd6720_socket_t socket)
{
    return PCMCIA_BASE;
}

/******************************************************************************
 * Name: ralib_get_flash_base_local_addr
 *
 * Description: returns the base local processor address for the
 *              internal flash device
 *
 * Input: None
 *
 * Return Value: base local processor address for internal flash
 *
 *******************************************************************************/
ulong ralib_get_flash_base_local_addr (void)
{
    return INTERNAL_FLASH_BASE;
}


/*
 * rsp_disable_io_write_buffer
 * Disable the write_buffer for IO devices, returning
 * the previous state of the DCL_IO_WRBUF bit.
 */

ushort rsp_disable_io_write_buffer (void)
{
    ushort		old_dcl_asic_reg;
    volatile ushort	*dcl_asic_reg_ptr;

    dcl_asic_reg_ptr = (ushort*) ADRSPC_DCL;

    old_dcl_asic_reg = *dcl_asic_reg_ptr;
    *dcl_asic_reg_ptr = old_dcl_asic_reg & ~DCL_IO_WRBUF;

    /*
     * Flush this from write buffers by reading what we just wrote
     */
    *dcl_asic_reg_ptr;

    return(old_dcl_asic_reg);
}




/*
 * rsp_restore_io_write_buffer
 * Restore the previous state of the IO write buffer
 */

void rsp_restore_io_write_buffer (ushort dcl_asic_reg)
{
    volatile ushort	*dcl_asic_reg_ptr;

    dcl_asic_reg_ptr = (ushort*) ADRSPC_DCL;
    if (dcl_asic_reg & DCL_IO_WRBUF)
	*dcl_asic_reg_ptr |= DCL_IO_WRBUF;
    else
	*dcl_asic_reg_ptr &= ~DCL_IO_WRBUF;
}

