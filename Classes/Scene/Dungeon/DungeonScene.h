#ifndef __War__DungeonScene__
#define __War__DungeonScene__

#include "cocos2d.h"
#include "HmsVector.h"
#include "Chara.h"
//#include "WebSocket.h"
#include "BackgroundLayer.h"
#include "UILayer.h"
#include "FieldLayer.h"
#include "DialogLayer.h"
#include "ResultLayer.h"
#include "ResultModalLayer.h"
#include "ActiveSkill.h"
#include "ActiveSkillEffectLayer.h"
#include "CardLayer.h"
#include "PassiveSkill.h"
#include "PassiveSkillEffectLayer.h"
#include "ComboEffectLayer.h"
#include "TutorialNaviLayer.h"
#include "StartBattleEffectLayer.h"
#include "ActiveBuffer.h"
#include "PassiveBuffer.h"
#include "UIEffectLayer.h"
#include "DungeonSubMenu.h"
#include "SkillCheckLayer.h"
#include "SkillConfirmLayer.h"
#include "SupportInfoLayer.h"
#include "ChatBaseUpperLayer.h"
#include "HttpConnect.h"
#include "json11.hpp"
#include "Player.h"

#define MAX_POWER_COST 99 //最大士気
#define MAX_CHARA_VALUE 999//最大ダメージ
#define MIN_CHARA_VALUE 0
#define MAX_CHARA_HP 999 //最大HPの限界
#define MIN_CHARA_HP 1 //最小HPの限界

#define POPUP_TAG 1000
#define REPLAY_FINISH_BUTTON_TAG 880
#define REPLAY_TURN_NUM_TAG 881
#define REPLAY_PAUSE_BUTTON_TAG 882
#define REPLAY_FAVORITE_BUTTON_TAG 883
#define COMBO_MAGNI 20.0f //コンボ倍率パーセント

