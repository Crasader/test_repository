#include "Chara.h"
#include "HmsGeometricFunction.h"
#include "HmsUtilityFunction.h"
#include "HmsTemplateFunction.h"
#include "Numeric.h"
#include "DungeonScene.h"
#include "json11.hpp"
#include "TurnManage.h"
#include "CharaList.h"
#include "OutlineSprite.h"

#define IMG_Z 1002

#define DAMAGE_IMG_Z 4002

#define IMG_TAG 1002
#define ADD_IMG_TAG 4002 //選択時表示画像など、一時的に追加で出す画像
#define SKILL_ABLE_TAG 4004

#define IMG_RATIO 0.333f

bool Chara::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    ratio = getVisibleRatio();
    field_index = -1;
    hand_index = -1;
    
    dead_flg = false;
    setPassiveSkillId(0);
    
    cost_sprite.clear();
    
    numeric = nullptr;
    
    setCascadeOpacityEnabled(true);
    setCascadeColorEnabled(true);
    
    setAnchorPoint(Point(0.5f,0.5f));
    
    
    for(int i=0; i<3; i++) {
        iconSprite[i] = Sprite::create("init/Dungeon/design/blank.png");
    }
    
    return true;
}

void Chara::revival(){//死亡からのリセット
    DungeonScene *dung = static_cast<DungeonScene *>(getParent()->getParent());
    dead_flg = false;
    
    for(int ii=0; ii<2; ii++){
        auto itr = dung->player[ii]->whole_status_passive.begin();
        while (itr != dung->player[ii]->whole_status_passive.end()) {
            if((*itr)["primary"] == getPrimaryId()){
                dung->player[ii]->whole_status_passive.erase(itr);//全体パッシブの削除
            }else{
                itr++;
            }
        }
        /*auto itr2 = dung->uiLayer->passiveBufferIcon[ii].begin();
        while (itr2 != dung->uiLayer->passiveBufferIcon[ii].end()) {
            if((*itr2).first == getPrimaryId()){
                dung->uiLayer->removePassiveIcon(getPrimaryId());//全体パッシブアイコンの削除
            }else{
                itr2++;
            }
        }*/
        
        
        /*for(int i=0; i<dung->player[ii]->whole_status_passive.size(); i++){
            if(dung->player[ii]->whole_status_passive.at(i)["primary"] == getPrimaryId()){
                dung->player[ii]->whole_status_passive.erase(dung->player[ii]->whole_status_passive.begin()+i);//全体パッシブの削除
            }
        }
        for(int i=0; i<dung->uiLayer->passiveBufferIcon[ii].size(); i++){
            if(dung->uiLayer->passiveBufferIcon[ii].at(i).first == getPrimaryId()){
                dung->uiLayer->removePassiveIcon(ii, getPrimaryId());//全体パッシブアイコンの削除
            }
        }*/
    }
    dung->uiLayer->removePassiveIcon(getPrimaryId());//全体パッシブアイコンの削除
    
    
    runAction(Sequence::create(DelayTime::create(0.3f), CallFunc::create([this,dung](){//アイコンの位置全体修正
        dung->uiLayer->resetIconPosition();
    }), NULL));
    
    
    field_index = -1;
    hand_index = -1;
    combination_count = 0;
    active_skill_able_count = SaveData::loadInt("active_skill_limit");//アクティブスキル使用可能回数リセット
    
    used_skill_flg=false;
    chara_status_active.clear();
    chara_status_passive.clear();
    
    if(eliminate_flg == false){//デッキに戻す 消去フラグが付いているカード以外
        dung->deck[getBelongingIndex()].push_back(this);
    }
    if(eliminate_flg == true){//消去フラグが付いているこのカードがデッキにあったら消す
        auto itr = find_if(dung->deck[getBelongingIndex()].begin(), dung->deck[getBelongingIndex()].end(),
                           [this](Chara *chara){return chara->getPrimaryId() == this->getPrimaryId();});
        if(itr != dung->deck[getBelongingIndex()].end()) dung->deck[getBelongingIndex()].erase(itr);//あったらけす
    }
    
    static_cast<DungeonScene *>(getParent()->getParent())->checkIllegal("Chara->erase()", false);
    
}

void Chara::initialize(){
    DungeonScene *dung = static_cast<DungeonScene *>(getParent()->getParent());
    if(sprite_map["cant_attack"]!=nullptr){
        static_cast<CardLayer *>(getParent())->removeChild(sprite_map["cant_attack"]);
    }
    if(sprite_map["cant_skill"]!=nullptr){
        static_cast<CardLayer *>(getParent())->removeChild(sprite_map["cant_skill"]);
    }
    
    auto childrens = getChildren();
    for(auto child : childrens){
        if(child->getTag() != 3888){//skillCheckLayerのchara_selectだけは外す
            child->removeFromParent();
        }
    }
    
    if(getDeadFlg() == true){//死亡からのリセットの時
        revival();
    }
    
    ignoreAnchorPointForPosition(false);
    setContentSize(Size(100*ratio,100*ratio));
    setAnchorPoint(Vec2(0.0,0.0));
    setPosition(static_cast<DungeonScene *>(getParent()->getParent())->fieldLayer->deck_point[getBelongingIndex()]);
    setOpacity(255);
    setColor(Color3B(255,255,255));
    
    setCharaTurnCount(getCharaTurnCountDef());
    
    if(getType()==CHARA_TYPE_SUPPORT){
        value = getCharaTurnCountDef();
    }else if(isAttackAndDefense()){
        value = base_value;
        combo_value=base_value;
    }
    
    
    //空白を入れておく
    sprite_map["chara"] = nullptr;
    sprite_map["type"] = nullptr;
    sprite_map["combination_r"] = nullptr;
    sprite_map["combination_l"] = nullptr;
    sprite_map["reverse"] = nullptr;
    sprite_map["nokori"] = nullptr;
    sprite_map["cant_attack"] = nullptr;
    sprite_map["cant_skill"] = nullptr;
	sprite_map["rare_shine"] = nullptr;
    sprite_map["turn"] = nullptr;
    sprite_map["name"] = nullptr;
    cost_sprite.clear();
    particle_rare = nullptr;
  
	hideSkillAble();
    
    string filename = "";
    
//背景とキャラ画像とレア度をまとめて一枚に
    auto render = RenderTexture::create(getContentSize().width,getContentSize().height);
    render->setAnchorPoint(Vec2(0.5f,0.5f));
    render->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2), 0.5, 0.5));
    render->begin();//書き込み開始
    
    //キャラ背景
    auto back_sprite = Sprite::create("download/Chara/parts100/card_bk" + to_string(getRare()) + ".png");
    back_sprite->setScale(ratio);
    back_sprite->setPosition(getContentSize()/2);
    back_sprite->visit();
    
    //キャラ本体
    auto chara_sprite = Sprite::create("download/Chara/chara100/" + to_string(getCharaId()) + ".png");
    chara_sprite->setScale(ratio);
    chara_sprite->setPosition(getContentSize()/2);
    chara_sprite->visit();
    
    //レア度
    auto rare_sprite = Sprite::create("download/Chara/parts100/card_rare" + to_string(getRare()) + ".png");
    rare_sprite->setScale(ratio);
    rare_sprite->setPosition(getContentSize()/2);
    rare_sprite->visit();

    render->end();//書き込み終了
    sprite_map["chara"] = Sprite::createWithTexture(render->getSprite()->getTexture());//テクスチャーから取り出してスプライトにしないとなぜかオパシティが効かなくなる
    sprite_map["chara"]->setScale(1.0f, -1.0f);//なんでこれ入れないと逆さになるんだよ・・・意味不明   →RenderTextureは上下反転するらしい
    
    addChild(sprite_map["chara"]);
    
    
/*    auto glprog = new GLProgram();
    glprog->initWithFilenames("init/Shader/gray.vsh", "init/Shader/gray.fsh");
    glprog->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_POSITION, GLProgram::VERTEX_ATTRIB_POSITION);
    glprog->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_TEX_COORD, GLProgram::VERTEX_ATTRIB_TEX_COORD);
    glprog->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_COLOR, GLProgram::VERTEX_ATTRIB_COLOR);
    glprog->link();
    glprog->updateUniforms();
    
    auto call1 = CallFunc::create([&glprog](){
        glprog->updateUniforms();
        float temp = clock();
        glprog->setUniformLocationWith1f(glprog->getUniformLocationForName("u_rate"), temp-floor(temp));
        
    });
    runAction(RepeatForever::create(Sequence::create(DelayTime::create(0.2f),call1, NULL)));
 
    
    sprite_map["back"]->setGLProgram(glprog);
    
*/
    //this->addChild(sprite_map["back"], 0);
    
    
    //リバース背景
    if(getBelongingIndex()==TurnManage::getPlayerIndex()){
        sprite_map["reverse"] = Sprite::create("download/Chara/parts100/card_bk1_reverse.png");
    }else{
        sprite_map["reverse"] = Sprite::create("download/Chara/parts100/card_bk2_reverse.png");
    }
    sprite_map["reverse"]->setScale(ratio);
    sprite_map["reverse"]->setPosition(Point(0,0));
    this->addChild(sprite_map["reverse"],0);
    
    
    //パーティクル出力
    if(particle_rare==nullptr && SaveData::loadBool("effect")) {
        if(getRare()==5) {
            string str_rare = toString(getRare());
            particle_rare = ParticleSystemQuad::create("download/Effect/Particle/plus_effect_rare5.plist");
            particle_rare->setPosition(Point(-38*ratio, 35*ratio));
            particle_rare->setDuration(-1);
            particle_rare->setScale(0.4f*ratio);
            particle_rare->setPositionType(ParticleSystem::PositionType::GROUPED);
            this->addChild(particle_rare, 2);
        }
        if(getRare()>=6) {
            string str_rare = toString(getRare());
            particle_rare = ParticleSystemQuad::create("download/Effect/Particle/plus_effect_rare6.plist");
            particle_rare->setPosition(Point(-38*ratio, 35*ratio));
            particle_rare->setDuration(-1);
            particle_rare->setScale(0.4f*ratio);
            particle_rare->setPositionType(ParticleSystem::PositionType::GROUPED);
            this->addChild(particle_rare, 2);
        }
    }
    if(particle_rare!=nullptr) particle_rare->setVisible(true);
    
    //キャラタイプ
    setTypeSprite();

   
    //スキルコスト・配置コスト背景
    sprite_map["cost_back"] = Sprite::create("download/Chara/parts100/card_cost_bk.png");
    sprite_map["cost_back"]->setScale(ratio);
    sprite_map["cost_back"]->setPosition(Point(0,0));
    this->addChild(sprite_map["cost_back"],2);
    sprite_map["cost_back"]->setVisible(false);
    
  
    //戦闘力表示
    numeric = Numeric::create();
    this->addChild(numeric,5);
    if(getType()==CHARA_TYPE_ATTACK||getType()==CHARA_TYPE_DEFENSE||getType()==CHARA_TYPE_BALANCE){//軍師以外表示
        numeric->setPosition(Vec2(0,-33*getVisibleRatio()));
        numeric->setValue(value, false);
        
    }else if(getType()==CHARA_TYPE_SUPPORT){//軍師は生存ターン表示
        numeric->setPosition(Vec2(1,-33*getVisibleRatio()));
        numeric->setValue(value, false);
        
    }else if(getType() == CHARA_TYPE_LEADER){//総大将は士気
        numeric->setPosition(Vec2(0,-35*getVisibleRatio()));
        numeric->setValue(static_cast<DungeonScene *>(getParent()->getParent())->player[getBelongingIndex()]->getPower(), false);
    }
    value_point = numeric->getPosition();
    
    //コンボマーク表示
    comboSpriteUpdate();
    
    //SR以上の場合、クリッピングによる光を演出
    if(getRare() >= 5){
    
        
        
    }else if(getRare() > 2) {
        sprite_map["rare_shine"] = Sprite::create("download/Chara/parts400/light_bar.png");
        float kira_start_posx = -getContentSize().width / 2 - sprite_map["rare_shine"]->getContentSize().width / 2;
        float kira_start_posy = getContentSize().height / 2 + 20;
        float kira_end_posx = 0.0f;
        float kira_end_posy = 0.0f;
        sprite_map["rare_shine"]->setAnchorPoint(Vec2(0.5f, 0.5f));
        sprite_map["rare_shine"]->setPosition(Point(kira_start_posx, kira_start_posy));
        sprite_map["rare_shine"]->setRotation(45.0f);
        sprite_map["rare_shine"]->setOpacity(220.0f);
        sprite_map["rare_shine"]->setScale(ratio/6);

        Sprite* chara_rare_img = Sprite::create("download/Chara/parts400/card_rare" + to_string(getRare()) + "_mask.png");
        chara_rare_img->setAnchorPoint(Vec2(0.5, 0.5));
        chara_rare_img->setScale(ratio/4);
        
        auto clipper = ClippingNode::create();
        clipper->setStencil(chara_rare_img);
        clipper->setAlphaThreshold(0.0f);
        clipper->addChild(sprite_map["rare_shine"]);
        //clipper->setPosition(sprite_map["rare"]->getPosition());
        addChild(clipper,10000);
        
        //アクション
        Sequence* seq = Sequence::create(DelayTime::create(1.2f), MoveTo::create(2.0f, Point(kira_end_posx,kira_end_posy)), Place::create(Point(kira_start_posx, kira_start_posy)),  nullptr);
        RepeatForever* repeat = RepeatForever::create(seq);
        sprite_map["rare_shine"]->runAction(repeat);
        
    }
    
    if(getType() == CHARA_TYPE_LEADER){//総大将の位置
        CCLOG("ポイント：%d,%f",getBelongingIndex(),dung->ratio);
        setPosition(dung->fieldLayer->castle_point[getBelongingIndex()]);
    }
    
    
    
    showCardParam();
}

void Chara::showCardParam(){//どのパラメータを表示するか
    DungeonScene *dung = static_cast<DungeonScene *>(getParent()->getParent());
    
    if((getHandIndex()>=0 && TurnManage::getPlayerIndex()==getBelongingIndex())){//自分の手札の時 フレンドカード味方も相手も
        if(reverse_flg == false){
            showSpriteHand();
        }else{
            showSpriteDeck();
        }
        
    }else if(isInHand() && TurnManage::getEnemyIndex()==getBelongingIndex()){//敵の手札の時
        if((dung->replay_flg==true && dung->replay_other_flg==true) || CharaList::getLeader(reverseIndex(getBelongingIndex()))->statusCheck(STATUS_TYPE_HAND_OPEN)){
            showSpriteHand();//リプレイ中か手札を見るスキルを敵が使っている時
        }else{
            showSpriteDeck();
        }
    
    }else if(getFieldIndex()>=0){//フィールドの時
        if(TurnManage::getPlayerIndex()!=getBelongingIndex() && dung->wholeStatusCheck(STATUS_TYPE_DARKNESS, TurnManage::getEnemyIndex()) && getFieldIndex() > 1){//敵　または　ブラインド効果状態
            
            showSpriteDeck();
            
        }else{
            showSpriteField();
        }
    }else if(getType() == CHARA_TYPE_LEADER){
        showSpriteField();
        
    }else if(isFriend() && TurnManage::getEnemyIndex()==getBelongingIndex()){//敵のフレンドカード
        showSpriteHand();
    }else{//デッキ内
        showSpriteDeck();
    }
}

