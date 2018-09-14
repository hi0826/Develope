#include "Core.h"
#include "Util\MiniDump.h"

CCore IOCP_SERVER;
int main()
{
	CMiniDump::Begin();
	IOCP_SERVER.Initialize();

	IOCP_SERVER.Run();

	IOCP_SERVER.Close();
	CMiniDump::End();

	return 0;
}