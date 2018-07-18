#include "stdafx.h"
#include "Crypt.h"
#include "CriticalSection.h"
#include "MultiThreadSync.h"
#include "CircularQueue.h"
#include "NetworkSession.h"
#include "PacketSession.h"

CPacketSession::CPacketSession(VOID) // 생성자 
{
	ZeroMemory(mPacketBuffer, sizeof(mPacketBuffer)); //초기화

	mRemainLength            = 0;
	mCurrentPacketNumber     = 0;
	mLastReadPacketNumber    = 0;
	// 생성자에서는 기본적인 초기화가 이루어진다. 
}

CPacketSession::~CPacketSession(VOID)
{
}

BOOL CPacketSession::Begin(VOID)
{

	CThreadSync  Sync;

	ZeroMemory(mPacketBuffer, sizeof(mPacketBuffer));

	mRemainLength         = 0;
	mCurrentPacketNumber  = 0;
	mLastReadPacketNumber = 0;

	//보내는 데이터를 저장할 큐를 초기화 합니다. 역시 CircularQueue입니다.
	if (!WriteQueue.Begin())
		return FALSE;
	
	
	return CNetWorkSession::Begin();
}

BOOL CPacketSession::End(VOID)
{
	CThreadSync Sync;   

	mLastReadPacketInfoVectorForUdp.clear(); //UDP에서 사용할 받은 패킷정보 저장 벡터 

	if (!WriteQueue.End())                   //큐 종료 
		return FALSE;

	return CNetWorkSession::End();           //CNetworkSession 개체 종료 
}

BOOL CPacketSession::ReadPacketForIocp(DWORD readLength)
{ //CNetworkSession에서 데이터를 받아오는 함수 
  // ReadPacketForIocp 함수는  FALSE가 될때까찌 while문을 돌립니다. 
  
	CThreadSync Sync; 

    //CNetworkSession의 함수를 이용해서 mPacketBuffer에 받은 데이터를 입력해줍니다. 
	if (!CNetWorkSession::ReadForIocp(mPacketBuffer + mRemainLength, readLength))
		return FALSE;

	mRemainLength += readLength;
	return TRUE;
}

BOOL CPacketSession::ReadPacketForEventSelect(VOID)
{
	CThreadSync Sync; 
	
	DWORD ReadLength = 0; 

	if (!CNetWorkSession::ReadForEventSelect(mPacketBuffer + mRemainLength, ReadLength))
		return FALSE;

	mRemainLength += ReadLength;
	return TRUE;
}

BOOL CPacketSession::ReadFromPacketForIocp(LPSTR remoteAddress, USHORT & remotePort, DWORD readLength)
{ // 리턴값이 FALSE일떄까지 while문을 계속 돌려준다. 
	CThreadSync Sync; 

	if (!CNetWorkSession::ReadFromForIocp(remoteAddress, remotePort, mPacketBuffer + mRemainLength, readLength))
	    return FALSE;

	mRemainLength += readLength;

	return TRUE; 

}

BOOL CPacketSession::ReadFromPacketForEventSelect(LPSTR remoteAddress, USHORT & remotePort)
{
	CThreadSync Sync; 

	DWORD ReadLength = 0;

	if (!CNetWorkSession::ReadFromForEventSelect(remoteAddress, remotePort, mPacketBuffer + mRemainLength, ReadLength))
		return FALSE;

	mRemainLength += ReadLength;

	return TRUE; 
}