#define STATUS_TYPE_REMOVE_GUARD 701 //除去無効
#define STATUS_TYPE_CANT_ACTION 702 //行動不能
#define STATUS_TYPE_CANT_SKILL 703 //スキル封印
#define STATUS_TYPE_CANT_SKILL_ATTACK 704 //スキル封印 攻撃武将
#define STATUS_TYPE_CANT_SKILL_DEFENSE 705 //スキル封印　防御武将
#define STATUS_TYPE_SPEAR 706 //貫通
#define STATUS_TYPE_COMBO_UPDOWN 707 //コンボ威力増減
#define STATUS_TYPE_COMBO_UPDOWN_RED 708 //コンボ威力増減　赤色
#define STATUS_TYPE_COMBO_UPDOWN_GREEN 709 //コンボ威力増減　緑色
#define STATUS_TYPE_COMBO_UPDOWN_BLUE 710 //コンボ威力増減　青色
#define STATUS_TYPE_DIRECT_UPDOWN 711 //スキル攻撃によるダメージ増減
#define STATUS_TYPE_COUNTER 712 //カウンター
#define STATUS_TYPE_PLAYER_ATTACK_UPDOWN 713 //プレイヤー攻撃でのダメージ上下
#define STATUS_TYPE_CHARM 714 //誘惑・魅了
#define STATUS_TYPE_DARKNESS 715 //自分のフィールドカードの内容を相手から隠す
#define STATUS_TYPE_CANT_ACTION_BELOW 716 //戦闘力？？以下行動不能
#define STATUS_TYPE_STEEL_SHIELD 717//鉄壁：一度だけ敵の攻撃に耐える
#define STATUS_TYPE_PLACE_FORBIDDEN_FORWARD 718 //前方配置不可
#define STATUS_TYPE_PLACE_FORBIDDEN_SUPPORT 719 //前方軍師配置不可
#define STATUS_TYPE_ANTI_ATTACKER 720 //攻撃武将にのみ攻撃
#define STATUS_TYPE_NEXT_TURN_DEAD 721 //次のターン死亡
#define STATUS_TYPE_CANT_ACTION_GUARD 722 //行動不能無効
#define STATUS_TYPE_COMBO_UPDOWN_ZERO 723 //コンボ効果ゼロ
#define STATUS_TYPE_ESCAPE_GUARD 724 //攻撃で死なずに行動不能状態になる
#define STATUS_TYPE_COST_DOWN_ALL 725 //コストダウンオール
#define STATUS_TYPE_COST_DOWN_ATTACK 726 //コストダウンアタック
#define STATUS_TYPE_COST_DOWN_DEFENSE 727 //コストダウンディフェンス
#define STATUS_TYPE_COST_DOWN_SUPPORT 728 //コストダウンサポート
#define STATUS_TYPE_ATTACK_MORALE_GAIN 729 //総大将ダメージで士気を得る
#define STATUS_TYPE_CANT_ACTION_ABOVE 730 //戦闘力？？以下行動不能
#define STATUS_TYPE_HAND_OPEN 731 //手札公開
#define STATUS_TYPE_ANTI_COUNTER 732 //カウンター封じ
#define STATUS_TYPE_DEFENSE_BREAK 733 //攻撃すると防御低下
#define STATUS_TYPE_TWICE_ATTACK 734 //２回攻撃
#define STATUS_TYPE_COMBO_UPDOWN_DIFFERENT 735 //コンボ威力増減　コンボ左右違う
#define STATUS_TYPE_ENDURE 736 //HP１で耐える
#define STATUS_TYPE_STOPPING_ATTACKER 737 //攻撃カードを置いた時にそのカードは行動不能になる
#define STATUS_TYPE_FIXED_POWER 738 //上昇士気固定
#define STATUS_TYPE_DRAW_CONFINE 739 //カード引く数制限
#define STATUS_TYPE_CANT_SUMMONS 740 //Xコストを召喚できない
#define STATUS_TYPE_WAITING_FORCE 741 //攻撃できない
#define STATUS_TYPE_CANT_ACTION_COMBO 742 //コンボカラー？？以外行動不能
#define STATUS_TYPE_CANT_ACTION_COST_BELOW 743 //コストX以下　行動不能
#define STATUS_TYPE_CANT_ACTION_COST_MORE 744 //コストX以上　行動不能
#define STATUS_TYPE_CANT_ACTION_RARE_BELOW 745 //レア度X以下　行動不能
#define STATUS_TYPE_CANT_ACTION_RARE_MORE 746 //レア度X以上　行動不能
#define STATUS_TYPE_INVALID_DECREASE 747//戦闘力低下無効
#define STATUS_TYPE_HEAL_JAMMING 748//回復妨害
#define STATUS_TYPE_SKILL_DAMAGE_CUT 749//スキルダメージカット
#define STATUS_TYPE_LIMITED_MAX_COST 750//士気がX以上になるとそれ以上増えなくなる
#define STATUS_TYPE_SNOWMAN 751//雪だるま設置
#define STATUS_TYPE_FREEZE 752//氷漬け
#define STATUS_TYPE_SKILL_DAMAGE_UP 753//スキルダメージアップ
#define STATUS_TYPE_ACCUMULATION 754//ためる
#define STATUS_TYPE_CANT_SKILL_SUPPORT 755 //スキル封印　軍師
#define STATUS_TYPE_CANT_ACTION_ESCAPE 756 //不屈からくる行動不能
#define STATUS_TYPE_RANDOM_GUARD 757 //攻撃で破壊されるのを確率で無効
#define STATUS_TYPE_CANT_SKILL_GUARD 758 //スキル封印無効

USING_NS_CC;
using namespace std;
using namespace network;

class DungeonScene : public Layer, public SuperAnim::SuperAnimNodeListener
{

public:
    EventListenerTouchOneByOne *listener;
    float ratio;
    
    //リプレイで使用
    bool replay_flg = false;//リプレイモードかどうか
    bool replay_pause_flg = false;//リプレイを中断するかどうか
    bool replay_other_flg = false;//リプレイモードで他人のリプレイかどうか
    bool replay_favorite_flg = false;//リプレイでお気に入りかどうか
    
