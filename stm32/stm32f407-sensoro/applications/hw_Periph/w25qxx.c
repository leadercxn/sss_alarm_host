#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include <drv_spi.h>

#include <sfud.h>
#include <spi_flash.h>
#include <spi_flash_sfud.h>
#include <sfud_def.h>

#define LOG_TAG     "W25QXX"
#include <ulog.h> 

#include <user_board.h>
#include <w25qxx.h>

#ifndef W25Q_DEVICE_NAME
#define W25Q_DEVICE_NAME        "W25Q128"
#endif

#ifndef SPI_BUS_NAME
#define SPI_BUS_NAME            "spi1"
#endif

#ifndef SPI_DEVICE_NAME
#define SPI_DEVICE_NAME         "spi10"
#endif

#define W25QXX_BUFF_SIZE        512



rt_uint8_t rData[W25QXX_BUFF_SIZE];
rt_uint8_t wData[W25QXX_BUFF_SIZE] = {"QSPI test"};

static sfud_flash_t m_sfud_dev;    
static rt_spi_flash_device_t m_w25q;
/**
 * @brief 硬件初始化，指定片选IO，挂载设备
 */
static int rt_hw_spi_flash_init(void)
{

#if 0       //QSPI模式  2020-02-01打开QSPI编译有错，F407暂时还没有QSPI控制器
    #define QSPI_CS_PIN             GET_PIN(A , 4)

    if(RT_EOK != stm32_qspi_bus_attach_device( SPI_BUS_NAME, SPI_DEVICE_NAME, (rt_uint32_t)QSPI_CS_PIN, 1, RT_NULL, RT_NULL) )
    {
        return -RT_ERROR;
    }
#endif    

#if 1
    rt_hw_spi_device_attach( SPI_BUS_NAME , SPI_DEVICE_NAME , GPIO_SPI1_W25QXX_CS, SPI1_W25QXX_CS_PIN);
#endif

#ifdef  RT_USING_SFUD
    if( RT_NULL == rt_sfud_flash_probe( W25Q_DEVICE_NAME, SPI_DEVICE_NAME ) )
    {
        return -RT_ERROR;
    };
#endif
    m_sfud_dev = rt_sfud_flash_find_by_dev_name(W25Q_DEVICE_NAME);

    return RT_EOK;
}
INIT_COMPONENT_EXPORT(rt_hw_spi_flash_init);                // 导出到自动初始化 


/**
 * @brief 读取w25q flash里面的数据
 */
int w25qxx_read( uint32_t addr_read , uint8_t *buff , uint32_t bytes )
{
    if( SFUD_SUCCESS == sfud_read(m_sfud_dev, addr_read, bytes, buff) )         // 读flash数据
    {
        return RT_EOK;
    }
    return -RT_ERROR;
}




/**
 * @brief w25q 命令列表 -- spi测试
 */
static void w25q_demo_test(int argc, char *argv[])
{
    typedef struct              
    {               
        uint32_t start_addr ;                                       // 语音的开始地址
        uint32_t size       ;                                       // 语音条的大小
    } voice_info_t ;      

    typedef struct              
    {               
        uint32_t        voice_count;                                // 语音条的数量
        voice_info_t    info[16];                      // 语音条信息的结构
    } voice_header_t; 

    uint32_t voice_count_test = 0 ;

    //m_sfud_dev = rt_sfud_flash_find_by_dev_name(W25Q_DEVICE_NAME);

    if(m_sfud_dev == RT_NULL)
    {
        rt_kprintf("sfud can't find %s device.\n", W25Q_DEVICE_NAME);
    }
    else
    {
        rt_kprintf("sfud device name: %s, sector_count: %d, bytes_per_sector: %d, block_size: %d , capacity:%d  \n", 
                    m_sfud_dev->name, m_sfud_dev->chip.capacity / m_sfud_dev->chip.erase_gran, 
                    m_sfud_dev->chip.erase_gran, m_sfud_dev->chip.erase_gran , m_sfud_dev->chip.capacity );
        #if 0                                                                           // 写测试
        if(sfud_erase_write(m_sfud_dev, 0x002000, sizeof(wData), wData) == SFUD_SUCCESS)
        {
            rt_kprintf("sfud api write data to w25q128(address:0x2000) success.\n");
        }
        #endif

        if(sfud_read(m_sfud_dev, 0x000000, sizeof(rData), rData) == SFUD_SUCCESS)         // 读flash数据
        {
            for( uint16_t i = 0 ; i < 22 ; i++ )                                        // 打印buff
            {
                rt_kprintf(" rData[%d] = 0x%02x , \n" , i , rData[i] );
            }
        }

        if( SFUD_SUCCESS == sfud_read(m_sfud_dev, 0 , sizeof(uint32_t), (uint8_t *)&voice_count_test)  )         // 读flash数据
        {
                rt_kprintf(" voice_count_test = %d \n" , voice_count_test );
        }

    }
}

MSH_CMD_EXPORT(w25q_demo_test, w25q demo test );       // 导出到 msh 命令列表中



/**
 * @brief w25q 命令列表 -- spi测试
 */
static void w25q_erase_chip(int argc, char *argv[])
{
    sfud_chip_erase(m_sfud_dev);
    rt_kprintf(" erase finish \n"  );
}

#ifdef FINSH_USING_MSH
#include <finsh.h>
/* 导出命令到 FinSH 控制台 */
MSH_CMD_EXPORT_ALIAS( w25q_erase_chip, w25q_erase_chip , w25q erase full chip );
#endif /* FINSH_USING_MSH */

