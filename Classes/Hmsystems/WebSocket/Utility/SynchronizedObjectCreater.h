#ifndef __war__SynchronizedObjectCreater__
#define __war__SynchronizedObjectCreater__

#include "cocos2d.h"
#include "json11.hpp"

USING_NS_CC;

class SynchronizedObject;
class Synchronizer;

class SynchronizedObjectCreater : public Node{
public:
    static SynchronizedObjectCreater* create();
    void add(SynchronizedObject* object);
    SynchronizedObject* getSynchronizedObject(json11::Json json,
                                              Synchronizer* synchronizer);
private:
    virtual bool init();
    
    std::vector<SynchronizedObject*> _synchronized_object_list;
};


#endif /* defined(__war__SynchronizedObjectCreater__) */