BOOL CPacketSession::WritePacket(DWORD protocol, const BYTE * packet, DWORD packetLength)
{
	CThreadSync Sync; //동기화 개체 

	if (!packet) return FALSE;

	// LENGTH(4) | PACKET_NUMBER(4) | PROTOCOL(4) | DATA(4084)
	// 패킷의 전체 길이 계산 
	// 패킷 전체 길이 = 패킷 길이 + 패킷 고유 번호 + 프로토콜 + 데이터 길이 
	DWORD PacketLength = sizeof(DWORD)/*LENGTH*/ +
		                 sizeof(DWORD)/*PACKET_NUMBER*/ +
		                 sizeof(DWORD)/*PROTOCOL*/ +
		                 packetLength;

	//패킷 길이가 최대 버퍼 길이보다 길면 실패합니다.
	if (PacketLength >= MAX_BUFFER_LENGTH)
		return FALSE;

	//패킷 번호를 하나 증가시킵니다.
	mCurrentPacketNumber++;
	
	//들어온 데이터를 이용해서 패킷을 만들기 위한 버퍼 
	BYTE TempBuffer[MAX_BUFFER_LENGTH] = { 0, };

	//패킷을 만듭니다. 
	//패킷에 인덱스를 붙여 순서 보정을 해주어야 합니다. 

	//1.패킷의 전체길이를 패킷 맨 앞 4바이트에 넣어준다.
	CopyMemory(TempBuffer, &PacketLength, sizeof(DWORD));

	//2.패킷의 패킷넘버(암호화될 것)를 그 다음 4바이트에 넣어준다 
	CopyMemory(TempBuffer + 
		       sizeof(DWORD)/*LENGTH*/,
		       &mCurrentPacketNumber, sizeof(DWORD));

	//3.패킷의 패킷번호 즉 무슨 패킷인지를 그 다음 4바이트에 넣어준다.
	CopyMemory(TempBuffer +
		       sizeof(DWORD) +
		       sizeof(DWORD),
		       &protocol, sizeof(DWORD));
   
	//4.나머지 공간에 패킷의 내용 즉 정보를 넣어주도록한다.
	CopyMemory(TempBuffer +
		       sizeof(DWORD) +
		       sizeof(DWORD) +
		       sizeof(DWORD),
		       packet, packetLength);

	CCrypt::Encrypt(TempBuffer + sizeof(DWORD), TempBuffer + sizeof(DWORD), PacketLength-sizeof(DWORD));

	BYTE* writeData = WriteQueue.Push(this, TempBuffer, PacketLength);

	return CNetWorkSession::Write(writeData, PacketLength);
}

BOOL CPacketSession::WriteToPacket(LPCSTR remoteAddress, USHORT remotePort, DWORD protocol, const BYTE * packet, DWORD packetLength)
{
	CThreadSync Sync; //동기화 개체 

	if (!packet) return FALSE;

	// LENGTH(4) | PACKET_NUMBER(4) | PROTOCOL(4) | DATA(4084)
	// 패킷의 전체 길이 계산 
	// 패킷 전체 길이 = 패킷 길이 + 패킷 고유 번호 + 프로토콜 + 데이터 길이 
	DWORD PacketLength = sizeof(DWORD)/*LENGTH*/ +
		sizeof(DWORD)/*PACKET_NUMBER*/ +
		sizeof(DWORD)/*PROTOCOL*/ +
		packetLength;

	//패킷 길이가 최대 버퍼 길이보다 길면 실패합니다.
	if (PacketLength >= MAX_BUFFER_LENGTH)
		return FALSE;

	//패킷 번호를 하나 증가시킵니다.
	mCurrentPacketNumber++;

	//들어온 데이터를 이용해서 패킷을 만들기 위한 버퍼 
	BYTE TempBuffer[MAX_BUFFER_LENGTH] = { 0, };

	//패킷을 만듭니다. 
	//패킷에 인덱스를 붙여 순서 보정을 해주어야 합니다. 

	//1.패킷의 전체길이를 패킷 맨 앞 4바이트에 넣어준다.
	CopyMemory(TempBuffer, &PacketLength, sizeof(DWORD));

	//2.패킷의 패킷넘버(암호화될 것)를 그 다음 4바이트에 넣어준다 
	CopyMemory(TempBuffer +
		sizeof(DWORD)/*LENGTH*/,
		&mCurrentPacketNumber, sizeof(DWORD));

	//3.패킷의 패킷번호 즉 무슨 패킷인지를 그 다음 4바이트에 넣어준다.
	CopyMemory(TempBuffer +
		sizeof(DWORD) +
		sizeof(DWORD),
		&protocol, sizeof(DWORD));

	//4.나머지 공간에 패킷의 내용 즉 정보를 넣어주도록한다.
	CopyMemory(TempBuffer +
		sizeof(DWORD) +
		sizeof(DWORD) +
		sizeof(DWORD),
		packet, packetLength);

	CCrypt::Encrypt(TempBuffer + sizeof(DWORD), TempBuffer + sizeof(DWORD), PacketLength - sizeof(DWORD));

	BYTE* writeData = WriteQueue.Push(this, TempBuffer, PacketLength);

	return CNetWorkSession::WriteTo(remoteAddress,remotePort,writeData, PacketLength);
}

