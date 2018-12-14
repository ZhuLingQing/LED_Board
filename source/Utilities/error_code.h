#ifndef _ERROR_CODE_H_
#define _ERROR_CODE_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int ERR_CODE;

typedef enum __e_err_module{
	E_SYSTEM = 0,
	E_PERIPHERAL,
	E_DRIVER,
	E_USB,
	E_CONSUMER,
	E_DPRODUCER,//5
	E_LCD,
	E_FPGA,
	E_BOOT,
	E_FLASH,
}ERR_MODULE;

typedef enum __e_err_value{
	ERR_NO = 0,
	E_MALLOC,
	E_PARAMETER,
	E_LENGTH,
	E_ADDRESS,
	E_DATA,//5
	E_COUNTER,
	E_NULL_POINT,
	E_STRUCTURE_TYPE,
	E_CHECKSUM,
	E_TIMEOUT,//10
	E_STATUS,
	E_FCREATE,
	E_FREAD,
	E_FWRITE,
	E_FSEEK,//15
	E_PWR_OVERCURRENT,
	E_PWR_LEAK,
	E_PWR_OVER,
	E_FRAME,
    E_SPIDMA,//20
}ERR_VALUE;

ERR_CODE set_last_error(ERR_CODE error);
ERR_CODE set_module_error(ERR_MODULE module, ERR_VALUE value);
ERR_CODE get_last_error();
void PRINT_ERROR(ERR_CODE error);

#define set_peripheral_err(val)		set_module_error(E_PERIPHERAL,val)
#define set_usb_err(val)			set_module_error(E_USB,val)
#define set_consumer_err(val)		set_module_error(E_CONSUMER,val)
#define set_producer_err(val)		set_module_error(E_DPRODUCER,val)
#define set_lcd_err(val)			set_module_error(E_LCD,val)
#define set_fpga_err(val)			set_module_error(E_FPGA,val)
#define set_boot_err(val)			set_module_error(E_BOOT,val)
#define set_flash_err(val)			set_module_error(E_FLASH,val)

#ifdef __cplusplus
}
#endif

#endif
