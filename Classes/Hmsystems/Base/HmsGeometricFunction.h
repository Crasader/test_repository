#ifndef __HMS_GEOMETRIC_FUNCTION_H__
#define __HMS_GEOMETRIC_FUNCTION_H__

#include "cocos2d.h"
#include "common.h"

using namespace std;
USING_NS_CC;

//画像の拡大縮小比率を取得する
float getVisibleRatio();

//画面の左右のスペースサイズを取得する
float getVisibleSpaceWidth();

//画面の上下のスペースサイズを取得する
float getVisibleSpaceHeight();

//画面上の座標を、640x960平面の座標に変換する
Vec2 convertTo640x960(Vec2 point);

//640x960平面の座標を、画面上の座標に変換する
Vec2 convertFrom640x960(Vec2 point);

//画面上の座標を、端末ディスプレイ縦横比率の座標に変換する
Vec2 convertToDisplaySize(Vec2 point, float anchor_display_x, float anchor_display_y);

//端末ディスプレイ縦横比率の座標を、画面上の座標に変換する
Vec2 convertFromDisplaySize(Vec2 point, float anchor_display_x, float anchor_display_y);

#endif // __HMS_GEOMETRIC_FUNCTION_H__
