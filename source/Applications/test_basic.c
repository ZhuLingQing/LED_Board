#include "board.h"

typedef struct {
	unsigned int baseAddress;
	unsigned int devCount;
}DEBUG_PARAM;

typedef struct __debug_item{
	const char *cmd_code;                                  //字符型变量
	const char* cmd_string;                         //字符型指针
	void (*cmd_function)(DEBUG_PARAM *param);                         //
}const DEBUG_ITEM;

#define __DEBUG_ITEM_DECLARE(code,name)			{code,#name,debug_##name}

extern DEBUG_ITEM c_debug_item[];

void get_cmd_code(char *code, int max_len)
{
	int n = 0;

	memset(code,0,max_len);
	TRACE("\r\nInput Command code:");
	while(n < max_len)
	{
		*code = TRACE_GetChar();
		switch(*code)
		{
		case 0x1B://esc
		case ' ':
		case '\r':
		case '\n':
			*code = '\0';
			return;
		case 0x08://back space
		case 0x7F://del
			if(n)
			{
				TRACE_PutChar(*code);
				TRACE_PutChar(' ');
				TRACE_PutChar(*code);
				n--;
				code--;
			}
			break;
		default:
			TRACE_PutChar(*code);
			code++;
			n++;
			break;
		}
	}
	*code = '\0';
}

void TASK_Debug( void *pvParameters )
{
	char cmd[16] = {0};
	DEBUG_ITEM* debug_item_list;       //定义DEBUG_ITEM类型的指针变量debug_item_list
	DEBUG_PARAM param = {0};
	Sleep(100);
	TRACE_LINE(" ========== DEBUG MENU ==========");
	for( ;; )
	{
		get_cmd_code(cmd,sizeof(cmd) - 1);
		debug_item_list = c_debug_item;                         //= &c_debug_item 给指针传递地址，c_debug_item数组的地址就是首地址
		while(debug_item_list->cmd_code)
		{
			if(strcmp(debug_item_list->cmd_code,cmd) == 0)           		//*debug_item_list.cmd_code == key
			{
				TRACE_LINE("\r\n%s",debug_item_list->cmd_string);
				debug_item_list->cmd_function(&param);       			//函数返回，根据key
				TRACE_LINE("\r\nDone");
				break;
			}
			debug_item_list++;									//指针数据指向下一个，地址加1

		}
		if(debug_item_list->cmd_code == 0)
			TRACE_LINE("\r\nUnsupport cmd code");
	}
}

void debug_list_all_commands(DEBUG_PARAM *param)
{
	DEBUG_ITEM* debug_item_list;       //定义DEBUG_ITEM类型的指针变量debug_item_list
	debug_item_list = c_debug_item;
	while(debug_item_list->cmd_function)
	{
		TRACE_LINE("CMD: %8s. FEATURE: %s.",debug_item_list->cmd_code,debug_item_list->cmd_string);
		debug_item_list++;
	}
}

DEBUG_ITEM c_debug_item[] = {	
	__DEBUG_ITEM_DECLARE("?",list_all_commands),
	{0,NULL,NULL}
};

