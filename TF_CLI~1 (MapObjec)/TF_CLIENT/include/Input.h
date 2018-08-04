#pragma once
#include "SingleTone.h"

//�������� ����Ű�� ���� �� ������ŭ
// ���ڵ� �ᱹ�� ���� 
typedef struct _KeyInfo
{
	std::string strName;
	std::vector<DWORD> vecKey;
	bool bKeyDown;  //ó�� ������
	bool bKeyPress; //������ ������
	bool bKeyUp;    //Ű�������� 

	_KeyInfo() : bKeyDown(false), bKeyPress(false), bKeyUp(false) {}
}KEYINFO, *PKEYINFO;
//�پ��� Ű������ ���� �� �ְԲ�..

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
	//��ũ�� �Լ�
	template <typename T>
	bool AddKey(const T& data)
	{
	
		//Typeid => Ÿ���� ������ �ĺ���ȣ ������� 
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

		//Typeid => Ÿ���� ������ �ĺ���ȣ ������� 
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

