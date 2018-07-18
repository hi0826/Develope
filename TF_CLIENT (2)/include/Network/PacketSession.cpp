#include "stdafx.h"
#include "Crypt.h"
#include "CriticalSection.h"
#include "MultiThreadSync.h"
#include "CircularQueue.h"
#include "NetworkSession.h"
#include "PacketSession.h"

CPacketSession::CPacketSession(VOID) // ������ 
{
	ZeroMemory(mPacketBuffer, sizeof(mPacketBuffer)); //�ʱ�ȭ

	mRemainLength            = 0;
	mCurrentPacketNumber     = 0;
	mLastReadPacketNumber    = 0;
	// �����ڿ����� �⺻���� �ʱ�ȭ�� �̷������. 
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

	//������ �����͸� ������ ť�� �ʱ�ȭ �մϴ�. ���� CircularQueue�Դϴ�.
	if (!WriteQueue.Begin())
		return FALSE;
	
	
	return CNetWorkSession::Begin();
}

BOOL CPacketSession::End(VOID)
{
	CThreadSync Sync;   

	mLastReadPacketInfoVectorForUdp.clear(); //UDP���� ����� ���� ��Ŷ���� ���� ���� 

	if (!WriteQueue.End())                   //ť ���� 
		return FALSE;

	return CNetWorkSession::End();           //CNetworkSession ��ü ���� 
}

