#include "BracketPartsSpriteBatchNode.h"

//各定数の初期化
const std::vector<std::string> BracketPartsSpriteBatchNode::PARTS_PATH_NORMAL = {
    "",
    "init/Home/Event/TournamentEvent/line/line1.png",
    "init/Home/Event/TournamentEvent/line/line2.png",
    "init/Home/Event/TournamentEvent/line/line3.png",
    "init/Home/Event/TournamentEvent/line/line4.png",
    "init/Home/Event/TournamentEvent/line/line5.png",
    "init/Home/Event/TournamentEvent/line/line6.png",
    "init/Home/Event/TournamentEvent/line/line7.png",
};

const std::vector<std::string> BracketPartsSpriteBatchNode::PARTS_PATH_RED = {
    "",
    "init/Home/Event/TournamentEvent/line/line_red1.png",
    "init/Home/Event/TournamentEvent/line/line_red2.png",
    "init/Home/Event/TournamentEvent/line/line_red3.png",
    "init/Home/Event/TournamentEvent/line/line_red4.png",
    "init/Home/Event/TournamentEvent/line/line_red5.png",
    "init/Home/Event/TournamentEvent/line/line_red6.png",
    "init/Home/Event/TournamentEvent/line/line_red7.png",
};

const int BracketPartsSpriteBatchNode::PARTS_ROTATION[] = {-1, 0, 0, 0, 0 , 0, 180, 180} ;
const bool BracketPartsSpriteBatchNode::PARTS_FLIP_X_FLG[] = {false, false, false, false, false, false, false, false} ;



BracketPartsSpriteBatchNode::BracketPartsSpriteBatchNode()
{
}

BracketPartsSpriteBatchNode* BracketPartsSpriteBatchNode::create(int parts_type, bool is_mask)
{
    BracketPartsSpriteBatchNode *sprite_batch_node = new BracketPartsSpriteBatchNode();
    if (sprite_batch_node && sprite_batch_node->init(parts_type,is_mask))
    {
        sprite_batch_node->autorelease();
        return sprite_batch_node;
    }
    CC_SAFE_DELETE(sprite_batch_node);
    return nullptr;
}

bool BracketPartsSpriteBatchNode::init(int parts_type, bool is_mask)
{
    std::string image_path ;
    if(is_mask) image_path = PARTS_PATH_RED[parts_type] ;
    else image_path = PARTS_PATH_NORMAL[parts_type] ;
    
    int capacity = 29 ;
    if(parts_type == 2){
        capacity = 50 ;
    }
    
    if (!SpriteBatchNode::initWithFile(image_path, capacity))
    {
        return false;
    }
    
    return true;
}

void BracketPartsSpriteBatchNode::onEnter()
{
    SpriteBatchNode::onEnter();
}



