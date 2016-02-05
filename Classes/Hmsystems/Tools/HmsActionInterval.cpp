#include "HmsActionInterval.h"


NS_CC_BEGIN

//
// AnchorPointMoveBy
//


AnchorPointMoveBy* AnchorPointMoveBy::create(float duration, const Vec2 &deltaAnchorPoint)
{
    AnchorPointMoveBy *ret = new (std::nothrow) AnchorPointMoveBy();
    
    if (ret)
    {
        if (ret->initWithDuration(duration, deltaAnchorPoint))
        {
        ret->autorelease();
        }
        else
        {
            delete ret;
            ret = nullptr;
        }
    }
    
    return ret;
}

bool AnchorPointMoveBy::initWithDuration(float duration, const Vec2& deltaAnchorPoint)
{
    bool ret = false;
    
    if (ActionInterval::initWithDuration(duration))
    {
        _anchorPointDelta = deltaAnchorPoint;
        ret = true;
    }
    
    return ret;
}

AnchorPointMoveBy* AnchorPointMoveBy::clone() const
{
    // no copy constructor
    auto a = new (std::nothrow) AnchorPointMoveBy();
    a->initWithDuration(_duration, _anchorPointDelta);
    a->autorelease();
    return a;
}

void AnchorPointMoveBy::startWithTarget(Node *target)
{
    ActionInterval::startWithTarget(target);
    _previousAnchorPoint = _startAnchorPoint = target->getAnchorPoint();
}

AnchorPointMoveBy* AnchorPointMoveBy::reverse() const
{
    return AnchorPointMoveBy::create(_duration, -_anchorPointDelta);
}


void AnchorPointMoveBy::update(float t)
{
    if (_target)
    {
#if CC_ENABLE_STACKABLE_ACTIONS
        Vec2 currentPos = _target->getAnchorPoint();
        Vec2 diff = currentPos - _previousAnchorPoint;
        _startAnchorPoint = _startAnchorPoint + diff;
        Vec2 newPos =  _startAnchorPoint + (_anchorPointDelta * t);
        _target->setAnchorPoint(newPos);
        _previousAnchorPoint = newPos;
#else
        _target->setAnchorPoint(_startAnchorPoint + _anchorPointDelta * t);
#endif // CC_ENABLE_STACKABLE_ACTIONS
    }
}

//
// AnchorPointMoveTo
//

AnchorPointMoveTo* AnchorPointMoveTo::create(float duration, const Vec2& andhorPoint)
{
    AnchorPointMoveTo *ret = new (std::nothrow) AnchorPointMoveTo();
    
    if (ret)
    {
        if (ret->initWithDuration(duration, andhorPoint))
        {
            ret->autorelease();
        }
        else
        {
            delete ret;
            ret = nullptr;
        }
    }
    
    return ret;
}


bool AnchorPointMoveTo::initWithDuration(float duration, const Vec2& andhorPoint)
{
    bool ret = false;
    
    if (ActionInterval::initWithDuration(duration))
    {
        _endAnchorPoint = andhorPoint;
        ret = true;
    }
    
    return ret;
}

AnchorPointMoveTo* AnchorPointMoveTo::clone() const
{
    // no copy constructor
    auto a = new (std::nothrow) AnchorPointMoveTo();
    a->initWithDuration(_duration, _endAnchorPoint);
    a->autorelease();
    return a;
}

void AnchorPointMoveTo::startWithTarget(Node *target)
{
    AnchorPointMoveBy::startWithTarget(target);
    _anchorPointDelta = _endAnchorPoint - target->getAnchorPoint();
}

AnchorPointMoveTo* AnchorPointMoveTo::reverse() const
{
    CCASSERT(false, "reverse() not supported in AnchorPointMoveTo");
    return nullptr;
}


NS_CC_END
