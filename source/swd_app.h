/*!
 * \file      swd_app.h
 * \brief     
 * \details
 * \author    Jim LIN
 * \date
 * \copyright Rode Microphones
 */

#ifndef _SWD_APP_H
#define _SWD_APP_H

//#include <zephyr.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <thread_timer.h>
//#include <audio_hal.h>
//#include "wgo2.h"
#include "nrf5.h"
#include "target.h"

/* Exported macro ------------------------------------------------------------*/
#define SWD_RECX_MSGQ_DEPTH                         (30)
#define SWD_APP_STACK_SIZE                          (1024)

#define SWD_INIT_MSG                            (1)
#define SWD_READ_MSG                            (2)
#define SWD_WRITE_MSG                           (3)
#define SWD_ERASE_MSG                           (4)

#define SWD_READ_TEST_MSG                       (8)

/* Exported Structure ------------------------------------------------------- */


/* Exported functions ------------------------------------------------------- */
/*
 ** swd_app_Init
 */
void swd_app_Init(void);
/*
 ** swd_mode_Enable
 */
void swd_mode_Enable(void);
/*
 ** swd_mode_Disable
 */
void swd_mode_Disable(void);
/*
 ** swd_connect_read_test
 */
void swd_connect_read_test(void);
/*
 ** send_read_test_msg_to_swd
 */
void send_read_test_msg_to_swd(void);
/*
 ** swd_connect_read_test_result
 */
bool swd_connect_read_test_result(void);
/*
 ** swd_thread_create
 */
void swd_thread_create(void);
#endif  // _SWD_APP_H
