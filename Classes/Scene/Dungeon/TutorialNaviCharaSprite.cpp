#include "TutorialNaviCharaSprite.h"

TutorialNaviCharaSprite* TutorialNaviCharaSprite::create()
{
    TutorialNaviCharaSprite *sprite = new TutorialNaviCharaSprite();
    
    if (sprite && sprite->init())
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool TutorialNaviCharaSprite::init()
{
    if (!Sprite::initWithFile("init/Tutorial/navi1.png"))
    {
        return false;
    }
    HMSLOG("TutorialNaviCharaSprite::init()");
    
    _message_center_position_y = 79;
    talk_state = TalkState::STATE_TALK_WAIT; //トーク待機状態にする
    
    return true;
}

Label *TutorialNaviCharaSprite::createMessageLabel(string message)
{
    TTFConfig ttfConfig(FONT_NAME, 25, GlyphCollection::DYNAMIC);
    TTFConfig ttfConfigMessage(ttfConfig);
    
    auto label = Label::createWithTTF(ttfConfigMessage, message);
    label->setDimensions(0,0) ;
    label->setTextColor(Color4B::WHITE);
    label->setPosition(Point(180, _message_center_position_y));
    label->setAnchorPoint(Point(0, 0.5)) ;
    
    return label;
}

//トーク待機状態にする
void TutorialNaviCharaSprite::talkWait(){
    stopActionByTag(TAG_STATE_CHANGE_ACTION) ;
    removeAllChildren() ;
    talk_state = TalkState::STATE_TALK_WAIT ;
}

//メッセージに\tを含めるとそこで文字の表示を若干止めてます
void TutorialNaviCharaSprite::talkStart(string message){
    if(talk_state == TalkState::STATE_TALK_WAIT){
        talk_state = TalkState::STATE_TALK_NOW ;
        talk_label = createMessageLabel(message) ;
        runTalkAction(talk_label, message.c_str()) ;
        this->addChild(talk_label) ;
    }
}

void TutorialNaviCharaSprite::forceAllTalk()
{
    stopActionByTag(TAG_STATE_CHANGE_ACTION) ;
    if(isTalkNow()){
        for(int i = 0; i < talk_label->getStringLength(); i++) {
            Sprite *charSprite = talk_label->getLetter(i);
            if(nullptr != charSprite) {
                charSprite->setVisible(true);
            }
        }
        talk_state = TalkState::STATE_TALK_END;
        showTriangle() ;
    }
}

bool TutorialNaviCharaSprite::isTalkWait(){
    return (talk_state == TalkState::STATE_TALK_WAIT);
}

bool TutorialNaviCharaSprite::isTalkNow(){
    return (talk_state == TalkState::STATE_TALK_NOW);
}

bool TutorialNaviCharaSprite::isTalkEnd(){
    return (talk_state == TalkState::STATE_TALK_END);
}

//しゃべるアクションを開始
void *TutorialNaviCharaSprite::runTalkAction(Label *label, const char message_str[])
{
    float talk_speed_standard= 0.08f ;
    float talk_speed_tab = 1.0f ;
    float delay_time = 0.0f ;

    for(int i = 0; i < label->getStringLength(); i++) {

		float now_speed;
        if(message_str[i] == '\t')now_speed = talk_speed_tab ;
        else now_speed = talk_speed_standard ;
        
        Sprite *charSprite = label->getLetter(i);
        if(nullptr != charSprite) {
            
            charSprite->setVisible(false);
            charSprite->runAction(Sequence::createWithTwoActions(DelayTime::create(delay_time), Show::create()));
            delay_time += now_speed ;

        }
    }
    
    
    
    //しゃべりが終わった後に状態を遷移する
    CallFunc *state_change_func= CallFunc::create([this]() {
                                    talk_state = TalkState::STATE_TALK_END ;
                                    showTriangle();
                                    }
                                  );//ラムダ式
    Sequence *state_change = Sequence::createWithTwoActions(DelayTime::create(delay_time), state_change_func) ;
    state_change->setTag(TAG_STATE_CHANGE_ACTION) ;
    runAction(state_change);
}

void TutorialNaviCharaSprite::showTriangle(){
    //ジャンプし続ける三角
    DrawNode *drawNode = getTriangleShape() ;
    drawNode->setPosition(Point(this->getContentSize().width - 20, 20)) ;
    RepeatForever *jump_forever = RepeatForever::create(JumpBy::create(2, Vec2(0,0), 30, 4)) ;
    drawNode->runAction(jump_forever) ;
    addChild(drawNode) ;
}

DrawNode* TutorialNaviCharaSprite::getTriangleShape()
{
    float triangle_width = 30;
    float triangle_height = 20;
    
    //下半分のテスト
    auto shape = DrawNode::create();
    static Vec2 triangle[3];

    triangle[0] = Vec2(0,0);
    triangle[1] = Vec2(triangle_width/2, triangle_height);
    triangle[2] = Vec2(-triangle_width/2, triangle_height);
    
    static Color4F green(0, 1, 0, 1);
    shape->drawPolygon(triangle, 4, Color4F::WHITE, 1, Color4F::BLACK);
    return shape;
}