#include "SynchronizedObjectCreater.h"
#include "SynchronizedObject.h"

using namespace std;
using namespace json11;

SynchronizedObjectCreater* SynchronizedObjectCreater::create(){
    SynchronizedObjectCreater *node = new SynchronizedObjectCreater();
    
    if (node && node->init())
    {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}

bool SynchronizedObjectCreater::init(){
    if(!Node::init()) return false;
    return true;
}

void SynchronizedObjectCreater::add(SynchronizedObject *object){
    _synchronized_object_list.push_back(object);
    this->addChild(object);
}

SynchronizedObject* SynchronizedObjectCreater::getSynchronizedObject(Json json, Synchronizer* synchronizer){
    SynchronizedObject* clone = nullptr;
    string objectName = json["object_name"].string_value();
    if (_synchronized_object_list.size() < 1) {
        CCLOG("## objectList is null, plz add SynchronizedObjectCreater!!");
        return nullptr;
    }
    for_each(_synchronized_object_list.begin(), _synchronized_object_list.end(),
             [&clone, &objectName, json, synchronizer](SynchronizedObject* object){
                 if(object->getObjectName() == objectName){
                     clone = object->clone(json, synchronizer);
                 }
             });
    if(clone == nullptr){
        CCLOG("## %s is invalid object name! plz add %s in SynchronizedObjectCreater.",
              objectName.c_str(), objectName.c_str());
        return nullptr;
    }
    return clone;
}