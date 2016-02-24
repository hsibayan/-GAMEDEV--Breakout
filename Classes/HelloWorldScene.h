#pragma once

#include "cocos2d.h"
USING_NS_CC;

class HelloWorld : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();
	virtual bool init();

	CREATE_FUNC(HelloWorld);

	void update(float) override;

private:
	bool onContactBegin(PhysicsContact &contact);
	bool collided(int, int, int, int);
	
	Sprite *ball;
	Sprite *platform;
	Sprite *startpage;
	Sprite *endpage;
	Sprite *pausepage;

	PhysicsBody *ballBody;
	PhysicsBody *platformBody;
	PhysicsBody *wallBody;
	PhysicsBody *groundBody;
	PhysicsBody *brickBody;

	char movement;
	boolean moving, onStartPage = true;
	Vec2 currVelocity;
	int currPage;
};