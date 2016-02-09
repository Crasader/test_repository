//  ParticleSystemPool.h

#ifndef __androidtest__ParticleSystemPool__
#define __androidtest__ParticleSystemPool__

#include "cocos2d.h"
#include "HmsConsoleUtil.h"
#include "NParticleSystemQuad.h"

class ParticleSystemPool : public cocos2d::Ref {

public:
	static ParticleSystemPool* create(std::string filename, size_t size);
	bool init(std::string filename, size_t size);
	void push(NParticleSystemQuad* particle);
	NParticleSystemQuad* pop();

protected:
	/* プールの実体 */
	cocos2d::Vector<NParticleSystemQuad*> _pool;
};



#endif /* defined(__androidtest__ParticleSystemPool__) */
