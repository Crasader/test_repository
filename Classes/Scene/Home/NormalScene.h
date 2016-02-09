#ifndef __War__NormalScene__
#define __War__NormalScene__

#include "cocos2d.h"
#include "HomeCommon.h"
#include "HMSFontUtil.h"
#include "HmsConsoleUtil.h"
#include "HmsGeometricFunction.h"
#include "HttpConnect.h"
#include "json11.hpp"
#include <time.h>
#include "XTLayer.h"
#include "UIAsyncImageSprite.h"
#include "TitleBarLayer.h"
#include "TapEffectLayer.h"
#include "HeaderLayer.h"
#include "FooterLayer.h"
#include "MypageLayer.h"
#include "GachaMenuLayer.h"
#include "RevisionGachaMenuLayer.h"
#include "BoxGachaLayer.h"
#include "GachaConfirmationLayer.h"
#include "GachaResultLayer.h"
#include "ShopMenuLayer.h"
#include "StoneShopMenuLayer.h"
#include "QuestMenuLayer.h"
#include "SubMenuLayer.h"
#include "OtherLayer.h"
#include "PresentMenuLayer.h"
#include "NormalQuestLayer.h"
#include "NormalQuestStageLayer.h"
#include "EventMenuLayer.h"
#include "EventRankMenuLayer.h"
#include "RankingEventRankMenuLayer.h"
#include "RankingEventTopLayer.h"
#include "BossEventRankMenuLayer.h"
#include "BossEventTopLayer.h"
#include "ChallengeEventRankMenuLayer.h"
#include "ChallengeEventMenuLayer.h"
#include "OfflineEnemyListLayer.h"
#include "DungeonScene.h"
#include "FriendMenuLayer.h"
#include "SaveData.h"
#include "cocos-ext.h"
#include "ModalLayer.h"
#include "FriendSearchLayer.h"
#include "FriendIntroductionLayer.h"
#include "OptionsLayer.h"
#include "ModelchangeLayer.h"
#include "QuestionLayer.h"
#include "HelpLayer.h"
#include "ModelchangecodeLayer.h"
#include "popupAlertLayer.h"
#include "NicknameLayer.h"
#include "SoundLayer.h"
#include "FollowListLayer.h"
#include "ProfileLayer.h"
#include "CommentLayer.h"
#include "HmsUtilityFunction.h"
#include "FollowListLayer.h"
#include "UserDetailLayer.h"
#include "PopupStayLayer.h"
#include "PopupStayButtonLayer.h"
#include "PopupNoChangePageLayer.h"
#include "PopupChangePageLayer.h"
#include "PopupChangePageLayer.h"
#include "AllTapDisableLayer.h"
#include "BattleTopLayer.h"
#include "BattleFreeLayer.h"
#include "BattleFriendChoiceLayer.h"
#include "BattleDeckChoiceLayer.h"
#include "AnzSound.h"
#include "BattleRoomTopLayer.h"
#include "CreateRoomLayer.h"
#include "SearchRoomLayer.h"
#include "ReplayListLayer.h"
#include "CharaMenuLayer.h"
#include "CharaListLayer.h"
#include "CharaSellListLayer.h"
#include "CharaExchangeLayer.h"
#include "CharaEvolutionListLayer.h"
#include "CharaEvolutionConfirmLayer.h"
#include "DeckFormationTopLayer.h"
#include "GeneralFormationLayer.h"
#include "DeckFormationLayer.h"
#include "TournamentEventTopLayer.h"
#include "TournamentListLayer.h"
#include "TournamentEntryListLayer.h"
#include "TournamentEventDeckChoiceLayer.h"
#include "TournamentEventConfirmLayer.h"
#include "TournamentEventBattleWaitLayer.h"
#include "TournamentEventBracketLayer.h"
#include "TournamentRecordLayer.h"
#include "TournamentTermSprite.hpp"
#include "TeamTopLayer.h"
#include "TeamNoBelongLayer.h"
#include "TeamSearchTeamLayer.h"
#include "TeamListLayer.h"
#include "TeamCreateTeamLayer.h"
#include "TeamManagementBaseLayer.h"
#include "TeamMemberListLayer.h"
#include "TeamMemberDetailLayer.h"
#include "TeamInformationChangeLayer.h"
#include "TeamInvitationLayer.h"
#include "BazaarTopLayer.h"
#include "BazaarSearchListLayer.h"
#include "BazaarExhibitTopLayer.h"
#include "BazaarExhibitCharaListLayer.h"
#include "BazaarExhibitConfirmLayer.h"
#include "TeamSearchListLayer.h"
#include "TeamApplicationListLayer.h"
#include "TeamInvitedListLayer.h"
#include "TeamBattleEventLayer.h"
#include "TeamBattleEventEntryListLayer.h"
#include "TeamBattleEventBattleWaitLayer.h"
#include "TeamBattleEventDeckSelectLayer.h"
#include "TeamBattleEventItemExchangeLayer.h"
#include "HttpConnectHms.h"
#include "LoadingManager.h"
#include "PopupOk.h"

