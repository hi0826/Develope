#include "stdafx.h"
#include "CriticalSection.h"
#include "MultiThreadSync.h"
#include "CircularQueue.h"
#include "NetworkSession.h"
#include "PacketSession.h"
#include "EventSelect.h"
#include "ClientSession.h"


CClientSession::CClientSession(VOID)
{
}


CClientSession::~CClientSession(VOID)
{
}

BOOL CClientSession::BeginTcp(LPSTR remoteAddress, USHORT remotePort)
{// TCP로 사용할 때 사용하는 함수 
 // 접속할 상대 주소와 포트를 파라미터로 입력합니당.

	//정상적인 파라미터인지 체크합니다. 
	if (!remoteAddress || remotePort <= 0) return FALSE;

	//사용할 접속 개체를 초기화 시켜준다
	//실제로는 CPacketSession의 Begin을 호출 
	if (mSession.Begin()) {
		End();
		return FALSE;
	}

	//접속 개체를 TCP로 선언할 것을 선언한다
	// 선언할것을 선언한다는게 뭔개소리인지 몰겠네 
	if (!mSession.TCPBind()) {
		End();
		return FALSE;
	}

	//EventSelect 방식으로 해당 소켓의 이벤트를 받아오는 것을 등록 
	if (CEventSelect::Begin(mSession.GetSocket())) {
		End();
		return FALSE;
	}

	//접속할 주소에 접속합니당 
	if (!mSession.Connect(remoteAddress, remotePort)) {
		End();
		return FALSE;
	}

	return TRUE;
}

BOOL CClientSession::BeginUdp(USHORT remotePort)
{// UDP로 사용할 때 초기화 하는 함수
 // TCP와는 달리 포트만을 입력한다  왜지? 

	//파라미터가 정상적인지 페크합니다. 
	if (remotePort <= 0) return FALSE;

	//사용할 개체를 초기화 합니다. 
	if (!mSession.Begin()) {
		End();
		return FALSE;
	}

	//UDP 바인드를 해준다 
	if (!mSession.UDPBind(remotePort)) {
		End();
		return FALSE;
	}
	// UDP라고 지정 
	IsUdp = TRUE; 
	return TRUE;
}

BOOL CClientSession::End(VOID)
{
	//연결 개체를 종료 
	mSession.End();

	//EvenSelect 개체를 종료 
	CEventSelect::End();
	return TRUE; 
}

BOOL CClientSession::WritePacket(DWORD protocol, const BYTE * packet, DWORD packetLength)
{// TCP 데이터를 전송하는 함수  
 // 파라미터로 사용할 프로토콜, 패킷, 패킷 길이를 넣어주게 됩니다.

	//데이터 전송함수를 호출 
	if (!mSession.WritePacket(protocol, packet, packetLength)) return FALSE;

	// 바로 WriteComplete를 해줍니다. 
	// 왜냐하면 CEventSelect의 FD_WRITE가 꼭 보내기 성공 이후 발생하지 않을 수도 
	// 있기 때문입니다. 
	if (!mSession.WriteComplete()) return FALSE;

	return TRUE;
}

BOOL CClientSession::WriteToPacket(LPCSTR remoteAddress, USHORT remotePort, DWORD protocol, const BYTE * packet, DWORD packetLength)
{//UDP 데이터를 전송하는 함수 
 //파라미터로 사용할 프로토콜 패킷 패킷 길이를 넣어주면 됨 
	if (!mSession.WriteToPacket(remoteAddress, remotePort, protocol, packet, packetLength))
		return FALSE;

	if (!mSession.WriteComplete()) return FALSE;
	
	return TRUE;
}

BOOL CClientSession::ReadPacket(DWORD & protocol, BYTE * packet, DWORD & packetlength)
{
	VOID* Object = NULL;

	//큐에서 패킷 한 개를 받아옵니다. 
	return mReadPacketQueue.Pop(&Object,protocol,packet,packetlength);
}

BOOL CClientSession::ReadFromPacket(DWORD & protocol, LPSTR remoteAddress, USHORT & remotePort, BYTE * packet, DWORD & packetLength)
{
	VOID* Object = NULL;

	return mReadPacketQueue.Pop(&Object, protocol, packet, packetLength, remoteAddress, remotePort);

}

BOOL CClientSession::GetLocalIP(WCHAR * pIP)
{
	return mSession.GetLocalIP(pIP);
}

USHORT CClientSession::GetLocalPort(VOID)
{
	return mSession.GetLocalPort();
}


VOID CClientSession::OnIoRead(VOID)
{ //데이터가 발생했을 때 호출되는 가상함수 
	
	// 패킷을 저장하는 버퍼 
	BYTE      Packet[MAX_BUFFER_LENGTH] = { 0, };
	//패킷의 길이를 저장하는 변수 
	DWORD     PacketLength = 0;
	//프로토콜을 저장하는 변수 
	DWORD     Protocol = 0;

	//UDP일 경우와 TCP일 경우를 구분합니다. 
	if (IsUdp)
	{
		//UDP일 경우에는 보낸 주소와 포트 정보를 받아와야 하기때문에 
		//따로 변수를 선언 시켜줘야댐 
		CHAR        RemoteAddress[32] = { 0, };
		USHORT      RemotePort        = 0;

		//CNetWorkSession 에서 CPacketSession으로 데이터를 복사 하고 
		if (mSession.ReadFromPacketForEventSelect(RemoteAddress, RemotePort))
		{   
			// 패킷을 가져온뒤에 packet을 큐에 저장시켜줍니다! 
			while (mSession.GetPacket(RemoteAddress, RemotePort, Protocol, Packet, PacketLength))
				mReadPacketQueue.Push(this, Protocol, Packet, PacketLength,RemoteAddress,RemotePort);
		}
	}
	else
	{   //이새끼도 마찬가지지요!  
		if (mSession.ReadPacketForEventSelect())
		{   
			// 이새끼도 마찬가지에요! 
			while (mSession.GetPacket(Protocol, Packet, PacketLength))
				mReadPacketQueue.Push(this, Protocol, Packet, PacketLength);
		}

	}
}