BOOL CPacketSession::WriteComplete(VOID)
{   
	// 보내기가 완료된데이터를 Pop해주는 역할 임시적으로 Queue에 넣어줬던 데이터를 빼준다. 
	CThreadSync Sync; 
	//WriteQueue에서 Pop해주면 됨 
	return WriteQueue.Pop();
}

BOOL CPacketSession::ResetUdp(VOID)
{ //UDP 받은 주소 벡터를 리셋합니다. 
	CThreadSync Sync;
	mLastReadPacketInfoVectorForUdp.clear();
	return TRUE;
}

BOOL CPacketSession::GetPacket(DWORD & protocol, BYTE * packet, DWORD & packetLength)
{
	CThreadSync Sync; 

	if (!packet) return FALSE;
	if (mRemainLength < sizeof(DWORD)) return FALSE;
	// mRemainLength 는 mPacketBuffer의 길이를 나타내 주는데
	// 4바이트 이상 받지 않으면 처리를 해주지 않습니다. 

	INT PacketLength = 0; 
	CopyMemory(&PacketLength, mPacketBuffer, sizeof(INT));//앞의 4바이트를 읽어서 패킷의 길이를 구합니다. 

	//패킷 길이가 최대 버퍼 길이를 넘거나 0보다 작은 경우 잘못된 패킷으로 판단, 버퍼를 지웁니다.
	if (PacketLength > MAX_BUFFER_LENGTH || PacketLength <= 0) //Invalid Packet
	{
		mRemainLength = 0;
		return FALSE;
	}

	// 읽은 패킷 길이가 버퍼에 있는 길이보다 작거나 같을 때 즉 온전한 한개의 패킷이 있거나
	// 그 이상 있을 때
    
	if (PacketLength <= mRemainLength)
	{
		CCrypt::Decrypt(
			mPacketBuffer + sizeof(DWORD)/*LENGHT*/,
			mPacketBuffer + sizeof(DWORD)/*LENGTH*/,
			PacketLength - sizeof(DWORD)/*LENGTH*/);

		DWORD PacketNumber      = 0;
		DWORD Protocol          = 0;

		//패킷길이와 번호를 알아옵니다. 
		CopyMemory(&PacketNumber, mPacketBuffer + sizeof(DWORD)/*LENGTH*/,
			sizeof(DWORD));
		CopyMemory(&Protocol, mPacketBuffer
			+ sizeof(DWORD)/*LENGTH*/
			+ sizeof(DWORD)/*PACKETNUMBER*/, sizeof(DWORD));

		protocol        = Protocol;
		packetLength    = PacketLength- sizeof(DWORD)/*LENGTH*/
			                          - sizeof(DWORD)/*PACKETNUMBER*/
			                          - sizeof(DWORD)/*PROTOCOL*/;
		
		//PACKET을 가져옵니다.
		CopyMemory(packet, mPacketBuffer +
			     sizeof(DWORD)/*LENGTH*/ +
		   sizeof(DWORD)/*PACKETNUMBER*/ +
			   sizeof(DWORD)/*PROTOCOL*/, packetLength);
		
		
		//한 패킷을 처리하고 남은 길이가 0보다 클 경우 뒤의 메모리를 앞으로 당겨줍니다. 
		if (mRemainLength - PacketLength > 0)
			MoveMemory(mPacketBuffer, mPacketBuffer + PacketLength, mRemainLength - PacketLength);

		mRemainLength -= PacketLength;

		if (mRemainLength <= 0)
		{
			mRemainLength = 0;
			ZeroMemory(mPacketBuffer, sizeof(mPacketBuffer));
		}

		if (PacketNumber <= mLastReadPacketNumber)
			return FALSE;
		else
		{
			mLastReadPacketNumber = PacketNumber;
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CPacketSession::GetPacket(LPSTR remoteAddress, USHORT remotePort, DWORD & protocol, BYTE * packet, DWORD & packetLength)
{ //받은 패킷을 분석하는 함수입니다. 
  //기본은 TCP와 동일하므로 설명하지 않고 틀린 부분만 설명하도록 하겠습니다.

	CThreadSync Sync;

	if (!packet) return FALSE;

	if (mRemainLength < sizeof(DWORD)) return FALSE;

	//패킷사이즈를 카피합니다. 
	INT PacketLength = 0;
	CopyMemory(&PacketLength, mPacketBuffer, sizeof(INT));

	//패킷의 사이즈가 버퍼보다 크거나 0보다 작을 경우 무시합니다. 
	if (packetLength <= 0 || PacketLength > MAX_BUFFER_LENGTH)  //InValidSocket
	{
		mRemainLength = 0;
		return FALSE;
	}

	//만약 한개 이상의 완전한 패킷이 있을 경우 
	if (PacketLength <= mRemainLength)
	{
		//패킷 사이즈 이후의 데이터들을 복호화 해줍니다. 
		CCrypt::Decrypt(mPacketBuffer + sizeof(DWORD)/*LENGTH*/,
			            mPacketBuffer + sizeof(DWORD)/*LENGTH*/,
			            PacketLength - sizeof(DWORD)/*LENGTH*/);

		DWORD PacketNumber         = 0;
		DWORD Protocol             = 0;

		//패킷 번호를 읽어옵니다. 
		CopyMemory(&PacketNumber, mPacketBuffer + sizeof(DWORD), sizeof(DWORD));
		//프로토콜 번호를 읽어옵니다. 
		CopyMemory(&Protocol, mPacketBuffer + sizeof(DWORD)/*LENGTH*/ 
			                                + sizeof(DWORD)/*PACKETNUMBER*/
			                                , sizeof(DWORD));

		protocol     = Protocol;
		packetLength = PacketLength - sizeof(DWORD) /*LENGTH*/ 
			                        - sizeof(DWORD) /*PACKETNUMBER*/
			                        - sizeof(DWORD) /*PROTOCOL*/;

		//실제 패킷 데이터를 읽어옵니다. 
		CopyMemory(packet, mPacketBuffer + sizeof(DWORD) /*LENGTH*/ 
			                             + sizeof(DWORD) /*PACKETNUMBER*/
			                             + sizeof(DWORD) /*PROTOCOL*/
			                             , PacketLength);

		//버퍼에서 패킷을 하나 읽고 버퍼를 정리해줍니다. 즉 읽은 패킷 외의 나머지를 앞으로 당겨줍니다
		if (mRemainLength - PacketLength > 0)
			MoveMemory(mPacketBuffer, mPacketBuffer + PacketLength
				                    , mRemainLength - PacketLength);
		
		//읽은만큼 남은거에서 빼줘야지 
		mRemainLength -= PacketLength;

		if (mRemainLength <= 0)
		{
			mRemainLength = 0;
			ZeroMemory(mPacketBuffer, sizeof(mPacketBuffer));
		}

		//여기서는 TCP의 GetPacket과 다른 부분 
		//mLastReadPacketInfoVectorForUdp 에서 방금 받은 패킷을 받은적이 있는지 검사 
        //만약 있을 경우 그 정보에 패킷 번호만을 업데이트 해줍니다. 
		for (DWORD i = 0; i < mLastReadPacketInfoVectorForUdp.size(); ++i)
		{
			READ_PACKET_INFO ReadPacketInfo = mLastReadPacketInfoVectorForUdp[i];
			
			//같은 주소, 같은 포트의 정보가 이미 벡터 mLastReadPacketInfoVectorForUdp에 있는지 검사
			if (!strcmp(ReadPacketInfo.RemoteAddress, remoteAddress) &&
				ReadPacketInfo.RemotePort == remotePort)
			{
				//만약 받은 데이터의 패킷 번호가 지금까지의 번호보다 큰 번호일 경우 업데이트를 해줍니다
				//그리고 TRUE반환 
				if (ReadPacketInfo.PacketNumber < PacketNumber)
				{
					mLastReadPacketInfoVectorForUdp[i].PacketNumber = PacketNumber;
					return TRUE;
				}
				else
					return FALSE;
			}
		}
		//만약 새로운 주소에서 받은 데이터일 경우 새로 벡터에 추가해줍니다. 
		READ_PACKET_INFO ReadPacketInfo;
		ReadPacketInfo.PacketNumber = PacketNumber;
		ReadPacketInfo.RemotePort = remotePort;
		strcpy(ReadPacketInfo.RemoteAddress, remoteAddress);

		mLastReadPacketInfoVectorForUdp.push_back(ReadPacketInfo);
		return TRUE;
	}

	//온전한 패킷상태도 없을경우에~ 
	return FALSE;
}

