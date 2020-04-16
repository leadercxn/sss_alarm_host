#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include <at.h>
#include <user_board.h>


#define LOG_TAG     "BC26_HANDLE"
#include <ulog.h> 

#ifdef  BC26_SAMPLE_CLIENT_NAME
#define BC26_DEV_NAME     BC26_SAMPLE_CLIENT_NAME
#else

#ifndef BC26_DEV_NAME
#define BC26_DEV_NAME     "uart1"
#endif

#endif




#ifdef  BC26_SAMPLE_RECV_BUFF_LEN
#define BC26_RECV_BUFF_LEN  BC26_SAMPLE_RECV_BUFF_LEN
#else
#define BC26_RECV_BUFF_LEN  512
#endif


static rt_uint8_t m_init_done = 0 ;

/**
 * @brief 初始化 at_client 
 */
static int my_at_client_init(void)
{
    int err_code = RT_EOK ;
    if( 0 == m_init_done )
    {
        err_code = at_client_init(BC26_DEV_NAME,  BC26_RECV_BUFF_LEN);
        if( 0 == err_code )
        {
            LOG_D("at client init success auto" );
        }
        else
        {
            LOG_D("at client init fail auto , err_code = %d" , err_code );
        }
        m_init_done = 1 ;
    }
    return err_code;
}
INIT_COMPONENT_EXPORT(my_at_client_init);                                       // 导进component 自动初始化

/**
 * @brief URC +IP(前缀)的回调函数
 */
static void urc_ip_func(struct at_client *client, const char *data , rt_size_t size)
{
    LOG_D("data: %s " , data ) ;
}

/**
 * @brief URC +CPIN(前缀)的回调函数
 */
static void urc_cpin_func(struct at_client *client, const char *data , rt_size_t size)
{
    LOG_D("data: %s " , data ) ;
}

/**
 * @brief URC 列表
 */
static struct at_urc urc_table[] = {
    {"+IP",   "\r\n",     urc_ip_func},
    {"+CPIN",   "\r\n",   urc_cpin_func},
};

/**
 * @brief 初始化urc列表
 */
static int at_client_urc_init(void)
{
    at_set_urc_table( urc_table , sizeof(urc_table)/sizeof(urc_table[0]) );
    return RT_EOK;
}
INIT_APP_EXPORT(at_client_urc_init);                                                // 应用初始化







/**
 *@brief at_client 测试 
 */
static int at_send(int argc, char *argv[])
{
    at_response_t resp = RT_NULL;
    int err_code = RT_EOK ;
    rt_size_t recv_bufsz = 256 ;

    if( 0 == m_init_done )
    {
        err_code = at_client_init(BC26_DEV_NAME,  recv_bufsz);
        if( 0 == err_code )
        {
            LOG_D("at client init success" );
        }
        else
        {
            LOG_D("at client init fail , err_code = %d" , err_code );
        }
        m_init_done = 1 ;
    }

    if (argc != 2)
    {
        LOG_E("at_cli_send [command]  - AT client send commands to AT server.");
        return -RT_ERROR;
    }

    /* 创建响应结构体，设置最大支持响应数据长度为 512 字节，响应数据行数无限制，超时时间为 5 秒 */
    resp = at_create_resp(512, 0, rt_tick_from_millisecond(5000));
    if (!resp)
    {
        LOG_E("No memory for response structure!");
        return -RT_ENOMEM;
    }

    /* 发送 AT 命令并接收 AT Server 响应数据，数据及信息存放在 resp 结构体中 */
    if (at_exec_cmd(resp, argv[1]) != RT_EOK)
    {
        LOG_E("AT client send commands failed, response error or timeout !");
        return -RT_ERROR;
    }

    /* 命令发送成功 */
    LOG_D("AT Client send commands to AT Server success!");

    /* 删除响应结构体 */
    at_delete_resp(resp);

    return RT_EOK;
}

MSH_CMD_EXPORT(at_send, AT Client send commands);


/**
 * @brief resp_parse demo
 */
static int at_resp_test(int argc, char *argv[])
{
    at_response_t resp = RT_NULL;
    int err_code = RT_EOK ;
    rt_size_t recv_bufsz = 256 ;

    char recv_buff[256] ;                  //别用 char *buff; 容易造成野指针

    if( 0 == m_init_done )
    {
        err_code = at_client_init(BC26_DEV_NAME,  recv_bufsz);
        if( 0 == err_code )
        {
            LOG_D("at client init success" );
        }
        else
        {
            LOG_D("at client init fail , err_code = %d" , err_code );
        }
        m_init_done = 1 ;
    }

    /* 创建响应结构体，设置最大支持响应数据长度为 512 字节，响应数据行数无限制，超时时间为 5 秒 */
    resp = at_create_resp(512, 0, rt_tick_from_millisecond(5000));
    if (!resp)
    {
        LOG_E("No memory for response structure!");
        return -RT_ENOMEM;
    }
    /* 发送 AT 命令并接收 AT Server 响应数据，数据及信息存放在 resp 结构体中 */
    if (at_exec_cmd(resp, "AT+CIMI" ) != RT_EOK)
    {
        LOG_E("AT client send commands failed, response error or timeout !");
        return -RT_ERROR;
    }


    /* 解析获取串口配置信息，1 表示解析响应数据第一行，'%*[^=]'表示忽略等号之前的数据 */
    at_resp_parse_line_args(resp, 2 ,"%s", recv_buff );
    LOG_D("recv_buff=%s " , recv_buff );

    /* 删除服务器响应结构体 */
    at_delete_resp(resp);

}
MSH_CMD_EXPORT(at_resp_test, at resp test); 
