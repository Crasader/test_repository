#include "FooterLayer.h"

bool FooterLayer::init()
{
	if ( !Layer::init() )
	{
		return false;
	}
	HMSLOG("FooterLayer::init()");
	
	ratio = getVisibleRatio();
	
	return true;
}

void FooterLayer::createButtonSprite()
{
	//初期化
	normal_button.clear();
	selected_button.clear();
	
	//ボタン画像の作成
	for (int i = 0; i < 6; i++)
	{
		if(i==0) normal_button.push_back(Sprite::create("init/Footer/bt_footer_base_r" + toString<int>(i+1) + ".png"));//背景(赤):表示中
		else normal_button.push_back(Sprite::create("init/Footer/bt_footer_base_" + toString<int>(i+1) + ".png"));//背景(黒):通常
		
		Sprite* normal_img = Sprite::create("init/Footer/bt_footer_icon_" + toString<int>(i+1) + ".png");//シルエット画像
		normal_img->setPosition(Point(normal_button[i]->getContentSize() / 2));
		normal_button[i]->addChild(normal_img);
		
		Sprite* normal_name = Sprite::create("init/Footer/bt_footer_name_" + toString<int>(i+1) + ".png");//文字画像
		normal_name->setPosition(Point(normal_button[i]->getContentSize() / 2));
		normal_button[i]->addChild(normal_name);
		
		if(i==0) selected_button.push_back(Sprite::create("init/Footer/bt_footer_base_r" + toString<int>(i+1) + ".png"));
		else selected_button.push_back(Sprite::create("init/Footer/bt_footer_base_" + toString<int>(i+1) + ".png"));
		selected_button[i]->setColor(Color3B(180, 180, 180));
		
		Sprite* selected_img = Sprite::create("init/Footer/bt_footer_icon_" + toString<int>(i+1) + ".png");//シルエット画像
		selected_img->setPosition(Point(selected_button[i]->getContentSize() / 2));
		selected_img->setColor(Color3B(180, 180, 180));
		selected_button[i]->addChild(selected_img);
		
		Sprite* selected_name = Sprite::create("init/Footer/bt_footer_name_" + toString<int>(i+1) + ".png");//文字画像
		selected_name->setPosition(Point(selected_button[i]->getContentSize() / 2));
		selected_name->setColor(Color3B(180, 180, 180));
		selected_button[i]->addChild(selected_name);
	}
    
    SaveData::saveBool("normal_scene_change_page_ok_flg", true);
}

void FooterLayer::recreateButtonSprite(int tag_id)
{
	//ボタンの再構成
	for (int i = 0; i < 6; i++)
	{
		int ti = i + 1;
		HMSLOG("ti = %d", ti);
		if(ti==tag_id) normal_button[i]->setTexture("init/Footer/bt_footer_base_r" + toString<int>(ti) + ".png");//背景(赤):表示中
		else normal_button[i]->setTexture("init/Footer/bt_footer_base_" + toString<int>(ti) + ".png");//背景(黒):通常
		
		if(ti==tag_id) selected_button[i]->setTexture("init/Footer/bt_footer_base_r" + toString<int>(ti) + ".png");
		else selected_button[i]->setTexture("init/Footer/bt_footer_base_" + toString<int>(ti) + ".png");
		selected_button[i]->setColor(Color3B(180, 180, 180));
	}
}

void FooterLayer::showButton()
{
	//初期化
	menuItem.clear();
	
	//メニューアイテムの作成
	for (int i = 0; i < 6; i++)
	{
		menuItem.push_back(MenuItemSprite::create(normal_button[i], selected_button[i], CC_CALLBACK_1(FooterLayer::changePage, this)));
		menuItem[i]->setScale(ratio);
		menuItem[i]->setTag(i+1);
	}
	
	auto Menu = Menu::create(menuItem[0], menuItem[1], menuItem[2], menuItem[3], menuItem[4], menuItem[5], nullptr);
	Menu->alignItemsHorizontallyWithPadding(0);
	Menu->setPosition(convertFromDisplaySize(Point(320, 54), 0.5, 0));
	this->addChild(Menu, ZORDER_BUTTON, HOME_BUTTON_TAG);
}

void FooterLayer::onEnter()
{
	Layer::onEnter();
	
	//ボタンの作成
	createButtonSprite();
	
	//ボタンの表示
	showButton();
}

void FooterLayer::changePage(Ref *sender){
	
	MenuItem* menuItem = (MenuItem *)sender;
	int tag = menuItem->getTag();
	
	auto normal_scene = (NormalScene *)this->getParent();
	
	//ガチャタブ情報をプレミアムガチャで保存
	SaveData::saveInt("gacha_tab",2);
	
	if (tag == HOME_BUTTON_TAG) {
		normal_scene->changePage("Mypage");
		//recreateButtonSprite(HOME_BUTTON_TAG);
	}else if (tag == CHARA_BUTTON_TAG) {
		normal_scene->changePage("CharaMenu");
		//recreateButtonSprite(CHARA_BUTTON_TAG);
	}else if (tag == QUEST_BUTTON_TAG) {
		normal_scene->changePage("QuestMenu");
		this->removeNewImg(1);
		//recreateButtonSprite(QUEST_BUTTON_TAG);
	}else if (tag == GACHA_BUTTON_TAG) {
		normal_scene->changePage("RevisionGachaMenu");
		this->removeNewImg(3);
		//recreateButtonSprite(GACHA_BUTTON_TAG);
	}else if (tag == SHOP_BUTTON_TAG) {
		normal_scene->changePage("ShopMenu");
		//recreateButtonSprite(SHOP_BUTTON_TAG);
	}else if (tag == FRIEND_BUTTON_TAG) {
		normal_scene->changePage("FriendMenu");
		//recreateButtonSprite(FRIEND_BUTTON_TAG);
	}
	
}
void FooterLayer::addNewImg(int number){
	auto new_img = Sprite::create("init/Footer/new.png");
	new_img->setScale(0.8f);
	auto repeat = RepeatForever::create(Sequence::create(
														ScaleTo::create(0.4f, 1.0f),
														FadeTo::create(0.2f, 170),
														ScaleTo::create(0.4f, 0.8f),
														FadeTo::create(0.2f, 255),
														nullptr
														)
										);
	new_img->runAction(repeat);

	new_img->setPosition(menuItem[number]->getContentSize().width / 2 , menuItem[number]->getContentSize().height / 2 + 15);
	menuItem[number]->addChild(new_img, 2 , 555);
	if(number==1) new_img_quest = true;
	if(number==3) new_img_gacha = true;
}
void FooterLayer::removeNewImg(int number)
{
	auto new_sp = menuItem[number]->getChildByTag(555);
    
    if (new_sp) {
        new_sp->removeFromParent();
    }
    
	if(number==1){
		SaveData::saveBool("new_img_quest", false);
		new_img_quest = false;
	}
	if(number==3){
		SaveData::saveBool("new_img_gacha", false);
		new_img_gacha = false;
	}
}