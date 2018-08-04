#pragma once
template <typename T>
class CSingleTone
{
protected:
	CSingleTone() {}
	virtual ~CSingleTone() {}

private:
	static T* m_pInstance;

public:
	static T* GET_SINGLE()
	{
		if (!m_pInstance) m_pInstance = new T;
		return m_pInstance;
	};

	static void DESTROY_SINGLE()
	{
		if (m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = NULL;
		}
	}
};
template <typename T> T*   CSingleTone<T>::m_pInstance = 0;