BOOL CPacketSession::ReadPacketForIocp(DWORD readLength)
{ //CNetworkSession���� �����͸� �޾ƿ��� �Լ� 
  // ReadPacketForIocp �Լ���  FALSE�� �ɶ����� while���� �����ϴ�. 
  
	CThreadSync Sync; 

    //CNetworkSession�� �Լ��� �̿��ؼ� mPacketBuffer�� ���� �����͸� �Է����ݴϴ�. 
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
{ // ���ϰ��� FALSE�ϋ����� while���� ��� �����ش�. 
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
	CThreadSync Sync; //����ȭ ��ü 

	if (!packet) return FALSE;

	// LENGTH(4) | PACKET_NUMBER(4) | PROTOCOL(4) | DATA(4084)
	// ��Ŷ�� ��ü ���� ��� 
	// ��Ŷ ��ü ���� = ��Ŷ ���� + ��Ŷ ���� ��ȣ + �������� + ������ ���� 
	DWORD PacketLength = sizeof(DWORD)/*LENGTH*/ +
		                 sizeof(DWORD)/*PACKET_NUMBER*/ +
		                 sizeof(DWORD)/*PROTOCOL*/ +
		                 packetLength;

	//��Ŷ ���̰� �ִ� ���� ���̺��� ��� �����մϴ�.
	if (PacketLength >= MAX_BUFFER_LENGTH)
		return FALSE;

	//��Ŷ ��ȣ�� �ϳ� ������ŵ�ϴ�.
	mCurrentPacketNumber++;
	
	//���� �����͸� �̿��ؼ� ��Ŷ�� ����� ���� ���� 
	BYTE TempBuffer[MAX_BUFFER_LENGTH] = { 0, };

	//��Ŷ�� ����ϴ�. 
	//��Ŷ�� �ε����� �ٿ� ���� ������ ���־�� �մϴ�. 

	//1.��Ŷ�� ��ü���̸� ��Ŷ �� �� 4����Ʈ�� �־��ش�.
	CopyMemory(TempBuffer, &PacketLength, sizeof(DWORD));

	//2.��Ŷ�� ��Ŷ�ѹ�(��ȣȭ�� ��)�� �� ���� 4����Ʈ�� �־��ش� 
	CopyMemory(TempBuffer + 
		       sizeof(DWORD)/*LENGTH*/,
		       &mCurrentPacketNumber, sizeof(DWORD));

	//3.��Ŷ�� ��Ŷ��ȣ �� ���� ��Ŷ������ �� ���� 4����Ʈ�� �־��ش�.
	CopyMemory(TempBuffer +
		       sizeof(DWORD) +
		       sizeof(DWORD),
		       &protocol, sizeof(DWORD));
   
	//4.������ ������ ��Ŷ�� ���� �� ������ �־��ֵ����Ѵ�.
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
	CThreadSync Sync; //����ȭ ��ü 

	if (!packet) return FALSE;

	// LENGTH(4) | PACKET_NUMBER(4) | PROTOCOL(4) | DATA(4084)
	// ��Ŷ�� ��ü ���� ��� 
	// ��Ŷ ��ü ���� = ��Ŷ ���� + ��Ŷ ���� ��ȣ + �������� + ������ ���� 
	DWORD PacketLength = sizeof(DWORD)/*LENGTH*/ +
		sizeof(DWORD)/*PACKET_NUMBER*/ +
		sizeof(DWORD)/*PROTOCOL*/ +
		packetLength;

	//��Ŷ ���̰� �ִ� ���� ���̺��� ��� �����մϴ�.
	if (PacketLength >= MAX_BUFFER_LENGTH)
		return FALSE;

	//��Ŷ ��ȣ�� �ϳ� ������ŵ�ϴ�.
	mCurrentPacketNumber++;

	//���� �����͸� �̿��ؼ� ��Ŷ�� ����� ���� ���� 
	BYTE TempBuffer[MAX_BUFFER_LENGTH] = { 0, };

	//��Ŷ�� ����ϴ�. 
	//��Ŷ�� �ε����� �ٿ� ���� ������ ���־�� �մϴ�. 

	//1.��Ŷ�� ��ü���̸� ��Ŷ �� �� 4����Ʈ�� �־��ش�.
	CopyMemory(TempBuffer, &PacketLength, sizeof(DWORD));

	//2.��Ŷ�� ��Ŷ�ѹ�(��ȣȭ�� ��)�� �� ���� 4����Ʈ�� �־��ش� 
	CopyMemory(TempBuffer +
		sizeof(DWORD)/*LENGTH*/,
		&mCurrentPacketNumber, sizeof(DWORD));

	//3.��Ŷ�� ��Ŷ��ȣ �� ���� ��Ŷ������ �� ���� 4����Ʈ�� �־��ش�.
	CopyMemory(TempBuffer +
		sizeof(DWORD) +
		sizeof(DWORD),
		&protocol, sizeof(DWORD));

	//4.������ ������ ��Ŷ�� ���� �� ������ �־��ֵ����Ѵ�.
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
	// �����Ⱑ �Ϸ�ȵ����͸� Pop���ִ� ���� �ӽ������� Queue�� �־���� �����͸� ���ش�. 
	CThreadSync Sync; 
	//WriteQueue���� Pop���ָ� �� 
	return WriteQueue.Pop();
}

BOOL CPacketSession::ResetUdp(VOID)
{ //UDP ���� �ּ� ���͸� �����մϴ�. 
	CThreadSync Sync;
	mLastReadPacketInfoVectorForUdp.clear();
	return TRUE;
}

BOOL CPacketSession::GetPacket(DWORD & protocol, BYTE * packet, DWORD & packetLength)
{
	CThreadSync Sync; 

	if (!packet) return FALSE;
	if (mRemainLength < sizeof(DWORD)) return FALSE;
	// mRemainLength �� mPacketBuffer�� ���̸� ��Ÿ�� �ִµ�
	// 4����Ʈ �̻� ���� ������ ó���� ������ �ʽ��ϴ�. 

	INT PacketLength = 0; 
	CopyMemory(&PacketLength, mPacketBuffer, sizeof(INT));//���� 4����Ʈ�� �о ��Ŷ�� ���̸� ���մϴ�. 

	//��Ŷ ���̰� �ִ� ���� ���̸� �Ѱų� 0���� ���� ��� �߸��� ��Ŷ���� �Ǵ�, ���۸� ����ϴ�.
	if (PacketLength > MAX_BUFFER_LENGTH || PacketLength <= 0) //Invalid Packet
	{
		mRemainLength = 0;
		return FALSE;
	}

	// ���� ��Ŷ ���̰� ���ۿ� �ִ� ���̺��� �۰ų� ���� �� �� ������ �Ѱ��� ��Ŷ�� �ְų�
	// �� �̻� ���� ��
    
	if (PacketLength <= mRemainLength)
	{
		CCrypt::Decrypt(
			mPacketBuffer + sizeof(DWORD)/*LENGHT*/,
			mPacketBuffer + sizeof(DWORD)/*LENGTH*/,
			PacketLength - sizeof(DWORD)/*LENGTH*/);

		DWORD PacketNumber      = 0;
		DWORD Protocol          = 0;

		//��Ŷ���̿� ��ȣ�� �˾ƿɴϴ�. 
		CopyMemory(&PacketNumber, mPacketBuffer + sizeof(DWORD)/*LENGTH*/,
			sizeof(DWORD));
		CopyMemory(&Protocol, mPacketBuffer
			+ sizeof(DWORD)/*LENGTH*/
			+ sizeof(DWORD)/*PACKETNUMBER*/, sizeof(DWORD));

		protocol        = Protocol;
		packetLength    = PacketLength- sizeof(DWORD)/*LENGTH*/
			                          - sizeof(DWORD)/*PACKETNUMBER*/
			                          - sizeof(DWORD)/*PROTOCOL*/;
		
		//PACKET�� �����ɴϴ�.
		CopyMemory(packet, mPacketBuffer +
			     sizeof(DWORD)/*LENGTH*/ +
		   sizeof(DWORD)/*PACKETNUMBER*/ +
			   sizeof(DWORD)/*PROTOCOL*/, packetLength);
		
		
		//�� ��Ŷ�� ó���ϰ� ���� ���̰� 0���� Ŭ ��� ���� �޸𸮸� ������ ����ݴϴ�. 
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
{ //���� ��Ŷ�� �м��ϴ� �Լ��Դϴ�. 
  //�⺻�� TCP�� �����ϹǷ� �������� �ʰ� Ʋ�� �κи� �����ϵ��� �ϰڽ��ϴ�.

	CThreadSync Sync;

	if (!packet) return FALSE;

	if (mRemainLength < sizeof(DWORD)) return FALSE;

	//��Ŷ����� ī���մϴ�. 
	INT PacketLength = 0;
	CopyMemory(&PacketLength, mPacketBuffer, sizeof(INT));

	//��Ŷ�� ����� ���ۺ��� ũ�ų� 0���� ���� ��� �����մϴ�. 
	if (packetLength <= 0 || PacketLength > MAX_BUFFER_LENGTH)  //InValidSocket
	{
		mRemainLength = 0;
		return FALSE;
	}

	//���� �Ѱ� �̻��� ������ ��Ŷ�� ���� ��� 
	if (PacketLength <= mRemainLength)
	{
		//��Ŷ ������ ������ �����͵��� ��ȣȭ ���ݴϴ�. 
		CCrypt::Decrypt(mPacketBuffer + sizeof(DWORD)/*LENGTH*/,
			            mPacketBuffer + sizeof(DWORD)/*LENGTH*/,
			            PacketLength - sizeof(DWORD)/*LENGTH*/);

		DWORD PacketNumber         = 0;
		DWORD Protocol             = 0;

		//��Ŷ ��ȣ�� �о�ɴϴ�. 
		CopyMemory(&PacketNumber, mPacketBuffer + sizeof(DWORD), sizeof(DWORD));
		//�������� ��ȣ�� �о�ɴϴ�. 
		CopyMemory(&Protocol, mPacketBuffer + sizeof(DWORD)/*LENGTH*/ 
			                                + sizeof(DWORD)/*PACKETNUMBER*/
			                                , sizeof(DWORD));

		protocol     = Protocol;
		packetLength = PacketLength - sizeof(DWORD) /*LENGTH*/ 
			                        - sizeof(DWORD) /*PACKETNUMBER*/
			                        - sizeof(DWORD) /*PROTOCOL*/;

		//���� ��Ŷ �����͸� �о�ɴϴ�. 
		CopyMemory(packet, mPacketBuffer + sizeof(DWORD) /*LENGTH*/ 
			                             + sizeof(DWORD) /*PACKETNUMBER*/
			                             + sizeof(DWORD) /*PROTOCOL*/
			                             , PacketLength);

		//���ۿ��� ��Ŷ�� �ϳ� �а� ���۸� �������ݴϴ�. �� ���� ��Ŷ ���� �������� ������ ����ݴϴ�
		if (mRemainLength - PacketLength > 0)
			MoveMemory(mPacketBuffer, mPacketBuffer + PacketLength
				                    , mRemainLength - PacketLength);
		
		//������ŭ �����ſ��� ������� 
		mRemainLength -= PacketLength;

		if (mRemainLength <= 0)
		{
			mRemainLength = 0;
			ZeroMemory(mPacketBuffer, sizeof(mPacketBuffer));
		}

		//���⼭�� TCP�� GetPacket�� �ٸ� �κ� 
		//mLastReadPacketInfoVectorForUdp ���� ��� ���� ��Ŷ�� �������� �ִ��� �˻� 
        //���� ���� ��� �� ������ ��Ŷ ��ȣ���� ������Ʈ ���ݴϴ�. 
		for (DWORD i = 0; i < mLastReadPacketInfoVectorForUdp.size(); ++i)
		{
			READ_PACKET_INFO ReadPacketInfo = mLastReadPacketInfoVectorForUdp[i];
			
			//���� �ּ�, ���� ��Ʈ�� ������ �̹� ���� mLastReadPacketInfoVectorForUdp�� �ִ��� �˻�
			if (!strcmp(ReadPacketInfo.RemoteAddress, remoteAddress) &&
				ReadPacketInfo.RemotePort == remotePort)
			{
				//���� ���� �������� ��Ŷ ��ȣ�� ���ݱ����� ��ȣ���� ū ��ȣ�� ��� ������Ʈ�� ���ݴϴ�
				//�׸��� TRUE��ȯ 
				if (ReadPacketInfo.PacketNumber < PacketNumber)
				{
					mLastReadPacketInfoVectorForUdp[i].PacketNumber = PacketNumber;
					return TRUE;
				}
				else
					return FALSE;
			}
		}
		//���� ���ο� �ּҿ��� ���� �������� ��� ���� ���Ϳ� �߰����ݴϴ�. 
		READ_PACKET_INFO ReadPacketInfo;
		ReadPacketInfo.PacketNumber = PacketNumber;
		ReadPacketInfo.RemotePort = remotePort;
		strcpy(ReadPacketInfo.RemoteAddress, remoteAddress);

		mLastReadPacketInfoVectorForUdp.push_back(ReadPacketInfo);
		return TRUE;
	}

	//������ ��Ŷ���µ� ������쿡~ 
	return FALSE;
}

