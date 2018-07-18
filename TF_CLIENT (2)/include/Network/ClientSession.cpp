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
{// TCP�� ����� �� ����ϴ� �Լ� 
 // ������ ��� �ּҿ� ��Ʈ�� �Ķ���ͷ� �Է��մϴ�.

	//�������� �Ķ�������� üũ�մϴ�. 
	if (!remoteAddress || remotePort <= 0) return FALSE;

	//����� ���� ��ü�� �ʱ�ȭ �����ش�
	//�����δ� CPacketSession�� Begin�� ȣ�� 
	if (mSession.Begin()) {
		End();
		return FALSE;
	}

	//���� ��ü�� TCP�� ������ ���� �����Ѵ�
	// �����Ұ��� �����Ѵٴ°� �����Ҹ����� ���ڳ� 
	if (!mSession.TCPBind()) {
		End();
		return FALSE;
	}

	//EventSelect ������� �ش� ������ �̺�Ʈ�� �޾ƿ��� ���� ��� 
	if (CEventSelect::Begin(mSession.GetSocket())) {
		End();
		return FALSE;
	}

	//������ �ּҿ� �����մϴ� 
	if (!mSession.Connect(remoteAddress, remotePort)) {
		End();
		return FALSE;
	}

	return TRUE;
}

BOOL CClientSession::BeginUdp(USHORT remotePort)
{// UDP�� ����� �� �ʱ�ȭ �ϴ� �Լ�
 // TCP�ʹ� �޸� ��Ʈ���� �Է��Ѵ�  ����? 

	//�Ķ���Ͱ� ���������� ��ũ�մϴ�. 
	if (remotePort <= 0) return FALSE;

	//����� ��ü�� �ʱ�ȭ �մϴ�. 
	if (!mSession.Begin()) {
		End();
		return FALSE;
	}

	//UDP ���ε带 ���ش� 
	if (!mSession.UDPBind(remotePort)) {
		End();
		return FALSE;
	}
	// UDP��� ���� 
	IsUdp = TRUE; 
	return TRUE;
}

BOOL CClientSession::End(VOID)
{
	//���� ��ü�� ���� 
	mSession.End();

	//EvenSelect ��ü�� ���� 
	CEventSelect::End();
	return TRUE; 
}

BOOL CClientSession::WritePacket(DWORD protocol, const BYTE * packet, DWORD packetLength)
{// TCP �����͸� �����ϴ� �Լ�  
 // �Ķ���ͷ� ����� ��������, ��Ŷ, ��Ŷ ���̸� �־��ְ� �˴ϴ�.

	//������ �����Լ��� ȣ�� 
	if (!mSession.WritePacket(protocol, packet, packetLength)) return FALSE;

	// �ٷ� WriteComplete�� ���ݴϴ�. 
	// �ֳ��ϸ� CEventSelect�� FD_WRITE�� �� ������ ���� ���� �߻����� ���� ���� 
	// �ֱ� �����Դϴ�. 
	if (!mSession.WriteComplete()) return FALSE;

	return TRUE;
}

BOOL CClientSession::WriteToPacket(LPCSTR remoteAddress, USHORT remotePort, DWORD protocol, const BYTE * packet, DWORD packetLength)
{//UDP �����͸� �����ϴ� �Լ� 
 //�Ķ���ͷ� ����� �������� ��Ŷ ��Ŷ ���̸� �־��ָ� �� 
	if (!mSession.WriteToPacket(remoteAddress, remotePort, protocol, packet, packetLength))
		return FALSE;

	if (!mSession.WriteComplete()) return FALSE;
	
	return TRUE;
}

BOOL CClientSession::ReadPacket(DWORD & protocol, BYTE * packet, DWORD & packetlength)
{
	VOID* Object = NULL;

	//ť���� ��Ŷ �� ���� �޾ƿɴϴ�. 
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
{ //�����Ͱ� �߻����� �� ȣ��Ǵ� �����Լ� 
	
	// ��Ŷ�� �����ϴ� ���� 
	BYTE      Packet[MAX_BUFFER_LENGTH] = { 0, };
	//��Ŷ�� ���̸� �����ϴ� ���� 
	DWORD     PacketLength = 0;
	//���������� �����ϴ� ���� 
	DWORD     Protocol = 0;

	//UDP�� ���� TCP�� ��츦 �����մϴ�. 
	if (IsUdp)
	{
		//UDP�� ��쿡�� ���� �ּҿ� ��Ʈ ������ �޾ƿ;� �ϱ⶧���� 
		//���� ������ ���� ������ߴ� 
		CHAR        RemoteAddress[32] = { 0, };
		USHORT      RemotePort        = 0;

		//CNetWorkSession ���� CPacketSession���� �����͸� ���� �ϰ� 
		if (mSession.ReadFromPacketForEventSelect(RemoteAddress, RemotePort))
		{   
			// ��Ŷ�� �����µڿ� packet�� ť�� ��������ݴϴ�! 
			while (mSession.GetPacket(RemoteAddress, RemotePort, Protocol, Packet, PacketLength))
				mReadPacketQueue.Push(this, Protocol, Packet, PacketLength,RemoteAddress,RemotePort);
		}
	}
	else
	{   //�̻����� ������������!  
		if (mSession.ReadPacketForEventSelect())
		{   
			// �̻����� ������������! 
			while (mSession.GetPacket(Protocol, Packet, PacketLength))
				mReadPacketQueue.Push(this, Protocol, Packet, PacketLength);
		}

	}
}