void Chara::cardReverse(int player_index, bool flg){

    if(flg != reverse_flg){
        
        auto scale = ScaleTo::create(0.28f, 0, 1);
        auto ease = EaseSineIn::create(scale);
        
        CallFunc *func = CallFunc::create([&,this,player_index,flg](){
            
            reverse_flg = flg;
            this->showCardParam();
        });
        
        auto scale2 = ScaleTo::create(0.28f, 1, 1);
        auto ease2 = EaseSineOut::create(scale2);
        
        runAction(Sequence::create(ease,func,ease2, NULL));

    }
}


void Chara::showSpriteDeck(){
    card_state = "close";
    
    sprite_map["chara"]->setVisible(false);
    sprite_map["type"]->setVisible(false);
    sprite_map["cost_back"]->setVisible(false);
    if(sprite_map["combination_r"] != nullptr) sprite_map["combination_r"]->setVisible(false);
    if(sprite_map["combination_l"] != nullptr) sprite_map["combination_l"]->setVisible(false);
    if(sprite_map["nokori"]!= nullptr)sprite_map["nokori"]->setVisible(false);
    if(sprite_map["rare_shine"]!=nullptr) sprite_map["rare_shine"]->setVisible(false);
    if(sprite_map["turn"]!=nullptr) sprite_map["turn"]->setVisible(false);
    if(sprite_map["cant_attack"]!=nullptr) sprite_map["cant_attack"]->setVisible(false);
    if(sprite_map["cant_skill"]!=nullptr) sprite_map["cant_skill"]->setVisible(false);
    
    sprite_map["reverse"]->setVisible(true);
    if(numeric != nullptr) numeric->setVisible(false);
    showCost("none");
    
    if(getActiveSkillId()>0){
        for(Sprite *tempSprite : cost_sprite) tempSprite->setVisible(false);
    }
    
    if(particle_rare!=nullptr) particle_rare->setVisible(false);
}

void Chara::showSpriteHand(){
    card_state = "open";
    
    sprite_map["reverse"]->setVisible(false);
    sprite_map["chara"]->setVisible(true);
    sprite_map["type"]->setVisible(true);
    sprite_map["cost_back"]->setVisible(true);
    if(sprite_map["combination_r"]!=nullptr) sprite_map["combination_r"]->setVisible(true);
    if(sprite_map["combination_l"]!=nullptr) sprite_map["combination_l"]->setVisible(true);
    if(sprite_map["nokori"]!=nullptr) sprite_map["nokori"]->setVisible(false);
    if(sprite_map["rare_shine"]!=nullptr) sprite_map["rare_shine"]->setVisible(true);
    if(sprite_map["turn"]!=nullptr) sprite_map["turn"]->setVisible(true);
    
    if(numeric != nullptr) numeric->setVisible(true);
    showCost("place_cost");
    
    if(particle_rare!=nullptr) particle_rare->setVisible(true);
}
void Chara::showSpriteField(){
    DungeonScene *dung = static_cast<DungeonScene *>(getParent()->getParent());
    card_state = "open";
    
    sprite_map["reverse"]->setVisible(false);
    sprite_map["chara"]->setVisible(true);
    if(sprite_map["rare_shine"]!=nullptr) sprite_map["rare_shine"]->setVisible(true);
    
    if(getType() == CHARA_TYPE_LEADER && erase_leader_flg == true){//死亡時リーダー
        sprite_map["type"]->setVisible(false);
        if(sprite_map["rare_shine"]!=nullptr) sprite_map["rare_shine"]->setVisible(false);
    }else if(getType() == CHARA_TYPE_SUPPORT){
        sprite_map["type"]->setVisible(false);
    }else{
        sprite_map["type"]->setVisible(true);
    }
    
    if(getActiveSkillId() > 0){
        sprite_map["cost_back"]->setVisible(true);
        showCost("skill_cost");
    }else{
        sprite_map["cost_back"]->setVisible(false);
        showCost("none");
    }
    if(sprite_map["combination_r"]!=nullptr) sprite_map["combination_r"]->setVisible(true);
    if(sprite_map["combination_l"]!=nullptr) sprite_map["combination_l"]->setVisible(true);
    if(sprite_map["nokori"]!=nullptr) sprite_map["nokori"]->setVisible(true);
    if(sprite_map["turn"]!=nullptr) sprite_map["turn"]->setVisible(true);
    if(numeric != nullptr) numeric->setVisible(true);
    
    
    if(statusCheck(STATUS_TYPE_FREEZE)){//氷漬があるとき
        if(sprite_map["cant_attack"]==nullptr){//画像がない時
            sprite_map["cant_attack"] = Sprite::create("download/Dungeon/ice.png");
            static_cast<CardLayer *>(getParent())->addChild(sprite_map["cant_attack"],5);
            sprite_map["cant_attack"]->setScale(0.0f);
            sprite_map["cant_attack"]->runAction(EaseBackOut::create(ScaleTo::create(0.4f, ratio)));
            
        }else if(sprite_map["cant_attack"]->getTag() == STATUS_TYPE_CANT_ACTION){//行動不能の画像がある時
            static_cast<CardLayer *>(getParent())->removeChild(sprite_map["cant_attack"]);
            sprite_map["cant_attack"] = Sprite::create("download/Dungeon/ice.png");
            static_cast<CardLayer *>(getParent())->addChild(sprite_map["cant_attack"],5);
            sprite_map["cant_attack"]->setScale(0.0f);
            sprite_map["cant_attack"]->runAction(EaseBackOut::create(ScaleTo::create(0.4f, ratio)));
        
        }else{
            sprite_map["cant_attack"]->setScale(ratio);
        }
        
        sprite_map["cant_attack"]->setTag(STATUS_TYPE_FREEZE);//タグの設定
        sprite_map["cant_attack"]->setPosition(Point(static_cast<DungeonScene *>(getParent()->getParent())->fieldLayer->field_position_list[getBelongingIndex()].at(field_index).x, static_cast<DungeonScene *>(getParent()->getParent())->fieldLayer->field_position_list[getBelongingIndex()].at(field_index).y+5*ratio));
        sprite_map["cant_attack"]->setVisible(true);
        
    }else if(statusCheck(STATUS_TYPE_CANT_ACTION)){//行動不能があるとき
        if(sprite_map["cant_attack"]==nullptr){
            sprite_map["cant_attack"] = Sprite::create("download/Chara/icon/icon15.png");
            static_cast<CardLayer *>(getParent())->addChild(sprite_map["cant_attack"],5);
            sprite_map["cant_attack"]->setScale(0.0f);
            sprite_map["cant_attack"]->runAction(EaseBackOut::create(ScaleTo::create(0.4f, ratio)));
            
        }else if(sprite_map["cant_attack"]->getTag() == STATUS_TYPE_FREEZE){//こおりづけの画像がある時
            static_cast<CardLayer *>(getParent())->removeChild(sprite_map["cant_attack"]);
            sprite_map["cant_attack"] = Sprite::create("download/Chara/icon/icon15.png");
            static_cast<CardLayer *>(getParent())->addChild(sprite_map["cant_attack"],5);
            sprite_map["cant_attack"]->setScale(0.0f);
            sprite_map["cant_attack"]->runAction(EaseBackOut::create(ScaleTo::create(0.4f, ratio)));
            
        }else{
            sprite_map["cant_attack"]->setScale(ratio);
        }
        
        sprite_map["cant_attack"]->setTag(STATUS_TYPE_CANT_ACTION);//タグの設定
        sprite_map["cant_attack"]->setPosition(Point(static_cast<DungeonScene *>(getParent()->getParent())->fieldLayer->field_position_list[getBelongingIndex()].at(field_index)));
        sprite_map["cant_attack"]->setVisible(true);
            
    }else{//何もない時
        if(sprite_map["cant_attack"]!=nullptr){
            sprite_map["cant_attack"]->runAction(Sequence::create(Spawn::create(EaseBackIn::create(ScaleTo::create(0.5f, ratio/2)),FadeTo::create(0.3f, 0), NULL),RemoveSelf::create(), NULL));
            //static_cast<CardLayer *>(getParent())->removeChild(sprite_map["cant_attack"]);
        }
        sprite_map["cant_attack"] = nullptr;
    }
    
    
    if(statusCheck(STATUS_TYPE_CANT_SKILL)){//スキル封印がある時
        if(sprite_map["cant_skill"]==nullptr){
            sprite_map["cant_skill"] = Sprite::create("download/Chara/icon/icon14.png");
            static_cast<CardLayer *>(getParent())->addChild(sprite_map["cant_skill"],500);
            sprite_map["cant_skill"]->setScale(0.0f);
            sprite_map["cant_skill"]->setRotation(-1.0f);
            sprite_map["cant_skill"]->runAction(Spawn::create(RotateTo::create(0.4f, 0.0f),EaseBackOut::create(ScaleTo::create(0.4f, ratio*0.76f)), NULL));
            
        }else{
            sprite_map["cant_skill"]->setScale(ratio*0.76f);
        }
        
        if(getType() == CHARA_TYPE_LEADER){
            sprite_map["cant_skill"]->setPosition(dung->fieldLayer->castle_point[getBelongingIndex()].x + 32*ratio, dung->fieldLayer->castle_point[getBelongingIndex()].y + 35*ratio);
        }else{
            sprite_map["cant_skill"]->setPosition(dung->fieldLayer->field_position_list[getBelongingIndex()].at(getFieldIndex()).x + 32*ratio, dung->fieldLayer->field_position_list[getBelongingIndex()].at(getFieldIndex()).y + 35*ratio);
        }
        
        sprite_map["cant_skill"]->setVisible(true);
        
    }else{
        if(sprite_map["cant_skill"]!=nullptr){
            static_cast<CardLayer *>(getParent())->removeChild(sprite_map["cant_skill"]);
        }
        sprite_map["cant_skill"] = nullptr;
    }
    
    if(particle_rare!=nullptr) particle_rare->setVisible(true);
}

void Chara::showTurnCount(int new_count, float delay){
    if(getType()==CHARA_TYPE_SUPPORT){
        if(numeric->target_value != new_count){
            
            this->runAction(Sequence::create(DelayTime::create(delay),CallFunc::create([&,new_count](){
                
                this->removeChild(numeric,true);
                numeric = Numeric::create();
                this->addChild(numeric,5);
                numeric->setValue(new_count);
                
                float ratio = getVisibleRatio();
                ScaleTo *scale1 = ScaleTo::create(0.25f, 1.1f);
                MoveTo *move1 = MoveTo::create(0.25f, Point(0, 20*ratio));
                ScaleTo *scale2 = ScaleTo::create(0.25f, 1.0f);
                MoveTo *move2 = MoveTo::create(0.25f, Point(0, 0));
                Sequence *action = Sequence::create(Spawn::createWithTwoActions(scale1, move1), Spawn::createWithTwoActions(scale2, move2), nullptr);
                
                numeric->runAction(action);
                
            }), nullptr));
        }
    }
}

void Chara::showCost(string kind){
    
    int cost = 0;
    if(kind == "place_cost"){
        cost = getPlaceCost();
    }else if(kind == "skill_cost"){
        cost = getActiveSkillCost();
    }else if(kind == "none"){
        
    }else{
        HMSLOG("showCostエラー");
    }
    
    for(auto sprite:cost_sprite) removeChild(sprite,true);
    cost_sprite.clear();
    
    if(kind != "none"){
        
        int fig2 = floor(cost/10.0f);
        int fig1 = cost % 10;
        
        /*
        string num_pass;
        if(getPlaceCost() > place_cost && kind == "place_cost"){
            num_pass = "download/Chara/parts100/card_num_r";
        }else if(getPlaceCost() < place_cost && kind == "place_cost"){
            num_pass = "download/Chara/parts100/card_num_b";
        }else{
            num_pass = "download/Chara/parts100/card_num";
        }
        */
        
        if(fig2==0){//1桁
            cost_sprite.push_back(OutlineSprite::createWithLabel(Label::createWithTTF(to_string(fig1),FONT_NAME,26*ratio),1.5f*getVisibleRatio()));
            cost_sprite.front()->setPosition(Point(36*ratio, 36*ratio));
            this->addChild(cost_sprite.front(),400);
            
        }else{//2桁
            cost_sprite.push_back(OutlineSprite::createWithLabel(Label::createWithTTF(to_string(fig1),FONT_NAME,26*ratio),1.5f*getVisibleRatio()));
            cost_sprite.push_back(OutlineSprite::createWithLabel(Label::createWithTTF(to_string(fig2),FONT_NAME,26*ratio),1.5f*getVisibleRatio()));
        
            for(int i=0; i<cost_sprite.size(); i++){
                cost_sprite.at(i)->setPosition(Point((38-17*i)*ratio, 36*ratio));
                this->addChild(cost_sprite.at(i),400);
            }
        }
        for(auto sprite : cost_sprite){
            sprite->setCascadeOpacityEnabled(true);
            
            if(getPlaceCost() > place_cost && kind == "place_cost"){
                ((OutlineSprite *)sprite)->outline->setColor(Color3B::RED);
            }else if(getPlaceCost() < place_cost && kind == "place_cost"){
                ((OutlineSprite *)sprite)->outline->setColor(Color3B(0, 152, 255));
            }else{
                ((OutlineSprite *)sprite)->outline->setColor(Color3B::BLACK);
            }
            
        }
    }
}

int Chara::getPrimaryId()//プライマリはキャラリストに入っている順番
{
    for(int i=0; i<CharaList::chara_list.size(); i++){
        if(CharaList::chara_list.at(i) == this){
            return i;
        }
    }
    CCLOG("このキャラはキャラリストに存在しません");
    return NULL;
}

bool Chara::isOnField()
{
    if(getFieldIndex()>=0) return true;
    else return false;
}
bool Chara::isInDeck()
{
    DungeonScene *dung = static_cast<DungeonScene *>(getParent()->getParent());
    for(int ii=0; ii<2; ii++){
        for(Chara *chara : dung->deck[ii]){
            if(this == chara){
                return true;
            }
        }
    }
    return false;
}
bool Chara::isInHand()
{
    if(getHandIndex()==-1 || getHandIndex()==6){
        return false;
    }else{
        return true;
    }
}
bool Chara::isFriend(){
    if(getHandIndex()==6){
        return true;
    }else{
        return false;
    }
}

