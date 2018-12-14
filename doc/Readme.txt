所有参数配置都在board.h里



//LED
LED_COUNT						LED数量，
LED_FAST_INTERVAL		快闪变化周期(ms)
LED_SLOW_INTERVAL		慢闪变化周期(ms)

//BUTTON
BUTTON_COUNT				1
BUTTON_DEATH				按键死区时间，防抖(ms)
BUTTON_INTERVAL			按键周期(ms)

//RS232
RS232_BAUDRATE			串口波特率
RS232_RX_BUF_SIZE		串口接收缓冲区大小（不建议修改）
RS232_TX_BUF_SIZE		串口发送缓冲区大小（不建议修改）
RS232_RX_TIMEOUT		串口接收帧超时(ms)，不需要超时就设0

//CAN
CAN_BAUDRATE				CAN波特率
CAN_RX_BUF_SIZE			CAN接收缓冲区大小（不建议修改）
CAN_TX_BUF_SIZE			CAN发送缓冲区大小（不建议修改）
CAN_RX_TIMEOUT			CAN接收帧超时(ms)，不需要超时就设0
CAN_RX_STD_ID				接收标准ID，任意地址都收就注释掉
CAN_RX_EXT_ID				接收扩展ID，任意地址都收就注释掉
CAN_TX_STD_ID				本机标准ID
CAN_TX_EXT_ID				本机扩展ID
CAN_TX_FRAME_TYPE		发送帧类型值
CAN_TX_RTR_TYPE			发送PRP值

功能验证：
send_all_off.bin		灯全灭
send_all_on.bin			灯全常亮
send_mix.bin				灯混闪

发送时可以通过WINHEX/UltraEdit根据板的地址修改第三个字节的值。
板的地址可以通过获取按键帧的第三字节来确认。