#include "HelloWorldScene.h"
USING_NS_CC;

#define COCOS2D_DEBUG 1

Scene* HelloWorld::createScene()
{
	// 'scene' is an autorelease object
	//auto scene = Scene::create();
	auto scene = Scene::createWithPhysics();
	auto layer = HelloWorld::create();

	scene->addChild(layer);

	return scene;
}

bool HelloWorld::init()
{
	if (CCLayer::init())
	{

		// Basic stuff
		movement = ' ';
		moving = false;

		/* pages:
		0 - start
		1 - game proper
		3 - game over	*/

		// Set up ball
		ballBody = PhysicsBody::createCircle(15, PhysicsMaterial(0, 1, 0));
		ballBody->setDynamic(true);
		ballBody->setCollisionBitmask(1);
		ballBody->setContactTestBitmask(true);
		ballBody->setTag(0);
		ballBody->setGravityEnable(false);
		ball = Sprite::create("ball.png");
		ball->setPosition(500, 45);
		ball->setPhysicsBody(ballBody);


		// Set up ground
		groundBody = PhysicsBody::createEdgeSegment(Vec2(0, 0), Vec2(1000, 0), PhysicsMaterial(0, 0, 1), 3);
		groundBody->setDynamic(false);
		groundBody->setCollisionBitmask(2);
		groundBody->setContactTestBitmask(true);
		auto ground = Node::create();
		ground->setPhysicsBody(groundBody);


		// Set up platform
		platformBody = PhysicsBody::createBox(Size(200, 10), PhysicsMaterial(0, 1.1f, 0));
		platformBody->setDynamic(false);
		platformBody->setCollisionBitmask(3);
		platformBody->setContactTestBitmask(true);
		platform = Sprite::create("platform.jpg");
		platform->setPosition(500, 40);
		platform->setPhysicsBody(platformBody);


		// Set up walls
		wallBody = PhysicsBody::createEdgeBox(Size(1000, 550), PhysicsMaterial(0, 1, 0), 3);
		wallBody->setDynamic(false);
		wallBody->setCollisionBitmask(3);
		wallBody->setContactTestBitmask(true);
		auto walls = Node::create();
		walls->setPosition(500, 275);
		walls->setPhysicsBody(wallBody);
		walls->setColor(ccc3(255,255,255));


		// Set up bricks
		Sprite *brick;
		int tagCounter = 1;
		for (int i = 0; i < 6; i++) {
			for (int j = 0; j < 10; j++) {
				switch (i) {
				case 0: brick = Sprite::create("redbrick.jpg"); break;
				case 1: brick = Sprite::create("orangebrick.jpg"); break;
				case 2: brick = Sprite::create("brownbrick.jpg"); break;
				case 3: brick = Sprite::create("yellowbrick.jpg"); break;
				case 4: brick = Sprite::create("greenbrick.jpg"); break;
				case 5: brick = Sprite::create("bluebrick.jpg");
				}

				brickBody = PhysicsBody::createBox(Size(100, 30), PhysicsMaterial(0, 1, 0));
				brickBody->setTag(tagCounter);
				brickBody->setDynamic(false);
				brickBody->setCollisionBitmask(4);
				brickBody->setContactTestBitmask(true);
				
				brick->setTag(tagCounter);
				brick->setAnchorPoint(Point(0, 0));
				brick->setPosition(Point(j * 100, (490 - (i * 30))));
				brick->setPhysicsBody(brickBody);
				this->addChild(brick);

				tagCounter++;
			}
		}

		
		// Add sprites
		this->addChild(platform);
		this->addChild(walls);
		this->addChild(ground);
		this->addChild(ball);


		if (onStartPage) {
			startpage = Sprite::create("startpage.jpg");
			startpage->setPosition(500, 275);
			this->addChild(startpage);
		}


		// Contact listener
		auto contactListener = EventListenerPhysicsContact::create();
		contactListener->onContactBegin = CC_CALLBACK_1(HelloWorld::onContactBegin, this);
		getEventDispatcher()->addEventListenerWithSceneGraphPriority(contactListener, this);


		// Move listener
		auto moveEvent = EventListenerKeyboard::create();
		Director::getInstance()->getOpenGLView()->setIMEKeyboardState(true);
		moveEvent->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* event) {
			switch (keyCode) {

				case EventKeyboard::KeyCode::KEY_LEFT_ARROW:    
					if (currPage == 1) movement = 'l'; moving = true; break;

				case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:	
					if (currPage == 1) movement = 'r'; moving = true; break;

				case EventKeyboard::KeyCode::KEY_SPACE:
					switch (currPage) {
					case 0:	// START
						currPage = 1;
						onStartPage = false;
						startpage->runAction(MoveTo::create(1, Vec2(500, -550)));
						this->removeChild(startpage);
						ball->getPhysicsBody()->setVelocity(Vec2(400, 400));
						break;
					case 3:	// GAME OVER
						this->removeAllChildren();
						init();
						ball->getPhysicsBody()->setVelocity(Vec2(300, 400)); 
					} break;

				case EventKeyboard::KeyCode::KEY_ESCAPE:
					if (currPage == 0 || currPage == 3)
						Director::getInstance()->end();
			};
		};
		moveEvent->onKeyReleased = [=](EventKeyboard::KeyCode keyCode, Event* event) {

			switch (keyCode) {
			case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
			case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:	moving = false; break;
			}
			
		};
		getEventDispatcher()->addEventListenerWithSceneGraphPriority(moveEvent, this);

		if (onStartPage)
			currPage = 0;
		else
			currPage = 1;

		this->scheduleUpdate();

	}
	return true;
}

bool HelloWorld::onContactBegin(PhysicsContact &contact) {
	/*
	Bitmasks:
	1 - ball
	2 - ground
	3 - platform / walls
	4 - bricks
	*/

	PhysicsBody *a = contact.getShapeA()->getBody();
	PhysicsBody *b = contact.getShapeB()->getBody();
	int A = a->getCollisionBitmask();
	int B = b->getCollisionBitmask();
	int tag_a = a->getTag();
	int tag_b = b->getTag();


	// ball / ground
	if (collided(1, 2, A, B)) {
		if (currPage != 3) {
			ball->getPhysicsBody()->setVelocity(Vec2(0, 0));
			currPage = 3;
			endpage = Sprite::create("endpage.png");
			endpage->setPosition(500, 275);
			this->addChild(endpage);
		}
	}

	// ball / brick
	if (collided(1, 4, A, B)) {
		if (tag_a != 0) {
			this->removeChildByTag(tag_a);
		}
		else if (tag_b != 0)
			this->removeChildByTag(tag_b);	

		if (this->getChildrenCount() == 4) {
			currPage = 3;
			winpage = Sprite::create("YouDidIt.jpg");
			winpage->setPosition(500, 275);
			this->addChild(winpage);
		}
	}

	return true;
}

bool HelloWorld::collided(int x, int y, int a, int b) {
	if ((x == a && y == b) || (x == b && y == a)) return true;
	else return false;
}

void HelloWorld::update(float delta) {

	if (currPage != 3 && moving) {
		int x = platform->getPositionX();
		switch (movement) {
		case 'r': x += 10; break;
		case 'l': x -= 10;
		}
		if(x >= 100 && x <= 900)
			platform->setPositionX(x);
	}

}