void Chara::setFieldIndex(int field_index)
{
    this->field_index = field_index;
    if(field_index>=0){
        hand_index = -1;//手札indexを初期化
    }
}
int Chara::getFieldIndex()
{
    return field_index;
}
void Chara::setHandIndex(int hand_index)
{
    this->hand_index = hand_index;
    if(hand_index>=0){
        field_index = -1;//フィールドindexを初期化
    }
}
int Chara::getHandIndex()
{
    return hand_index;
}

void Chara::setValue(int value)
{
    int value_limit = value;
    if(value_limit > MAX_CHARA_VALUE) value_limit = MAX_CHARA_VALUE;//最大値最小値に変更
    if(value_limit < MIN_CHARA_VALUE) value_limit = MIN_CHARA_VALUE;
    
    this->value = value_limit;
}
int Chara::getValue()
{
    return value;
}
void Chara::setBaseValue(int value)
{
    int value_limit = value;
    if(value_limit > MAX_CHARA_VALUE) value_limit = MAX_CHARA_VALUE;//最大値最小値に変更
    if(value_limit < MIN_CHARA_VALUE) value_limit = MIN_CHARA_VALUE;
    
    this->base_value = value_limit;
}
int Chara::getBaseValue()
{
    return base_value;
}

void Chara::resetCombinationCount()
{
    combination_count = 0;
}
int Chara::getCombinationCount()
{
    return combination_count;
}

void Chara::setPlaceCost(int place_cost)
{
    this->place_cost = place_cost;
}
int Chara::getPlaceCost()
{
    int temp_cost = place_cost;
    DungeonScene *dung = static_cast<DungeonScene *>(getParent()->getParent());
    for(int i=0; i<dung->player[getBelongingIndex()]->whole_status_active.size(); i++){
        if(dung->player[getBelongingIndex()]->whole_status_active.at(i)["state"] == STATUS_TYPE_COST_DOWN_ALL){
            temp_cost -= dung->player[getBelongingIndex()]->whole_status_active.at(i)["value"];
        }
        if(dung->player[getBelongingIndex()]->whole_status_active.at(i)["state"] == STATUS_TYPE_COST_DOWN_ATTACK && isAttacker()){
            temp_cost -= dung->player[getBelongingIndex()]->whole_status_active.at(i)["value"];
        }
        if(dung->player[getBelongingIndex()]->whole_status_active.at(i)["state"] == STATUS_TYPE_COST_DOWN_DEFENSE && isDefender()){
            temp_cost -= dung->player[getBelongingIndex()]->whole_status_active.at(i)["value"];
        }
        if(dung->player[getBelongingIndex()]->whole_status_active.at(i)["state"] == STATUS_TYPE_COST_DOWN_SUPPORT && getType()==CHARA_TYPE_SUPPORT){
            temp_cost -= dung->player[getBelongingIndex()]->whole_status_active.at(i)["value"];
        }
    }
    
    for(Chara *chara : CharaList::getOneSide(getBelongingIndex())){
        for(int j=0; j<chara->chara_status_active.size(); j++){
            if(chara->chara_status_active.at(j)["state"] == STATUS_TYPE_COST_DOWN_ALL){
                temp_cost -= chara->chara_status_active.at(j)["value"];
            }
            if(chara->chara_status_active.at(j)["state"] == STATUS_TYPE_COST_DOWN_ATTACK && isAttacker()){
                temp_cost -= chara->chara_status_active.at(j)["value"];
            }
            if(chara->chara_status_active.at(j)["state"] == STATUS_TYPE_COST_DOWN_DEFENSE && isDefender()){
                temp_cost -= chara->chara_status_active.at(j)["value"];
            }
            if(chara->chara_status_active.at(j)["state"] == STATUS_TYPE_COST_DOWN_SUPPORT && getType()==CHARA_TYPE_SUPPORT){
                temp_cost -= chara->chara_status_active.at(j)["value"];
            }
        }
    }
    
    if(temp_cost < 0) temp_cost=0;//コストはマイナスにならない
    
    return temp_cost;
}

void Chara::setDeadFlg(bool dead_flg)
{
    this->dead_flg = dead_flg;
}
bool Chara::getDeadFlg()
{
    return dead_flg;
}
bool Chara::isAlive()
{
    return !dead_flg;
}
bool Chara::isMyChara()
{
    if(getBelongingIndex()==TurnManage::getPlayerIndex()){
        return true;
    }else{
        return false;
    }
}
bool Chara::isEnemyChara(){
    return !isMyChara();
}

Sprite *Chara::getImage()
{
    return sprite_map["chara"];
}

void Chara::showValue()
{
    if(getFieldIndex() >= 2 && statusCheck(STATUS_TYPE_DARKNESS) && getBelongingIndex()==TurnManage::getEnemyIndex()){//敵が暗闇状態の時
        return;
    }
   
    if(numeric->target_value != getValue()) numeric->setValue(getValue(), false);
    numeric->setPosition(Point(value_point.x, value_point.y));
}

bool Chara::showValue(int new_value ,float delay)
{
    //HMSLOG("オールド:%d  ニュー:%d プライマリ:%d",old_value,new_value,getPrimaryId());
    if(getFieldIndex() >= 2 && statusCheck(STATUS_TYPE_DARKNESS) && getBelongingIndex()==TurnManage::getEnemyIndex()){
        return false;
    }
    
    if((numeric->target_value !=new_value && dead_flg==false && numeric->isVisible()==true)){
        
        this->runAction(Sequence::create(DelayTime::create(delay),CallFunc::create([&,new_value](){
            
            int target = numeric->target_value;
        
            numeric->setValue(new_value);
            
            ScaleTo *scale0;
            if(new_value > target){
                scale0 = ScaleTo::create(0.1f, 0.9f);
            }else{
                scale0 = ScaleTo::create(0.1f, 0.9f);
            }
            auto sca_ease0 = EaseSineInOut::create(scale0);
            MoveTo *move0;
            if(new_value > target){
                move0 = MoveTo::create(0.1f, Point(value_point.x, value_point.y-5*ratio));
            }else{
                move0 = MoveTo::create(0.1f, Point(value_point.x, value_point.y+5*ratio));
            }
            auto ease0 = EaseSineInOut::create(move0);

            
            ScaleTo *scale1;
            if(new_value > target){
                scale1 = ScaleTo::create(0.4f, 1.4f);
            }else{
                scale1 = ScaleTo::create(0.4f, 1.2f);
            }
     //       auto sca_ease = EaseSineInOut::create(scale1);
            MoveTo *move1;
            if(new_value > target){
                 move1 = MoveTo::create(0.4f, Point(value_point.x, value_point.y+15*ratio));
            }else{
                 move1 = MoveTo::create(0.4f, Point(value_point.x, value_point.y-10*ratio));
            }
            auto ease1 = EaseSineOut::create(move1);
            
            
            ScaleTo *scale2 = ScaleTo::create(0.25f, 1.0f);
            MoveTo *move2 = MoveTo::create(0.25f, Point(value_point.x, value_point.y));
            
            Sequence *action = Sequence::create(Spawn::create(ease0,sca_ease0, NULL),Spawn::create(ease1,scale1, NULL),Spawn::create(move2,scale2, NULL),nullptr);
            
            numeric->stopAllActions();
            numeric->runAction(action);
            
        }), nullptr));
        return true;
    }else{
        return false;
    }
}

void Chara::erase()
{
    //画像不可視
    if(cost_sprite.empty() == false) for(Sprite *tempSprite : cost_sprite) tempSprite->setVisible(false);
    sprite_map["cost_back"]->setVisible(false);
    if(numeric!=nullptr) numeric->setVisible(false);
    if(sprite_map["combination_l"]!=nullptr) sprite_map["combination_l"]->setVisible(false);
    if(sprite_map["combination_r"]!=nullptr) sprite_map["combination_r"]->setVisible(false);
    sprite_map["type"]->setVisible(false);
    if(sprite_map["cant_attack"]!=nullptr) sprite_map["cant_attack"]->setVisible(false);
    if(sprite_map["cant_skill"]!=nullptr) sprite_map["cant_skill"]->setVisible(false);
    if(sprite_map["turn"]!=nullptr) sprite_map["turn"]->setVisible(false);
    if(sprite_map["nokori"]!=nullptr) sprite_map["nokori"]->setVisible(false);
    if(sprite_map["rare_shine"]!=nullptr) sprite_map["rare_shine"]->setVisible(false);
    sprite_map["reverse"]->setVisible(false);
    if(particle_rare!=nullptr) particle_rare->setVisible(false);
    hideSkillAble();
    
    chara_status_active.clear();
    chara_status_passive.clear();
    
    revival();//ステリセット
    
    while (true) {
        if(getChildByTag(ADD_IMG_TAG) == nullptr){
            break;
        }else{
            removeChildByTag(ADD_IMG_TAG);
        }
    }
    
    int r = random(2, 4);
    int y = this->getPositionY();
    int center_y = convertFrom640x960(Point(320, 480)).y;
    
    
    CallFunc *finish_func = CallFunc::create([&,this](){
        this->initialize();//カードの初期化
        //this->showCardParam(this->getBelongingIndex());
    });
    
    if(r==0) {
        //横への転がり
        int x = 640;
        if(y<center_y) x = -x;
        Point target_point = convertFromDisplaySize(Point(x, 0), 0.5, 0.5);
        
        Sprite *img = (Sprite *)this->getChildByTag(IMG_TAG);
        JumpTo *jump = JumpTo::create(1.0f, target_point, 100*ratio, 2);
        RotateBy *rotateby = RotateBy::create(1.0f, 180);
        Spawn *action = Spawn::create(jump, rotateby, nullptr);
		CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/dead1.mp3"); //SE
        img->runAction(Sequence::create(action, finish_func, NULL));
    } else if(r==1) {
        //ひねりながら消える
        sprite_map["chara"]->setVisible(false);
        Sprite *img = Sprite::create("download/Chara/chara100/" + to_string(getCharaId()) + ".png");
        img->setScale(ratio*IMG_RATIO);

        NodeGrid *nodeGrid = NodeGrid::create();
        this->addChild(nodeGrid);
        nodeGrid->addChild(img);
        
        Twirl *twirl = Twirl::create(1.0f, Size(100, 113), Point(0, 0), 2, 0);
        //SplitRows *split = SplitRows::create(1.0f, 10);
		CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/dead2.mp3"); //SE
        nodeGrid->runAction(Sequence::create(twirl, finish_func, NULL));
    } else if(r==2) {
        //横に真っ二つ
        Size size = sprite_map["chara"]->getContentSize();
        float width = size.width;
        float height = size.height;
        sprite_map["chara"]->setVisible(false);

        //上半分
        Rect rect1 = Rect(0, 0, width, height/2);//画像左上が基準
        Sprite *sprite1 = Sprite::create("download/Chara/chara100/" + to_string(getCharaId()) + ".png", rect1);
        sprite1->setScale(ratio*IMG_RATIO);
        sprite1->setPositionY(height/4*IMG_RATIO*ratio);
        this->addChild(sprite1,4);
        
        //下半分
        Rect rect2 = Rect(0, height/2, width, height/2);//画像左上が基準
        Sprite *sprite2 = Sprite::create("download/Chara/chara100/" + to_string(getCharaId()) + ".png", rect2);
        sprite2->setScale(ratio*IMG_RATIO);
        sprite2->setPositionY(-height/4*IMG_RATIO*ratio);
        this->addChild(sprite2,4);
        
        float dt = 0.4f;
        
        MoveBy *move1 = MoveBy::create(dt, Point(-20*ratio, 0));
        FadeOut *fadeout1 = FadeOut::create(dt);
        Spawn *action1 = Spawn::create(move1, fadeout1, nullptr);
        sprite1->runAction(action1);
        
        MoveBy *move2 = MoveBy::create(dt, Point(+20*ratio, 0));
        FadeOut *fadeout2 = FadeOut::create(dt);
        Spawn *action2 = Spawn::create(move2, fadeout2, nullptr);
		CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/dead3.mp3"); //SE
        sprite2->runAction(Sequence::create(action2, finish_func, NULL));
    } else if(r==3) {
        //縦に真っ二つ
        Size size = sprite_map["chara"]->getContentSize();
        float width = size.width;
        float height = size.height;
        sprite_map["chara"]->setVisible(false);
        
        //左半分
        Rect rect1 = Rect(0, 0, width/2, height);//画像左上が基準
        Sprite *sprite1 = Sprite::create("download/Chara/chara100/" + to_string(getCharaId()) + ".png", rect1);
        sprite1->setScale(ratio*IMG_RATIO);
        sprite1->setPositionX(-width/4*IMG_RATIO*ratio);
        this->addChild(sprite1,4);
        
        //右半分
        Rect rect2 = Rect(width/2, 0, width/2, height);//画像左上が基準
        Sprite *sprite2 = Sprite::create("download/Chara/chara100/" + to_string(getCharaId()) + ".png", rect2);
        sprite2->setScale(ratio*IMG_RATIO);
        sprite2->setPositionX(width/4*IMG_RATIO*ratio);
        this->addChild(sprite2,4);
        
        float dt = 0.4f;
        
        MoveBy *move1 = MoveBy::create(dt, Point(0, 20*ratio));
        FadeOut *fadeout1 = FadeOut::create(dt);
        Spawn *action1 = Spawn::create(move1, fadeout1, nullptr);
        sprite1->runAction(action1);
        
        MoveBy *move2 = MoveBy::create(dt, Point(0, -20*ratio));
        FadeOut *fadeout2 = FadeOut::create(dt);
        Spawn *action2 = Spawn::create(move2, fadeout2, nullptr);
		CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/dead4.mp3"); //SE
        sprite2->runAction(Sequence::create(action2, finish_func, NULL));
    } else {
        //縦に伸びながらフェードアウト
        sprite_map["chara"]->setVisible(false);
        Sprite *img = Sprite::create("download/Chara/chara100/" + to_string(getCharaId()) + ".png");
        img->setScale(ratio*IMG_RATIO);
        this->addChild(img,4);
        
        FadeOut *fadeout = FadeOut::create(0.3f);
        ScaleBy *scaleby = ScaleBy::create(0.3f, 0.5f, 1.2f);
        Spawn *action = Spawn::create(fadeout, scaleby, nullptr);
		CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/dead5.mp3"); //SE
        img->runAction(Sequence::create(action, finish_func, NULL));
    }
}

