#pragma once

typedef struct _READ_PACKET_INFO
{ // �⺻������ UDP�� ������ ���� ���� �ּҿ� ��Ʈ ��ȣ�� �˾ƾ��ϹǷ� �����Ұ��� �ʿ��ϴ�
	CHAR       RemoteAddress[14];     // �����͸� ���� �ּ�
	USHORT     RemotePort;            // �����͸� ���� ��Ʈ��ȣ
	DWORD      PacketNumber;          // ���� �������� ��Ŷ ��ȣ 
}READ_PACKET_INFO;


class CPacketSession : public CNetWorkSession
{
public:
	CPacketSession(VOID);
	virtual ~CPacketSession(VOID);

private:
	BYTE                           mPacketBuffer[MAX_BUFFER_LENGTH * 3]; // 4096*3 
	INT                            mRemainLength; //mPacketBuffer�� ���� ����, �� ��Ŷ�� ó���ϰ� nagle�˰��� ���� �ϼ��������� ��Ŷ�� ���� �����̴�. 
	DWORD                          mCurrentPacketNumber; // ���� ��뿡�� ������ ����� ��Ŷ ��ȣ 

	DWORD                          mLastReadPacketNumber; //���������� ���� ��Ŷ��ȣ, �̰����� Reliable UDP���� ���� ��Ŷ���� �ƴ��� Ȯ���Ѵ�. 

	CCircularQueue                 WriteQueue; 
	
	std::vector<READ_PACKET_INFO>  mLastReadPacketInfoVectorForUdp;


public:

	//�⺻���� �ʱ�ȭ �Լ��� ���� �Լ� 
	BOOL     Begin(VOID);
	BOOL     End(VOID);


	BOOL     ReadPacketForIocp(DWORD readLength); //TCP
 	BOOL     ReadPacketForEventSelect(VOID); //TCP

	BOOL     ReadFromPacketForIocp(LPSTR remoteAddress, USHORT& remotePort, DWORD readLength); //UDP
	BOOL     ReadFromPacketForEventSelect(LPSTR remoteAddress, USHORT& remotePort); //UDP
   
	BOOL     WritePacket(DWORD protocol, const BYTE* packet, DWORD packetLength); //TCP
	BOOL     WriteToPacket(LPCSTR remoteAddress, USHORT remotePort, DWORD protocol, const BYTE* packet, DWORD packetLength); //UDP
	BOOL     WriteComplete(VOID); //Send�Ϸ�� ȣ���Ѵ�. 

	BOOL     ResetUdp(VOID);

	BOOL     GetPacket(DWORD& protocol, BYTE* packet, DWORD& packetLength); //TCP
	BOOL     GetPacket(LPSTR remoteAddress, USHORT remotePort, DWORD& protocol, BYTE* packet, DWORD& packetLength); //UDP

};

