#ifndef __war__TournamentEventBracketSprite__
#define __war__TournamentEventBracketSprite__

#include "cocos2d.h"
//#include "NormalScene.h"
#include "HMSFontUtil.h"
#include "HmsGeometricFunction.h"
#include "HmsUtilityFunction.h"
#include "HmsTemplateFunction.h"
#include "json11.hpp"
#include "Utility.h"
#include "HmsActionInterval.h"
#include "BracketPartsSpriteBatchNode.h"
#include "BracketNodeDataList.h"
#include "BracketDrawDataGenerator.h"
#include "BracketMoveInfo.h"


using namespace json11;
USING_NS_CC;

class TournamentEventBracketSprite : public Sprite
{
private:
    //SpriteBatchNodeを継承したクラスでスプライトを表示する
    HmsVector<BracketPartsSpriteBatchNode*> _all_parts_normal  ;
    HmsVector<BracketPartsSpriteBatchNode*> _all_parts_red  ;
    
    //SpriteBatchNodeを初期化する
    void initBracketSpriteAllParts() ;
    
    void changeScene() ; //シーンの変更
    
    float _tile_size = 30 ;
    
    void createBracketDrawData(json11::Json tournament_event_bracket_node_list, json11::Json user_info);
    void userInfoJsonToMap(json11::Json user_info) ;
    
    void createBracketList() ;
    void createSprite() ;
    void createAnimation() ;
    
    void createBottomUserSprite(BracketUserPositionData *bracketUserPositionData ) ;
    void createUserSprite(BracketUserPositionData *bracketUserPositionData) ;
    void createUserAnimation(BracketUserPositionData *bracketUserPositionData, int anim_start_time) ;
    
    void createRedBracketAnimationByRoundNo(int round_no, float anim_start_time, float anim_duration_time) ;
    void createRedBracketAnimation(vector<Point> anim_root, float anim_start_time, float anim_duration_time) ;
    
    void createRedBracketAnimationNowBattle(float anim_start_time, float anim_duration_time) ;
    void createRedBracketAnimationNowBattle(vector<Point> anim_root, float anim_start_time, float anim_duration_time, int user_master_id) ;
    
    //カメラ視点移動アニメーション(バトル時)
    void createBattleViewPointMoveAnimation(float anim_start_time, float anim_duration_time) ;
    
    //カメラ視点移動アニメーション(閲覧時)
    void createDisplayViewPointMoveAnimation() ;
    
    Size _contentSize ;
    
    Point _top ;
    
    int desideTileTypeByPosition(Point prev, Point now, Point next) ; //一個前の座標、現在の座標、次の座標からtile_typeを求める
    
    //タグ
    enum Tag{
        T_VS,
        T_TARGET_MARK,
    };
    
    //ZOrder
    enum ZOrder{
        Z_BRACKET_PARTS,
        Z_RED_BRACKET_PARTS,
        Z_CHARA_BACK,
        Z_VS,
        Z_VICTORY,
        Z_TARGET_MARK,
    };
    
    float ratio;
    
    bool _is_now_battle ;
    
    //トーナメント名を保存
    string _tournament_name ;
    
    //ブラケットの情報を作成するクラス
    BracketDrawDataGenerator *_bracket_draw_data_generator ;
    //作成されたデータ
    vector<vector<int>> _bracket_draw_data ;
    
    //ユーザーのデータ
    map<int, int> _chara_id_by_user_master_id ; //<user_master_id, chara_id> ;
    map<int, bool> _win_state_by_user_master_id ; //<user_master_id, is_win> ;
    map<int, string> _user_name_by_user_master_id ; //<user_master_id, user_name> ;
    
    vector<vector<BracketPartsSpriteBatchNode*>> _masked_bracket_parts ;
    
    ActionInterval* createVsAction(float duration) ;
    
    Node *createVerticalFont(string text) ; //縦書きの文字を作成
    
public:
    
    Point convertBracketIndexesToPosition(Point point) ; //独自に定義しているポイントをスプライト上の絶対座標に変換する
    Point convertPositionToBracketIndexes(Point point) ; //スプライト上の絶対座標を独自に定義しているポイントに変更する
    Point convertPositionToAnchorPoint(Point point) ; //スプライト上のPositionをAnchorPointに変換する
    Point convertAnchorPointToPosition(Point point) ; //スプライト上のAnchorPointをPositionに変換する
    
    TournamentEventBracketSprite(); //コンストラクタ
    ~TournamentEventBracketSprite() ; //デストラクタ
    
    static TournamentEventBracketSprite* create(bool is_now_battle, Json json, Size contentSize);
    virtual bool init(bool is_now_battle, Json json, Size contentSize);
    
    void setupData(Json json); //データの設定
    
    void onEnter(); //初期実行関数
    
    enum SwipeDirection{
        UPPER_RIGHT, //右上
        UP, //上
        LEFT, //左
        BELOW, //下
        LOWER_RIGHT, //右下
    };
    
    
    //ターゲットのスプライトの生成
    void initTargetMark() ;
    Sprite *_target_mark ;
    void actionTargetMark(Point target_point) ;
    
    //移動可能な位置を探索
    void searchMoveNodeInfo() ;
    
    //移動出来る場所情報
    BracketMoveInfo *_bracket_move_info ;
    BracketMoveInfo* getBracketMoveInfo() ;
    
    //移動する
    void moveNodeBySwipeDirection(SwipeDirection swipeDirection, const std::function<void()> end_callback) ;
};

#endif /* defined(__war__TournamentEventBracketSprite__) */
