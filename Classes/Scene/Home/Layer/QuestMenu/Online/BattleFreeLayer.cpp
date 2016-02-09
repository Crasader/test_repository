#include "BattleFreeLayer.h"
#include "BattleFreeConfirmLayer.h"

bool BattleFreeLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("BattleFreeLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void BattleFreeLayer::onEnter(){
    Layer::onEnter();
    
    //ページタイトルの作成
    //HeaderLayer::setupTitleBar("オンライン対戦", "BattleTop", this);
    
    BattleFreeConfirmLayer *layer = BattleFreeConfirmLayer::create();
    this->addChild(layer, 10);
	layer->setup();
}