    //子レイヤー
    BackgroundLayer* backgroundLayer;
    UILayer* uiLayer;
    FieldLayer* fieldLayer;
    CardLayer* cardLayer;
    DialogLayer* dialogLayer;
    Layer* effectLayer;
    Layer* damageLayer;
    ResultLayer* resultLayer;
    ActiveSkillEffectLayer* activeSkillEffectLayer;
    ActiveSkill* activeSkill;
    PassiveSkill* passiveSkill;
    PassiveSkillEffectLayer* passiveSkillEffectLayer;
    ComboEffectLayer* comboEffectLayer;
    TutorialNaviLayer* tutorialNaviLayer;
    StartBattleEffectLayer* startBattleEffectLayer;
    ActiveBuffer* activeBuffer;
    PassiveBuffer* passiveBuffer;
    UIEffectLayer* uiEffectLayer;
	SkillCheckLayer* skillCheckLayer = nullptr;
	SkillConfirmLayer* skill_confirm_layer;
    SupportInfoLayer* support_info_layer = nullptr;
    ChatBaseUpperLayer *chat_base_upper_layer = nullptr;
    
    string winner_string = "";
    bool game_end_flg = false;
    
    //ゲーム完了後送信用パラメータ
    int param_nodamage_flg = 1;
    int param_noskill_flg = 1;
    
    //スコアアタック用パラメータ
    int score_param_damage = 0;
    int score_param_chara_count = 0;
    int score_param_skill_count = 0;
    int score_param_overkill = 0;
    //ボスイベント用パラメータ
    int score_param_attack_damage = 0;
    time_t boss_event_limit_time = 0;
    time_t boss_event_rest_time = 86400;
    HmsVector<int> damage_list;
    bool get_damage_data_flg = false;
    long boss_event_stage_log_id;
    long boss_event_stage_command_history_id;
    int boss_event_enemy_user_master_id;
    HmsVector<string> support_user_name_list;
    HmsVector<string> support_rate_up_list;
    
    //プレイヤーデータ
    Json json_master;
    Player *player[2];//プレイヤーデータはこのクラスに統一
    vector<Chara *> deck[2];//デッキデータ
	Json json_skill_master;//スキルデータ
   
    string state;
    /* --- stateの種類 ---
     "attack"
     "attack_effect"
     "character_arrival"
     "use_skill"
     "place_character"　//キャラ・スキル選択場面
     "place_effect"  //追加 キャラを置いた後のエフェクト
    */
    
    //通信関連
    int battle_list_id;
    HmsVector<string> data_received;
    bool data_exec_flg;//対戦相手のコマンド実行中にtrueになる
    bool data_send_flg;//コマンドを対戦相手に送る場合にtrueになる
    vector<map<string , Json>> send_message_vec;
    bool sending_message_flg = false;//メッセージ送信中フラグ
    
    int last_time[2];
    int connection_flg[2];
    int finish_rest_time = 0;
    bool initialize_effect_flg = false;//initializeEffect終了時にtrueになる
    bool data_recovery_flg = false;//対戦時に初めのデータの復旧が完了するとtrueになる
    bool first_turn_set_end = false;//最初のセットターンが終わるとtrueになる
    bool popup_restart_flg = false;//バックグラウンドからの復旧時にtrueになる
    //int turn_no_for_recovery;
    bool turn_change_wait = false;//change_turnが来ている時は新しくキャラを置いたりアタックしたりをできないようにする。change_turnが来ているのに攻撃でき、それが相手に伝わらないのでおかしくなってしまう。
    int last_battle_log_id = 0;
    std::mt19937 rand_engine;//メルセンヌツイスタ：ランダム用エンジン
    
