#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include <arpa/inet.h>
#include <netdev.h>         /* 当需要网卡操作是，需要包含这两个头文件 */

#include <user_board.h>

/**
 * @brief for drv_eth.c
 */
void phy_reset(void)
{

    rt_pin_mode(VCC_3V3_2_SUPPLY, PIN_MODE_OUTPUT);         
    rt_pin_write(VCC_3V3_2_SUPPLY, PIN_HIGH);
    rt_thread_mdelay(10);

    rt_pin_mode(LAN8720_RESET, PIN_MODE_OUTPUT);    
    rt_pin_write(LAN8720_RESET, PIN_LOW);
    rt_thread_mdelay(50);
    rt_pin_write(LAN8720_RESET, PIN_HIGH);
    rt_kprintf("lan8720_phy_reset finish \r\n");
}


/**
 * @brief netdev 设备测试
 */
static int find_netdev_test( int argc ,char **argv )
{
    struct netdev *netdev = RT_NULL ;

    if( argc != 2 )
    {
        rt_kprintf("find_netdev [netdev_name]  -- find netdev name. \r\n");
        return -1;
    }

    netdev = netdev_get_by_name(argv[1]);   // 通过名称获取网卡对象 

    if( RT_NULL == netdev )
    {
        rt_kprintf("cloud not find the %s device \r\n" , argv[1]);
        return -1;
    }
    else
    {
        rt_kprintf("find the netdev name is %s \r\n" , netdev->name);
    }


    if( 1 == netdev_is_up(netdev) )
    {
        rt_kprintf(" %s is up\r\n" , netdev->name);
    }
    else
    {
        rt_kprintf(" %s is not up\r\n" , netdev->name);
    }

    if( 1 == netdev_is_link_up(netdev) )
    {
        rt_kprintf(" %s is link_up\r\n" , netdev->name);
    }
    else
    {
        rt_kprintf(" %s is not link_up\r\n" , netdev->name);
    }

    if( 1 == netdev_is_internet_up(netdev) )
    {
        rt_kprintf(" %s is internet_up\r\n" , netdev->name);
    }
    else
    {
        rt_kprintf(" %s is not internet_up\r\n" , netdev->name);
    }

    return 0;
}

#ifdef FINSH_USING_MSH
#include <finsh.h>
/* 导出命令到 FinSH 控制台 */
MSH_CMD_EXPORT_ALIAS( find_netdev_test, find_netdev, find netdev test );
#endif /* FINSH_USING_MSH */




