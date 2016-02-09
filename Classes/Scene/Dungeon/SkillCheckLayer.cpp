#include "SkillCheckLayer.h"
#include "HmsGeometricFunction.h"
#include "DungeonScene.h"
#include "ui/CocosGUI.h"
#include "Chara.h"
#include "TurnManage.h"

bool SkillCheckLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    HMSLOG("SkillCheckLayer::init()");
	//visible_flg = false;
    return true;
}
void SkillCheckLayer::setData(Chara *used_chara, int player_index)
{
    stopAllActions();
    removeAllChildren();
    closing_flg = false;
    
	ratio = getVisibleRatio();
	
	
	//スキル使用キャラの選択表示
    if(chara_select==nullptr){
        chara_select = Sprite::create("init/Dungeon/design/battle_skill_select.png");
        
        chara_select->setScale(ratio * 2);
        used_chara->addChild(chara_select,10,3888);
    }
    
    
    vector<string> skill_type_list;
	vector<string> skill_name_list;
    vector<string> skill_detail_name_list;
	vector<int> skill_cost_list;
    Sprite *header_bg_img;
    header_bg_img_list.clear();

    if(used_chara->getActiveSkillId()>0){
        if(used_chara->getBelongingIndex()==player_index){
			header_bg_img = Sprite::create("init/Dungeon/design/battle_skill_bk_player_as.png");
		} else {
			header_bg_img = Sprite::create("init/Dungeon/design/battle_skill_bk_enemy_as.png");
		}
        header_bg_img_list.push_back(header_bg_img);
        skill_type_list.push_back("active_skill");
		skill_name_list.push_back(used_chara->getActiveSkillName());
        skill_detail_name_list.push_back(used_chara->getActiveSkillDetail());
		skill_cost_list.push_back(used_chara->getActiveSkillCost());
    }
    if(used_chara->getPassiveSkillId()>0){
		if(used_chara->getBelongingIndex()==player_index){
			header_bg_img = Sprite::create("init/Dungeon/design/battle_skill_bk_player_ps.png");
		} else {
			header_bg_img = Sprite::create("init/Dungeon/design/battle_skill_bk_enemy_ps.png");
		}
        header_bg_img_list.push_back(header_bg_img);
        skill_type_list.push_back("passive_skill");
        skill_name_list.push_back(used_chara->getPassiveSkillName());
        skill_detail_name_list.push_back(used_chara->getPassiveSkillDetail());
        skill_cost_list.push_back(used_chara->getCharaTurnCount());
	}
	
	//上画面の表示
    for(int i=0; i<header_bg_img_list.size(); i++) {
        header_bg_img = header_bg_img_list.at(i);
        string skill_name = skill_name_list.at(i);
        string skill_detail_name = skill_detail_name_list.at(i);
        int skill_cost = skill_cost_list.at(i);
        //int y = i * 160*ratio;
        
        header_bg_img->setScale(ratio);
        
        if(used_chara->getBelongingIndex() == TurnManage::getEnemyIndex() && used_chara->isInHand()){//敵の手札が観れる場合は
            header_bg_img->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT *4/5), 0.5, 0.5) + Point(0, -160*ratio*(header_bg_img_list.size()-1-i)));
        }else if(used_chara->getBelongingIndex() == TurnManage::getEnemyIndex() && (used_chara->getType()==CHARA_TYPE_LEADER || used_chara->getType()==CHARA_TYPE_SUPPORT)){//敵の総大将とフィールドの軍師
            header_bg_img->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT *1/2), 0.5, 0.5)+Point(0, - 160*ratio*(header_bg_img_list.size()-1-i)));
        }else{
            header_bg_img->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH / 2, 160), 0.5, 0)+Point(0,used_chara->getPositionY()+ i * 160*ratio));
        }
        header_bg_img->setScale(0);
        
        this->addChild(header_bg_img, 1, 1);
        
        //スキル名
        if(skill_name=="")skill_name = "-";
        Label* skill_name_label = HMSFontUtil::createWithTTFBySystemFontArgument(skill_name, FONT_NAME_SYSTEM, 28);
        skill_name_label->setAnchorPoint(Point(0, 0.5));
        skill_name_label->setPosition(Point(header_bg_img->getContentSize().width / 2 + 20, header_bg_img->getContentSize().height - 50));
        skill_name_label->setColor(Color3B::ORANGE);//オレンジ色
        header_bg_img->addChild(skill_name_label, 2, 1);
        
        //スキル説明
        if(skill_detail_name == "")skill_detail_name = "スキルなし";
        auto skill_detail_label = HMSFontUtil::createWithTTFBySystemFontArgument(skill_detail_name, FONT_NAME_SYSTEM, 24);//全角38文字が限界です。
        skill_detail_label->setDimensions(460, 60 );
        skill_detail_label->setAnchorPoint(Point(0, 1.0));
        skill_detail_label->setPosition(Point(156, header_bg_img->getContentSize().height - 70));
        header_bg_img->addChild(skill_detail_label, 2, 2);
        
        
        auto nume = NumericCount::create();
        nume->ratio = 1.0f;
        nume->png_path = "init/Dungeon/design/battle_num_w";
        nume->setValue(skill_cost , "");
        nume->setPosition(Point(78, header_bg_img->getContentSize().height / 2 - 22));
        nume->setScale(2.0f);
        header_bg_img->addChild(nume,3);
        
        
        
        //スキル詳細のアニメーション
        auto *header_bg_img_scale_up = ScaleTo::create(0.1f, 1.0f * ratio);
        header_bg_img->runAction(
                                 Sequence::create(header_bg_img_scale_up,
                                                  nullptr
                                                  )
                                 );
    }
    
	//キャラ周りの青い枠のアニメーション
    chara_select->setOpacity(255);
	auto *chara_selct_scale_up = ScaleTo::create(0.1f, 1.0f * ratio);
	chara_select->runAction(
							 Sequence::create(chara_selct_scale_up,
											  DelayTime::create(0.2f),
											  nullptr
											  )
							 );
}

void SkillCheckLayer::close()
{
    if(closing_flg == true) return; //既にクローズ中の時はなし
    closing_flg = true;
    
    if(chara_select && chara_select!=nullptr){
        
        auto func = CallFunc::create([&,this](){
            this->removeAllChildrenWithCleanup(true);
            this->header_bg_img_list.clear();
        });
        auto func2 = CallFunc::create([&,this](){
            this->chara_select->removeFromParent();
            this->chara_select = nullptr;
            this->closing_flg = false;
        });
        
        //スキル詳細のアニメーション
        for(int i=0; i<header_bg_img_list.size(); i++) {
            Sprite *header_bg_img = header_bg_img_list.at(i);
            header_bg_img->stopAllActions();
            header_bg_img->runAction(Sequence::create(
                                                      ScaleTo::create(0.1f, 0.0f),
                                                      FadeOut::create(0.12f),
                                                      func,
                                                      nullptr)
                                     );
        }
        
        //キャラ周りの青い枠のアニメーション
        chara_select->stopAllActions();
        chara_select->runAction(
                                Sequence::create(ScaleTo::create(0.1f, 1.1f * ratio),
                                                 FadeOut::create(0.1f),
                                                 //RemoveSelf::create(),
                                                 func2,
                                                 nullptr
                                                 )
                                );
    }
}