#ifndef DYNAREGION_H
#define DYNAREGION_H

struct CDynaRegion
{
    void AddDynaRegion(const FloatRect& rect)
    {
        m_setLineX.emplace_back(RegionLine{rect.left, rect.right});
        m_setLineY.emplace_back(RegionLine{rect.top, rect.bottom});
    }

    void Clear()
    {
        m_setLineX.clear();
        m_setLineY.clear();
    }

    bool IsIntersect(float x, float y) const
    {
        for(size_t i = 0; i < m_setLineX.size(); i++)
        {
            const auto& line_x = m_setLineX[i];
            if(x >= line_x.start && x <= line_x.end)
            {
                const auto& line_y = m_setLineY[i];
                if(y >= line_y.start && y <= line_y.end)
                    return true;
            }
        }
        return false;
    }

    struct RegionLine
    {
        float start;
        float end;
    };
    std::vector<RegionLine> m_setLineX;
    std::vector<RegionLine> m_setLineY;
};
#endif /* DYNAREGION_H */
