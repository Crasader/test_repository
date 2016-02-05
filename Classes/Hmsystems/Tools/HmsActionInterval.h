
#ifndef __ANC_MOVE_TO__
#define __ANC_MOVE_TO__

#include "cocos2d.h"

NS_CC_BEGIN

class CC_DLL AnchorPointMoveBy : public ActionInterval
{
public:
    /** 
     * @param duration actionの時間
     * @param deltaAnchorAnchorPoint anchorpointの移動距離(相対位置)
     * @return An autoreleased AnchorPointMoveBy object.
     
     */
    static AnchorPointMoveBy* create(float duration, const Vec2& deltaAnchorPoint);

    //
    // Overrides
    //
    virtual AnchorPointMoveBy* clone() const override;
    virtual AnchorPointMoveBy* reverse(void) const  override;
    virtual void startWithTarget(Node *target) override;
    /**
     * @param time in seconds
     */
    virtual void update(float time) override;
    
CC_CONSTRUCTOR_ACCESS:
    AnchorPointMoveBy():_is3D(false) {}
    virtual ~AnchorPointMoveBy() {}

    /** initializes the action */
    bool initWithDuration(float duration, const Vec2& deltaAnchorPoint);

protected:
    bool _is3D;
    Vec2 _anchorPointDelta;
    Vec2 _startAnchorPoint;
    Vec2 _previousAnchorPoint;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(AnchorPointMoveBy);
};

class CC_DLL AnchorPointMoveTo : public AnchorPointMoveBy
{
public:
    /** 
     * Creates the action.
     * @param duration actionの時間
     * @param deltaAnchorAnchorPoint anchorpointの移動距離(絶対位置)
     * @return An autoreleased AnchorPointMoveBy object.
     */
    static AnchorPointMoveTo* create(float duration, const Vec2& andhorPoint);

    //
    // Overrides
    //
    virtual AnchorPointMoveTo* clone() const override;
    virtual AnchorPointMoveTo* reverse() const  override;
    virtual void startWithTarget(Node *target) override;
    
CC_CONSTRUCTOR_ACCESS:
    AnchorPointMoveTo() {}
    virtual ~AnchorPointMoveTo() {}

    /** 
     * initializes the action
     * @param duration in seconds
     */
    bool initWithDuration(float duration, const Vec2& andhorPoint);

protected:
    Vec2 _endAnchorPoint;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(AnchorPointMoveTo);
};


NS_CC_END

#endif //__ANC_MOVE_TO__
