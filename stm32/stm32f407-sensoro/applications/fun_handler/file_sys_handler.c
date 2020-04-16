#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>


#include <dfs_fs.h>
#include <user_board.h>

/**
 * @brief dfs挂载
 */
static int my_dfs_mount( int argc, char **argv )
{
    if( dfs_mount(W25Q_DEVICE_NAME,"/","elm",0,0 ) == 0 )
    {
        rt_kprintf("spi flash mount to / success \n");
    }
    else
    {
        rt_kprintf("spi flash mount to / v failed \n");
    }
    return RT_EOK;
}






#ifdef FINSH_USING_MSH
#include <finsh.h>
/* 导出命令到 FinSH 控制台 */
MSH_CMD_EXPORT_ALIAS( my_dfs_mount, dfs_mount , dfs mount  );
#endif /* FINSH_USING_MSH */



