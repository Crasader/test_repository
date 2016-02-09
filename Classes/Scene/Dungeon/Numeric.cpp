#include "Numeric.h"
#include "HmsGeometricFunction.h"
#include "Chara.h"
#include "NumberSprite.h"

bool Numeric::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    ignoreAnchorPointForPosition(false);
    setAnchorPoint(Vec2(0.5,0.5));
    setContentSize(Size(0,0));
    
    now_value = 0;
    counter = 0;
    
    return true;
}

void Numeric::setValue(int value, bool counting_flg)
{
    if(counting_flg == true){
        this->now_value = target_value;
        this->last_value = now_value;
        this->target_value = value;
        counter = 0;
        
        setFont();
        
        //スケジューラー登録
        this->schedule(schedule_selector(Numeric::update), 0.0333f);
    }else{
        now_value = value;
        target_value = value;
        setFont();
    }
}

void Numeric::setFont(bool high_flg)
{
    if(now_value < 0) now_value = 0;
    string str = to_string(now_value);
    sprite_vec.clear();
    removeAllChildren();
    
    for(int i=0; i<str.length(); i++) {
        string s = str.substr(i, 1);
        //string filename = "download/Chara/parts100/card_num" + s + ".png";
        
        auto *temp = NumberSprite::getSprite(stoi(s), "value", 29*getVisibleRatio());
        //CCLOG("スプライトテスト：%f",temp->getPosition().x);
        auto copy = Sprite::createWithTexture(temp);
        sprite_vec.push_back(copy);
        
        sprite_vec.back()->setAnchorPoint(Vec2(0.5,0.5));
        if(static_cast<Chara *>(getParent())->getType()==CHARA_TYPE_ATTACK || static_cast<Chara *>(getParent())->getType()==CHARA_TYPE_DEFENSE||static_cast<Chara *>(getParent())->getType()==CHARA_TYPE_BALANCE){
    
            int x = i*21;
            if(str.length() == 3) x = i*21-8;
            sprite_vec.back()->setPosition(Point(x*getVisibleRatio(), 0));
            //sprite_vec.back()->setScale(sprite_vec.back()->getScale() * 0.95);
        }/*else if(static_cast<Chara *>(getParent())->getType()==CHARA_TYPE_DEFENSE){
            
            int x = i*20*0.8f;
            sprite_vec.back()->setPosition(Point(x*getVisibleRatio(), 0));
            sprite_vec.back()->setScale(getVisibleRatio()*0.8f);
            
        }else if(static_cast<Chara *>(getParent())->getType()==CHARA_TYPE_BALANCE){
            
            int x = i*20*0.9f;
            sprite_vec.back()->setPosition(Point(x*getVisibleRatio(), 0));
            sprite_vec.back()->setScale(getVisibleRatio()*0.9f);
            
        }*/else if(static_cast<Chara *>(getParent())->getType()==CHARA_TYPE_LEADER){
            
            int x = i*20+4;
            sprite_vec.back()->setPosition(Point(x*getVisibleRatio(), 2));
            sprite_vec.back()->setScale(sprite_vec.back()->getScale()*1.07f);
        }else if(static_cast<Chara *>(getParent())->getType()==CHARA_TYPE_SUPPORT){
            int x = 0;
            if(str.length()>=2){
                x = i*13-14;
                sprite_vec.back()->setScale(sprite_vec.back()->getScale() * 0.82f);
            }else{
                x = i*18-4;
                //sprite_vec.back()->setScale(getVisibleRatio());
            }
            sprite_vec.back()->setPosition(Point(x*getVisibleRatio(), 0));
            
        }
        
        if(high_flg == true){
            this->addChild(sprite_vec.back(),1);
            sprite_vec.back()->setGlobalZOrder(0.5);//エフェクトより上へ
        }else{
            this->addChild(sprite_vec.back(),1);
            sprite_vec.back()->setGlobalZOrder(0);
        }
    }
}

void Numeric::update(float dt)
{
    float time = 0.3f;
    int some = time/0.0333;
    
    if(counter > some){
        counter = 0;
        this->unschedule(schedule_selector(Numeric::update));
        setFont();
    }else{
        if(now_value != (int)((float)(target_value - last_value)/(float)some*(float)counter + last_value)){
            now_value = (int)((float)(target_value - last_value)/(float)some*(float)counter + last_value);
            setFont(true);
        }
        
        counter++;
    }
}


/*void Numeric::setValueEffect(int old_value, int new_value){
    
    for(int i=0; i<sprite_vec.size(); i++){
        //sprite_vec.at(i)->setPosition(Point(100,100));
        HMSLOG("バリューエフェクト%f",sprite_vec.at(i)->getPosition().x);
        //float ratio = getVisibleRatio();
        ScaleTo *scale1 = ScaleTo::create(0.25f, 1.4f);
        //MoveTo *move1 = MoveTo::create(0.25f, Point(0, 40*ratio));
        ScaleTo *scale2 = ScaleTo::create(0.25f, 1.0f);
        //MoveTo *move2 = MoveTo::create(0.25f, Point(0, 0));
        Sequence *action = Sequence::create(scale1,scale2, nullptr);
        
        sprite_vec.at(i)->runAction(action);
    }
}*/