    //タップ関連
    Chara *tapped_chara;
    int tapped_id = 0;
    Point tapped_revision;
    int tapped_field_index;
    int placed_index;
    float tap_time;
    float move_distance;
	bool long_tap_check;
	Chara *long_tapped_chara;
    int replace_field_index;
    Chara *set_chara;
    void attackButtonClicked();
    
    //状態遷移に対応したデータ
    vector< map<string, int>> attack_info;
    string use_skill_data;
    string place_character_data;//キャラ配置時に利用
    string passive_effect_data;//キャラ配置時に利用
    string combination_dataL;//キャラ配置時に利用
    string combination_dataR;//キャラ配置時に利用
    string set_chara_data;//キャラ配置時に利用
    vector<string> counter_character_data;
    vector<string> turn_skill_data_active;//ターン変更時のスキルエフェクトに利用
    vector<string> turn_skill_data_passive;//ターン変更時のスキルエフェクトに利用
    vector<string> turn_skill_data_hand;//ターン変更時のスキルエフェクトに利用
    string counter_attack_data;
    //*************************************************************//
    //以下メソッド
    //*************************************************************//
    
    //基本メソッド
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(DungeonScene);
    
    //開始判定用
    void checkBattleStart();
    void checkBattleStartCallback(HttpClient *sender, HttpResponse *response);
    
    void getDungeonData();
	void getDungeonDataCallback(HttpClient *sender, HttpResponse *response);
	void getDungeonDataCallback2(HttpClient *sender, HttpResponse *response);
    
    //状態遷移に対応したロジック
    bool useSkill(int primary_id, string extra_data, int rand_seed_rec = -1);
    void useSkillEnd();
    void placeCharacter(Chara *tapped_chara, int field_index, int rand_seed_rec = -1);
    void attack(bool index, bool waiting_flg = false, int rand_seed_rec = -1);
    map<string,int> attackAlone(Chara *chara);
    void tutorialAttack(float index =0.0f);
    int attacker_index;
    int defender_index;
    bool supplyCharacter();
    void changeTurn(float dt);
    void changeTurnEnd(float dt);
    void setTurn(int turn_no);
    void addPower(float dt);
    void turnMinusSupport(float dt);//軍師のターンマイナス
    
    //上記に関連したメソッド
    string beforeAfterCharaValue();//スキル使用前後の戦闘力をデータ出力
    void resetAllCharaValue();
    void calcActiveBuffer();
    void calcPassiveBuffer();
    void calcCombinationValue();
    void reCalcCombination();
    bool valueShowUpdate(float delay);
    
    //状態遷移に対応した演出
    void afterStartBattleEffect(float dt) ;
    void initializeEffect(float dt);
    void placeCharacterEffect(float dt);
    void combinationCharacterEffectL(float dt);
    void combinationCharacterEffectR(float dt);
    void passiveSkillEffect(float dt);
    void attackEffect(float dt);
    void attackEffect2(int index);
    void attackEffect3(int index);
    void attackEffect4(int index);
    void counterEffect(float dt);
	void playerHpUpdate(int index, int damage, int playerHp);
    void eraseCharacterEffect(Chara *chara);
    void startBattleEffect(SEL_SCHEDULE s);
    void changeTurnEffect(float dt);
    void addPowerEffect(float dt);
    void selectAbleEffect(bool able = true);
    void moveAbleEffect();
    void turnBufferEffect(float dt);//毎ターン効果のパッシブのエフェクト
    //後処理
    void postLogicCleanChara(float dt);
    void postLogicSetStatusPlace(float dt);
    
    //自分のターンかどうか
    bool isMyTurn();
    
    //自分のターンで、かつキャラ配置時かどうか
    bool isCharacterPlacable();
   
    //タップしたキャラで置き換える
    void replaceChara(Chara *new_chara, Chara *old_chara, int rand_seed = -1);
    
    //タップしたキャラを元の位置に戻す
    void restoreChara(Chara *chara);
    
    //タップ情報をリセットする
    void resetTapInfo();
    
    //プレイヤー情報更新
    void updatePlayerInfo(float percentage1, float percentage2, int hp1, int hp2);
    
