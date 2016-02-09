#include "TeamInfoLayer.h"
#include "HmsGeometricFunction.h"
#include "HmsTemplateFunction.h"
//#include "UserDefaultUtil.h"
#include "Utility.h"
#include "NormalScene.h"
#include "TeamManagementBaseLayer.h"
#include "popupConfirmLayer.h"

USING_NS_CC;

//生成
bool TeamInfoLayer::init()
{
    if(!Layer::init()) {
        return false;
    }
    
    return true;
}

//セット
void TeamInfoLayer::setData()
{
//    TeamManagementBaseLayer *base = (TeamManagementBaseLayer*)this->getParent();
//    NormalScene *normal_scene = (NormalScene*)this->getParent()->getParent();
    
    //リーダー分岐
    if(SaveData::loadBool("leader_flg")) {
        //情報変更
        Sprite *info_change = Sprite::create("init/Team/management/base/bt_info_change.png");
        info_change->setPosition(convertFromDisplaySize(Point(170, 680), 0.5, 1));
        info_change->setScale(getVisibleRatio());
        this->addChild(info_change, 0, 1);
        //リーダー引き継ぎ
        Sprite *leader_change = Sprite::create("init/Team/management/base/bt_leader_change.png");
        leader_change->setPosition(convertFromDisplaySize(Point(469, 680), 0.5, 1));
        leader_change->setScale(getVisibleRatio());
        this->addChild(leader_change, 0, 2);
        //勧誘
        Sprite *invitation = Sprite::create("init/Team/management/base/bt_invitation.png");
        invitation->setPosition(convertFromDisplaySize(Point(170, 545), 0.5, 1));
        invitation->setScale(getVisibleRatio());
        this->addChild(invitation, 0, 4);
        //脱退
        Sprite *quit = Sprite::create("init/Team/management/base/bt_quit.png");
        quit->setPosition(convertFromDisplaySize(Point(469, 545), 0.5, 1));
        quit->setScale(getVisibleRatio());
        this->addChild(quit, 0, 3);
    } else {
        //情報参照
        Sprite *reference = Sprite::create("init/Team/management/base/bt_team_info.png");
        reference->setPosition(convertFromDisplaySize(Point(170, 680), 0.5, 1));
        reference->setScale(getVisibleRatio());
        this->addChild(reference, 0, 1);
        //勧誘
        Sprite *invitation = Sprite::create("init/Team/management/base/bt_invitation.png");
        invitation->setPosition(convertFromDisplaySize(Point(469, 680), 0.5, 1));
        invitation->setScale(getVisibleRatio());
        this->addChild(invitation, 0, 4);
        //脱退
        Sprite *quit = Sprite::create("init/Team/management/base/bt_quit.png");
        quit->setPosition(convertFromDisplaySize(Point(170, 545), 0.5, 1));
        quit->setScale(getVisibleRatio());
        this->addChild(quit, 0, 3);
    }
    
    //タップ許可
    this->setTouchEnabled(true);
}

void TeamInfoLayer::xtTouchesBegan(cocos2d::CCSet* _touches, cocos2d::CCEvent* event)
{
}
void TeamInfoLayer::xtTouchesMoved(cocos2d::CCSet* _touches, cocos2d::CCEvent* event)
{
}
void TeamInfoLayer::xtTouchesEnded(cocos2d::CCSet* _touches, cocos2d::CCEvent* event)
{
    CCSetIterator it;
    for(it=_touches->begin(); it!=_touches->end(); ++it) {
        Touch *_touch = (Touch *)(*it);
        Vec2 pointOne = Director::getInstance()->convertToUI(_touch->getLocationInView());
        Vec2 location = this->convertToNodeSpace(pointOne);
        //HMSLOG("tap_point=%f,%f [device]", location.x, location.y);
        Vec2 point = convertTo640x960(location);
        //HMSLOG("tap_point=%f,%f [640x960]", point.x, point.y);
        
        ////タップ判定////
        
        //情報変更・参照
        Sprite *info_change = (Sprite*)this->getChildByTag(1);
        if(info_change!=NULL) {
            Rect info_change_rect = info_change->boundingBox();
            if(info_change_rect.containsPoint(location)) {
                NormalScene *normal_scene = (NormalScene*)this->getParent()->getParent();
                //Utility::changeScene((CCScene*)normal_scene, "TeamInfoChangeScene");
                normal_scene->changePage("TeamInfoChangeScene");
                return;
            }
        }
        
        //リーダー引き継ぎ
        Sprite *leader_change = (Sprite*)this->getChildByTag(2);
        if(leader_change!=NULL) {
            Rect leader_change_rect = leader_change->boundingBox();
            if(leader_change_rect.containsPoint(location)) {
                NormalScene *normal_scene = (NormalScene*)this->getParent()->getParent();
                //Utility::changeScene((CCScene*)normal_scene, "TeamLeaderChangeScene");
                normal_scene->changePage("TeamLeaderChangeScene");
                return;
            }
        }
        
        //脱退
        Sprite *quit = (Sprite*)this->getChildByTag(3);
        if(quit!=NULL) {
            Rect quit_rect = quit->boundingBox();
            if(quit_rect.containsPoint(location)) {
//                NormalScene *normal_scene = (NormalScene*)this->getParent()->getParent();
                
                string url = HOST;
                url += "team/team_petition_member.php";
                url += "?user_master_id=";
                string user_master_id = SaveData::loadString("user_master_id");
                url += user_master_id;
                
                url += "&team_id=";
                HmsList team_data;
                team_data.import_tsv(SaveData::loadString("team_data"));
                url += team_data.getValue(0, "team_id");
                
                url += "&type=r";
                
                //HMSLOG("url=%s", url.c_str());
                
                //SEL_CallFuncND selector = callfuncND_selector(NormalScene::httpTeamResignCompleted);
                string text = "[" + team_data.getValue(0, "team_name") + "]";
                text += "\n";
                text += "本当に脱退してもよろしいですか?";
                popupConfirmLayer *confirm = popupConfirmLayer::create(text);
                //confirm->setData(text, url, selector, (CCScene*)normal_scene);
                auto ok_func = [this](){
                };
                auto ng_func = [](){}; //いいえを押したとき何もしない
                confirm->setFunction(ok_func, ng_func);

                
                this->addChild(confirm, 9999);
                
                return;
            }
        }
        
        //勧誘
        Sprite *invitation = (Sprite*)this->getChildByTag(4);
        if(invitation!=NULL) {
            Rect invitation_rect = invitation->boundingBox();
            if(invitation_rect.containsPoint(location)) {
                NormalScene *normal_scene = (NormalScene*)this->getParent()->getParent();
                //Utility::changeScene((CCScene*)normal_scene, "TeamInvitationScene");
                normal_scene->changePage("TeamInvitationScene");
                return;
            }
        }
    }
}