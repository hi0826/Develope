#pragma once
#include "../Util/SingleTone.h"
#include "../Include.h"
#include "../Object/User.h"
class CDB :public CSingleTone<CDB>
{
private:
	SQLHENV   henv;
	SQLHDBC   hdbc;
	SQLHSTMT  hstmt;
	SQLRETURN retcode;
public:
	CDB();
	virtual ~CDB();

public:
	bool ConnectDB();
	void DisConnectDB();
	void HandleDiagnosticRecord(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode);

public:
	OBJECT_DATA GetUserData(int id, CUser& user, char*name, bool* issucc);
	void SaveUserData(CUser& user);
};

