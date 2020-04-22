#ifndef IDGENPOOL_H
#define IDGENPOOL_H

#include <deque>
#include "LoggingMgr.h"

template<class IDType>
class IDGenPool
{
public:  
    IDGenPool(){}
    ~IDGenPool(){}
    
    void start(IDType startID, IDType max_count)
    {
        m_lastID = startID;
        m_maxID = startID + max_count;
    }
    

    IDType get()
    {
        if(m_idPool.empty())
        {
            m_lastID++;
            if(m_lastID >= m_maxID)
            {
                LOGFATAL("id overflow id:{}", m_lastID);
                m_lastID = m_maxID;
            }
            return m_lastID;
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
    IDType m_lastID = 0;
    IDType m_maxID = 0;
    
};

#endif /* IDGENPOOL_H */
