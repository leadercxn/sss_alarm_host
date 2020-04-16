#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include <button.h>
#include <user_board.h>

Button_t m_pair_button;             //创建pair按键句柄
Button_t m_install_button;          //创建install按键句柄

/**
 * @brief 按键硬件初始化
 */
static int button_hw_init(void)
{

    rt_pin_mode(PAIR_BUTTON, PIN_MODE_INPUT_PULLUP);     //配置成上拉输入
    rt_pin_mode(INSTALL_BUTTON, PIN_MODE_INPUT_PULLUP);

    rt_kprintf("button_hw_init finish \r\n");
    return RT_EOK;
}

INIT_COMPONENT_EXPORT(button_hw_init);            //导入到自动初始化


/**
 *@brief 读取pair按键的点评状态
 */
static rt_uint8_t read_pair_key(void)
{
    rt_pin_read(PAIR_BUTTON);
}

/**
 *@brief 读取install按键的点评状态
 */
static rt_uint8_t read_install_key(void)
{
    rt_pin_read(INSTALL_BUTTON);
}

/**
 * @brief pair按键单击回调
 */
static void pair_key_click_callback(void *btn)
{
    rt_kprintf("pair key Click!\r\n");
}

/**
 * @brief pair按键长按回调
 */
static void pair_key_long_callback(void *btn)
{
    rt_kprintf("pair key long!\r\n");
}

/**
 * @brief pair按键长按释放回调
 */
static void pair_key_long_free_callback(void *btn)
{
    rt_kprintf("pair key long free!\r\n");
}





/**
 * @brief install按键回调
 */
static void install_key_click_callback(void *btn)
{
    rt_kprintf("install key Click!\r\n");
}

/**
 * @brief install按键长按回调
 */
static void install_key_long_callback(void *btn)
{
    rt_kprintf("install key long!\r\n");
}

/**
 * @brief install按键长按回调
 */
static void install_key_long_free_callback(void *btn)
{
    rt_kprintf("install key long free!\r\n");
}



/**
 * @brief 按键软件初始化
 */
static int button_sf_init(void)
{
//    m_pair_button.Read_Button_Level = read_pair_key();          //函数指针赋值
//    m_install_button.Read_Button_Level = read_install_key();    //函数指针赋值

    Button_Create(  "PAIR_KEY",             //名字
                    &m_pair_button,         //句柄
                    read_pair_key,          //按键点评检测函数
                    PIN_LOW);               //触发电平

    Button_Attach(&m_pair_button, BUTTON_DOWM, pair_key_click_callback );                       //Click
    Button_Attach(&m_pair_button, BUTTON_LONG, pair_key_long_callback );                        //long
    Button_Attach(&m_pair_button, BUTTON_LONG_FREE, pair_key_long_free_callback );              //long_free

    Button_Create(  "INSTALL_KEY",             //名字
                    &m_install_button,         //句柄
                    read_install_key,          //按键点评检测函数
                    PIN_LOW);                  //触发电平

    Button_Attach(&m_install_button, BUTTON_DOWM, install_key_click_callback );                       //Click
    Button_Attach(&m_install_button, BUTTON_LONG, install_key_long_callback );                        //long
    Button_Attach(&m_install_button, BUTTON_LONG_FREE, install_key_long_free_callback );              //long_free

    rt_kprintf("button_sf_init finish \r\n");
    return RT_EOK;
}
INIT_APP_EXPORT(button_sf_init);                                                // 应用初始化

