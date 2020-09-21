#ifndef AIPATHFINDER_H
#define AIPATHFINDER_H

#include "BaseCode.h"

class CAIActor;

class CAIPathFinder
{
public:
    CAIPathFinder() {}
    virtual ~CAIPathFinder() {}

    virtual Vector2              SearchStep(const Vector2& dest, float move_spd) = 0;
    virtual std::vector<Vector2> SearchAll(const Vector2& dest, float move_spd)  = 0;
};

class CAIPathFinder_Normal : public CAIPathFinder
{
public:
    CAIPathFinder_Normal(CAIActor* pActor);
    Vector2              SearchStep(const Vector2& dest, float move_spd) override;
    std::vector<Vector2> SearchAll(const Vector2& dest, float move_spd) override { return m_SearchResult; }

private:
    CAIActor*            m_pActor;
    std::deque<Vector2>  m_LastSearchList;
    std::vector<Vector2> m_SearchResult;
};

class CAIPathFinder_AStar : public CAIPathFinder
{
public:
    CAIPathFinder_AStar(CAIActor* pActor);
    Vector2              SearchStep(const Vector2& dest, float move_spd) override { return dest; }
    std::vector<Vector2> SearchAll(const Vector2& dest, float move_spd) override { return m_SearchResult; }

private:
    CAIActor*            m_pActor;
    std::deque<Vector2>  m_LastSearchList;
    std::vector<Vector2> m_SearchResult;
};

#endif /* AIPATHFINDER_H */