    //状態変更を行う
    void setState(string new_state);
    string getState();
    
    //???
    void iconChangeOrder(float dt);
    void bufferMinusTurn();
    bool wholeStatusCheck(int state, int index);
    int wholeStatusValue(int state, int index, string return_type);
    int wholeStatusValueMultiply(int state, int index);
    NumericCount *nume;//残り時間少ないカウント用
    bool count_timer_flg[4];
    
    //ゲーム終了判定
    bool gameoverJudge(string over_state = "");
    void goResult();
    void continueReborn();//コンティニューで復活
    
    //バトルモード取得
    string getBattleMode();
    
    //クエストかどうか
    bool isQuestBattle();
    
    //イベントかどうか
    bool isEventBattle();
    
    //ランキングイベントかどうか
    bool isRankingEventBattle();
    
    //ボスイベントかどうか
    bool isBossEventBattle();
    
    //試練イベントかどうか
    bool isChallengeEventBattle();
    
    //トーナメントイベントかどうか
    bool isTournamentEventBattle();
    
    //チュートリアルナビゲーションを表示中かどうか
    bool isTutorialNaviActive();
    
    //チュートリアルかどうか
    bool isTutorial();
    
    //チュートリアル用
    void tutorialNaviSkill(float dt);
    
    //通信用バトルデータ作成
    Json getBattleData();
    void setBattleData(Json json_string);
    void printBattleData();
    void updateFieldLook();
    
    //不正チェック用
    int status_check_sum = 0;
    void createStatusCheckSum();//チェックサム計算
    string battle_data_for_check = "";
    void checkIllegal(string text, bool check_flg);//check_flg:trueならばチェックを行う、falseならばデータだけを作成する
    void notifyIllegalCallback(HttpClient *sender, HttpResponse *response);
    
    //ボスイベント用
    void sendCommand();
    void sendCommandDataCallback(HttpClient *sender, HttpResponse *response);
    void getDamageData();
    void getDamageDataCallback(HttpClient *sender, HttpResponse *response);

    //通信時対戦相手の演出終了
    void finishRivalEffect(float dt = 0.0f);
    
    //対戦時の状態管理
    //initialize_effect バトル開始演出
    //regist Websocketへの登録処理＋データ復旧
    //set_turn 初めのセットターン
    //ok 通常運転中
    //restart 再起動待ち
    string getVsState();
    
    //バックグラウンドからの復旧後、タイトルシーンへ
    void popupRestartMessage();
    
    //websocket関連
    void sendMessage();
    void sendMessageOthers(float dt);
    string URLEncode(string input);
    string URLEncode2(string &str);
    void onMessage(string mesage);
    
    //タップ処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
    void longTapCounter(float dt);
    
    //スケジューラー
    void updateOne(float dt);//0.3秒ごと実行
    void updateSendMessage(float dt);
    bool cpu_moving;//CPUがカード動かし中
    void runBattleData(string battle_data);//バトルデータの実行
    void onEnter(); //初期実行関数
};

//****************************//
//コーディングルール
//****************************//
//コードは概ね短いほど優れている
//変数名やメソッド名はなるべく意味のあるものにする
//コメントは多いほど良い
//英語に注意。綴りの誤りは基本的に必ず直す
//特に変数は名詞、メソッド名は動詞が良い

//変数名は_（アンダーバー）区切りとする。例 int init_flg
//メソッド名は区切らないが、単語の最初を大文字とする 例 initializeEffect()

//,（カンマ）のあとは基本的にスペースを入れる
//=（イコール）のあと、つまり代入文にも基本的にはスペースを入れる
//==は左右くっつける
//&&や||は左右を離す

//ヌル値はnullptrで統一

//for文の書き方は以下が望ましい
//for(int i=0; i<1; i++) {
//...
//}

//ifelse文の書き方は以下が望ましい
//if() {
//...
//} else if() {
//...
//}

#endif /* defined(__War__DungeonScene__) */
