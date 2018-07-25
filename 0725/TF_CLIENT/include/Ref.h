#pragma once
class CRef
{
protected:
	int    m_nReferences = 0;
public:
	CRef();
	virtual ~CRef() =0;
public:
	void AddRef()  { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
};

