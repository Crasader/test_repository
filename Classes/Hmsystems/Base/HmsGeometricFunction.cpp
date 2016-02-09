#include "HmsGeometricFunction.h"

//画像の拡大縮小比率を取得する
float getVisibleRatio()
{
    Size visibleSize = Director::getInstance()->getVisibleSize();
    float display_ratio = visibleSize.height / visibleSize.width;
    float ratio = 1.0;
    if(display_ratio<1.5) {//横が長い場合、縦の縮尺比率に合わせる
        ratio = visibleSize.height / DISPLAY_HEIGHT;
    } else {//それ以外は横の縮尺比率に合わせる
        ratio = visibleSize.width / DISPLAY_WIDTH;
    }
    return ratio;
}

//画面の左右のスペースサイズを取得する
float getVisibleSpaceWidth()
{
    Size visibleSize = Director::getInstance()->getVisibleSize();
    float display_ratio = visibleSize.height / visibleSize.width;
    float space_width = 0;
    if(display_ratio<1.5) {
        space_width = visibleSize.width - (visibleSize.height*2.0/3.0);
        space_width /= 2.0;
    }
    return space_width;
}

//画面の上下のスペースサイズを取得する
float getVisibleSpaceHeight()
{
    Size visibleSize = Director::getInstance()->getVisibleSize();
    float display_ratio = visibleSize.height / visibleSize.width;
    float space_height = 0;
    if(display_ratio>1.5) {
        space_height = visibleSize.height - (visibleSize.width*3.0/2.0);
        space_height /= 2.0;
    }
    return space_height;
}

//画面上の座標を、640x960平面の座標に変換する
Vec2 convertTo640x960(Vec2 point)
{
    /*
    float ratio = getVisibleRatio();
    float space_width = getVisibleSpaceWidth();
    float space_height = getVisibleSpaceHeight();
    
    float x = (point.x - space_width) / ratio;
    float y = (point.y - space_height) / ratio;
     */
    
    return convertToDisplaySize(point, 0.5, 0.5);
}

//640x960平面の座標を、画面上の座標に変換する
Vec2 convertFrom640x960(Vec2 point)
{
    /*
    float ratio = getVisibleRatio();
    float space_width = getVisibleSpaceWidth();
    float space_height = getVisibleSpaceHeight();
    
    float x = point.x * ratio + space_width;
    float y = point.y * ratio + space_height;
     */
    
    return convertFromDisplaySize(point, 0.5, 0.5);
}

//画面上の座標を、端末ディスプレイ縦横比率の座標に変換する
Vec2 convertToDisplaySize(Vec2 point, float anchor_display_x, float anchor_display_y)
{
    float ratio = getVisibleRatio();
    float space_width = getVisibleSpaceWidth();
    float space_height = getVisibleSpaceHeight();
    
    float x = (point.x - space_width * 2 * anchor_display_x) / ratio;
    float y = (point.y - space_height * 2 * anchor_display_y) / ratio;
    return Point(x, y);
}

//端末ディスプレイ縦横比率の座標を、画面上の座標に変換する
Vec2 convertFromDisplaySize(Vec2 point, float anchor_display_x, float anchor_display_y)
{
    float ratio = getVisibleRatio();
    float space_width = getVisibleSpaceWidth();
    float space_height = getVisibleSpaceHeight();
    
    float x = point.x * ratio + space_width * 2 * anchor_display_x;
    float y = point.y * ratio + space_height * 2 * anchor_display_y;
    
    return Point(x ,y);
}