void Chara::eraseLeader(){//総大将が死んだ時
    if(erase_leader_flg == false){
        erase_leader_flg = true;
        
        auto dungeon = static_cast<DungeonScene *>(getParent()->getParent());
        if(dungeon->isRankingEventBattle() && getBelongingIndex()==TurnManage::getEnemyIndex()){//国士無双イベントで敵が死んだ時
            displayOverkill(dungeon->score_param_overkill);//オーバーキルダメージを表示
        }
        
        if(numeric!=nullptr){
            numeric->setScale(0);
            numeric->setVisible(false);
        }
        if(particle_rare!=nullptr){
            removeChild(particle_rare);
            particle_rare = nullptr;
        }
        
        sprite_map["type"]->setVisible(false);
        if(sprite_map["rare_shine"]!=nullptr) sprite_map["rare_shine"]->setVisible(false);
        if(sprite_map["cant_attack"]!=nullptr) sprite_map["cant_attack"]->setVisible(false);
        if(sprite_map["cant_skill"]!=nullptr) sprite_map["cant_skill"]->setVisible(false);
        
        if(sprite_map["nokori"]!=nullptr) sprite_map["nokori"]->setVisible(false);
        
        hideSkillAble();
        
        Point point = getPosition();
        
        MoveTo* move1 = MoveTo::create(0.0333f, Point(point.x+4*ratio, point.y));
        MoveTo* move2 = MoveTo::create(0.0333f, Point(point.x-4*ratio, point.y));
        MoveTo* move3 = MoveTo::create(0.0333f, Point(point.x, point.y));
        DelayTime* delay_t = DelayTime::create(1.0f);
        
        auto seq = Sequence::create(move1,move3,move2,move3, NULL);
        
        auto rep = Repeat::create(seq,20);
        
        auto tint = TintTo::create(1.5f, 200, 50,50);
        auto fade = FadeOut::create(2.0f);
        auto seq2 = Sequence::create(delay_t,fade, NULL);
        auto func = CallFunc::create([this,dungeon](){
            
            dungeon->gameoverJudge();
        });
        this->runAction(Spawn::create(tint,seq2,Sequence::create(delay_t,rep, NULL),Sequence::create(DelayTime::create(2.4f) ,func,NULL), NULL));
        
    }
}

void Chara::rebornLeader(){
    auto dungeon = static_cast<DungeonScene *>(getParent()->getParent());
   //ここから //stopAllActions(); 
    erase_leader_flg = false;
    initialize();
    
    auto reborn_light = ParticleSystemQuad::create("download/Effect/Particle/reborn1.plist");
    reborn_light->setPosition(Point(0, 0));
    reborn_light->setScale(ratio*1.5f);
    this->addChild(reborn_light, 200);
    
    auto reborn_light2 = ParticleSystemQuad::create("download/Effect/Particle/reborn2.plist");
    reborn_light2->setPosition(Point(0, 0));
    reborn_light2->setScale(ratio*1.5f);
    this->addChild(reborn_light2, 200);
    
    
    
    //自分へのターン変更から開始
    auto func = CallFunc::create([dungeon](){
        dungeon->setState("Directioning");
        TurnManage::setTurnIndex(TurnManage::getEnemyIndex());
        dungeon->changeTurn(0.0f);
    });
    runAction(Sequence::create(DelayTime::create(0.6f),func, NULL));

}

void Chara::displayRedBlink(float delay_time)
{
    //赤く点滅
    Sprite *img = Sprite::create("init/Dungeon/design/square_red.png");
    img->setScale(ratio);
    img->setOpacity(120);
    img->setPosition(Point(0, 0));
    this->addChild(img,1000, ADD_IMG_TAG);
    DelayTime *delay = DelayTime::create(delay_time);
    CallFunc *expire1 = CallFunc::create([img]()
                                         {
                                             img->removeFromParentAndCleanup(true);
                                         });//ラムダ式

    img->runAction(Sequence::create(delay, expire1, nullptr));
}

void Chara::displayDamage(int damage)
{
    bool heal_flg = false;
    if(damage < 0){
        heal_flg = true;
        damage = damage * -1;
    }
    int random = (arc4random()%10 - 5)*10;
    int random2 = (arc4random()%10 - 5)*10;
    
    string damageStr = to_string(damage);
    
    if(heal_flg == false){//ダメージの時
    
        Sprite *dmg_pop = Sprite::create("init/Dungeon/design/balloon_damage.png");
        dmg_pop->setAnchorPoint(Vec2(0.5,0.5));
        dmg_pop->setScale(0.2f*ratio);
        dmg_pop->setPosition(getPosition() + Point(random*ratio, random2*ratio));
        //dmg_pop->setGlobalZOrder(1000);
        dmg_pop->setCascadeOpacityEnabled(true);//透明度を子にも適用
        dmg_pop->setCascadeColorEnabled(true);
        static_cast<CardLayer *>(getParent())->addChild(dmg_pop, 5000);
        
        //TTFConfig ttfConfig(FONT_NAME, 50, GlyphCollection::DYNAMIC);
        //TTFConfig ttfConfigEffect(ttfConfig);
        //ttfConfigEffect.distanceFieldEnabled = true;
        
        //auto label = Label::createWithTTF(ttfConfigEffect, damageStr);
        auto label = HMSFontUtil::createWithTTFBySystemFontArgument(damageStr, FONT_NAME_SYSTEM, 50);
        
        label->setAnchorPoint(Point(0.5f,0.5f));
        
        if(heal_flg == true){
            label->setTextColor(Color4B(40, 255, 40, 255));
        }else{
            label->setTextColor(Color4B::WHITE);
            //label->enableShadow();
            //label->enableOutline(Color4B::WHITE, 1.0*ratio);
        }
        
        label->setPosition(Point(dmg_pop->getContentSize().width/2, dmg_pop->getContentSize().height/2));
        
        //label->enableOutline(Color4B::BLACK, 2);
        //label->setGlobalZOrder(1001);
        dmg_pop->addChild(label);
        
        
        ScaleTo *scale;
        if(damage>=400){
            scale = ScaleTo::create(0.5f, 1.25f*ratio);
        }else if(damage>=300){
            scale = ScaleTo::create(0.5f, 1.1f*ratio);
        }else if(damage>=200){
            scale = ScaleTo::create(0.5f, 0.95f*ratio);
        }else if(damage>=100){
            scale = ScaleTo::create(0.5f, 0.85f*ratio);
        }else if(damage<=9){
            scale = ScaleTo::create(0.5f, 0.6f*ratio);
        }else{
            scale = ScaleTo::create(0.5f, 0.7f*ratio);
        }
        auto easeE = EaseElasticOut::create(scale, 0.01);
        auto delay = DelayTime::create(0.1f);
        auto fadeO = FadeOut::create(0.15f);
        auto remove = RemoveSelf::create();
        dmg_pop->runAction(Sequence::create(easeE,delay,fadeO,remove, NULL));
        
    }else{//回復の時
        
        string damageStr = to_string(damage);
        HmsVector<string> damageVec;
        for(int i =0;i<damageStr.size();i++){
            damageVec.push_back(damageStr.substr(i,1));
        }
      
        auto label = HMSFontUtil::createWithTTFBySystemFontArgument(damageStr, FONT_NAME_SYSTEM, 50);
        
        int random = (arc4random()%10 - 5)*3;
        int random2 = (arc4random()%10 - 5)*3;
        
        label->setTextColor(Color4B(40, 255, 40, 255));
        
        label->setPosition(convertFromDisplaySize(Point(10-random*ratio-11*ratio*(int)damageVec.size(), -135+random2*ratio), 0.5, 0.5));
        label->setScale(0.3f*ratio);
        label->enableOutline(Color4B::WHITE, 2);
        
        this->addChild(label,1000);
        label->setGlobalZOrder(1001);
        
        MoveTo *move = MoveTo::create(0.8f, Point(10-random*ratio-11*ratio*(int)damageVec.size(), 40+random2*ratio));//数字一つ一つの場所調整
        auto easeAction = EaseBackOut::create(move);//行き過ぎて少し戻るイージング
        ScaleTo *scale = ScaleTo::create(0.5f, 1.0f*ratio);//大きく
        auto easeAction2 = EaseBackOut::create(scale);//行き過ぎて少し戻るイージング
        
        auto spawn = Spawn::create(easeAction,easeAction2,nullptr);
        
        auto delay = DelayTime::create(0.0f+(damageVec.size())*0.2f);//停止時間
        auto fade = FadeOut::create(0.1f);//フェードアウト
        auto sequence = Sequence::create(spawn,delay,fade, nullptr);
        auto remove= RemoveSelf::create(true);//アクション終わったら削除
        label->runAction(Sequence::create(sequence,remove, nullptr));
    }
}

void Chara::displayOverkill(int damage)
{
    
/*    string damageStr = to_string(damage);
    
    Sprite *dmg_pop = Sprite::create("init/Dungeon/design/balloon_damage.png");
    dmg_pop->setAnchorPoint(Vec2(0.5,0.5));
    dmg_pop->setScale(0.2f*ratio);
    dmg_pop->setPosition(getPosition()+Point(0,150));
    //dmg_pop->setGlobalZOrder(1000);
    dmg_pop->setCascadeOpacityEnabled(true);//透明度を子にも適用
    dmg_pop->setCascadeColorEnabled(true);
    static_cast<CardLayer *>(getParent())->addChild(dmg_pop, 5000);
    
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument(damageStr, FONT_NAME_SYSTEM, 50);
    
    label->setAnchorPoint(Point(0.5f,0.5f));
    label->setTextColor(Color4B::YELLOW);
    
    
    label->setPosition(Point(dmg_pop->getContentSize().width/2, dmg_pop->getContentSize().height/2));
    
    dmg_pop->addChild(label);
    
    
    ScaleTo *scale;
    if(damage>=1000){
        scale = ScaleTo::create(0.5f, 0.95f*ratio);
    }else if(damage>=100){
        scale = ScaleTo::create(0.5f, 0.85f*ratio);
    }else if(damage<=9){
        scale = ScaleTo::create(0.5f, 0.6f*ratio);
    }else{
        scale = ScaleTo::create(0.5f, 0.7f*ratio);
    }
    auto delay0 = DelayTime::create(0.4f);
    auto easeE = EaseElasticOut::create(scale, 0.01);
    auto delay = DelayTime::create(2.1f);
    auto fadeO = FadeOut::create(0.15f);
    auto remove = RemoveSelf::create();
    dmg_pop->runAction(Sequence::create(delay0,easeE,delay,fadeO,remove, NULL));*/
    
}

void Chara::displayWhiteBlink(float delay_time)
{
    //白く点滅
    Sprite *img = Sprite::create("init/Dungeon/design/square_white.png");
    img->setScale(ratio);
    img->setPosition(Point(0, 0));
    img->setOpacity(100);
    this->addChild(img, 500,ADD_IMG_TAG);
    DelayTime *delay = DelayTime::create(delay_time);
    CallFunc *expire1 = CallFunc::create([img]()
                                         {
                                             img->removeFromParentAndCleanup(true);
                                         });//ラムダ式

    img->runAction(Sequence::create(delay, expire1, nullptr));
}

void Chara::showYellowBlink()
{
    if(this->getChildByTag(ADD_IMG_TAG)==nullptr) {
        Sprite *img = Sprite::create("init/Dungeon/design/square_white.png");
        img->setScale(ratio);
        img->setPosition(Point(0, 0));
        img->setOpacity(80);
        this->addChild(img, 500, ADD_IMG_TAG);
        
        auto tinto = TintTo::create(0.6, 127,127,127);
        auto ease1 = EaseInOut::create(tinto, 2);
        auto spa1 = Spawn::create(tinto,ease1, NULL);
        
        auto tinto2 = TintTo::create(0.6, 255, 255, 255);
        auto ease2 = EaseInOut::create(tinto2, 2);
        auto spa2 = Spawn::create(tinto2,ease2, NULL);
        
        auto seq = Sequence::create(spa1, spa2, NULL);
        auto rep = RepeatForever::create(seq);
        img->runAction(rep);
        
    }
}

void Chara::hideYellowBlink()
{
    if(this->getChildByTag(ADD_IMG_TAG) != nullptr){
        this->removeChildByTag(ADD_IMG_TAG);
    }
}

bool Chara::skillAble(){//スキルが使える状態なのかそうでないのかのフラグ
    auto dungeon = static_cast<DungeonScene *>(getParent()->getParent());
    //if(dungeon->isCharacterPlacable()==false) return false;//これが原因・・・
    if(!(isOnField() || getType()==CHARA_TYPE_LEADER)) return false;//フィールドのキャラかリーダー以外はダメ
    if(isAlive()==false) return false;//死んでるときはダメ
    if(getActiveSkillId()<=0) return false;//スキルを所持してないときはダメ
    if(!(getActiveSkillCost() <= dungeon->player[getBelongingIndex()]->getNowPower())) return false;//スキルが使える士気がない時
    if(statusCheck(STATUS_TYPE_CANT_SKILL)) return false;
    if(statusCheck(STATUS_TYPE_CANT_ACTION)) return false;
    if(active_skill_able_count <= 0) return false;//スキル使える回数が残っていない時
    
    HmsVector<string> target_types = split(getActiveSkillTarget(), "#");
    if(target_types.size() >= 3){
        HmsVector<string> useable = split(target_types.at(2), "_");
        if(useable.at(0) == "moreTurn"){
            CCLOG("モア%d",stoi(useable.at(1)));
            CCLOG("ターン%d",TurnManage::getNowTurn());
            if(!(stoi(useable.at(1)) <= TurnManage::getNowTurn())){//Xターン以降の時使用可能
                return false;
            }
        }else if (useable.at(0) == "thisTurn"){
            if(!(stoi(useable.at(1)) == TurnManage::getNowTurn())){//Xターンの時使用可能
                return false;
            }
        }else if (useable.at(0) == "moreHp"){
            if(!(stoi(useable.at(1)) <= dungeon->player[getBelongingIndex()]->getHp())){//HPがX以上の時使用可能
                return false;
            }
        }else if (useable.at(0) == "lessHp"){
            if(!(stoi(useable.at(1)) >= dungeon->player[getBelongingIndex()]->getHp())){//HPがX以下の時使用可能
                return false;
            }
        }else if (useable.at(0) == "fieldLonely"){
            for(Chara *chara : CharaList::getOneSide(getBelongingIndex())){
                if(chara->isOnField() && chara!=this){//自分のフィールドにこのキャラしか居ない時使用可能
                    return false;
                }
            }
        }else if (useable.at(0) == "allCombo"){//すべてのコンボが繋がっている時
            for(int i=2; i<=6; i++){
                Chara *chara_temp = CharaList::getAtField(getBelongingIndex(), i);
                if(chara_temp == nullptr) return false;
                if(!(chara_temp->combination_count%10 >= 1.0f)) return false;//その場所にキャラが居て、右側がコンボで繋がっている時
            
            }
        }
    }
    
    return true;
}

bool Chara::moveAble(){//手札から動かせるかどうか
    auto dungeon = static_cast<DungeonScene *>(getParent()->getParent());
    if(isInHand()==false && isFriend()==false) return false;
    if(getPlaceCost() > dungeon->player[TurnManage::getPlayerIndex()]->getNowPower()) return false;//士気がたりない時
    
    for(auto passive : dungeon->player[getBelongingIndex()]->whole_status_passive){
        if(passive["state"]==STATUS_TYPE_CANT_SUMMONS && passive["value"]==getPlaceCost()){//召喚禁止のコストだった時
            return false;
        }
    }
    for(auto active : dungeon->player[getBelongingIndex()]->whole_status_active){
        if(active["state"]==STATUS_TYPE_CANT_SUMMONS && active["value"]==getPlaceCost()){//召喚禁止のコストだった時
            return false;
        }
    }
    
    return true;
}

