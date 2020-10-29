#ifndef PerSecondCount_h__
#define PerSecondCount_h__

#include <atomic>
#include <chrono>
#include <deque>
#include <numeric>

#include <time.h>

class PerSecondCount
{
public:
    PerSecondCount() {}

    size_t GetTotal() { return m_nTotal; }
    float  GetAvgBPS()
    {
        CalcAvgBPS();
        return m_fAvgBPS;
    }
    void CalcAvgBPS()
    {
        std::chrono::high_resolution_clock::time_point curTime = std::chrono::high_resolution_clock::now();
        if(curTime > m_NextCalcTime)
        {
            size_t                       nCur = m_nCur.exchange(0);
            std::chrono::duration<float> dur  = curTime - m_NextCalcTime + std::chrono::seconds(1);
            float                        bps  = (float)nCur / dur.count() / 1024.0f;
            m_BPSData.push_back(bps);
            if(m_BPSData.size() > 10)
                m_BPSData.pop_front();
            m_fAvgBPS = std::accumulate(m_BPSData.begin(), m_BPSData.end(), 0.0f) / m_BPSData.size();

            m_NextCalcTime = curTime + std::chrono::seconds(1);
        }
    }

    void AddCount(size_t nCount)
    {
        m_nTotal += nCount;
        m_nCur += nCount;
    }

private:
    std::atomic<size_t> m_nTotal = 0;
    std::atomic<size_t> m_nCur   = 0;

    float m_fAvgBPS = 0.0f;

    std::chrono::high_resolution_clock::time_point m_NextCalcTime;
    std::deque<float>                              m_BPSData;
};

#endif // PerSecondCount_h__
