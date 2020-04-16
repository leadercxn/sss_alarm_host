#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include <user_board.h>


/**
 * @brief PWRKEY , RESET IO的复位
 */
static int nb_hw_init(void)
{
    rt_pin_mode(PCIE_PWR_EN, PIN_MODE_OUTPUT);     //配置成输出
    rt_pin_mode(PCIE_RESET, PIN_MODE_OUTPUT);


    rt_pin_write(PCIE_PWR_EN, PIN_LOW);
    rt_thread_mdelay(500);
    rt_pin_write(PCIE_PWR_EN, PIN_HIGH);

    rt_pin_write(PCIE_RESET, PIN_LOW);
    rt_thread_mdelay(50);
    rt_pin_write(PCIE_RESET, PIN_HIGH);

    rt_kprintf("PCIe_hw_init finish \r\n");
    return RT_EOK;
}
INIT_COMPONENT_EXPORT(nb_hw_init);            //导入到自动初始化


