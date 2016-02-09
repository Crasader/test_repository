#ifndef __War__QuestMenuLayer__
#define __War__QuestMenuLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "HmsScrollLayer.h"

#define DEGREE_BUTTON_TAG 10
#define REWARD_BUTTON_TAG 11
#define BATTLE_HELP_BUTTON_TAG 12
#define END_DATE_LABEL_TAG 13

#define DEGREE_URL HOST"user/?&p=page__battle_degree"    //称号一覧を開く用URL
#define REWARD_URL HOST"user/?&p=page__reward_list"      //報酬一覧を開く用URL
#define BATTLE_HELP_URL HOST"user/?&p=page__battle_help" //出陣ヘルプを開く用URL

USING_NS_CC;

class QuestMenuLayer : public Layer , public ScrollViewDelegate
{
private:
    
    //タグ
    enum Tag{
        T_Quest = 1,
        T_Online,
        T_Event,
        T_Tournament,
        T_Ranking,
//        T_Boss,
        T_Offline,
    };
    
    EventListenerTouchOneByOne *listener;
    HmsVector<Sprite *> _list_sp; //選択項目の画像
    
    //イベントのインデックス
    int _index_story_button;
    int _index_online_button;
    int _index_event_button;
    int _index_tournament_event_button;
    int _index_ranking_event_button;
//    int _index_boss_event_button;
    int _index_offline_button;
    
    string _challenger_exists_result; //クイック対戦に対戦相手が存在するかチェックした結果(いる:challenger_coming いない:challenger_nothing)
    string _rank_point_up_rate; //オンラインバトルの称号Ptの倍率。あがってる場合バナーの表示を変更する。
    
    HmsVector<string > _image_pass; //表示するリスト画像のパスを格納していく
    
public:
    QuestMenuLayer(); //コンストラクタ
    
    int _cell_size; //セルの大きさ
    
    float ratio;
    Json _event_list; //イベントのデータ
    bool _event_open_flg; //イベント開催フラグ

    virtual bool init();
    CREATE_FUNC(QuestMenuLayer);

    bool isTournamentEventOpen(); //トーナメントイベントが開催しているかどうかの判定
    bool isRankingEventOpen(); //ランキングイベントが開催しているかどうかの判定
//    bool isBossEventOpen(); //ボスイベントが開催しているかどうかの判定
    void createEventListener(); //イベントリスナーの作成
    void createListSprite(); //リスト画像の作成
    void createWebViewButton(); //報酬、称号、ヘルプボタンの作成
    
    void changePage(int tag); //ページの遷移
    void showWebPage(Ref* sender); //ウェブページの表示
    
    void initBattleUserSpriteData(); //デッキ選択画面で使用するデータを初期化
    
    //クイック対戦挑戦者チェックの通信
    void ArenaChallengerCheckRequest(); //クイック対戦に対戦相手が存在するかチェック。
    void ArenaChallengerCheckCallback(HttpClient *sender, HttpResponse *response);//クイック対戦に対戦相手が存在するかチェック(コールバック)
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    //    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
	
	//スプライトアニメーション
	Vector<SpriteFrame*> createSpriteFrame(string file_name);
	void setAnimateSpriteFrame(Sprite* _sprite, Vector<SpriteFrame*> sp_frames, float frame_time, float daly_time);
    
    void onEnter(); //初期実行関数
	
	//NEW表示
	void addNewImg(int number);
};

#endif /* defined(__War__QuestMenuLayer__) */
