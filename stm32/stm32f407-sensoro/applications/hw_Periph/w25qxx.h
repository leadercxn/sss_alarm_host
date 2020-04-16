#ifndef __W25QXX_H
#define __W25QXX_H




/**
 * @brief 读取w25q flash里面的数据
 */
int w25qxx_read( uint32_t addr_read , uint8_t *buff , uint32_t bytes );


/**
 * @brief w25q 命令列表 -- spi测试
 */
static void w25q_demo_test(int argc, char *argv[]);



#endif