void Chara::showSkillAble(){//スキル使用可能エフェクト
    auto dungeon = static_cast<DungeonScene *>(getParent()->getParent());
    
    if(getActiveSkillId()>0 && dungeon->player[getBelongingIndex()]->getHp()>0) {
        
        for(int i=0; i<cost_sprite.size(); i++){
            cost_sprite.at(i)->setOpacity(255);
        }
        
        if(getBelongingIndex() == TurnManage::getPlayerIndex() && static_cast<DungeonScene *>(getParent()->getParent())->isMyTurn()){//自分側のキャラで
            if (SaveData::loadBool("effect")) {//エフェクト有りの時
                if(skill_able == nullptr){
                    skill_able = ParticleSystemQuad::create("download/Effect/Particle/skill_able.plist");
                    skill_able->setPosition(Point(35*ratio,38*ratio));
                    skill_able->setScale(ratio);
                    skill_able->setDuration(-1);
                    this->addChild(skill_able, DAMAGE_IMG_Z, SKILL_ABLE_TAG);
                    
                    auto delay = DelayTime::create(0.3);
                    auto fadein = FadeIn::create(1.0f);
                    
                    auto seq = Sequence::create(delay,fadein, NULL);
                    skill_able->runAction(seq);
                }
                skill_able->setVisible(true);
            }else{
                
                for(int i=0; i<cost_sprite.size(); i++){
                    auto scale = ScaleTo::create(1.0f, 1.2f);
                    auto scale2 = ScaleTo::create(0.9f, 1.0f);
                    auto repeat = RepeatForever::create(Sequence::create(scale,scale2, NULL));
                    cost_sprite.at(i)->runAction(repeat);
                }
            }
        }
    }
}
void Chara::hideSkillAble()
{
    if(skill_able != nullptr){
        removeChild(skill_able);
        skill_able = nullptr;
    }
    
    if(active_skill_able_count <= 0 && (isOnField() || getType()==CHARA_TYPE_LEADER)){
        for(int i=0; i<cost_sprite.size(); i++){
            cost_sprite.at(i)->setOpacity(170);
        }
    }else{
        for(int i=0; i<cost_sprite.size(); i++){
            cost_sprite.at(i)->setOpacity(255);
        }
    }
}

void Chara::charaNameTab(bool appear_flg){//名前画像
    if(appear_flg == false || (statusCheck(STATUS_TYPE_DARKNESS) && getFieldIndex()>=2 && getBelongingIndex()==TurnManage::getEnemyIndex())){//表示を消す時
        if(sprite_map["name"]!=nullptr) removeChild(sprite_map["name"]);
        sprite_map["name"] = nullptr;
        
    }else{//表示する時
        if(sprite_map["name"]==nullptr){
            string name_image_path = StringUtils::format("download/Chara/name/%d.png", getCharaId());
            sprite_map["name"] = Sprite::create("download/Chara/name/"+to_string(getCharaId())+".png");
            this->addChild(sprite_map["name"],100000);
        }
        
        sprite_map["name"]->setPosition( 0, 71*ratio) ;
        sprite_map["name"]->setScale(ratio*2/3);
        sprite_map["name"]->setOpacity(230);
        sprite_map["name"]->setGlobalZOrder(0.6f);
    }
}

void Chara::scaleUp(bool up_flg)
{
    if(up_flg == true){
        auto scale = ScaleTo::create(0.1f, 1.12f);
        auto ease = EaseBackOut::create(scale);
        runAction(ease);
        
        auto scale2 = ScaleTo::create(0.1f, ratio*2/3*0.89f);
        auto ease2 = EaseBackOut::create(scale2);
        auto scale3 = MoveTo::create(0.1f, Vec2(0, 71* 0.89f* ratio));
        auto ease3 = EaseBackOut::create(scale3);
        if(sprite_map["name"]!=nullptr) sprite_map["name"]->runAction(Spawn::create(ease2, ease3, NULL));
       
    }else{
        auto scale = ScaleTo::create(0.1f, 1.0f);
        auto ease = EaseBackOut::create(scale);
        runAction(ease);
        
        auto scale2 = ScaleBy::create(0.1f, ratio*2/3);
        auto ease2 = EaseBackOut::create(scale2);
        auto scale3 = MoveTo::create(0.1f, Vec2(0, 71*ratio));
        auto ease3 = EaseBackOut::create(scale3);
        if(sprite_map["name"]!=nullptr) sprite_map["name"]->runAction(Spawn::create(ease2, ease3, NULL));
    }
}

/*void Chara::mascotScale(){
    
    auto scale = ScaleTo::create(0.08f, 1.2f*ratio);
    auto scale2 = ScaleTo::create(0.3f, 1.0f*ratio);
    auto ease = EaseElasticInOut::create(scale2, 9.0f);
    
    sprite_map["chara"]->runAction(Sequence::create(scale, ease, NULL));
}*/

int Chara::rightCharaFieldIndex(){//このキャラの右側のfiels_indexを返す
    if(field_index==0 || field_index==1 || field_index==7){
        return NULL;
    }else if(field_index>=2 && field_index<=6){
        return field_index+1;
    }else{
        return NULL;
    }
}
int Chara::leftCharaFieldIndex(){//このキャラの左側のfiels_indexを返す
    if(field_index==0 || field_index==1 || field_index==2){
        return NULL;
    }else if(field_index>=3 && field_index<=7){
        return field_index-1;
    }else{
        return NULL;
    }
}

vector<Chara *> Chara::getAroundFieldChara(string target){
    vector<Chara *> indexVec;
    
    if(target == "right"){//右側
        if(getFieldIndex()>=2 || getFieldIndex()<=6){
            for(Chara *chara : CharaList::getOneSide(getBelongingIndex())){
                if(chara->field_index >= 2 && chara->getFieldIndex()==getFieldIndex()+1){
                    indexVec.push_back(chara);
                }
            }
        }
    }else if(target == "left"){//左側
        if(getFieldIndex()>=3 || getFieldIndex()<=7){
            for(Chara *chara : CharaList::getOneSide(getBelongingIndex())){
                if(chara->field_index >= 2 && chara->getFieldIndex()==getFieldIndex()-1){
                    indexVec.push_back(chara);
                }
            }
        }
    }else if(target == "forward"){//前方の敵
        if(getFieldIndex()>=2 || getFieldIndex()<=7){
            for(Chara *chara : CharaList::getOneSide(reverseIndex(getBelongingIndex()))){
                int enemy_index = 9-getFieldIndex();//敵は逆
                
                if(chara->field_index >= 2 && chara->getFieldIndex()==enemy_index){
                    indexVec.push_back(chara);
                }
            }
        }
    }else if(target == "side"){//左右
        if(getFieldIndex()>=3 || getFieldIndex()<=7){//左
            for(int i=0;i<CharaList::getOneSide(getBelongingIndex()).size();i++){
                if(CharaList::getOneSide(getBelongingIndex()).at(i)->field_index >= 2 && CharaList::getOneSide(getBelongingIndex()).at(i)->getFieldIndex()==getFieldIndex()-1){
                    indexVec.push_back(CharaList::getOneSide(getBelongingIndex()).at(i));
                }
            }
        }
        if(getFieldIndex()>=2 || getFieldIndex()<=6){//右
            for(int i=0;i<CharaList::getOneSide(getBelongingIndex()).size();i++){
                if(CharaList::getOneSide(getBelongingIndex()).at(i)->field_index >= 2 && CharaList::getOneSide(getBelongingIndex()).at(i)->getFieldIndex()==getFieldIndex()+1){
                    indexVec.push_back(CharaList::getOneSide(getBelongingIndex()).at(i));
                }
            }
        }
        
    }else if(target == "own_3"){//自分と左右の３体
        if(getFieldIndex()>=3 || getFieldIndex()<=7){//左
            for(int i=0;i<CharaList::getOneSide(getBelongingIndex()).size();i++){
                if(CharaList::getOneSide(getBelongingIndex()).at(i)->field_index >= 2 && CharaList::getOneSide(getBelongingIndex()).at(i)->getFieldIndex()==getFieldIndex()-1){
                    indexVec.push_back(CharaList::getOneSide(getBelongingIndex()).at(i));
                }
            }
        }
        indexVec.push_back(this);//自分
        if(getFieldIndex()>=2 || getFieldIndex()<=6){//右
            for(int i=0;i<CharaList::getOneSide(getBelongingIndex()).size();i++){
                if(CharaList::getOneSide(getBelongingIndex()).at(i)->field_index >= 2 && CharaList::getOneSide(getBelongingIndex()).at(i)->getFieldIndex()==getFieldIndex()+1){
                    indexVec.push_back(CharaList::getOneSide(getBelongingIndex()).at(i));
                }
            }
        }
        
    }else if(target == "cross"){//斜め前方
        if(getFieldIndex()>=2 || getFieldIndex()<=6){//右前方
            for(int i=0;i<CharaList::getOneSide(reverseIndex(getBelongingIndex())).size();i++){
                int enemy_index = 8-getFieldIndex();
                
                if(CharaList::getOneSide(reverseIndex(getBelongingIndex())).at(i)->field_index >= 2 && CharaList::getOneSide(reverseIndex(getBelongingIndex())).at(i)->getFieldIndex()==enemy_index){
                    indexVec.push_back(CharaList::getOneSide(reverseIndex(getBelongingIndex())).at(i));
                }
            }
        }
        if(getFieldIndex()>=3 || getFieldIndex()<=7){//左前方
            for(int i=0;i<CharaList::getOneSide(reverseIndex(getBelongingIndex())).size();i++){
                int enemy_index = 10-getFieldIndex();
                
                if(CharaList::getOneSide(reverseIndex(getBelongingIndex())).at(i)->field_index >= 2 && CharaList::getOneSide(reverseIndex(getBelongingIndex())).at(i)->getFieldIndex()==enemy_index){
                    indexVec.push_back(CharaList::getOneSide(reverseIndex(getBelongingIndex())).at(i));
                }
            }
        }
    }else if(target == "forward_3"){//前方３体
        if(getFieldIndex()>=2 || getFieldIndex()<=6){//右前方
            for(int i=0;i<CharaList::getOneSide(reverseIndex(getBelongingIndex())).size();i++){
                int enemy_index = 8-getFieldIndex();
                
                if(CharaList::getOneSide(reverseIndex(getBelongingIndex())).at(i)->field_index >= 2 && CharaList::getOneSide(reverseIndex(getBelongingIndex())).at(i)->getFieldIndex()==enemy_index){
                    indexVec.push_back(CharaList::getOneSide(reverseIndex(getBelongingIndex())).at(i));
                }
            }
        }
        if(getFieldIndex()>=2 || getFieldIndex()<=7){//前方
            for(int i=0;i<CharaList::getOneSide(reverseIndex(getBelongingIndex())).size();i++){
                int enemy_index = 9-getFieldIndex();//敵は逆
                
                if(CharaList::getOneSide(reverseIndex(getBelongingIndex())).at(i)->field_index >= 2 && CharaList::getOneSide(reverseIndex(getBelongingIndex())).at(i)->getFieldIndex()==enemy_index){
                    indexVec.push_back(CharaList::getOneSide(reverseIndex(getBelongingIndex())).at(i));
                }
            }
        }
        if(getFieldIndex()>=3 || getFieldIndex()<=7){//左前方
            for(int i=0;i<CharaList::getOneSide(reverseIndex(getBelongingIndex())).size();i++){
                int enemy_index = 10-getFieldIndex();
                
                if(CharaList::getOneSide(reverseIndex(getBelongingIndex())).at(i)->field_index >= 2 && CharaList::getOneSide(reverseIndex(getBelongingIndex())).at(i)->getFieldIndex()==enemy_index){
                    indexVec.push_back(CharaList::getOneSide(reverseIndex(getBelongingIndex())).at(i));
                }
            }
        }
    }else if(target == "forward_right"){//右前方
        if(getFieldIndex()>=2 || getFieldIndex()<=6){//右前方
            for(int i=0;i<CharaList::getOneSide(reverseIndex(getBelongingIndex())).size();i++){
                int enemy_index = 8-getFieldIndex();
                
                if(CharaList::getOneSide(reverseIndex(getBelongingIndex())).at(i)->field_index >= 2 && CharaList::getOneSide(reverseIndex(getBelongingIndex())).at(i)->getFieldIndex()==enemy_index){
                    indexVec.push_back(CharaList::getOneSide(reverseIndex(getBelongingIndex())).at(i));
                }
            }
        }
    }else if(target == "forward_left"){//左前方
        if(getFieldIndex()>=3 || getFieldIndex()<=7){//左前方
            for(int i=0;i<CharaList::getOneSide(reverseIndex(getBelongingIndex())).size();i++){
                int enemy_index = 10-getFieldIndex();
                
                if(CharaList::getOneSide(reverseIndex(getBelongingIndex())).at(i)->field_index >= 2 && CharaList::getOneSide(reverseIndex(getBelongingIndex())).at(i)->getFieldIndex()==enemy_index){
                    indexVec.push_back(CharaList::getOneSide(reverseIndex(getBelongingIndex())).at(i));
                }
            }
        }
    }
    return indexVec;
}

