#include "DB.h"

CDB::CDB()
{
}

CDB::~CDB()
{
}


bool CDB::ConnectDB()
{
	// Allocate environment handle
	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);

	// Set the ODBC version environment attribute
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	{
		retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);

		// Allocate connection handle
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
			retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

			// Set login timeout to 5 seconds
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
			{
				SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);

				// Connect to data source
				retcode = SQLConnect(hdbc, (SQLWCHAR*)L"2018_ODBC_TW", SQL_NTS, (SQLWCHAR*)L"sa", SQL_NTS, (SQLWCHAR*)L"gameserver", SQL_NTS);

				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
				{
					retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

					// Process data
					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
					{
						cout << "[DB Connection Success!]" << endl << endl;
					}
				}
				else
				{
					cout << "[DB unConnection]" << endl << endl;
					HandleDiagnosticRecord(hdbc, SQL_HANDLE_DBC, retcode);
					return false; 
				}
			}
		}
	}
	return true; 
}

void CDB::DisConnectDB()
{
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
	SQLFreeHandle(SQL_HANDLE_ENV, henv);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	SQLDisconnect(hdbc);
}

void CDB::HandleDiagnosticRecord(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode)
{
	_wsetlocale(LC_ALL, L"Korean");

	SQLSMALLINT iRec = 0;
	SQLINTEGER  iError;
	WCHAR       wszMessage[1000];
	WCHAR       wszState[SQL_SQLSTATE_SIZE + 1];

	if (RetCode == SQL_INVALID_HANDLE) {
		fwprintf(stderr, L"Invalid handle!\n");
		return;
	}
	while (SQLGetDiagRec(hType, hHandle, ++iRec, wszState, &iError, wszMessage,
		(SQLSMALLINT)(sizeof(wszMessage) / sizeof(WCHAR)), (SQLSMALLINT *)NULL) == SQL_SUCCESS) {
		// Hide data truncated..
		if (wcsncmp(wszState, L"01004", 5)) {
			fwprintf(stderr, L"[%5.5s] %s (%d)\n", wszState, wszMessage, iError);
		}
	}
}

OBJECT_DATA CDB::GetUserData(int id, CUser& user, char*name, bool* issucc)
{   
	*issucc = false; 
	WCHAR EXECORDER[256];
	OBJECT_DATA Data;

	SQLLEN L_ID,L_HP,L_MP,L_ATK,L_X,L_Y,L_Z;
	SQLINTEGER  HP, MP, ATK;
	SQLFLOAT POSX, POSY, POSZ;
	SQLWCHAR  ID[50];
	
	wsprintf(EXECORDER, L"EXEC dbo.LoadUserData %S", name);
	if (SQLExecDirect(hstmt, (SQLWCHAR*)EXECORDER, SQL_NTS) == SQL_ERROR)
	{  
		HandleDiagnosticRecord(hdbc, SQL_HANDLE_DBC, retcode);
	}

	if (retcode != SQL_ERROR)
	{
		retcode = SQLBindCol(hstmt, 1, SQL_C_WCHAR,&ID,  100, &L_ID);
		retcode = SQLBindCol(hstmt, 4, SQL_C_LONG, &HP,  100, &L_HP);
		retcode = SQLBindCol(hstmt, 5, SQL_C_LONG, &MP,  100, &L_MP);
		retcode = SQLBindCol(hstmt, 9, SQL_C_LONG, &ATK, 100, &L_ATK);
		retcode = SQLBindCol(hstmt, 6, SQL_DOUBLE, &POSX,100, &L_X);
		retcode = SQLBindCol(hstmt, 7, SQL_DOUBLE, &POSY,100, &L_Y);
		retcode = SQLBindCol(hstmt, 8, SQL_DOUBLE, &POSZ,100, &L_Z);
		for (int i = 0; ; i++)
		{ 
			retcode = SQLFetch(hstmt);
			if (retcode == SQL_NO_DATA)
			{
				break;
			}
			else if (SQL_ERROR != retcode || SQL_SUCCESS == retcode)
			{
				HandleDiagnosticRecord(hdbc, SQL_HANDLE_DBC, retcode);
				*issucc = true; 
				break;
			}
			//else *issucc =false;
		}
	
	}

	user.SetName(name);
	Data.ID    = id;
	Data.HP    = HP;
	Data.MP    = MP;
	Data.ATK   = ATK;
	Data.POS.x = POSX;
	Data.POS.y = POSY;
	Data.POS.z = POSZ;
	SQLCloseCursor(hstmt);
	return Data;
}

void CDB::SaveUserData(CUser& user)
{
	WCHAR EXECORDER[256];
	wsprintf(EXECORDER, TEXT("EXEC dbo.SaveUserData %S,%d,%d,%d,%f,%f,%f "),
		user.GetName(),user.GetHP(), user.GetMP(), user.GetATK(),
		user.GetPOS().x, user.GetPOS().y, user.GetPOS().z);

	if (SQLExecDirect(hstmt, (SQLWCHAR*)EXECORDER, SQL_NTS) == SQL_ERROR)
		HandleDiagnosticRecord(hdbc, SQL_HANDLE_DBC, retcode);

	if (retcode != SQL_ERROR)
	{
		while (SQLFetch(hstmt) == SQL_SUCCESS)
		{
		}
	}
	else printf("DB:: UserData Save Faild....\n");

	SQLCloseCursor(hstmt);
}
