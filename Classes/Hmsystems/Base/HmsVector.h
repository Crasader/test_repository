#ifndef __HMS_VECTOR_H__
#define __HMS_VECTOR_H__

#include "cocos2d.h"
#include "HmsConsoleUtil.h"

using namespace std;
USING_NS_CC;

template <class T>
class HmsVector {
protected:
    //内部のvector
    vector<T> vec;
    
public:
    //コンストラクタ
    HmsVector() {}
    explicit HmsVector(int size) {
        vec = std::vector<T>(size);
    }
    explicit HmsVector(int size, int value) {
        vec = std::vector<T>(size);
        for(int i=0; i<size; i++) {
            vec[i] = value;
        }
    }
    HmsVector(vector<T>& vec2) {
        vec = vec2;
    }
    
    //デストラクタ
    ~HmsVector() {}
    
    //全削除
    void clear() {
        vec.clear();
    }
    
    //サイズ取得
    int size() {
        return (int)vec.size();
    }
    
    //1件途中に挿入
    void insert(T t, int index) {
        vec.insert(vec.begin()+index, t);
    }
    void push_back(T t) {
        insert(t);
    }
    
    //代入
    void set(T t, int index) {
        if(index>=0 && index<size()) {
            vec[index] = t;
        } else {
            HMSLOG("out of bound in HmsVector set(). size=%d, index=%d", size(), index);
        }
    }
    
    //1件最後に挿入
    void insert(T t) {
        vec.push_back(t);
    }
    
    //最後の1件を削除
    void pop_back(){
        vec.pop_back();
    }
    
    //1件削除（メモリは減らないので注意）
    bool remove(int index);
    void erase(typename std::vector<T>::iterator position) {
        vec.erase(position);
    }
    
    //1件取得
    T at(int index) {
        if(index>=0 && index<size()) {
            return vec.at(index);
        } else {//配列の参照位置がオーバーしている場合は、アプリが落ちないよう最後の要素を返す。注意！！！
            HMSLOG("out of bound in HmsVector at(). size=%d, index=%d", size(), index);
            return vec.at(size()-1);
        }
    }
    /*
    T operator[](int index) {
        return at(index);
    }
     */
    /*
    T& operator[](int index) {
        T t = at(index);
        return t;
    }
    */
    T operator[](int index) {
        T t = at(index);
        return t;
    }
    
    //空かどうか
    bool empty() {
        if(size()==0) return true;
        else return false;
    }
    
    //1番最初を取得
    T front() {
        return vec.front();
    }
    
    T back() {
        return vec.back();
    }
    
    //vectorを返す
    vector<T> getVector();
    
    //メモリの動的確保
    void reserve(ssize_t n)
    {
        vec.reserve(n);
    }
    
    //イテレータ
    typename std::vector<T>::iterator begin() {
        return vec.begin();
    }
    
    typename std::vector<T>::iterator end() {
        return vec.end();
    }
    
    typename std::vector<T>::reverse_iterator rbegin() {
        return vec.rbegin();
    }
    
    typename std::vector<T>::reverse_iterator rend() {
        return vec.rend();
    }
};

template <class T> vector<T> HmsVector<T>::getVector(){
    return vec;
}

template <class T> bool HmsVector<T>::remove(int index)
{
    if(index>=0 && index<size()) {
        vec.erase(vec.begin() + index);
        return true;
    } else {
        HMSLOG("out of bound in HmsVector remove(). size=%d, index=%d", size(), index);
        return false;
    }
}

#endif // __HMS_VECTOR_H__