bool Chara::statusCheck(int state){
    
    DungeonScene *dung = static_cast<DungeonScene *>(getParent()->getParent());
    if(state == STATUS_TYPE_CANT_ACTION){//行動不能の判定
        if(getDeadFlg() == true) return false;
        
        for(int i=0; i<chara_status_active.size(); i++){
            if(chara_status_active.at(i)["state"] == state && statusCheck(STATUS_TYPE_CANT_ACTION_GUARD)==false){//その状態異常を持っている時
                return true;
            }
        }
        for(int i=0; i<chara_status_passive.size(); i++){
            if(chara_status_passive.at(i)["state"] == state && statusCheck(STATUS_TYPE_CANT_ACTION_GUARD)==false){//その状態異常を持っている時
                return true;
            }
        }
        
        for(int i=0; i<chara_status_active.size(); i++){
            if(chara_status_active.at(i)["state"] == STATUS_TYPE_FREEZE){//氷漬け
                return true;
            }
        }
        for(int i=0; i<chara_status_passive.size(); i++){
            if(chara_status_passive.at(i)["state"] == STATUS_TYPE_FREEZE){//氷漬け
                return true;
            }
        }
        /*
        for(int i=0; i<chara_status_active.size(); i++){
            if(chara_status_active.at(i)["state"] == STATUS_TYPE_CANT_ACTION_ESCAPE && statusCheck(STATUS_TYPE_CANT_ACTION_GUARD)==false){//不屈からくる行動不能
                return true;
            }
        }*/
        
        for(int i=0; i<dung->player[getBelongingIndex()]->whole_status_active.size(); i++){//戦闘力？？以上行動不能
            if(dung->player[getBelongingIndex()]->whole_status_active.at(i)["state"] == STATUS_TYPE_CANT_ACTION_ABOVE
               && dung->player[getBelongingIndex()]->whole_status_active.at(i)["value"] <= this->getValue()
               && getFieldIndex() > 1 && statusCheck(STATUS_TYPE_CANT_ACTION_GUARD)==false){
                
                return true;
            }
        }
        for(int i=0; i<dung->player[getBelongingIndex()]->whole_status_passive.size(); i++){
            if(dung->player[getBelongingIndex()]->whole_status_passive.at(i)["state"] == STATUS_TYPE_CANT_ACTION_ABOVE
               && dung->player[getBelongingIndex()]->whole_status_passive.at(i)["value"] <= this->getValue()
               && getFieldIndex() > 1 && statusCheck(STATUS_TYPE_CANT_ACTION_GUARD)==false){
                
                return true;
            }
        }
        
        for(int i=0; i<dung->player[getBelongingIndex()]->whole_status_active.size(); i++){//戦闘力？？以下行動不能
            if(dung->player[getBelongingIndex()]->whole_status_active.at(i)["state"] == STATUS_TYPE_CANT_ACTION_BELOW
               && dung->player[getBelongingIndex()]->whole_status_active.at(i)["value"] >= this->getValue()
               && getFieldIndex() > 1 && statusCheck(STATUS_TYPE_CANT_ACTION_GUARD)==false){
                
                return true;
            }
        }
        for(int i=0; i<dung->player[getBelongingIndex()]->whole_status_passive.size(); i++){
            if(dung->player[getBelongingIndex()]->whole_status_passive.at(i)["state"] == STATUS_TYPE_CANT_ACTION_BELOW
               && dung->player[getBelongingIndex()]->whole_status_passive.at(i)["value"] >= this->getValue()
               && getFieldIndex() > 1 && statusCheck(STATUS_TYPE_CANT_ACTION_GUARD)==false){
                
                return true;
            }
        }
        
        for(int i=0; i<dung->player[getBelongingIndex()]->whole_status_active.size(); i++){//コンボカラーXが無いカード　行動不能
            if(dung->player[getBelongingIndex()]->whole_status_active.at(i)["state"] == STATUS_TYPE_CANT_ACTION_COMBO
               && !(dung->player[getBelongingIndex()]->whole_status_active.at(i)["value"] == getCombinationL() || dung->player[getBelongingIndex()]->whole_status_active.at(i)["value"] == getCombinationR())
               && getFieldIndex() > 1 && statusCheck(STATUS_TYPE_CANT_ACTION_GUARD)==false){
                
                return true;
            }
        }
        for(int i=0; i<dung->player[getBelongingIndex()]->whole_status_passive.size(); i++){
            if(dung->player[getBelongingIndex()]->whole_status_passive.at(i)["state"] == STATUS_TYPE_CANT_ACTION_COMBO
               && !(dung->player[getBelongingIndex()]->whole_status_passive.at(i)["value"] == getCombinationL() || dung->player[getBelongingIndex()]->whole_status_passive.at(i)["value"] == getCombinationR())
               && getFieldIndex() > 1 && statusCheck(STATUS_TYPE_CANT_ACTION_GUARD)==false){
                
                return true;
            }
        }
        
        for(int i=0; i<dung->player[getBelongingIndex()]->whole_status_active.size(); i++){//コストX以下　行動不能
            if(dung->player[getBelongingIndex()]->whole_status_active.at(i)["state"] == STATUS_TYPE_CANT_ACTION_COST_BELOW
               && dung->player[getBelongingIndex()]->whole_status_active.at(i)["value"] >= this->place_cost
               && getFieldIndex() > 1 && statusCheck(STATUS_TYPE_CANT_ACTION_GUARD)==false){
                
                return true;
            }
        }
        for(int i=0; i<dung->player[getBelongingIndex()]->whole_status_passive.size(); i++){
            if(dung->player[getBelongingIndex()]->whole_status_passive.at(i)["state"] == STATUS_TYPE_CANT_ACTION_COST_BELOW
               && dung->player[getBelongingIndex()]->whole_status_passive.at(i)["value"] >= this->place_cost
               && getFieldIndex() > 1 && statusCheck(STATUS_TYPE_CANT_ACTION_GUARD)==false){
                
                return true;
            }
        }
        
        for(int i=0; i<dung->player[getBelongingIndex()]->whole_status_active.size(); i++){//コストX以上　行動不能
            if(dung->player[getBelongingIndex()]->whole_status_active.at(i)["state"] == STATUS_TYPE_CANT_ACTION_COST_MORE
               && dung->player[getBelongingIndex()]->whole_status_active.at(i)["value"] <= this->place_cost
               && getFieldIndex() > 1 && statusCheck(STATUS_TYPE_CANT_ACTION_GUARD)==false){
                
                return true;
            }
        }
        for(int i=0; i<dung->player[getBelongingIndex()]->whole_status_passive.size(); i++){
            if(dung->player[getBelongingIndex()]->whole_status_passive.at(i)["state"] == STATUS_TYPE_CANT_ACTION_COST_MORE
               && dung->player[getBelongingIndex()]->whole_status_passive.at(i)["value"] <= this->place_cost
               && getFieldIndex() > 1 && statusCheck(STATUS_TYPE_CANT_ACTION_GUARD)==false){
                
                return true;
            }
        }
        
        for(int i=0; i<dung->player[getBelongingIndex()]->whole_status_active.size(); i++){//レア度X以下行動不能
            if(dung->player[getBelongingIndex()]->whole_status_active.at(i)["state"] == STATUS_TYPE_CANT_ACTION_RARE_BELOW
               && dung->player[getBelongingIndex()]->whole_status_active.at(i)["value"] >= this->getRare()
               && getFieldIndex() > 1 && statusCheck(STATUS_TYPE_CANT_ACTION_GUARD)==false){
                
                return true;
            }
        }
        for(int i=0; i<dung->player[getBelongingIndex()]->whole_status_passive.size(); i++){
            if(dung->player[getBelongingIndex()]->whole_status_passive.at(i)["state"] == STATUS_TYPE_CANT_ACTION_RARE_BELOW
               && dung->player[getBelongingIndex()]->whole_status_passive.at(i)["value"] >= this->getRare()
               && getFieldIndex() > 1 && statusCheck(STATUS_TYPE_CANT_ACTION_GUARD)==false){
                
                return true;
            }
        }
        
        for(int i=0; i<dung->player[getBelongingIndex()]->whole_status_active.size(); i++){//レア度X以上行動不能
            if(dung->player[getBelongingIndex()]->whole_status_active.at(i)["state"] == STATUS_TYPE_CANT_ACTION_RARE_MORE
               && dung->player[getBelongingIndex()]->whole_status_active.at(i)["value"] <= this->getRare()
               && getFieldIndex() > 1 && statusCheck(STATUS_TYPE_CANT_ACTION_GUARD)==false){
                
                return true;
            }
        }
        for(int i=0; i<dung->player[getBelongingIndex()]->whole_status_passive.size(); i++){
            if(dung->player[getBelongingIndex()]->whole_status_passive.at(i)["state"] == STATUS_TYPE_CANT_ACTION_RARE_MORE
               && dung->player[getBelongingIndex()]->whole_status_passive.at(i)["value"] <= this->getRare()
               && getFieldIndex() > 1 && statusCheck(STATUS_TYPE_CANT_ACTION_GUARD)==false){
                
                return true;
            }
        }
        
        return false;
        
    }else if(state == STATUS_TYPE_CANT_SKILL){//スキル使用不可の判定
        if(statusCheck(STATUS_TYPE_CANT_SKILL_GUARD)){//スキル使用不可耐性
            return false;
        }
        
        for(int i=0; i<chara_status_active.size(); i++){
            if(chara_status_active.at(i)["state"] == state){//その状態異常を持っている時
                return true;
            }
        }
        for(int i=0; i<chara_status_passive.size(); i++){
            if(chara_status_passive.at(i)["state"] == state){//その状態異常を持っている時
                return true;
            }
        }
        for(int i=0; i<dung->player[getBelongingIndex()]->whole_status_active.size(); i++){
            if(dung->player[getBelongingIndex()]->whole_status_active.at(i)["state"] == state && getFieldIndex() > 1){
                return true;
            }
        }
        for(int i=0; i<dung->player[getBelongingIndex()]->whole_status_passive.size(); i++){
            if(dung->player[getBelongingIndex()]->whole_status_passive.at(i)["state"] == state && getFieldIndex() > 1){
                return true;
            }
            if(dung->player[getBelongingIndex()]->whole_status_passive.at(i)["state"] == STATUS_TYPE_CANT_SKILL_ATTACK &&
               getFieldIndex() > 1 && isAttacker()){
                return true;
            }
            if(dung->player[getBelongingIndex()]->whole_status_passive.at(i)["state"] == STATUS_TYPE_CANT_SKILL_DEFENSE &&
               getFieldIndex() > 1 && isDefender()){
                return true;
            }
            if(dung->player[getBelongingIndex()]->whole_status_passive.at(i)["state"] == STATUS_TYPE_CANT_SKILL_SUPPORT &&
               (getFieldIndex() == 0 || getFieldIndex() == 1) && getType()==CHARA_TYPE_SUPPORT){
                return true;
            }
        }
        return false;
    }else{
        
        for(int i=0; i<chara_status_active.size(); i++){
            if(chara_status_active.at(i)["state"] == state){//その状態異常を持っている時
                return true;
            }
        }
        for(int i=0; i<chara_status_passive.size(); i++){
            if(chara_status_passive.at(i)["state"] == state){//その状態異常を持っている時
                return true;
            }
        }
        for(int i=0; i<dung->player[getBelongingIndex()]->whole_status_active.size(); i++){
            if(dung->player[getBelongingIndex()]->whole_status_active.at(i)["state"] == state){
                return true;
            }
        }
        for(int i=0; i<dung->player[getBelongingIndex()]->whole_status_passive.size(); i++){
            if(dung->player[getBelongingIndex()]->whole_status_passive.at(i)["state"] == state){
                return true;
            }
        }
    }
    
    return false;//何も無かったらfalse
}

int Chara::skillCount(int state){//その単体の状態効果をいくつ持っているか
    int count = 0;
    for(int i=0; i<chara_status_active.size(); i++){
        if(chara_status_active.at(i)["state"] == state){
            count++;
        }
    }
    for(int i=0; i<chara_status_passive.size(); i++){
        if(chara_status_passive.at(i)["state"] == state){
            count++;
        }
    }
    
    return count;
}


float Chara::statusUpdown(int state){//状態異常のvalueを数える パーセント。　STATUS_TYPE_PLAYER_ATTACK_UPDOWNで使っている
    DungeonScene *dung = static_cast<DungeonScene *>(getParent()->getParent());
    float updown = 100.0f;
    int belong = getBelongingIndex();
    if(statusCheck(STATUS_TYPE_CHARM)) belong = reverseIndex(getBelongingIndex());//チャームがかかっている時は逆にする
    
    //敵武将につくマイナスの状態異常(バリアー)は割る、チャームの時は逆に
    for(Chara *enemy_one : CharaList::getOneSide(reverseIndex(belong))){
        for(int i=0; i<enemy_one->chara_status_active.size(); i++){
            if(enemy_one->chara_status_active.at(i)["state"] == state && enemy_one->chara_status_active.at(i)["value"] < 0){//その状態異常を持っている時
                updown = updown * (100.0f-(-1 * enemy_one->chara_status_active.at(i)["value"])) / 100.0f;
            }
        }
        for(int i=0; i<enemy_one->chara_status_passive.size(); i++){
            if(enemy_one->chara_status_passive.at(i)["state"] == state && enemy_one->chara_status_passive.at(i)["value"] < 0){//その状態異常を持っている時
                updown = updown * (100.0f-(-1 * enemy_one->chara_status_passive.at(i)["value"])) / 100.0f;
            }
        }
    }
    
    //敵軍師・総大将につくバリアーは割る、チャームの時は逆に
    for(int i=0; i<dung->player[reverseIndex(belong)]->whole_status_active.size(); i++){
        if(dung->player[reverseIndex(belong)]->whole_status_active.at(i)["state"] == state && dung->player[reverseIndex(belong)]->whole_status_active.at(i)["value"] < 0){
            updown = updown * (100.0f-(-1 *dung->player[reverseIndex(belong)]->whole_status_active.at(i)["value"])) / 100.0f;
        }
    }
    for(int i=0; i<dung->player[reverseIndex(belong)]->whole_status_passive.size(); i++){
        if(dung->player[reverseIndex(belong)]->whole_status_passive.at(i)["state"] == state && dung->player[reverseIndex(belong)]->whole_status_passive.at(i)["value"] < 0){
            updown = updown * (100.0f-(-1 *dung->player[reverseIndex(belong)]->whole_status_passive.at(i)["value"])) / 100.0f;
        }
    }
    
    //このキャラに付くプラスの状態異常（ダメージアップ）は足す
    for(int i=0; i<chara_status_active.size(); i++){
        if(chara_status_active.at(i)["state"] == state && chara_status_active.at(i)["value"] >= 0){//その状態異常を持っている時
            updown += chara_status_active.at(i)["value"];
        }
    }
    for(int i=0; i<chara_status_passive.size(); i++){
        if(chara_status_passive.at(i)["state"] == state && chara_status_passive.at(i)["value"] >= 0){//その状態異常を持っている時
            updown += chara_status_passive.at(i)["value"];
        }
    }
    
    //味方軍師・総大将に付くダメージアップは足す。チャームの時でも同じように足す
    for(int i=0; i<dung->player[getBelongingIndex()]->whole_status_active.size(); i++){
        if(dung->player[getBelongingIndex()]->whole_status_active.at(i)["state"] == state && dung->player[getBelongingIndex()]->whole_status_active.at(i)["value"] >= 0){
            updown += dung->player[getBelongingIndex()]->whole_status_active.at(i)["value"];
        }
    }
    for(int i=0; i<dung->player[getBelongingIndex()]->whole_status_passive.size(); i++){
        if(dung->player[getBelongingIndex()]->whole_status_passive.at(i)["state"] == state && dung->player[getBelongingIndex()]->whole_status_passive.at(i)["value"] >= 0){
            updown += dung->player[getBelongingIndex()]->whole_status_passive.at(i)["value"];
        }
    }
    
    //応援効果を足す。
    if(dung->isBossEventBattle() && getBelongingIndex()==TurnManage::getPlayerIndex()) {
        int count = dung->support_user_name_list.size();
        HMSLOG("応援%d人", count);
        if(count>=1 && count<=100) {
            int rate = fromString<int>(dung->support_rate_up_list.at(count));
            HMSLOG("rate=%d", rate);
            updown += rate;
        }
    }
    
    return updown;
}

