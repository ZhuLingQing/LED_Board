
void (*APP)();// = (void (*)())0x80000;

#define APP_ADDRESS		0x08008000

int main()
{
	APP = (void (*)())(*(int*)(APP_ADDRESS + 4));
	APP();
}