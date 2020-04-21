#ifndef __SER_ALPHA_H
#define __SER_ALPHA_H


//stm32的访问事件号
enum ALPHA_EVENT_ACCESS
{
    ALPHA_ALIVE_ACCESS = 1  ,             //alpha是否存活
    ALPHA_CMD_LORA_SLEEP    ,             //lora进入sleep模式
    ALPHA_CMD_LORA_OPEN_RX  ,             //lora打开接收数据功能
    ALPHA_CMD_LORA_OPEN_TX  ,             //lora发送数据
    ALPHA_CMD_READ_LORA_RX_BUF ,          //读取lora接收缓存区数据

    ALPHA_CMD_BLE_OPEN_SCAN  = 0x0A ,     //ble打开扫描
    ALPHA_CMD_BLE_CLOSE_SCAN ,            //ble关闭扫描
    ALPHA_CMD_BLE_OPEN_ADV   ,            //ble打开广播
    ALPHA_CMD_READ_BLE_EVENTS_BUF ,       //读取ble事件缓存区

};
















#endif

