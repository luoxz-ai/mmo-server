#ifndef IDGENPOOL_H
#define IDGENPOOL_H

#include<deque>

template<class IDType>
class IDGenPool
{
public:  
    IDGenPool(IDType startID = 0)
    :m_lastID(startID)
    {
    }
    
    void start(IDType startID)
    {
        m_lastID = startID;
    }

    IDType get()
    {
        if(m_idPool.empty())
        {
            return m_lastID++;
        }
        else
        {
            IDType id = m_idPool.front();
            m_idPool.pop_front();
            return id;
        }
    }

    void put(IDType id)
    {
        m_idPool.push_back(id);
    }
private:
    std::deque<IDType> m_idPool;
    IDType m_lastID;
    
};

#endif /* IDGENPOOL_H */
