#ifndef __SER_ALPHA_H
#define __SER_ALPHA_H


//stm32的访问事件号
enum ALPHA_EVENT_ACCESS
{
    ALPHA_ALIVE_ACCESS = 1  ,             //alpha是否存活
    ALPHA_CMD_SLEEP         ,             //alpha进入sleep模式
    ALPHA_CMD_OPEN_RX       ,             //alpha打开接收数据功能
    ALPHA_CMD_OPEN_TX       ,             //alpha发送数据
};
















#endif