int Chara::getNearBlankFieldIndex(){//空白で空いている自分に最も近くの味方フィールド
    DungeonScene *dung = static_cast<DungeonScene *>(getParent()->getParent());
    
    auto blankCheck = [dung, this](int field_index){//その場所が空白か否か
        if(field_index < 2 || field_index >7) return false;
        for(Chara *chara : CharaList::getOneSide(getBelongingIndex())){
            if(chara->getFieldIndex() == field_index){
                return false;
            }
        }
        if(dung->fieldLayer->equalPositionType(this, field_index , this->getBelongingIndex())==false){
            return false;
        }
        return true;
    };
    if(blankCheck(getFieldIndex()-1)) return getFieldIndex()-1;
    if(blankCheck(getFieldIndex()+1)) return getFieldIndex()+1;
    if(blankCheck(getFieldIndex()-2)) return getFieldIndex()-2;
    if(blankCheck(getFieldIndex()+2)) return getFieldIndex()+2;
    if(blankCheck(getFieldIndex()-3)) return getFieldIndex()-3;
    if(blankCheck(getFieldIndex()+3)) return getFieldIndex()+3;
    if(blankCheck(getFieldIndex()-4)) return getFieldIndex()-4;
    if(blankCheck(getFieldIndex()+4)) return getFieldIndex()+4;
    if(blankCheck(getFieldIndex()-5)) return getFieldIndex()-5;
    if(blankCheck(getFieldIndex()+5)) return getFieldIndex()+5;
    return 0;
}


int Chara::reverseIndex(int index){
    if(index==0){
        return 1;
    }else{
        return 0;
    }
}
void Chara::updateStatusIcon(){
    DungeonScene *dung = static_cast<DungeonScene *>(getParent()->getParent());
    
    vector< map<string, int> > active_temp = chara_status_active.getVector();
    vector< map<string, int> > passive_temp = chara_status_passive.getVector();
    
    auto itr = active_temp.begin();
    while (itr != active_temp.end()) {//行動不能とスキル封印のアイコンは別で表示するので除外 アイコンが見つからなかった場合も
        if((*itr)["state"] == STATUS_TYPE_CANT_ACTION || (*itr)["state"] == STATUS_TYPE_CANT_SKILL || dung->activeSkill->getIcon((*itr)["skill_id"]) == nullptr){
            active_temp.erase(itr);
        }else{
            itr++;
        }
    }
    
    auto itr2 = passive_temp.begin();
    while (itr2 != passive_temp.end()) {//行動不能とスキル封印のアイコンは別で表示するので除外 アイコンが見つからなかった場合も
        if((*itr2)["state"] == STATUS_TYPE_CANT_ACTION || (*itr2)["state"] == STATUS_TYPE_CANT_SKILL || (*itr2)["state"] == STATUS_TYPE_PLACE_FORBIDDEN_FORWARD || (*itr2)["state"] == STATUS_TYPE_PLACE_FORBIDDEN_SUPPORT || dung->passiveSkill->getIcon((*itr2)["skill_id"]) == nullptr){
            passive_temp.erase(itr2);
        }else{
            itr2++;
        }
    }
    /*while (true) {//行動不能とスキル封印のアイコンは別で表示するので除外 アイコンが見つからなかった場合も
        bool change_flg = false;
        for(int i=0; i<active_temp.size(); i++){
            if(active_temp.at(i)["state"] == STATUS_TYPE_CANT_ACTION || active_temp.at(i)["state"] == STATUS_TYPE_CANT_SKILL || dung->activeSkill->getIcon(active_temp.at(i)["skill_id"]) == nullptr){
                active_temp.erase(active_temp.begin() + i);
                //HMSLOG("アイコン除外.サイズ%lu",active_temp.size());
                change_flg = true;
                break;
            }
        }
        if(change_flg == false) break;
    }
    while (true) {//行動不能とスキル封印のアイコンは別で表示するので除外 アイコンが見つからなかった場合も
        bool change_flg = false;
        for(int i=0; i<passive_temp.size(); i++){
            if(passive_temp.at(i)["state"] == STATUS_TYPE_CANT_ACTION || passive_temp.at(i)["state"] == STATUS_TYPE_CANT_SKILL || passive_temp.at(i)["state"] == STATUS_TYPE_PLACE_FORBIDDEN_FORWARD || passive_temp.at(i)["state"] == STATUS_TYPE_PLACE_FORBIDDEN_SUPPORT || dung->passiveSkill->getIcon(passive_temp.at(i)["skill_id"]) == nullptr){
                passive_temp.erase(passive_temp.begin() + i);
                change_flg = true;
                break;
            }
        }
        if(change_flg == false) break;
    }*/
    
    
    if((active_temp.size() > 0 || passive_temp.size() > 0) && !(statusCheck(STATUS_TYPE_DARKNESS) &&getFieldIndex() >= 2 && getBelongingIndex()==TurnManage::getEnemyIndex())){
        
        currentIcon++;
        if(currentIcon*3 > active_temp.size()+passive_temp.size()-1){//すべてのバッファを表示したら最初から
            currentIcon=0;
        }
        
        for(int i=0;i<3;i++){
            iconSprite[i] = Sprite::create("init/Dungeon/design/blank.png");
            
            //アイコンスプライトの取得
            //Chara *skill_use_chara;
            
            if(passive_temp.size()>currentIcon*3+i){//パッシブのアイコン
    
                iconSprite[i] = dung->passiveSkill->getIcon(passive_temp.at(currentIcon*3+i)["skill_id"]);
            
            }else if(passive_temp.size()+active_temp.size()>currentIcon*3+i){//アクティブのアイコン
                
                iconSprite[i] = dung->activeSkill->getIcon(active_temp.at(currentIcon*3+i-passive_temp.size())["skill_id"]);
                
            }else{
                break;
            }
            
            iconSprite[i]->setScale(ratio*0.63f);
            Point temp_point = static_cast<DungeonScene *>(getParent()->getParent())->fieldLayer->field_position_list[getBelongingIndex()].at(getFieldIndex());
            iconSprite[i]->setPosition(temp_point + Point(35*ratio - i*35*ratio,72*ratio));
            static_cast<CardLayer *>(getParent())->addChild(iconSprite[i],10);
            iconSprite[i]->setOpacity(0);
            
            FadeTo *fadeIn = FadeTo::create(0.4,255);
            DelayTime * delay = DelayTime::create(0.8);
            FadeOut *fadeOut = FadeOut::create(0.4);
            RemoveSelf *remove = RemoveSelf::create();
            
            Sequence *seq = Sequence::create(fadeIn,delay,fadeOut,remove, nullptr);
            iconSprite[i]->runAction(seq);
            
        }
    }
}

void Chara::setTypeSprite(){
    
    //this->removeChild(sprite_map["type"],true);
    
    if(sprite_map["type"]==nullptr || sprite_map["type"]->getTag()!=getType()){//まだ画像がないか、画像とタイプが食い違っている時
        removeChild(sprite_map["type"],true);
        
        if(getType()==CHARA_TYPE_ATTACK) {
            sprite_map["type"] = Sprite::create("init/CharaMenu/icon_offense.png");
            sprite_map["type"]->setPosition(Point(-28*ratio, -31*ratio));
            sprite_map["type"]->setTag(CHARA_TYPE_ATTACK);
        }
        
        if(getType()==CHARA_TYPE_DEFENSE){
            sprite_map["type"] = Sprite::create("init/CharaMenu/icon_defense.png");
            sprite_map["type"]->setPosition(Point(-30*ratio, -30*ratio));
            sprite_map["type"]->setTag(CHARA_TYPE_DEFENSE);
        }
        
        if(getType()==CHARA_TYPE_BALANCE){
            sprite_map["type"] = Sprite::create("init/CharaMenu/icon_offense_defense.png");
            sprite_map["type"]->setPosition(Point(-28*ratio, -31*ratio));
            sprite_map["type"]->setTag(CHARA_TYPE_BALANCE);
        }
        
        if(getType()==CHARA_TYPE_SUPPORT){
            sprite_map["turn"] = Sprite::create("download/Chara/parts100/card_turn.png");
            sprite_map["turn"]->setPosition(Point(28*ratio, -37*ratio));
            sprite_map["turn"]->setScale(ratio);
            addChild(sprite_map["turn"], 2);
            
            sprite_map["type"] = Sprite::create("init/CharaMenu/icon_strategist.png");
            sprite_map["type"]->setPosition(Point(-31*ratio, -30*ratio));
            sprite_map["type"]->setTag(CHARA_TYPE_SUPPORT);
            
            sprite_map["nokori"] = Sprite::create("download/Chara/parts100/card_remnant.png");//残り
            sprite_map["nokori"]->setPosition(Point(-32*ratio, -37*ratio));
            sprite_map["nokori"]->setScale(ratio);
            addChild(sprite_map["nokori"], 3);
        }
        
        if(getType()==CHARA_TYPE_LEADER){
            sprite_map["type"] = Sprite::create("download/Chara/parts100/card_morale.png");
            sprite_map["type"]->setPosition(Point(-25*ratio, -37*ratio));
            sprite_map["type"]->setScale(ratio);
            sprite_map["type"]->setTag(CHARA_TYPE_LEADER);
        }
        
        
        sprite_map["type"]->setScale(ratio);
        this->addChild(sprite_map["type"],2);
    }
}


Json Chara::exportData()
{
    
    map<string, Json> data_chara_map;
    
    if(getCharaIdDef() != CharaList::chara_list.at(getPrimaryId())->getCharaId()){//キャラID
        data_chara_map.insert(make_pair("id", CharaList::chara_list.at(getPrimaryId())->getCharaId()));
    }
    if(CharaList::chara_list.at(getPrimaryId())->getFieldIndex() != -1){//フィールドインデックス
        data_chara_map.insert(make_pair("field", CharaList::chara_list.at(getPrimaryId())->getFieldIndex()));
    }
    if(CharaList::chara_list.at(getPrimaryId())->getHandIndex() != -1){//ハンドインデックス
        data_chara_map.insert(make_pair("hand", CharaList::chara_list.at(getPrimaryId())->getHandIndex()));
    }
    if(CharaList::chara_list.at(getPrimaryId())->getDeadFlg() == true){//デッドフラグ
        data_chara_map.insert(make_pair("dead", 1));
    }
    if(CharaList::chara_list.at(getPrimaryId())->getRare() != getRareDef()){//レア度
        data_chara_map.insert(make_pair("rare", CharaList::chara_list.at(getPrimaryId())->getRare()));
    }
    if(CharaList::chara_list.at(getPrimaryId())->getType() != getTypeDef()){//レア度
        data_chara_map.insert(make_pair("type", CharaList::chara_list.at(getPrimaryId())->getType()));
    }
    if(CharaList::chara_list.at(getPrimaryId())->getBaseValue() != getBaseValueDef()){//baseValue
        data_chara_map.insert(make_pair("base", CharaList::chara_list.at(getPrimaryId())->getBaseValue()));
    }

    if(CharaList::chara_list.at(getPrimaryId())->getPlaceCost() != getPlaceCostDef()){//place_cost
        data_chara_map.insert(make_pair("pla", CharaList::chara_list.at(getPrimaryId())->getPlaceCost()));
    }
    if(CharaList::chara_list.at(getPrimaryId())->getCombinationL() != getCombinationLDef()){//左コンボ
        data_chara_map.insert(make_pair("cmb_L", CharaList::chara_list.at(getPrimaryId())->getCombinationL()));
    }
    if(CharaList::chara_list.at(getPrimaryId())->getCombinationR() != getCombinationRDef()){//右コンボ
        data_chara_map.insert(make_pair("cmb_R", CharaList::chara_list.at(getPrimaryId())->getCombinationR()));
    }
    if(CharaList::chara_list.at(getPrimaryId())->active_skill_able_count != SaveData::loadInt("active_skill_limit")){//スキル使用回数
        data_chara_map.insert(make_pair("ski_lim", CharaList::chara_list.at(getPrimaryId())->active_skill_able_count));
    }
    
    
    if(CharaList::chara_list.at(getPrimaryId())->getActiveSkillId() != getActiveSkillIdDef()){//
        data_chara_map.insert(make_pair("act_id", CharaList::chara_list.at(getPrimaryId())->getActiveSkillId()));
    }
    if(CharaList::chara_list.at(getPrimaryId())->getActiveSkillValue() != getActiveSkillValueDef()){//
        data_chara_map.insert(make_pair("act_val", CharaList::chara_list.at(getPrimaryId())->getActiveSkillValue()));
    }
    if(CharaList::chara_list.at(getPrimaryId())->getActiveSkillTurn() != getActiveSkillTurnDef()){//
        data_chara_map.insert(make_pair("act_turn", CharaList::chara_list.at(getPrimaryId())->getActiveSkillTurn()));
    }
    if(CharaList::chara_list.at(getPrimaryId())->getActiveSkillName() != getActiveSkillNameDef()){//
        data_chara_map.insert(make_pair("act_name", CharaList::chara_list.at(getPrimaryId())->getActiveSkillName()));
    }
    if(CharaList::chara_list.at(getPrimaryId())->getActiveSkillTarget() != getActiveSkillTargetDef()){//
        data_chara_map.insert(make_pair("act_tag", CharaList::chara_list.at(getPrimaryId())->getActiveSkillTarget()));
    }
    if(CharaList::chara_list.at(getPrimaryId())->getActiveSkillDetail() != getActiveSkillDetailDef()){//
        data_chara_map.insert(make_pair("act_det", CharaList::chara_list.at(getPrimaryId())->getActiveSkillDetail()));
    }
    if(CharaList::chara_list.at(getPrimaryId())->getPassiveSkillId() != getPassiveSkillIdDef()){//
        data_chara_map.insert(make_pair("pas_id", CharaList::chara_list.at(getPrimaryId())->getPassiveSkillId()));
    }
    if(CharaList::chara_list.at(getPrimaryId())->getPassiveSkillValue() != getPassiveSkillValueDef()){//
        data_chara_map.insert(make_pair("pas_val", CharaList::chara_list.at(getPrimaryId())->getPassiveSkillValue()));
    }
    if(CharaList::chara_list.at(getPrimaryId())->getPassiveSkillName() != getPassiveSkillNameDef()){//
        data_chara_map.insert(make_pair("pas_name", CharaList::chara_list.at(getPrimaryId())->getPassiveSkillName()));
    }
    if(CharaList::chara_list.at(getPrimaryId())->getPassiveSkillTarget() != getPassiveSkillTargetDef()){//
        data_chara_map.insert(make_pair("pas_tag", CharaList::chara_list.at(getPrimaryId())->getPassiveSkillTarget()));
    }
    if(CharaList::chara_list.at(getPrimaryId())->getPassiveSkillDetail() != getPassiveSkillDetailDef()){//
        data_chara_map.insert(make_pair("pas_det", CharaList::chara_list.at(getPrimaryId())->getPassiveSkillDetail()));
    }
    if(eliminate_flg == true){//
        data_chara_map.insert(make_pair("eli_flg", CharaList::chara_list.at(getPrimaryId())->eliminate_flg));
    }
    
    if(CharaList::chara_list.at(getPrimaryId())->getBelongingIndex() != getBelongingIndexDef()){//
        data_chara_map.insert(make_pair("index", CharaList::chara_list.at(getPrimaryId())->getBelongingIndex()));
    }
    
    if(CharaList::chara_list.at(getPrimaryId())->chara_status_active.size()>0){
        data_chara_map.insert(make_pair("sta_act", Json(CharaList::chara_list.at(getPrimaryId())->chara_status_active.getVector())));
    }
    if(CharaList::chara_list.at(getPrimaryId())->chara_status_passive.size()>0){
        data_chara_map.insert(make_pair("sta_pas", Json(CharaList::chara_list.at(getPrimaryId())->chara_status_passive.getVector())));
    }
    
    return Json(data_chara_map);
}

