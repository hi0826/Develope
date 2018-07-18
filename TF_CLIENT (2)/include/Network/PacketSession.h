#pragma once

typedef struct _READ_PACKET_INFO
{ // 기본적으로 UDP는 보낼때 마다 상대방 주소와 포트 번호를 알아야하므로 저장할것이 필요하다
	CHAR       RemoteAddress[14];     // 데이터를 받은 주소
	USHORT     RemotePort;            // 데이터를 받은 포트번호
	DWORD      PacketNumber;          // 받은 베이터의 패킷 번호 
}READ_PACKET_INFO;


class CPacketSession : public CNetWorkSession
{
public:
	CPacketSession(VOID);
	virtual ~CPacketSession(VOID);

private:
	BYTE                           mPacketBuffer[MAX_BUFFER_LENGTH * 3]; // 4096*3 
	INT                            mRemainLength; //mPacketBuffer에 남은 길이, 즉 패킷을 처리하고 nagle알고리즘에 의해 완성되지않은 패킷이 남은 길이이다. 
	DWORD                          mCurrentPacketNumber; // 현재 상대에서 보낼때 사용할 패킷 번호 

	DWORD                          mLastReadPacketNumber; //마지막으로 받은 패킷번호, 이것으로 Reliable UDP에서 받은 패킷인지 아닌지 확인한다. 

	CCircularQueue                 WriteQueue; 
	
	std::vector<READ_PACKET_INFO>  mLastReadPacketInfoVectorForUdp;


public:

	//기본적인 초기화 함수와 종료 함수 
	BOOL     Begin(VOID);
	BOOL     End(VOID);


	BOOL     ReadPacketForIocp(DWORD readLength); //TCP
 	BOOL     ReadPacketForEventSelect(VOID); //TCP

	BOOL     ReadFromPacketForIocp(LPSTR remoteAddress, USHORT& remotePort, DWORD readLength); //UDP
	BOOL     ReadFromPacketForEventSelect(LPSTR remoteAddress, USHORT& remotePort); //UDP
   
	BOOL     WritePacket(DWORD protocol, const BYTE* packet, DWORD packetLength); //TCP
	BOOL     WriteToPacket(LPCSTR remoteAddress, USHORT remotePort, DWORD protocol, const BYTE* packet, DWORD packetLength); //UDP
	BOOL     WriteComplete(VOID); //Send완료시 호출한다. 

	BOOL     ResetUdp(VOID);

	BOOL     GetPacket(DWORD& protocol, BYTE* packet, DWORD& packetLength); //TCP
	BOOL     GetPacket(LPSTR remoteAddress, USHORT remotePort, DWORD& protocol, BYTE* packet, DWORD& packetLength); //UDP

};

