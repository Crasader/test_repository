
#include "OutlineSprite.h"
#include "cocos2d.h"

Sprite* OutlineSprite::createWithSprite(Sprite *basic_sprite, float outline_size){
    
    int draw_num = 16;
    //float stroke = 3.0f;
   
    auto render = RenderTexture::create(basic_sprite->getContentSize().width + outline_size * 2,basic_sprite->getContentSize().height+ outline_size * 2);
    
    render->setAnchorPoint(Vec2(0.5f,0.5f));
    render->begin();
    
    for(int i=0; i<draw_num; i++){
        auto sprite2 = Sprite::createWithTexture(basic_sprite->getTexture());//毎回新しくスプライト作らないとできないっぽい
        sprite2->setBlendFunc({GL_SRC_ALPHA,GL_ONE});//ブレンドファンクの設定
        float radian = CC_DEGREES_TO_RADIANS(360.0f/draw_num*i);//度をラジアンに変更
        sprite2->setAnchorPoint(Vec2(0.5f,0.5f));
        sprite2->setPosition(Vec2(basic_sprite->getContentSize().width/2 + outline_size, basic_sprite->getContentSize().height/2 + outline_size) + Vec2(outline_size * cosf(radian), outline_size * sinf(radian)));
        //sprite2->setColor(color);
        sprite2->visit();
    }

    render->end();
    render->getSprite()->getTexture()->setAntiAliasTexParameters();//これでジャギーが無くなる
   
    auto *new_sprite = create();
    new_sprite->outline = Sprite::createWithTexture(render->getSprite()->getTexture());
    new_sprite->outline->setScale(1.0f, -1.0f);//RenderTextureは上下反転するらしい
    new_sprite->addChild(new_sprite->outline, 0);//アウトラインの追加
    
    new_sprite->addChild(basic_sprite, 1);//元のスプライトを上に追加
    new_sprite->sprite = basic_sprite;
    
    return new_sprite;
}

Sprite* OutlineSprite::createWithLabel(Label *basic_label, float outline_size){
    //basic_label->enableShadow(Color4B::BLUE,Size(5,5));
    int draw_num = 16;
    
    auto render = RenderTexture::create(basic_label->getContentSize().width + outline_size * 2,basic_label->getContentSize().height+ outline_size * 2);
    
    render->setAnchorPoint(Vec2(0.5f,0.5f));
    render->begin();
    
    for(int i=0; i<draw_num; i++){
        
        //ラベルをコピーで作成
        //TTFConfig ttfConfig("fonts/DShirkg8.ttf",basic_label->getSystemFontSize());
        Label *copy_label = Label::createWithTTF(basic_label->getTTFConfig(), basic_label->getString());
        //copy_label->setContentSize(basic_label->getContentSize()+Size(outline_size*2,outline_size*2));
        copy_label->setBlendFunc({GL_SRC_ALPHA,GL_ONE});//ブレンドファンクの設定
        float radian = CC_DEGREES_TO_RADIANS(360.0f * i / draw_num);//度をラジアンに変更
        
        //float line_shadow = outline_size;
        float line_shadow = 0.0f;
        line_shadow = outline_size * sin(CC_DEGREES_TO_RADIANS((360.0f * i / draw_num)+45.0f+90.0f));//右下方向にシャドウを伸ばす
        if(line_shadow < 0.0f) line_shadow = 0.0f;
        line_shadow += outline_size/3.0f;//最薄部分の厚さ
        //CCLOG("シャドウ：%f",line_shadow);
        
        
        copy_label->setAnchorPoint(Vec2(0.5f,0.5f));
        copy_label->setPosition(Vec2(basic_label->getContentSize().width/2+outline_size, basic_label->getContentSize().height/2+outline_size) + Vec2(line_shadow * cosf(radian), line_shadow * sinf(radian)));
        //copy_label->setColor(color);
        copy_label->visit(Director::getInstance()->getRenderer(), Director::getInstance()->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW), true);
    }
    
    render->end();
    render->getSprite()->getTexture()->setAntiAliasTexParameters();//これでジャギーが無くなる
    
    auto *new_sprite = create();
    new_sprite->outline = Sprite::createWithTexture(render->getSprite()->getTexture());
    new_sprite->outline->setScale(1.0f, -1.0f);//RenderTextureは上下反転するらしい
    new_sprite->addChild(new_sprite->outline, 0);//アウトラインの追加
    //new_sprite->setContentSize(new_sprite->outline->getContentSize());
    new_sprite->addChild(basic_label, 1);//元のスプライトを上に追加
    new_sprite->label = basic_label;
    
    return new_sprite;
}

Texture2D* OutlineSprite::createWithLabelOneSprite(Label *basic_label, float outline_size){
    auto sprite = (OutlineSprite *)createWithLabel(basic_label, outline_size);
    
    //auto new_spr =  Sprite::create("download/Chara/parts100/card_num5.png");
    sprite->setScale(1.0f, -1.0f);//RenderTextureは上下反転するらしい
    sprite->setPosition(sprite->getPosition() + Vec2(sprite->outline->getContentSize().width/2,sprite->outline->getContentSize().height/2));
    sprite->setAnchorPoint(Vec2(0,1));
    sprite->outline->setColor(Color3B::BLACK);
    auto render = RenderTexture::create(sprite->outline->getContentSize().width,sprite->outline->getContentSize().height);
    //render->setAnchorPoint(Vec2(0.5f,0.5f));
    render->begin();
    sprite->visit();
    render->end();
    render->getSprite()->getTexture()->setAntiAliasTexParameters();//これでジャギーが無くなる
    //auto new_spr = Sprite::createWithTexture(render->getSprite()->getTexture());
    //render->setScale(1.0f, -1.0f);//RenderTextureは上下反転するらしい
    return render->getSprite()->getTexture();
}