void Chara::importData(Json all_json)
{
    DungeonScene *dung = static_cast<DungeonScene *>(getParent()->getParent());
    
    
    string err;
    //Json all_json = Json::parse(data, err);
    
    vector<map<string, Json>> data_charas;
    map<string, Json> json_player1 = dung->json_master["player_data1"].object_items();
    map<string, Json> json_player2 = dung->json_master["player_data2"].object_items();
    
    
    vector<Json> all_json_chara = json_player1["card"].array_items();
    for(Json temp : json_player2["card"].array_items()){
        all_json_chara.push_back(temp);
    }
    
    map<string, Json> init_chara_map;
    if(getPrimaryId()<all_json_chara.size()) init_chara_map = all_json_chara.at(getPrimaryId()).object_items();
    
    //vector<Json> data_charas = all_json.array_items();
    //for(int i=0; i<data_charas.size(); i++){
    map<string, Json> data_chara_map = all_json.object_items();
    CCLOG("キャラジェイソン%s,プライマリ%d,フィールド%d",all_json.dump().c_str(),getPrimaryId(),data_chara_map["field"].int_value());
    if(data_chara_map["id"].is_null()==false){
        CCLOG("ID変更");
        setCharaId(data_chara_map["id"].int_value());
    }else{
        if(init_chara_map.empty()==false) setCharaId(init_chara_map["id"].int_value());
    }
    
    if(data_chara_map["field"].is_null()==false){
        setFieldIndex(data_chara_map["field"].int_value());
        CCLOG("フィールド変更%d",getFieldIndex());
    }else{
        setFieldIndex(-1);
    }
    
    if(data_chara_map["hand"].is_null()==false){
        CCLOG("ハンド変更");
        setHandIndex(data_chara_map["hand"].int_value());
    }else{
        setHandIndex(-1);
    }
    
    if(data_chara_map["rare"].is_null()==false){setRare(data_chara_map["rare"].int_value());
    }else{setRare(getRareDef());};
    
    if(data_chara_map["type"].is_null()==false){setType(data_chara_map["type"].int_value());
    }else{setType(getTypeDef());};
    
    if(data_chara_map["base"].is_null()==false){setBaseValue(data_chara_map["base"].int_value());
    }else{setBaseValue(getBaseValueDef());};
    
    if(data_chara_map["cmb_L"].is_null()==false){setCombinationL(data_chara_map["cmb_L"].int_value());
    }else{setCombinationL(getCombinationLDef());};
    
    if(data_chara_map["cmb_R"].is_null()==false){setCombinationR(data_chara_map["cmb_R"].int_value());
    }else{setCombinationR(getCombinationRDef());};
    
    if(data_chara_map["ski_lim"].is_null()==false){active_skill_able_count = data_chara_map["ski_lim"].int_value();
    }else{active_skill_able_count = SaveData::loadInt("active_skill_limit");};
    
    if(data_chara_map["act_id"].is_null()==false){setActiveSkillId(data_chara_map["act_id"].int_value());
    }else{setActiveSkillId(getActiveSkillIdDef());};

    if(data_chara_map["act_val"].is_null()==false){setActiveSkillValue(data_chara_map["act_val"].int_value());
    }else{setActiveSkillValue(getActiveSkillValueDef());};
    
    if(data_chara_map["act_turn"].is_null()==false){setActiveSkillTurn(data_chara_map["act_turn"].int_value());
    }else{setActiveSkillTurn(getActiveSkillTurnDef());};
    
    if(data_chara_map["act_name"].is_null()==false){setActiveSkillName(data_chara_map["act_name"].string_value());
    }else{setActiveSkillName(getActiveSkillNameDef());};

    if(data_chara_map["act_tag"].is_null()==false){setActiveSkillTarget(data_chara_map["act_tag"].string_value());
    }else{setActiveSkillTarget(getActiveSkillTargetDef());};

    if(data_chara_map["act_det"].is_null()==false){setActiveSkillDetail(data_chara_map["act_det"].string_value());
    }else{setActiveSkillDetail(getActiveSkillDetailDef());};
    
    if(data_chara_map["pas_id"].is_null()==false){setPassiveSkillId(data_chara_map["pas_id"].int_value());
    }else{setPassiveSkillId(getPassiveSkillIdDef());};
    
    if(data_chara_map["pas_val"].is_null()==false){setPassiveSkillValue(data_chara_map["pas_val"].int_value());
    }else{setPassiveSkillValue(getPassiveSkillValueDef());};
    
    if(data_chara_map["pas_name"].is_null()==false){setPassiveSkillName(data_chara_map["pas_name"].string_value());
    }else{setPassiveSkillName(getPassiveSkillNameDef());};
    
    if(data_chara_map["pas_tag"].is_null()==false){setPassiveSkillTarget(data_chara_map["pas_tag"].string_value());
    }else{setPassiveSkillTarget(getPassiveSkillTargetDef());};

    if(data_chara_map["pas_det"].is_null()==false){setPassiveSkillDetail(data_chara_map["pas_det"].string_value());
    }else{setPassiveSkillDetail(getPassiveSkillDetailDef());};
    
    if(data_chara_map["eli_flg"].is_null()==false){eliminate_flg = data_chara_map["eli_flg"].bool_value();
    }else{eliminate_flg = false;};
    
    
    
    /*if(getPrimaryId() < json_player1["card"].array_items().size()){
        CharaList::chara_list.at(getPrimaryId())->belongingIndex = 0;
    }else if(getPrimaryId() < all_json_chara.size()){
        CharaList::chara_list.at(getPrimaryId())->belongingIndex = 1;
    }*/
    
    if(data_chara_map["index"].is_null()==false){
        CCLOG("インデックス変更");
        setBelongingIndex(data_chara_map["index"].int_value());
    }else{
        setBelongingIndex(getBelongingIndexDef());
    }
    
    if(data_chara_map["sta_act"].is_null()==false){
        CharaList::chara_list.at(getPrimaryId())->chara_status_active.clear();
        
        vector<Json> ski_vec = data_chara_map["sta_act"].array_items();
        for(int i=0; i<ski_vec.size(); i++){
            map<string, Json> ski_map = ski_vec.at(i).object_items();
            map<string, int> temp;
            auto itr = ski_map.begin();
            for(itr=ski_map.begin(); itr!=ski_map.end(); itr++){
                temp.insert(make_pair(itr->first, itr->second.int_value()));
            }
            CharaList::chara_list.at(getPrimaryId())->chara_status_active.push_back(temp);
        }
    }else{
        CharaList::chara_list.at(getPrimaryId())->chara_status_active.clear();
    }
    if(data_chara_map["sta_pas"].is_null()==false){
        CharaList::chara_list.at(getPrimaryId())->chara_status_passive.clear();
        
        vector<Json> ski_vec = data_chara_map["sta_pas"].array_items();
        for(int i=0; i<ski_vec.size(); i++){
            map<string, Json> ski_map = ski_vec.at(i).object_items();
            map<string, int> temp;
            auto itr = ski_map.begin();
            for(itr=ski_map.begin(); itr!=ski_map.end(); itr++){
                temp.insert(make_pair(itr->first, itr->second.int_value()));
            }
            CharaList::chara_list.at(getPrimaryId())->chara_status_passive.push_back(temp);
        }
    }else{
        CharaList::chara_list.at(getPrimaryId())->chara_status_passive.clear();
    }
    
}

Chara* Chara::copy(){//このキャラのコピーを作る。初期化はしない。デッキやキャラリストに入れない。
    DungeonScene *dung = static_cast<DungeonScene *>(getParent()->getParent());
    Chara *chara = Chara::create();
    dung->cardLayer->addChild(chara);
    
    chara->setPosition(this->getPosition());
    chara->setCharaId(this->getCharaId());
    chara->setPlaceCost(this->place_cost);
    chara->setBaseValue(this->getBaseValue());
    chara->setCharaTurnCount(this->getCharaTurnCount());
    chara->setCharaTurnCountDef(this->getCharaTurnCountDef());
    chara->setType(this->getType());
    chara->setCharaName(this->getCharaName());
    chara->setCharaSecondName(this->getCharaSecondName());
    chara->setRare(this->getRare());
    chara->active_skill_able_count = this->active_skill_able_count;
    
    chara->place_type = this->place_type;
    chara->place_priority = this->place_priority;
    chara->skill_priority = this->skill_priority;
    
    //アクティブスキル持ちの場合
    int chara_active_skill_id = this->getActiveSkillId();
    if(chara_active_skill_id > 0){
        chara->setActiveSkillId(this->getActiveSkillId());
        chara->setActiveSkillName(this->getActiveSkillName());
        chara->setActiveSkillDetail(this->getActiveSkillDetail());
        chara->setActiveSkillTarget(this->getActiveSkillTarget());
        chara->setActiveSkillValue(this->getActiveSkillValue());
        chara->setActiveSkillTurn(this->getActiveSkillTurn());
        chara->setActiveSkillCost(this->getActiveSkillCost());
    }
    
    //パッシブスキル持ちの場合
    int chara_passive_skill_id = this->getPassiveSkillId();
    if(chara_passive_skill_id > 0){
        chara->setPassiveSkillId(this->getPassiveSkillId());
        chara->setPassiveSkillName(this->getPassiveSkillName());
        chara->setPassiveSkillDetail(this->getPassiveSkillDetail());
        chara->setPassiveSkillTarget(this->getPassiveSkillTarget());
        chara->setPassiveSkillValue(this->getPassiveSkillValue());
    }
    
    chara->setCombinationL(this->getCombinationL());
    chara->setCombinationR(this->getCombinationR());
    
    return chara;
}

bool Chara::isAttacker(){
    if(getType() == CHARA_TYPE_ATTACK || getType() == CHARA_TYPE_BALANCE){
        return true;
    }else{
        return false;
    }
}

bool Chara::isDefender(){
    if(getType() == CHARA_TYPE_DEFENSE || getType() == CHARA_TYPE_BALANCE){
        return true;
    }else{
        return false;
    }
}

bool Chara::isAttackAndDefense(){
    if(getType() == CHARA_TYPE_ATTACK || getType() == CHARA_TYPE_DEFENSE || getType() == CHARA_TYPE_BALANCE){
        return true;
    }else{
        return false;
    }
}

void Chara::valueUpSkill(int value, string type){
    
    if(statusCheck(STATUS_TYPE_INVALID_DECREASE) && value <= 0){//低下効果無効状態で低下が来てもスルー
        return;
    }
    
    
    
    if(type == "integer"){
        setValue(getValue() + value);//戦闘力固定値
    }else if(type == "percent"){
        setValue(getValue() + combo_value * value/100.0f);//戦闘力パーセント
    }else{
        CCLOG("！！！！！！！！！！！！！！！！！！！！！！！！！！！！エラー：戦闘力増減タイプがおかしいです");
        setValue(getValue() + combo_value * value/100.0f);//戦闘力パーセント
    }
}

void Chara::comboSpriteUpdate(){
    if(sprite_map["combination_l"]==nullptr || sprite_map["combination_l"]->getTag() != getCombinationL()){//画像がないか、画像が正しくない時
        
        if(sprite_map["combination_l"]!=nullptr && sprite_map["combination_l"]->getTag() != getCombinationL()){//まだ画像はあるがおかしい時
            removeChild(sprite_map["combination_l"]);//消す
            sprite_map["combination_l"]=nullptr;
        }
        
        if(getBelongingIndex() == TurnManage::getPlayerIndex() && getCombinationL()>0){//プレイヤー側の時
            string filename = "download/Chara/parts100/card_combo"+ to_string(getCombinationL()) + "_l.png";
            sprite_map["combination_l"] = Sprite::create(filename);
            sprite_map["combination_l"]->setScale(ratio);
            sprite_map["combination_l"]->setPosition(Point(0, 0));
            sprite_map["combination_l"]->setTag(getCombinationL());
            addChild(sprite_map["combination_l"], 2);
        }
        if(getBelongingIndex() == TurnManage::getEnemyIndex() && getCombinationR()>0){//敵側の時
            string filename = "download/Chara/parts100/card_combo"+ to_string(getCombinationR()) +"_l.png";
            sprite_map["combination_l"] = Sprite::create(filename);
            sprite_map["combination_l"]->setScale(ratio);
            sprite_map["combination_l"]->setPosition(Point(0, 0));
            sprite_map["combination_l"]->setTag(getCombinationR());
            addChild(sprite_map["combination_l"], 2);
        }
    }
    
    if(sprite_map["combination_r"]==nullptr || sprite_map["combination_r"]->getTag() != getCombinationR()){//画像がないか、画像が正しくない時
        
        if(sprite_map["combination_r"]!=nullptr && sprite_map["combination_r"]->getTag() != getCombinationR()){//まだ画像はあるがおかしい時
            removeChild(sprite_map["combination_r"]);//消す
            sprite_map["combination_r"]=nullptr;
        }
        
        if(getBelongingIndex() == TurnManage::getPlayerIndex() && getCombinationR()>0){//プレイヤー側の時
            string filename = "download/Chara/parts100/card_combo"+ to_string(getCombinationR()) + "_r.png";
            sprite_map["combination_r"] = Sprite::create(filename);
            sprite_map["combination_r"]->setScale(ratio);
            sprite_map["combination_r"]->setPosition(Point(0, 0));
            sprite_map["combination_r"]->setTag(getCombinationR());
            addChild(sprite_map["combination_r"], 2);
        }
        if(getBelongingIndex() == TurnManage::getEnemyIndex() && getCombinationL()>0){//敵側の時{//敵側の時
            string filename = "download/Chara/parts100/card_combo"+ to_string(getCombinationL()) +"_r.png";
            sprite_map["combination_r"] = Sprite::create(filename);
            sprite_map["combination_r"]->setScale(ratio);
            sprite_map["combination_r"]->setPosition(Point(0, 0));
            sprite_map["combination_r"]->setTag(getCombinationL());
            addChild(sprite_map["combination_r"], 2);
        }
    }
}


