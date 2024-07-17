//#include "wgo2.h"
#include "swd_app.h"
//#include "fuel_gauge.h"
#include "dap.h"
#include "target.h"
//#include "wgo2_priorities.h"
#include "malloc.h"
#ifdef CONFIG_MFI_IAP2
#include "mfi_app.h"
#endif


#define SYS_LOG_NO_NEWLINE
#ifdef SYS_LOG_DOMAIN
#undef SYS_LOG_DOMAIN
#endif
#define SYS_LOG_DOMAIN "swd"
#ifdef SYS_LOG_LEVEL
#undef SYS_LOG_LEVEL
#endif
#define SYS_LOG_LEVEL CONFIG_SYS_LOG_DEFAULT_LEVEL
#define SYS_LOG_NO_NEWLINE
//#include <logging/sys_log.h>
#include "fsl_debug_console.h"

/************************      Local Variables              *****************/
bool is_swd_mode=false;
bool is_swd_recx_msgq_init=false;
bool is_swd_read_test_pass=false;
bool is_swd_read_test_done=false;
//struct k_msgq swd_recx_msgq;
//static wgo2_app_data_t swd_app_data;

//uint8_t __aligned(4) swd_recx_msgq_buffer[SWD_RECX_MSGQ_DEPTH * sizeof(wgo2_app_msgq_t)];
/************************      Local Functions              *****************/
/*
 ** swd_app_Init
 */
void swd_app_Init(void)
{
	is_swd_mode=false;
	
}

/*
 ** swd_mode_Enable
 */
void swd_mode_Enable(void)
{
    PRINTF("swd is enabled\n");
	if(!is_swd_mode)
	{
		is_swd_mode=true;

#ifdef CONFIG_MFI_IAP2
		mfi_i2c_deinit();
#endif
//		fuel_gauge_i2c_deinit();
//      PRINTF("De-init of mfi and fuel gauge\n");
	}
	
}	

/*
 ** swd_mode_Disable
 */
void swd_mode_Disable(void)
{
	if(is_swd_mode)
	{
//		fuel_gauge_i2c_init();

#ifdef CONFIG_MFI_IAP2
		mfi_init();
#endif
		is_swd_mode=false;
	}
	
}
/*
 ** get_swd_mode
 */
bool get_swd_mode(void)
{
	return is_swd_mode;
}


/*
 ** send_read_test_msg_to_swd
 */
#if 0
void send_read_test_msg_to_swd(void)
{
    wgo2_app_msgq_t swd_q;//, rx_q; unused variable
	swd_q.msg1=SWD_READ_TEST_MSG;

 	if(!is_swd_recx_msgq_init)
    {
        is_swd_recx_msgq_init=true;
        k_msgq_init(&swd_recx_msgq, swd_recx_msgq_buffer, sizeof(wgo2_app_msgq_t), SWD_RECX_MSGQ_DEPTH);
    }


    while (k_msgq_put(&swd_recx_msgq, &swd_q, K_NO_WAIT) != 0) 
	{
        /* message queue is full: purge old data & try again */
        k_msgq_purge(&swd_recx_msgq);
        PRINTF("ERR! swd msg full\n");
     
    }
}
#endif


/*
 ** swd_connect_read_test
 */
void swd_connect_read_test(void)
{
	uint32_t val=0;
	is_swd_read_test_done=false;
	//is_swd_read_test_pass=false;
	swd_mode_Enable();
//    k_sleep(K_MSEC(1));
	target_connect();
	val=target_read_word(0x20000000);
	PRINTF("Nordic sram addr 0x20000000 value 0x%x\n", val);

	target_write_word(0x20000000, 0x12345678);

	val=target_read_word(0x20000000);
	if(val == 0x12345678)
	{
		PRINTF("swd memory test passes, sram addr 0x20000000 new value 0x%x\n", val);
	}
	else 
	{
		PRINTF("swd memory test fails, sram addr 0x20000000 new value 0x%x\n", val);
	}
	is_swd_read_test_done=true;
}

/*
 ** swd_connect_read_test_result
 */
bool swd_connect_read_test_result(void)
{
	return is_swd_read_test_done;
}

/*
 ** swd_main_loop
 */
#if 0
static void swd_main_loop(void *param1, void *param2, void *param3)
{
    wgo2_app_msgq_t recx_q;
    //int32_t status=0;
    //uint8_t *data_ptr;
	//uint32_t len;
    //void *ptr=NULL;

    while(1)
    {
        k_msgq_get(&swd_recx_msgq, &recx_q, K_FOREVER);
        switch(recx_q.msg1)
        {
            case SWD_INIT_MSG:
            case SWD_READ_MSG:
			case SWD_WRITE_MSG:
			case SWD_ERASE_MSG:
                break;
			case SWD_READ_TEST_MSG:
				PRINTF("swd read test msg\n");
				swd_connect_read_test();
				break;
            default:
                break;
        }
    }    
}
#endif


/*
 ** swd_thread_create
 */
#if 0
void swd_thread_create(void)
{
    SYS_LOG_INF("%d: \n", __LINE__);

    if(!is_swd_recx_msgq_init)
    {
        is_swd_recx_msgq_init=true;
        k_msgq_init(&swd_recx_msgq, swd_recx_msgq_buffer, sizeof(wgo2_app_msgq_t), SWD_RECX_MSGQ_DEPTH);
    }

    swd_app_data.stack = (u8_t *) malloc(SWD_APP_STACK_SIZE);

    if (swd_app_data.stack == NULL)
    {
        SYS_LOG_INF("%d: malloc swd thread stack error\n", __LINE__);
        while (1);
    }

    swd_app_data.thread = (os_tid_t)os_thread_create((char *)swd_app_data.stack,
                                                           SWD_APP_STACK_SIZE,
                                                           swd_main_loop,
                                                           NULL, NULL, NULL,
                                                           SWD_THREAD_PRIORITY, 0, OS_NO_WAIT);
    if(swd_app_data.thread!=(os_tid_t) NULL)
    {
        SYS_LOG_INF("%d: swd thread is created sucessfully\n", __LINE__);
    }
    else
    {
        SYS_LOG_ERR("%d: failed to create swd thread !\n",  __LINE__);
    }

}
#endif
