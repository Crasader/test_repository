
#ifndef __HMS_ACTION_INSTANT__
#define __HMS_ACTION_INSTANT__

#include "cocos2d.h"

NS_CC_BEGIN

/** @class ChangeTexture
* @brief change the sprite texture.
*/
class CC_DLL ChangeTexture : public ActionInstant
{
public:
    /** Create the action.
     *
     * @param filename 変更する画像
     * @return  autorelease対象のスプライトを返却
     */
    static ChangeTexture * create(const std::string& filename);

    //
    // Overrides
    //
    /**
     * @param time In seconds.
     */
    virtual void update(float time) override;
    virtual ChangeTexture* clone() const override;
    
CC_CONSTRUCTOR_ACCESS:
    ChangeTexture() :_filename("") {}
    virtual ~ChangeTexture() {}

    /** init the action */
    bool initWithChangeTexture(const std::string& filename);

protected:
    std::string _filename;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(ChangeTexture);
};

NS_CC_END

#endif //__HMS_ACTION_INSTANT__
