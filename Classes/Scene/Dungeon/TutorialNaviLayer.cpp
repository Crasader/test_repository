#include "TutorialNaviLayer.h"
#include "DungeonScene.h"
#include "TutorialTipLayer.h"

#define TUTORIAL_COMMENT_KEY "tutorial_comment_index"
#define TUTORIAL_COMMENT_MAX 20
bool TutorialNaviLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("TutorialNaviLayer::init()");
    
    ratio = getVisibleRatio();
    parent = nullptr;
    sel = nullptr;
    
    return true;
}

void TutorialNaviLayer::onEnter(){
    Layer::onEnter() ;
    initalize() ;
    initListener() ;
}

void TutorialNaviLayer::initListener(){
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(TutorialNaviLayer::onTouchBegan, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

//戻り値がfalseで他レイヤーのタップ可能
bool TutorialNaviLayer::onTouchBegan(Touch *touch, Event *event){
	
    if(navi_chara_sprite->isTalkWait()){
        if(CommentIndex::COMMENT_END!=_index){
            //コメントが終了していない場合ダンジョンのタップを不可能にする
            return true;
        }else{
            return false;
        }
    }else if(navi_chara_sprite->isTalkNow()){
        //トーク中にタップした場合全てのコメントを強制的に表示する
        navi_chara_sprite->forceAllTalk() ;
        return true ;
    }else if(navi_chara_sprite->isTalkEnd()){
        string str = to_string((int)_index);
        if(str.length()==1) str = "0" + str;
        Sprite *img = Sprite::create("download/Tutorial/" + str + ".png");
        if(img==nullptr || img->getContentSize().width!=480) {//画像表示がない場合はそのまま消える
            HMSLOG("not found");
            
            callback();
            
        } else {//表示画像がある場合は、該当の画像をだす
            
            //tips未表示の場合 tipsを表示
            if(visited_tips==false){
                visited_tips = true ;
                Size size = img->getContentSize();
                
                TutorialTipLayer *tip_layer = TutorialTipLayer::create();
                this->addChild(tip_layer, 100);
                tip_layer->setup((int)_index);
            }
        }

        return true ;
    }
    
    return false;
}

void TutorialNaviLayer::callback()
{

    if(_index==CommentIndex::COMMENT_5 || _index==CommentIndex::COMMENT_7){
        //COMMENT_5に関しては連続してコメントを表示する
        navi_chara_sprite->talkWait() ;
        setCommentComplete(_index, true);
        CommentIndex new_index = (CommentIndex)((int)_index+1) ;
        _index = new_index ;
        string message = createMessageByCommentIndex(_index) ;
        naviCharaTalkStart(message) ;
    }else{
        CallFunc *func = CallFunc::create([this]() {
            //トーク終了時にタップすると登録したselectorに登録されているメソッドを呼ぶ
            if(parent!=nullptr && sel!=nullptr) {
                if(CommentIndex::COMMENT_END != _index)parent->scheduleOnce(sel, 0.0f);
                sel = nullptr;
            }
            navi_chara_sprite->talkWait() ;
            setCommentComplete((CommentIndex)_index, true);
            _index = CommentIndex::COMMENT_END;
        });
        
        ActionInterval *move = getUpAction() ;
        navi_chara_sprite->runAction(Sequence::create(move, func, nullptr)) ;
    }
}

void TutorialNaviLayer::removeListener()
{
    //タッチイベントOFF
    _eventDispatcher->removeEventListener(listener);
}

void TutorialNaviLayer::setup(Layer *parent, CommentIndex index)
{
    //tips表示済みフラグfalse
    visited_tips = false ;
    
    _index = index;
    
    string message = "";

    bool finish_flg = TutorialNaviLayer::checkCommentComplete(index);
    if(finish_flg==true) {
        close();
        return;
    } else {
        message = createMessageByCommentIndex(_index) ;
    }
    
    //ナビキャラをしゃべらせるアクション
    CallFunc *comment_func = CallFunc::create([this, message]() { this->naviCharaTalkStart(message) ; });//ラムダ式
    //ナビキャラを上から出現させるアクションの取得
    ActionInterval *move = getDownAction() ;
    //キャラを出現させて喋らせる
    if(navi_chara_sprite!=nullptr)
        navi_chara_sprite->runAction(Sequence::create(move, comment_func, nullptr)) ;
    //SE
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/navi_open.mp3",false);
}

string TutorialNaviLayer::createMessageByCommentIndex(CommentIndex index) {
    
    string message ;
    
    if(index==CommentIndex::COMMENT_1) message = "さあ、早速ゲーム開始よ！\n今回は私のデッキを使ってみてね♪";
    if(index==CommentIndex::COMMENT_2) message = "まずはアナタのターンね！\n場にキャラを出してみて♪";
    if(index==CommentIndex::COMMENT_3) message = "その調子よ！\nキャラを出すには士気が必要なの！\n続けてもう1枚出してみて♪";
    if(index==CommentIndex::COMMENT_4) message = "これで準備ができたわ！\n待機ボタンを押してね♪";
    if(index==CommentIndex::COMMENT_5) message = "場に出したキャラのうち\n「攻撃キャラ」が攻撃してくれるわ♪";
    if(index==CommentIndex::COMMENT_6) message = "でも、気をつけて！\n【先攻1ターン目】はキャラを\n置いても攻撃が出来ないわ♪";
    if(index==CommentIndex::COMMENT_7) message = "【後攻1ターン目】の士気は\n通常の値に『+2』されるの！\nだから、相手の士気は「8」なのよ♪";
	if(index==CommentIndex::COMMENT_8) message = "！！！\n敵の攻撃が来るわ！";
    if(index==CommentIndex::COMMENT_9) message = "でも、大丈夫！防御キャラが\n身を挺して防いでくれたわ♪";
    if(index==CommentIndex::COMMENT_10) message = "さあ、反撃開始よ！\n次のキャラを出してね♪";
    if(index==CommentIndex::COMMENT_11) message = "今のは軍師キャラね！軍師は場全体に\n効果を与えることができるのよ。\nこれで味方全体の攻撃力がupしたわ♪";
    if(index==CommentIndex::COMMENT_12) message = "パワーアップした攻撃を\nお見舞いしましょ！\n攻撃ボタンを押してみて♪";
    if(index==CommentIndex::COMMENT_13) message = "うん！うん！いい感じ！\n攻撃を重ねて\n相手の総大将を倒したら勝利よ♪";
    if(index==CommentIndex::COMMENT_14) message = "キャッ！危ない！\n防御キャラが無いから\n攻撃を受けてしまったわ！！";
    if(index==CommentIndex::COMMENT_15) message = "プンプンッ！\nか弱い乙女を傷つけるなんて最低！\nスキルを使いトドメをさしちゃって♪";
    
    return message ;
}

void TutorialNaviLayer::initalize()
{
    //ナビキャラが降りてくる位置
    navi_chara_down_position = convertFromDisplaySize(Point(320, 960), 0.5, 1.0) ;
    
    //キャラ画像作成
    navi_chara_sprite = TutorialNaviCharaSprite::create() ;
    
    //キャラ画像の初期位置設定
    navi_chara_up_position = convertFromDisplaySize(Point(320, 960 + navi_chara_sprite->getContentSize().width), 0.5, 1.0) ;
    
    //ナビキャラを初期位置に設定
    navi_chara_sprite->setScale(ratio) ;
    navi_chara_sprite->setAnchorPoint(Point(0.5,1)) ;
    navi_chara_sprite->setPosition(navi_chara_up_position) ;
    
    this->addChild(navi_chara_sprite) ;
}

void TutorialNaviLayer::setScheduler(Layer *parent, SEL_SCHEDULE sel)
{
    this->parent = parent;
    this->sel = sel;
}

void TutorialNaviLayer::naviCharaTalkStart(string comment)
{
    HMSLOG("ナビキャラが喋る") ;
    navi_chara_sprite->talkStart(comment) ;
}

ActionInterval *TutorialNaviLayer::getDownAction()
{
    DelayTime *delay1 = DelayTime::create(0.5) ;
    DelayTime *delay2 = DelayTime::create(0.5) ;
    ActionInterval *move = MoveTo::create(0.2f, navi_chara_down_position);
    Sequence *sequence = Sequence::create(delay1, move, delay2, nullptr);
    return sequence;
}

ActionInterval *TutorialNaviLayer::getUpAction()
{
    ActionInterval *move = MoveTo::create(0.2f, navi_chara_up_position);
    return move;
}

void TutorialNaviLayer::close()
{
    this->removeFromParentAndCleanup(true);
}

void TutorialNaviLayer::initCompleteData()
{
    for(int i=1; i<=TUTORIAL_COMMENT_MAX; i++) {
        TutorialNaviLayer::setCommentComplete((CommentIndex)i, false);
    }
}

bool TutorialNaviLayer::checkCommentComplete(CommentIndex index)
{
    string key_name = TUTORIAL_COMMENT_KEY + toString((int)index);
    bool finish_flg = SaveData::loadInt(key_name.c_str());
    return finish_flg;
}

void TutorialNaviLayer::setCommentComplete(CommentIndex index, bool finish_flg)
{
    string key_name = TUTORIAL_COMMENT_KEY + toString((int)index);
    SaveData::saveInt(key_name.c_str(), finish_flg);
}

bool TutorialNaviLayer::checkCommentAllComplete(){
    
    bool check = true ;
    
    //TIPSが全て完了済みか調べる
    check &= checkCommentComplete(CommentIndex::COMMENT_1) ;
    check &= checkCommentComplete(CommentIndex::COMMENT_2) ;
    check &= checkCommentComplete(CommentIndex::COMMENT_3) ;
    check &= checkCommentComplete(CommentIndex::COMMENT_4) ;
    check &= checkCommentComplete(CommentIndex::COMMENT_5) ;
    check &= checkCommentComplete(CommentIndex::COMMENT_6) ;
    check &= checkCommentComplete(CommentIndex::COMMENT_7) ;
    check &= checkCommentComplete(CommentIndex::COMMENT_8) ;
    check &= checkCommentComplete(CommentIndex::COMMENT_9) ;
    check &= checkCommentComplete(CommentIndex::COMMENT_10) ;
    check &= checkCommentComplete(CommentIndex::COMMENT_11) ;
    check &= checkCommentComplete(CommentIndex::COMMENT_12) ;
    check &= checkCommentComplete(CommentIndex::COMMENT_13) ;
    check &= checkCommentComplete(CommentIndex::COMMENT_14) ;
    check &= checkCommentComplete(CommentIndex::COMMENT_15) ;
    
    return check ;
    
}