USING_NS_CC;
using namespace std;
using namespace network;
using namespace extension;
using namespace json11;
using namespace CocosDenshion;

class NormalScene : public Layer
{
public:
    CC_SYNTHESIZE(string, _main_page_name, MainName);
    
    //タグ
    enum Tag{
        T_Main_Layer = 1,
        T_Title_Layer,
        T_Header_Layer,
        T_Footer_Layer,
        T_Info_Layer,
        T_Ranking_Layer,
        T_Next_Layer,
        T_Cloud1,
        T_Cloud2,
        T_Spark1,
        T_Spark2,
        T_Tournament_Time_Sprite,
        T_Tournament_Label,
        T_Tournament_Time_Label,
        T_Effect,
        T_Bg,
        T_Popup_Layer,
        T_All_Tap_Disable_Layer,
    };
    
    //ZOrder
    enum ZOrder{
        Z_Main_Layer,
        Z_Title_Layer,
        Z_Tournament_Time_Sprite,
        Z_Header_Layer,
        Z_Footer_Layer,
        Z_Info_Layer,
        Z_Ranking_Layer,
        Z_Next_Layer,
        Z_Cloud1,
        Z_Cloud2,
        Z_Spark1,
        Z_Spark2,
        Z_Tournament_Label,
        Z_Tournament_Time_Label,
        Z_Effect,
        Z_Bg, //使っていない
        Z_Popup_Layer,
        Z_All_Tap_Disable_Layer,
    };
    
    //背景状態
    enum BgStatus{
        Main =1,
        Event,
        Quest,
        Ranking,
        Challenge,
        Tournament,
        Team,
        Boss,
    };
    
    BgStatus _bg_status; //背景状態
    
    //ページ切り替え変数
    CC_SYNTHESIZE(int, _change_page_flg, ChangePageFlg);
    CC_SYNTHESIZE(int, _change_page_count, ChangePageCount);
    CC_SYNTHESIZE(string, _change_page_name, ChangePageName);
    
    //トーナメント変数
    static time_t _tournament_next_battle_time;
    
    //チーム対抗戦変数
    static time_t _team_battle_next_battle_time;
    
    NormalScene(); //コンストラクタ
    
    float ratio;
    static Scene* createScene(string page_name);
    static NormalScene* create(string page_name);
    virtual bool init(string page_name);
    
    
//    CREATE_FUNC(NormalScene);
    
    void updateOne(float dt);

    void showBg(); //背景画像の表示
    void cutLeftAndRightVisibleSpace(); //両サイドの余白に画像を配置する
    
    //シーンの遷移関数
    void changePage(string next_page_name);
    void changePageActionRemoveAndSet(string next_page_name); //ページアクション
    void setActionMainPage(string next_page_name); //ページを表示するアクション
    void changePageFadeAction(string next_page_name); //フェードアクションのページ遷移
    
    void getBgImagePathAndMusicPath(string next_page_name, string *path); //背景画像と音楽パスの取得
    void changeBgAction(string next_page_name); //背景画像の切り替えアクション
    
    
    void createCloud(); //雲の作成
    void createSparks(); //火の粉の作成
    
    void removeChallengerSprite(); //挑戦者ボタンがあったら消す
    
    void changeFotterAction(string next_page_name); //フッターの切り替え
    void hideHeaderLayer(); //フッターレイヤーを下に隠す
    void showHeaderLayer(); //フッターレイヤーを上に戻す
    void hideFotterLayer(); //フッターレイヤーを下に隠す
    void showFotterLayer(); //フッターレイヤーを上に戻す
    void showEffect(string filename, Vec2 position, float scale); //エフェクトの表示
    
    void checkTournamentStartDateSprite(string next_page_name); //トーナメントスプライト表示切り替え
    void showUntilStartDate(); //トーナメント開始時刻まで（エントリー終了時刻まで）の作成
    void removeUtilStartDate(); //トーナメント終了時刻画像を閉じる
    string createRemainTimeStr(); //残り時間文字列の作成
    
    void modalLayerCreate(int contents) ; //モーダルレイヤーの設定
    void modalLayerCreate(int contents, Layer *parent, SEL_SCHEDULE sel, int schedulerTime);
    void addTapDisableLayer();//ノーマルシーンにタップ無効化レイヤーを作成する
    void removeTapDisableLayer(); //タップ無効化レイヤーを削除する
    
    Layer* createPageLayer(string next_page_name);
    
    //------------------ charaデータ用処理  ----------------------
    void updateUserCharaDataByUserDefault(); //キャラデータの設定
    map<int, map<string, string> > character_list; //キャラクター一覧
    string sort_param; //キャラクターソート
    vector<int> sort_chara_type = {1,0,0,0,0};//キャラクターソート
    map<int, map<string, string> > deck_list; //デッキ一覧
    int deckNo = 1; //選択デッキ番号
    map<int, map<string, string> > active_skill_list; //
    map<int, map<string, string> > passive_skill_list; //
    bool deckCardChangeFlg; //デッキ内容変更フラグ
};

#endif /* defined(__War__NormalScene__) */
