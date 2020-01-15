#ifndef SINGLETON_H
#define SINGLETON_H

// thread unsafe

template<class T>
class Singleton
{
	Singleton() {}

public:
	virtual ~Singleton() {}

	T* Instance()
	{
		if(m_ptr == nullptr)
		{
			if(m_bReleased)
				return false;
			m_ptr = new Singleton;
		}
		return m_ptr;
	}

	void Release()
	{
		m_bReleased = true;
		if(m_ptr)
			delete m_ptr;
		m_ptr = nullptr;
	}

private:
	T*	 m_ptr		 = nullptr;
	bool m_bReleased = false;
};
#endif /* SINGLETON_H */
