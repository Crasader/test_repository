#include "TournamentEventBracketSprite.h"

TournamentEventBracketSprite::TournamentEventBracketSprite()
{
}

TournamentEventBracketSprite::~TournamentEventBracketSprite()
{
    HMSLOG("TournamentEventBracketSprite::~TournamentEventBracketSprite") ;
}

TournamentEventBracketSprite* TournamentEventBracketSprite::create(bool is_now_battle, Json json, Size contentSize)
{
    TournamentEventBracketSprite *sprite = new TournamentEventBracketSprite();
    if (sprite && sprite->init(is_now_battle, json, contentSize))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool TournamentEventBracketSprite::init(bool is_now_battle, Json json, Size contentSize)
{
    if (!Sprite::init())
    {
        return false;
    }
    
    float width = contentSize.width ;
    float height = contentSize.height - HEADER_SIZE ;
    _contentSize = contentSize ;
    _is_now_battle = is_now_battle;
    //ratio = getVisibleRatio();
    
    //データの設定
    setupData(json);
    
    //自身のサイズの指定
    this->setTextureRect(Rect(0,0,_bracket_draw_data[0].size()*_tile_size, _bracket_draw_data.size() * _tile_size)) ;
    
    //自身の色の設定
    this->setOpacity(0) ;
    
    
    return true;
}

void TournamentEventBracketSprite::onEnter()
{
    Sprite::onEnter();
    initBracketSpriteAllParts() ;
    createBracketList() ;
    createSprite() ;
    if(!_is_now_battle){
        //対戦表でタップ等で移動できる情報を取得
        _bracket_move_info = BracketMoveInfo::create() ;
        this->addChild(_bracket_move_info) ;
        //searchMoveNodeInfo() ;
        initTargetMark() ;
    }
    createAnimation() ;
}

void TournamentEventBracketSprite::initBracketSpriteAllParts(){
    for(int i=1; i<=7; i++){
        BracketPartsSpriteBatchNode *parts = BracketPartsSpriteBatchNode::create(i, false) ;
        _all_parts_normal.push_back(parts) ;
        this->addChild(parts, Z_BRACKET_PARTS) ;
    }
    
    for(int i=1; i<=7; i++){
        BracketPartsSpriteBatchNode *parts = BracketPartsSpriteBatchNode::create(i, true) ;
        _all_parts_red.push_back(parts) ;
        this->addChild(parts, Z_RED_BRACKET_PARTS) ;
    }
}

ActionInterval* TournamentEventBracketSprite::createVsAction(float duration){
    ScaleTo *scaleTo = ScaleTo::create(duration, 1) ;
    FadeTo *fadeTo = FadeTo::create(duration, 255) ;
    Spawn *spawn = Spawn::create(scaleTo, fadeTo, nullptr) ;
    EaseExponentialIn *ease_in = EaseExponentialIn::create(spawn) ;
    
    auto func = CallFunc::create([](){
        //SE
        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("init/SE/dora.mp3");
    });
    auto seq = Sequence::create(ease_in, func, nullptr);
    
    return seq ;
}

void TournamentEventBracketSprite::createBracketList(){
    
    //トーナメント一覧の作成
    int count = _bracket_draw_data.size() ;
    for (int i = 0; i < count ; i++)
    {
        vector<int> line = _bracket_draw_data[i] ;
        int line_size = line.size() ;
        
        //HMSLOG("line:%d sizeof:%d",i, line_size)  ;
        
        for(int j = 0 ; j < line_size ; j++){
            
            //マスクを保存するため定義
            int tile_type = line[j] ;
            if(tile_type>=1 && tile_type<=7){
                //対戦表画像の追加
                Sprite *bracket_sprite = Sprite::createWithTexture(_all_parts_normal[tile_type-1]->getTexture()) ;
                bracket_sprite->setPosition(Point(_tile_size*(j+0.5), _tile_size*(count-i-1+0.5)));
                _all_parts_normal[tile_type-1]->addChild(bracket_sprite,Z_BRACKET_PARTS);
            }
        }
    }
}

void TournamentEventBracketSprite::createSprite()
{
    //下端に表示するユーザー情報の表示
    for(map<int, int>::iterator iterator = _chara_id_by_user_master_id.begin(); iterator != _chara_id_by_user_master_id.end(); iterator++){
        HMSLOG("-------------------- user sprite create --------------------------") ;
        int user_master_id = iterator->first ;
        int chara_id = iterator->second ;
        HMSLOG("user_master_id:%d", user_master_id) ;
        HMSLOG("chara_id:%d", chara_id) ;
        
        BracketUserPositionData *bracketUserPositionData = _bracket_draw_data_generator->getUserPosition(user_master_id) ;
        if(bracketUserPositionData != nullptr){
            createBottomUserSprite(bracketUserPositionData) ;
        }
    }
    
    //上端に表示する決勝の文字
    Sprite *victory = Sprite::create("init/Home/Event/TournamentEvent/victory.png");
    victory->setAnchorPoint(Point(1, 0.5)) ;
    victory->setRotation(90) ;
    victory->setPosition(Point(this->getContentSize().width/2, this->getContentSize().height - _tile_size));
    this->addChild(victory, Z_VICTORY) ;
}

void TournamentEventBracketSprite::createBottomUserSprite(BracketUserPositionData *bracketUserPositionData)
{
    Point user_point = bracketUserPositionData->getDefaultUserPosition() ;
    int chara_id = bracketUserPositionData->getCharaId() ;
    int user_master_id = bracketUserPositionData->getUserMasterId() ;
    bool is_winner = _win_state_by_user_master_id[user_master_id] ;
    BracketUserPositionData::BottomDirection bottom_direction = bracketUserPositionData->getBottomDirection() ;
    int count = _bracket_draw_data.size() ;
    
    //キャラ画像の後ろの画像
    string chara_back_path;
    if(is_winner) chara_back_path = "init/Home/Event/TournamentEvent/bk_red.png" ;
    else chara_back_path = "init/Home/Event/TournamentEvent/bk_black.png" ;
    
    Sprite *chara_back = Sprite::create(chara_back_path) ;
    chara_back->setRotation(90) ;
    
    //x軸方向にずらす量
    float deviate_x_position = 0;
    if(bottom_direction == BracketUserPositionData::BottomDirection::LEFT) deviate_x_position = -0.4 ;
    else if(bottom_direction == BracketUserPositionData::BottomDirection::RIGHT) deviate_x_position = 0.4 ;
    
    //y軸方向のずらす量
    float deviate_y_position = 0.5;
    
    chara_back->setPosition(Point(_tile_size*(user_point.x+0.5+deviate_x_position), _tile_size*(count-user_point.y-1+0.5+deviate_y_position)));
    this->addChild(chara_back,Z_CHARA_BACK) ;
    
    //キャラ画像を追加
    string img_filename = "download/Chara/all100/" + toString<int>(chara_id) + ".png";
    
    Sprite *chara = Sprite::create(img_filename) ;
    chara->setPosition(chara_back->getContentSize()/2);
    chara_back->addChild(chara);
    
//    if(Utility::isTest()){
//        //test user_master_idを表示
//        auto test_user_master_id_label = HMSFontUtil::createWithTTFBySystemFontArgument(toString<int>(user_master_id), FONT_NAME_SYSTEM, 26);
//        test_user_master_id_label->setAnchorPoint(Point(0.5, 1)) ;
//        test_user_master_id_label->setPosition(Point(chara_back->getContentSize().width/2,-10+(user_master_id%10)*28));
//        chara_back->addChild(test_user_master_id_label);
//    }
    
    string user_name = _user_name_by_user_master_id[user_master_id] ;
    //Node *label_user_name = createVerticalFont(user_name) ;
    Label *label_user_name = HMSFontUtil::createWithTTFBySystemFontArgument(user_name, FONT_NAME_SYSTEM, 30);
    label_user_name->setAnchorPoint(Point(0, 0.5)) ;
    label_user_name->setHorizontalAlignment(TextHAlignment::LEFT);
    label_user_name->setPosition(Point(chara_back->getContentSize().width, chara_back->getContentSize().height/2)) ;
    chara_back->addChild(label_user_name);
    
    if(!is_winner){
        //敗者は暗くする
        Rect rect = Rect(0, 0, chara->getContentSize().width, chara->getContentSize().height);
        Sprite *chara_black = Sprite::create();
        chara_black->setTextureRect(rect) ;
        chara_black->setAnchorPoint(Point::ZERO) ;
        chara_black->setColor(Color3B::BLACK) ;
        chara_black->setOpacity(150) ;
        chara->addChild(chara_black);
    }
}

Node* TournamentEventBracketSprite::createVerticalFont(string text)
{
    Node *label_add_node = Node::create();
    //縦書きの際は横棒は縦になるので置換
    strReplace(text, "ー", "｜");
    strReplace(text, "-", "｜");
    //Label *user_name_label = HMSFontUtil::createWithTTFBySystemFontArgument(text, FONT_NAME_SYSTEM, 30);
    HmsVector<string> str_vec = splitByUtf8(text) ;
    for(int i = 0; i < str_vec.size(); i++) {
            Label *char_label = HMSFontUtil::createWithTTFBySystemFontArgument(str_vec[i], FONT_NAME_SYSTEM, 30);
            char_label->setHorizontalAlignment(TextHAlignment::CENTER);
            char_label->setVerticalAlignment(TextVAlignment::TOP);
            char_label->removeFromParent() ;
            char_label->setAnchorPoint(Point(0.5, 1));
            char_label->setPosition(Point(0, -i*30)) ;
            label_add_node->addChild(char_label) ;
    }
    
    return label_add_node ;
}

void TournamentEventBracketSprite::createAnimation()
{
    if(_is_now_battle){
        //バトル時のアニメーション
        for(int round_no=1; round_no<=_bracket_draw_data_generator->getTopRoundNo(); round_no++){
            //現在のバトル以外の全ての赤線を最初から表示
            createRedBracketAnimationByRoundNo(round_no, 0, 0) ;
        }
        //現在のバトルの赤線を2秒後から2秒間表示
        createRedBracketAnimationNowBattle(2, 2) ;
        
        //視点移動のアニメーションを再生
        createBattleViewPointMoveAnimation(2, 2) ;
        
        //ユーザーのスプライトを全て表示
        for(map<int, int>::iterator iterator = _chara_id_by_user_master_id.begin(); iterator != _chara_id_by_user_master_id.end(); iterator++){
            HMSLOG("-------------------- animation create --------------------------") ;
            int user_master_id = iterator->first ;
            int chara_id = iterator->second ;
            HMSLOG("user_master_id:%d", user_master_id) ;
            HMSLOG("chara_id:%d", chara_id) ;
            
            BracketUserPositionData *bracketUserPositionData = _bracket_draw_data_generator->getUserPosition(user_master_id) ;
            if(bracketUserPositionData != nullptr && !bracketUserPositionData->isDefaultPosition()){
                createUserSprite(bracketUserPositionData) ;
            }
        }
    }else{
        //バトル時じゃない時のアニメーション
        for(int round_no=1; round_no<=_bracket_draw_data_generator->getTopRoundNo(); round_no++){
            createRedBracketAnimationByRoundNo(round_no, round_no+1, 1) ;
        }
        
        //視点移動のアニメーションを再生
        createDisplayViewPointMoveAnimation() ;
        
        for(map<int, int>::iterator iterator = _chara_id_by_user_master_id.begin(); iterator != _chara_id_by_user_master_id.end(); iterator++){
            HMSLOG("-------------------- animation create --------------------------") ;
            int user_master_id = iterator->first ;
            int chara_id = iterator->second ;
            HMSLOG("user_master_id:%d", user_master_id) ;
            HMSLOG("chara_id:%d", chara_id) ;
            
            BracketUserPositionData *bracketUserPositionData = _bracket_draw_data_generator->getUserPosition(user_master_id) ;
            if(bracketUserPositionData != nullptr && !bracketUserPositionData->isDefaultPosition()){
                float anim_start_time = bracketUserPositionData->getRoundNo() + 2 ;
                createUserAnimation(bracketUserPositionData, anim_start_time) ;
            }
        }
        
    }
}

void TournamentEventBracketSprite::createUserSprite(BracketUserPositionData *bracketUserPositionData)
{
    Point user_point = bracketUserPositionData->getUserPosition() ;
    int chara_id = bracketUserPositionData->getCharaId() ;
    int user_master_id = bracketUserPositionData->getUserMasterId() ;
    bool is_winner = _win_state_by_user_master_id[user_master_id] ;
    
    string img_filename = "download/Chara/all100/" + toString<int>(chara_id) + ".png";
    
    //キャラ画像の後ろの画像
    string chara_back_path;
    if(is_winner) chara_back_path = "init/Home/Event/TournamentEvent/bk_red.png" ;
    else chara_back_path = "init/Home/Event/TournamentEvent/bk_black.png" ;
    
    Sprite *chara_back = Sprite::create(chara_back_path) ;
    chara_back->setRotation(90) ;
    chara_back->setCascadeOpacityEnabled(true) ;
    chara_back->setPosition(convertBracketIndexesToPosition(Point(user_point.x, user_point.y))) ;
    this->addChild(chara_back,Z_CHARA_BACK) ;
    
    Sprite *chara = Sprite::create(img_filename) ;
    chara->setPosition(chara_back->getContentSize()/2);
    chara_back->addChild(chara);
    
    if(_bracket_draw_data_generator->isNowBattleUserMaster(user_master_id)){
        chara_back->runAction(Sequence::createWithTwoActions(DelayTime::create(2), FadeOut::create(0.2))) ;
    }
    
    if(!is_winner){
        //敗者は暗くする
        Rect rect = Rect(0, 0, chara->getContentSize().width, chara->getContentSize().height);
        Sprite *chara_brack = Sprite::create();
        chara_brack->setTextureRect(rect) ;
        chara_brack->setAnchorPoint(Point::ZERO) ;
        chara_brack->setColor(Color3B::BLACK) ;
        chara_brack->setOpacity(150) ;
        chara->addChild(chara_brack);
    }
}

void TournamentEventBracketSprite::createUserAnimation(BracketUserPositionData *bracketUserPositionData, int anim_start_time)
{
    Point user_point = bracketUserPositionData->getUserPosition() ;
    int chara_id = bracketUserPositionData->getCharaId() ;
    int user_master_id = bracketUserPositionData->getUserMasterId() ;
    bool is_winner = _win_state_by_user_master_id[user_master_id] ;
    string img_filename = "download/Chara/all100/" + toString<int>(chara_id) + ".png";
    
    ScaleTo *scaleTo = ScaleTo::create(0.5, 1) ;
    EaseElasticOut *ease = EaseElasticOut::create(scaleTo) ;
    
    //キャラ画像の後ろの画像
    string chara_back_path;
    if(is_winner) chara_back_path = "init/Home/Event/TournamentEvent/bk_red.png" ;
    else chara_back_path = "init/Home/Event/TournamentEvent/bk_black.png" ;
    
    Sprite *chara_back = Sprite::create(chara_back_path) ;
    chara_back->setRotation(90) ;
    chara_back->setScale(0) ;
    chara_back->setPosition(convertBracketIndexesToPosition(Point(user_point.x, user_point.y))) ;
    chara_back->runAction(Sequence::createWithTwoActions(DelayTime::create(anim_start_time), ease)) ;
    this->addChild(chara_back,Z_CHARA_BACK) ;
    
    Sprite *chara = Sprite::create(img_filename) ;
    chara->setPosition(chara_back->getContentSize()/2);
    chara_back->addChild(chara);
    
    if(!is_winner){
        //敗者は暗くする
        FadeTo *fadeTo = FadeTo::create(1, 150);
        Rect rect = Rect(0, 0, chara->getContentSize().width, chara->getContentSize().height);
        Sprite *chara_brack = Sprite::create();
        chara_brack->setTextureRect(rect) ;
        chara_brack->setAnchorPoint(Point::ZERO) ;
        chara_brack->setColor(Color3B::BLACK) ;
        chara_brack->setOpacity(0) ;
        chara_brack->runAction(Sequence::createWithTwoActions(DelayTime::create(anim_start_time+0.5), fadeTo));
        chara->addChild(chara_brack);
    }
}

void TournamentEventBracketSprite::createRedBracketAnimationByRoundNo(int round_no, float anim_start_time, float anim_duration_time){
    vector<vector<Point>> battle_user_way = _bracket_draw_data_generator->getBattleUserWay(round_no);
    
    for(vector<vector<Point>>::iterator ite1 = battle_user_way.begin(); ite1 != battle_user_way.end(); ite1++){
        vector<Point> anim_root = *ite1 ;
        createRedBracketAnimation(anim_root, anim_start_time, anim_duration_time) ;
    }
}

void TournamentEventBracketSprite::createRedBracketAnimation(vector<Point> anim_root, float anim_start_time, float anim_duration_time){
    vector<vector<int>> red_bracket_draw_data = _bracket_draw_data_generator->getRedBracketDrawData();
    int count = red_bracket_draw_data.size() ;
    
    float anim_time = anim_duration_time / anim_root.size() ;
    float round_delay = 0 ;
    for(std::vector<Point>::iterator ite2 = anim_root.begin(); ite2 != anim_root.end(); ite2++){
        Point p = (*ite2) ;
        int tile_type = red_bracket_draw_data[p.y][p.x] ;
        
        if(tile_type>=1 && tile_type<=7){
            Sprite *bracket_sprite = Sprite::createWithTexture(_all_parts_red[tile_type-1]->getTexture()) ;
            bracket_sprite->setPosition(Point(convertBracketIndexesToPosition(Point(p.x, p.y)))) ;
            bracket_sprite->setOpacity(0) ;
            bracket_sprite->runAction(Sequence::createWithTwoActions(DelayTime::create(anim_start_time+round_delay), FadeIn::create(anim_time))) ;
            _all_parts_red[tile_type-1]->addChild(bracket_sprite,Z_RED_BRACKET_PARTS);
            round_delay += anim_time ;
        }
    }
}

/*
void TournamentEventBracketSprite::createDisplayViewPointMoveAnimation()
{
    string user_master_id = SaveData::loadString("user_master_id");
    BracketUserPositionData *bracketUserPositionData = _bracket_draw_data_generator->getUserPosition(fromString<int>(user_master_id)) ;
    
    if(bracketUserPositionData != nullptr){
        Point start_point = bracketUserPositionData->getUserPosition() ;
        
        //ユーザーが現在いる位置
        Point start_anchor_point = convertPositionToAnchorPoint(convertBracketIndexesToPosition(start_point)) ;
        this->setAnchorPoint(start_anchor_point) ;
    }
}
*/

void TournamentEventBracketSprite::createDisplayViewPointMoveAnimation()
{
    string user_master_id = SaveData::loadString("user_master_id");
    BracketUserPositionData *bracketUserPositionData = _bracket_draw_data_generator->getUserPosition(fromString<int>(user_master_id)) ;
    
    //初期位置に配置
    Point start_point = bracketUserPositionData->getDefaultUserPosition() ;
    Point start_anchor_point = convertPositionToAnchorPoint(convertBracketIndexesToPosition(start_point + Point(0, 6))) ;
    this->setAnchorPoint(start_anchor_point) ;
    
    
    //ユーザーの現在のラウンドnoからアニメーションの時間を決める(初期位置のときはアニメーションしない)
    if(!bracketUserPositionData->isDefaultPosition()){
        int round_no = bracketUserPositionData->getRoundNo() ;
        float anim_start_time = 2 ;
        float anim_duration_time = round_no ;
        if(bracketUserPositionData != nullptr){
            Point end_point = bracketUserPositionData->getUserPosition() ;
            Point end_anchor_point = convertPositionToAnchorPoint(convertBracketIndexesToPosition(end_point)) ;
            this->runAction(Sequence::create(DelayTime::create(anim_start_time), AnchorPointMoveTo::create(anim_duration_time, end_anchor_point),CallFunc::create(CC_CALLBACK_0(TournamentEventBracketSprite::searchMoveNodeInfo,this)),nullptr)) ;
        }
    }else{
        searchMoveNodeInfo() ;
    }
}

void TournamentEventBracketSprite::createBattleViewPointMoveAnimation(float anim_start_time, float anim_duration_time)
{
    string user_master_id = SaveData::loadString("user_master_id");
    BracketUserPositionData *bracketUserPositionData = _bracket_draw_data_generator->getUserPosition(fromString<int>(user_master_id)) ;
    
    if(bracketUserPositionData != nullptr){
        //アンカーポイントをユーザーのいる位置から、今回のバトルの分岐点に移動する
        Point start_point = bracketUserPositionData->getUserPosition() ;
        Point start_anchor_point = convertPositionToAnchorPoint(convertBracketIndexesToPosition(start_point)) ;
        
        Point end_point = _bracket_draw_data_generator->getVsCollisionPoint() ;
        Point end_anchor_point = convertPositionToAnchorPoint(convertBracketIndexesToPosition(end_point)) ;
        
        this->setAnchorPoint(start_anchor_point) ;
        this->runAction(Sequence::createWithTwoActions(DelayTime::create(anim_start_time), AnchorPointMoveTo::create(anim_duration_time, end_anchor_point))) ;
    }
}

void TournamentEventBracketSprite::createRedBracketAnimationNowBattle(float anim_start_time, float anim_duration_time)
{
    vector<Point> anim_root_left = _bracket_draw_data_generator->getNowBattleUserWay(0);
    int user_master_id_left =  _bracket_draw_data_generator->getNowBattleUserMaster(0);
    createRedBracketAnimationNowBattle(anim_root_left, anim_start_time, anim_duration_time, user_master_id_left) ;
    
    vector<Point> anim_root_right = _bracket_draw_data_generator->getNowBattleUserWay(1);
    int user_master_id_right =  _bracket_draw_data_generator->getNowBattleUserMaster(1);
    createRedBracketAnimationNowBattle(anim_root_right, anim_start_time, anim_duration_time, user_master_id_right) ;
}

void TournamentEventBracketSprite::createRedBracketAnimationNowBattle(vector<Point> anim_root, float anim_start_time, float anim_duration_time, int user_master_id){
    vector<vector<int>> red_bracket_draw_data = _bracket_draw_data_generator->getRedBracketDrawData();
    
    float anim_time = anim_duration_time / anim_root.size() ;
    float round_delay = 0 ;
    
    //キャラ画像の後ろの画像
    Sprite *chara_back = Sprite::create("init/Home/Event/TournamentEvent/bk_red.png") ;
    chara_back->setOpacity(0);
    chara_back->setCascadeOpacityEnabled(true) ;
    this->addChild(chara_back,Z_CHARA_BACK) ;
    
    //キャラ画像を追加
    int chara_id = _chara_id_by_user_master_id[user_master_id] ;
    if(chara_id==0) chara_id = 1001 ;
    string img_filename = "download/Chara/all100/" + toString<int>(chara_id) + ".png";
    Sprite *user_sprite = Sprite::create(img_filename) ;
    user_sprite->setRotation(90) ;
    user_sprite->setPosition(chara_back->getContentSize()/2) ;
    chara_back->addChild(user_sprite) ;
    
    
    for(std::vector<Point>::iterator ite2 = anim_root.begin(); ite2 != anim_root.end(); ite2++){
        Point p = (*ite2) ;
        HMSLOG("(x,y) = %f, %f", p.x, p.y) ;
        int tile_type = red_bracket_draw_data[p.y][p.x] ;
        
        
        if(tile_type>=1 && tile_type<=7){
            Sprite *bracket_sprite = Sprite::createWithTexture(_all_parts_red[tile_type-1]->getTexture()) ;
            bracket_sprite->setPosition(Point(convertBracketIndexesToPosition(Point(p.x, p.y)))) ;
            bracket_sprite->setOpacity(0) ;
            bracket_sprite->runAction(Sequence::createWithTwoActions(DelayTime::create(anim_start_time+round_delay), FadeIn::create(anim_time))) ;
            _all_parts_red[tile_type-1]->addChild(bracket_sprite,Z_RED_BRACKET_PARTS);
            round_delay += anim_time ;
        }
        
        //最後から5番目のマスの地点でvsアニメーションを出す
        if(distance(ite2, anim_root.end()) == 5){
            if(getChildByTag(T_VS) == nullptr) {
                Sprite *vs = Sprite::create("init/Home/Event/TournamentEvent/vs.png");
                vs->setPosition(convertBracketIndexesToPosition(_bracket_draw_data_generator->getVsCollisionPoint()));
                vs->setRotation(90) ;
                vs->setOpacity(0) ;
                vs->setScale(3) ;
                ActionInterval*action = createVsAction(anim_time*3) ;
                vs->runAction(Sequence::createWithTwoActions(DelayTime::create(anim_start_time+round_delay), action));
                this->addChild(vs,Z_VS,T_VS) ;
            }
        }
        
        //5マス目以降マスの位置にキャラを追従させる
        if(distance(anim_root.begin(), ite2) >= 3 && distance(ite2, anim_root.end()) >= 4){
            chara_back->runAction(Sequence::createWithTwoActions(DelayTime::create(anim_start_time+round_delay), MoveTo::create(anim_time,Point(convertBracketIndexesToPosition(Point(p.x, p.y)))))) ;
        }
        
        //6マス目にキャラをフェードインさせる
        if(distance(anim_root.begin(), ite2) == 4 && distance(ite2, anim_root.end()) >= 4){
            chara_back->runAction(Sequence::createWithTwoActions(DelayTime::create(anim_start_time+round_delay), FadeIn::create(anim_time*3))) ;
        }
    }
    
}

void TournamentEventBracketSprite::setupData(json11::Json json)
{
    //データの設定
    createBracketDrawData(json["tournament_event_bracket_node"].object_items(), json["user_info"].object_items()) ;
}

void TournamentEventBracketSprite::createBracketDrawData(json11::Json tournament_event_bracket_node_list, json11::Json user_info){
    
    BracketNodeDataList *bracket_node_data_list = BracketNodeDataList::create(tournament_event_bracket_node_list);
    this->addChild(bracket_node_data_list) ; //独自クラスをcocosシステムのメモリの管理下に置き、適切なタイミングでdeleteさせるためにaddChild
    userInfoJsonToMap(user_info) ;
    
//    if(Utility::isTest()){
//        //test出力
//        bracket_node_data_list->echoDataList() ;
//    }
    
    _bracket_draw_data_generator = BracketDrawDataGenerator::create(bracket_node_data_list, _chara_id_by_user_master_id) ;
    this->addChild(_bracket_draw_data_generator) ; //独自クラスをcocosシステムのメモリの管理下に置き、適切なタイミングでdeleteさせるためにaddChild
    _bracket_draw_data = _bracket_draw_data_generator->getBracketDrawData() ;
    
}


void TournamentEventBracketSprite::userInfoJsonToMap(json11::Json user_info){
    int count = countJsonData(user_info) ;
    for(int i=0; i<count; i++){
        Json user_image = user_info[toString<int>(i)].object_items() ;
        int user_master_id = user_image["user_master_id"].int_value() ;
        int card_id = user_image["friend_card_id"].int_value() ;
        bool win = user_image["win"].bool_value() ;
        string user_name = user_image["user_name"].string_value() ;
        
        if(card_id == 0)card_id = 1001 ; //0のときの仮画像
        _chara_id_by_user_master_id.insert(std::make_pair(user_master_id, card_id));
        _win_state_by_user_master_id.insert(std::make_pair(user_master_id, win));
        _user_name_by_user_master_id.insert(std::make_pair(user_master_id, user_name)) ;
    }
    
}

Point TournamentEventBracketSprite::convertPositionToAnchorPoint(Point point){
    return Point(point.x/getContentSize().width, point.y/getContentSize().height) ;
}

Point TournamentEventBracketSprite::convertAnchorPointToPosition(Point point){
    return Point(point.x*getContentSize().width, point.y*getContentSize().height) ;
}

Point TournamentEventBracketSprite::convertBracketIndexesToPosition(Point point){
    return Point((0.5+point.x) * _tile_size, ( _bracket_draw_data.size()-0.5-point.y) * _tile_size) ;
}

Point TournamentEventBracketSprite::convertPositionToBracketIndexes(Point point){
    return Point(point.x/_tile_size - 0.5, _bracket_draw_data.size()-0.5-point.y/_tile_size) ;
}

void TournamentEventBracketSprite::initTargetMark()
{
    Sprite *target_mark = Sprite::create("download/Dungeon/target.png");
    target_mark->setOpacity(0) ;
    this->addChild(target_mark, Z_TARGET_MARK, T_TARGET_MARK);
}

void TournamentEventBracketSprite::actionTargetMark(Point target_point)
{
    Sprite *target_mark = (Sprite *)this->getChildByTag(T_TARGET_MARK) ;
    target_mark->stopAllActions();
    target_mark->setPosition(convertBracketIndexesToPosition(target_point)) ;
    auto small = ScaleTo::create(0.3f, 3.0/4.0);
    auto big = ScaleTo::create(0.3f, 1);
    auto seq = Sequence::create(small, big, nullptr);
//    auto rotate = RotateBy::create(0.6f, 60);
    auto fade_in_out = Sequence::create(FadeIn::create(0.f), DelayTime::create(0.4), FadeOut::create(0.1f), nullptr) ;
    auto spa = Spawn::create(seq, fade_in_out, nullptr);
    target_mark->runAction(spa);
}

void TournamentEventBracketSprite::searchMoveNodeInfo() {
    
    HmsVector<Point> node_place ;//ノードの位置を全て格納
    HmsVector<Point> virtual_node_place ; //シードでは分岐点がないがあるものとあつかう。
    
    int count_lines = _bracket_draw_data.size() ;
    for (int i = 0; i < count_lines ; i++)
    {
        vector<int> line = _bracket_draw_data[i] ;
        int line_size = line.size() ;
        
        for(int j = 0 ; j < line_size ; j++){
            int tile_type = line[j] ;
            
            if(tile_type==9 || tile_type==3){
                node_place.push_back(Point(j, i)) ;
            }
            
            //2round目の位置にはノードが無い場合でもそこにノードがあるものとする
            if(i == count_lines-4 && tile_type==1){
                node_place.push_back(Point(j, i)) ;
                virtual_node_place.push_back(Point(j,i));
            }
            
            //下端のタイルが1の時、そのすぐ下がユーザーのスプライトのposition
            if(i == count_lines-1 && tile_type==1){
                node_place.push_back(Point(j, i+5)) ; // ユーザーが見えやすいようにy軸方向に+5
            }
        }
    }
    
    
    //現在の自分の位置を求める、ノードの上にいかなったから最も近いノードを現在地とする
    bool near_place_flg = false ;
    Point now_place = convertPositionToBracketIndexes(convertAnchorPointToPosition(this->getAnchorPoint()));
    Point near_place = Point(1000,1000); //最も近い距離を求めるために仮に遠くのポイントを初期値にして、近い距離で書き換えていく
    int count_nodes = node_place.size() ;
    for(int i=0; i<count_nodes; i++){
        if(now_place.distance(near_place) >= now_place.distance(node_place[i])){
            near_place = node_place[i] ;
            near_place_flg = true ;
        }
    }
    
    if(near_place_flg){
        now_place = near_place;
    }
    
    
    //一つ下の階層があるか調べ、あれば距離の差を求める
    float near_blew_y = 1000; //最も近い距離を求めるために仮に遠くのポイントを初期値にして、近い距離で書き換えていく
    bool near_below_flg = false ;
    
    for(int i=0; i<count_nodes; i++){
        Point compare_place = node_place[i] ;
        if(compare_place.y > now_place.y && compare_place.y < near_blew_y){
            near_blew_y = compare_place.y;
            near_below_flg = true ;
        }
    }
    
    bool left_node_find = false ; //左のノードを発見
    bool right_node_find = false ; //右のノードを発見
    bool lower_right_node_find = false ; ///右下のノードを発見
    bool lower_left_node_find = false ; //左下のノードを発見
    bool up_node_find = false ; //上のノードを発見
    
    Point near_left = Point(now_place.x-1000, now_place.y);
    Point near_right = Point(now_place.x+1000, now_place.y);
    Point near_lower_right = Point(now_place.x+1000, now_place.y+1000);
    Point near_lower_left = Point(now_place.x-1000, now_place.y+1000);
    Point near_up = Point(now_place.x, now_place.y-1000);
    
    //各方向で一番近いノードを探索
    for(int i=0; i<count_nodes; i++){
        Point compare_place = node_place[i] ;
        if(compare_place.y == now_place.y){
            //縦軸同値の比較
            if(compare_place.x<now_place.x && near_left.x<compare_place.x){
                //比較ポイントが現在のポイントよりも左側にあり、 かつ現在最も近い値を記録しているnear_leftよりも距離的に近かった場合
                left_node_find = true ;
                near_left = compare_place ;
            }
            if(compare_place.x>now_place.x && near_right.x>compare_place.x){
                right_node_find = true ;
                near_right = compare_place ;
            }
        }else if(compare_place.y == near_blew_y){
            //一つ下の階層を探索
            if(compare_place.x < now_place.x && near_lower_left.x < compare_place.x){
                //左下側で近いノードを探索
                lower_left_node_find = true ;
                near_lower_left = compare_place ;
            }else if(compare_place.x > now_place.x && near_lower_right.x > compare_place.x){
                //右下側で近いノードを探索
                lower_right_node_find = true ;
                near_lower_right = compare_place ;
            }
        }else if(compare_place.y < now_place.y){
            //縦軸方向に値が小さい場合の比較
            if(now_place.distance(near_up) > now_place.distance(compare_place)){
                up_node_find = true ;
                near_up = compare_place ;
            }
        }
    }
    
    //現在の場所が仮想的な分岐点である場合、次に右側に移動するときはキャラの場所に移動する
    int count = virtual_node_place.size();
    for(int i=0 ; i<count; i++){
        if(virtual_node_place[i] == now_place) {
            lower_left_node_find = true ;
            near_lower_left = virtual_node_place[i] + Point(0,3+5) ;
            lower_right_node_find = true ;
            near_lower_right = virtual_node_place[i] + Point(0,3+5) ;
        }
    }
    
    
    //移動に関する情報をBrakcetMoveInfoクラスに格納
    _bracket_move_info->setNearRight(near_right) ;
    _bracket_move_info->setNearLeft(near_left);
    _bracket_move_info->setNearLowerLeft(near_lower_left);
    _bracket_move_info->setNearLowerRight(near_lower_right);
    _bracket_move_info->setNearUp(near_up);

    _bracket_move_info->setRightNodeFind(right_node_find);
    _bracket_move_info->setLeftNodeFind(left_node_find);
    _bracket_move_info->setLowerLeftNodeFind(lower_left_node_find) ;
    _bracket_move_info->setLowerRightNodeFind(lower_right_node_find);
    _bracket_move_info->setUpNodeFind(up_node_find);
}

BracketMoveInfo* TournamentEventBracketSprite::getBracketMoveInfo(){
    return _bracket_move_info ;
}

void TournamentEventBracketSprite::moveNodeBySwipeDirection(SwipeDirection swipeDirection, const std::function<void()> end_callback){
    
    BracketMoveInfo::Distination distination;
    if(swipeDirection == SwipeDirection::UP) distination = BracketMoveInfo::Distination::Right ;
    else if(swipeDirection == SwipeDirection::BELOW) distination = BracketMoveInfo::Distination::Left ;
    else if(swipeDirection == SwipeDirection::UPPER_RIGHT) distination = BracketMoveInfo::Distination::LowerRight ;
    else if(swipeDirection == SwipeDirection::LOWER_RIGHT) distination = BracketMoveInfo::Distination::LowerLeft ;
    else if(swipeDirection == SwipeDirection::LEFT) distination = BracketMoveInfo::Distination::Up ;
    
    if(_bracket_move_info->hasValidDestination(distination)){
        //移動後、コールバックを呼び、その後、次に移動可能な場所を求める。
        Point move_anchor_point = convertPositionToAnchorPoint(convertBracketIndexesToPosition(_bracket_move_info->getValidDistination(distination)));
        this->runAction(Sequence::create(AnchorPointMoveTo::create(0.2, move_anchor_point), CallFunc::create(CC_CALLBACK_0(TournamentEventBracketSprite::searchMoveNodeInfo, this)), CallFunc::create(end_callback),  nullptr)) ;
        actionTargetMark(_bracket_move_info->getValidDistination(distination)) ;
    }else{
        HMSLOG("スワイプされた方向に対して有効な移動先がない") ;
        end_callback() ;
    }
}
