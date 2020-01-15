#ifndef AIPATHFINDER_H
#define AIPATHFINDER_H

#include "BaseCode.h"

class CAIActor;
class CAIPathFinder
{
public:
	CAIPathFinder(CAIActor* pActor);
	Vector2 SearchStep(const Vector2& dest, float move_spd);

private:
	CAIActor*			m_pActor;
	std::deque<Vector2> m_LastSearchList;
};
#endif /* AIPATHFINDER_H */
