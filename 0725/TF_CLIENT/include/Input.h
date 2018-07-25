#pragma once
#include "SingleTone.h"

//여러개의 가변키를 넣을 수 있을만큼
// 문자도 결국은 숫자 
typedef struct _KeyInfo
{
	std::string strName;
	std::vector<DWORD> vecKey;
	bool bKeyDown;  //처음 누를때
	bool bKeyPress; //누르고 있을때
	bool bKeyUp;    //키를땠을때 

	_KeyInfo() : bKeyDown(false), bKeyPress(false), bKeyUp(false) {}
}KEYINFO, *PKEYINFO;
//다양한 키조합을 넣을 수 있게끔..

class CInput :public CSingleTone<CInput>
{
private:
	std::unordered_map<string, PKEYINFO> m_mapKey;
	HWND                                 m_hwnd;
	PKEYINFO                             m_pCreateKey;

public:
	CInput();
	~CInput();

public:
	//매크로 함수
	template <typename T>
	bool AddKey(const T& data)
	{
	
		//Typeid => 타입의 고유한 식별번호 만들어줌 
		const char* pTType = typeid(T).name();

		if (strcmp(pTType, "char") == 0 || strcmp(pTType, "int") == 0)
		{
			m_pCreateKey->vecKey.push_back((DWORD)data);
		}
		else
		{
			m_pCreateKey->strName = data;
			m_mapKey.insert(make_pair(m_pCreateKey->strName, m_pCreateKey));
		}

		return true; 
	
	}

	template <typename T, typename ... Types>
	bool  AddKey(const T& data, const Types& ... arg)
	{
		if (!m_pCreateKey) m_pCreateKey = new KEYINFO;

		//Typeid => 타입의 고유한 식별번호 만들어줌 
		const char* pTType = typeid(T).name();

		if (strcmp(pTType, "char") == 0 || strcmp(pTType, "int") == 0)
		{
			m_pCreateKey->vecKey.push_back((DWORD)data);
		}
		else
		{
			m_pCreateKey->strName = data;
			m_mapKey.insert(make_pair(m_pCreateKey->strName, m_pCreateKey));
		}

		AddKey(arg...); 
		if (m_pCreateKey) m_pCreateKey = NULL;
		return true;
	}

private:
	PKEYINFO FindKey(const string& strKey) const;

public:
	bool Initialize(HWND hWnd);
	void Update(float fDeltatime);
	bool KeyPress(const string& strKey) const;
	bool KeyDown(const string& strKey) const;
	bool KeyUp(const string& strKey) const;

};

