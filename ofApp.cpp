#include "ofApp.h"



BaseObject::BaseObject() {
	trans = ofVec3f(0,0,0);
	scale = ofVec3f(1, 1, 1);
	rot = 0;
}

void BaseObject::setPosition(ofVec3f pos) {
	trans = pos;
}

//
// Basic Sprite Object
//
Sprite::Sprite() {
	speed = 0;
	velocity = ofVec3f(0, 0, 0);
	lifespan = -1;      // lifespan of -1 => immortal 
	birthtime = 0;
	bSelected = false;
	haveImage = false;
	name = "UnamedSprite";
	width = 40;
	height = 80;
}

// Return a sprite's age in milliseconds
//
float Sprite::age() {
	return (ofGetElapsedTimeMillis() - birthtime);
}

//  Set an image for the sprite. If you don't set one, a rectangle
//  gets drawn.
//
void Sprite::setImage(ofImage img) {
	image = img;
	haveImage = true;
	width = image.getWidth();
	height = image.getHeight();
}


//  Render the sprite
//
void Sprite::draw() {

	ofSetColor(ofColor::white);

	// draw image centered and add in translation amount
	//
	if (haveImage) {
		image.draw(-width / 2.0 + trans.x, -height / 2.0 + trans.y);
	}
	else {
		// in case no image is supplied, draw something.
		// 
		ofSetColor(ofColor::ghostWhite);
		ofDrawRectangle(-width / 2.0 + trans.x, -height / 2.0 + trans.y, width, height);
	}
}

//  Add a Sprite to the Sprite System
//
void SpriteSystem::add(Sprite s) {
	sprites.push_back(s);
}

// Remove a sprite from the sprite system. Note that this function is not currently
// used. The typical case is that sprites automatically get removed when the reach
// their lifespan.
//
void SpriteSystem::remove(int i) {
	sprites.erase(sprites.begin() + i);
}

// remove all sprites within a given dist of point, return number removed
//
int SpriteSystem::removeNear(ofVec3f point, float dist) {
	vector<Sprite>::iterator s = sprites.begin();
	vector<Sprite>::iterator tmp;
	int count = 0;

	while (s != sprites.end()) {
		ofVec3f v = s->trans - point;
		if (v.length() < dist) {
			tmp = sprites.erase(s);
			count++;
			s = tmp;
			if (haveSound) {
				collideSound.play();
			}
		}
		else s++;
	}
	return count;
}

//  Update the SpriteSystem by checking which sprites have exceeded their
//  lifespan (and deleting).  Also the sprite is moved to it's next
//  location based on velocity and direction.
//
void SpriteSystem::update() {

	if (sprites.size() == 0) return;
	vector<Sprite>::iterator s = sprites.begin();
	vector<Sprite>::iterator tmp;

	// check which sprites have exceed their lifespan and delete
	// from list.  When deleting multiple objects from a vector while
	// traversing at the same time, use an iterator.
	//
	while (s != sprites.end()) {
		if (s->lifespan != -1 && s->age() > s->lifespan) {
//			cout << "deleting sprite: " << s->name << endl;
			tmp = sprites.erase(s);
			s = tmp;
		}
		else s++;
	}

	//  Move sprite
	//
	for (int i = 0; i < sprites.size(); i++) {
		sprites[i].trans += sprites[i].velocity / ofGetFrameRate();
	}
}

//  Render all the sprites
//
void SpriteSystem::draw() {
	for (int i = 0; i < sprites.size(); i++) {
		sprites[i].draw();
	}
}

//  Create a new Emitter - needs a SpriteSystem
//
Emitter::Emitter(SpriteSystem *spriteSys) {
	sys = spriteSys;
	lifespan = -1;    // milliseconds
	started = false;
	sys->noChild = noChild;
	sys->setNo = setNo;
	lastSpawned = 0;
	rate = 1;    // sprites/sec
	haveChildImage = false;
	haveImage = false;
	velocity = ofVec3f(100, 100, 0);
	drawable = true;
	width = 50;
	height = 50;
	childWidth = 10;
	childHeight = 10;
}

//  Draw the Emitter if it is drawable. In many cases you would want a hidden emitter
//
//
void Emitter::draw() {
	//rotate the emitter
	ofPushMatrix();
	ofTranslate(trans.x, trans.y, 0);//move pivot to centre
	ofRotate(rot);//rotate from centre
	ofPushMatrix();
	ofTranslate(-trans.x, -trans.y, 0);//move back by the centre offset
	//draw image
	if (drawable) {
		if (haveImage) {
			ofSetColor(ofColor::white);
			image.draw(-image.getWidth() / 2.0 + trans.x, -image.getHeight() / 2.0 + trans.y);
		}
	}
	ofPopMatrix();
	ofPopMatrix();
	// draw sprite system
	//
	sys->draw();

	
}

//  Update the Emitter. If it has been started, spawn new sprites with
//  initial velocity, lifespan, birthtime.
//
void Emitter::update() {
	if (!started) {
		for (int i = 0; i < sys->sprites.size(); i++) {
			sys->remove(i);
		}
	}
	else {
		float time = ofGetElapsedTimeMillis();
		if (setNo) {
			if (count < noChild) {
				// spawn a new sprite
				Sprite sprite;
				if (haveChildImage) sprite.setImage(childImage);
				sprite.velocity = velocity;
				sprite.lifespan = lifespan;
				sprite.setPosition(trans);
				sprite.birthtime = time;
				sprite.width = childWidth;
				sprite.height = childHeight;
				sys->add(sprite);
				count += 1;
				lastSpawned = time;
				
			}
			
			
		}
		else {
			
			if ((time - lastSpawned) > (1000.0 / rate)) {
				// spawn a new sprite
				Sprite sprite;
				if (haveChildImage) sprite.setImage(childImage);
				sprite.velocity =velocity;
				sprite.lifespan = lifespan;
				
				sprite.birthtime = time;
				sprite.width = childWidth;
				sprite.height = childHeight;
				//sprite starts from the top of the emitter, not in the middle
				glm::vec3 sprite_pos = glm::normalize(velocity) * 30;
				sprite.setPosition(trans + sprite_pos);

				sprite.birthtime = time;
				sys->add(sprite);
				lastSpawned = time;
			}
		}
		sys->update();
	}
	
}

// Start/Stop the emitter.
//
void Emitter::start() {
	started = true;
	lastSpawned = ofGetElapsedTimeMillis();
	count = 0;
}

void Emitter::stop() {
	started = false;
	
}


void Emitter::setLifespan(float life) {
	lifespan = life;
}

void Emitter::setVelocity(ofVec3f v) {
	velocity = v;
}

void Emitter::setChildImage(ofImage img) {
	childImage = img;
	haveChildImage = true;
	childWidth = img.getWidth();
	childHeight = img.getHeight();
}

void Emitter::setImage(ofImage img) {
	image = img;
	haveImage = true;
	width = img.getWidth();
	height = img.getHeight();
}

void Emitter::setRate(float r) {
	rate = r;
}
void Emitter::integrate() {
	trans += velocity / ofGetFrameRate();
	velocity += acceleration / ofGetFrameRate();
	velocity = velocity * damping;

}
void Background::draw() {
	if (haveImage) {
		backgroundImage.resize(ofGetWindowWidth(), ofGetWindowHeight());
		ofImage bg1 = backgroundImage;
		backgroundImage.draw(position.x, position.y);
		
		bg1.draw(position.x, position.y - ofGetWindowHeight());
	}
}



//--------------------------------------------------------------
void ofApp::setup(){
	ofSetVerticalSync(true);
	
	// set up background image
	if (bg.backgroundImage.load("images/background.png")) {
		bg.haveImage = true;
	}
	bg.position =  ofVec3f(0, 0, 0);
	
	
	//set up background sound
	backgroundSound.load("sounds/background.mp3");
	backgroundSound.setLoop(true);
	backgroundSound.setVolume(0.3f);
	backgroundSound.play();

	//set up opening sound
	openingSound.load("sounds/opening.mp3");
	openingSound.play();

	//set up explosion sound of the ship
	explSound.load("sounds/blast.mp3");

	levelupSound.load("sounds/up.mp3");

	// set up play area for the turret or ship
	playarea = ofRectangle(20,20,ofGetWindowWidth()-20, ofGetWindowHeight()-20);
	
	

		
	// Create and setup emitters  
	//
	gun = new Emitter(new SpriteSystem());
	life = new Emitter(new SpriteSystem());
	alien1 = new Emitter(new SpriteSystem());
	alien2 = new Emitter(new SpriteSystem());
	alien3 = new Emitter(new SpriteSystem());
	alien4 = new Emitter(new SpriteSystem());
	alien5 = new Emitter(new SpriteSystem());
	
	// Set up  the gun/missile launcher
	// gun image and sound
	gunImage.load("images/rocket.png");
	
	if (gunSound.load("sounds/missle.mp3")) {
		haveSound = true;
	}
	//load missle image
	if (missleImage.load("images/missle.png")) {
		missleLoaded = true;
	}
	else {
		ofLogFatalError("can't load image: missle.png");
		ofExit();
	}
	gunSound.setLoop(true);
	gunSound.setVolume(0.3f);

	gun->setImage(gunImage);
	gun->setChildImage(missleImage);
	gun->setPosition(ofVec3f(ofGetWindowWidth() / 2.0, ofGetWindowHeight(), 0));
	gun->setVelocity(ofVec3f(0, -1000, 0));
	gun->ver_velocity = glm::vec3(0, 0, 0);
	gun->hor_velocity = glm::vec3(0,0, 0);
	gun->setRate(3);
	gun->setLifespan(ofGetWindowHeight()); //ms
	gun->speed = glm::length(gun->ver_velocity);
	gun->acceleration = glm::vec3(0, 0, 0);
	gun->damping = 0.99;
	gun->rot = 0;
	gun->angle = 0;


	// Set up  the bonus launcher
	// bonus image and sound
	pillImage.load("images/pill.png");
	pillImage.resize(75, 75);
	
	dropSound.load("sounds/bonus_drop.mp3");
	life->drawable = false;
	life->setChildImage(pillImage);
	life->childImage.resize(50, 50);
	life->setPosition(ofVec3f(ofRandom(0,ofGetWindowWidth()), 0, 0));
	life->setVelocity(ofVec3f(0, 200, 0));
	life->noChild = 1;
	life->setNo = true;
	life->setLifespan(7000); //ms
	if (life->sys->collideSound.load("sounds/bonus.mp3")) {
		life->sys->haveSound = true;
		
	}

	// Set up some reasonable parameters for the invader spirtes
	// invader 1
	alien1Image.load("images/alien1.png");
	alien1Image.resize(50,50);
	alien1->drawable = false;
	alien1->setPosition(ofVec3f(ofGetWindowWidth() / 2, 10, 0));
	alien1->setChildImage(alien1Image);
	alien1->childImage.resize(50, 50);
	alien1->velocity = glm::vec3(0, 200, 0);
	alien1->setLifespan(5000);
	alien1->setRate(currentplaytime / (1000 * 60)*0.1 + 1);
	alien1->setChildSize(50, 50);
	if (alien1->sys->collideSound.load("sounds/blast.mp3")) {
		alien1->sys->haveSound = true;
		alien1->sys->collideSound.setVolume(0.3f);
	}
	// insert to list of invaders
	aliens.push_back(alien1);
	
	// invader 2
	alien2Image.load("images/alien2.png");
	alien2->drawable = false; // make emitter itself invisible
 	alien2->setPosition(ofVec3f(ofGetWindowWidth() / 3, 10, 0));
	                
	alien2->setChildImage(alien2Image);
	alien2->childImage.resize(40, 40);
	alien2->velocity = glm::vec3(0, 300, 0);
	alien2->setLifespan(7000);
	alien2->setRate(currentplaytime/(1000*60)*0.1+0.5);
	alien2->setChildSize(alien2->childImage.getWidth(), alien2->childImage.getHeight());
	if (alien2->sys->collideSound.load("sounds/blast.mp3")) {
		alien2->sys->haveSound = true;
		alien2->sys->collideSound.setVolume(0.3f);
	}
	// insert to list of invaders
	aliens.push_back(alien2);
	
	// invader 3
	alien3Image.load("images/alien3.png");
	alien3->drawable = false; // make emitter itself invisible
	alien3->setPosition(ofVec3f(ofGetWindowWidth(), ofGetWindowHeight()/3, 0));
	alien3->setChildImage(alien3Image);
	alien3->childImage.resize(50, 50);
	alien3->velocity = glm::vec3(0, 400, 0);
	alien3->setLifespan(7000);
	alien3->setRate(currentplaytime / (1000 * 60)*0.1 + 0.5);
	alien3->setChildSize(alien3->childImage.getWidth(), alien3->childImage.getHeight());
	if (alien3->sys->collideSound.load("sounds/blast.mp3")) {
		alien3->sys->haveSound = true;
		alien3->sys->collideSound.setVolume(0.3f);
	}
	aliens.push_back(alien3);
	
	// invader 4
	alien4Image.load("images/alien4.png");
	alien4->drawable = false; // make emitter itself invisible
	alien4->setPosition(ofVec3f(ofGetWindowWidth() / 3, 10, 0));

	alien4->setChildImage(alien4Image);
	alien4->childImage.resize(40, 40);
	alien4->velocity = glm::vec3(0, 500, 0);
	alien4->setLifespan(7000);
	alien4->setRate(currentplaytime / (1000 * 60)*0.1 + 0.5);
	alien4->setChildSize(alien4->childImage.getWidth(), alien4->childImage.getHeight());
	if (alien4->sys->collideSound.load("sounds/blast.mp3")) {
		alien4->sys->haveSound = true;
		alien4->sys->collideSound.setVolume(0.3f);
	}
	// insert to list of invaders
	aliens.push_back(alien4);

	// invader 5
	alien5Image.load("images/alien5.png");
	alien5->drawable = false; // make emitter itself invisible
	alien5->setPosition(ofVec3f(0, ofGetWindowHeight()* 2/3, 0));
	alien5->setChildImage(alien5Image);
	alien5->childImage.resize(50, 50);
	alien5->velocity = glm::vec3(0, 400, 0);
	alien5->setLifespan(7000);
	alien5->setRate(currentplaytime / (1000 * 60)*0.1 + 0.5);
	alien5->setChildSize(alien5->childImage.getWidth(), alien5->childImage.getHeight());
	if (alien5->sys->collideSound.load("sounds/blast.mp3")) {
		alien5->sys->haveSound = true;
		alien5->sys->collideSound.setVolume(0.3f);
	}
	aliens.push_back(alien5);


	// set up the emitter forces
	//
	turbForce = new TurbulenceForce(ofVec3f(-20, -20, -20), ofVec3f(20, 20, 20));
	gravityForce = new GravityForce(ofVec3f(0, -10, 0));
	radialForce = new ImpulseRadialForce(2000.0);

	// set up the explosion force of invaders
	
	expEmit.sys->addForce(turbForce);
	expEmit.sys->addForce(gravityForce);
	expEmit.sys->addForce(radialForce);

	expEmit.setPosition(ofVec3f(ofGetWindowWidth()/2, ofGetWindowHeight()/2, 0));
	expEmit.setOneShot(true);
	expEmit.setEmitterType(RadialEmitter);
	expEmit.setGroupSize(50);
	expEmit.setLifespan(0.5);
	expEmit.setVelocity(ofVec3f(0, 200, 0));
	expEmit.setRate(5.0);
	expEmit.setParticleRadius(1);
	
	// set up the explosion effect for the ship
	
	expEmitShip.sys->addForce(turbForce);
	expEmitShip.sys->addForce(gravityForce);
	expEmitShip.sys->addForce(radialForce);

	expEmitShip.setOneShot(true);
	expEmitShip.setEmitterType(RadialEmitter);
	expEmitShip.setGroupSize(100);
	expEmitShip.setLifespan(1);
	expEmitShip.setVelocity(ofVec3f(0, 400, 0));
	expEmitShip.setRate(5.0);
	expEmitShip.setParticleRadius(2);
	
	// set up thruster fire effect for the ship
	thrusterShip.sys->addForce(turbForce);
	thrusterShip.sys->addForce(gravityForce);
	
	thrusterShip.setPosition(gun->trans);
	thrusterShip.setEmitterType(DiscEmitter);
	thrusterShip.setGroupSize(100);
	thrusterShip.setLifespan(0.5);
	thrusterShip.setVelocity(ofVec3f(0, 100, 0));
	thrusterShip.setRate(5.0);
	thrusterShip.radius = 5;
	thrusterShip.setParticleRadius(1);
	thrusterShip.start();

	
	//gui.add(rate.setup("rate", 2, 1, 10));
	
	bHide = false;
	
	// start program with an idle mode only start when the space bar is pressed the first time
	startAnim = false;
	bSpaceDown = false;
	//gameStartTime = ofGetElapsedTimeMillis();
	gameOver = false;
	score = 0; // restart score
	hit = 0; // restart gun hitted number
	gunLife = 3; // set player health = 3 lives
	mouseLast = gun->trans;
	
	level = 0;
	instruction = false; // press i to access instruction
}

//--------------------------------------------------------------
void ofApp::update() {
	//scrolling background
	if (startAnim) {
		if (bg.position.y > ofGetWindowHeight())
		{
			bg.position.y = 0;
		}
		bg.position.y += 0.01 * ofGetFrameRate();
		float angle;
		switch (moveDir)
		{
		case MoveUp:
			move = 1;
			gun->acceleration =glm::vec3(0,250,0);
			gun->ver_velocity += gun->acceleration / ofGetFrameRate();
			gun->trans -= gun->ver_velocity / ofGetFrameRate();
			break;
		case MoveDown:
			move = 2;
			gun->acceleration = glm::vec3(0, 250, 0);
			gun->ver_velocity += gun->acceleration / ofGetFrameRate();
			gun->trans += gun->ver_velocity / ofGetFrameRate();
			break;
		case MoveLeft:
			move =3;
			gun->acceleration = glm::vec3(250, 0, 0);
			gun->hor_velocity += gun->acceleration / ofGetFrameRate();
			gun->trans -= gun->hor_velocity / ofGetFrameRate();
			break;
		case MoveRight:
			move = 4;
			gun->acceleration = glm::vec3(250, 0, 0);
			gun->hor_velocity += gun->acceleration / ofGetFrameRate();
			gun->trans += gun->hor_velocity / ofGetFrameRate();
			break;
		case MoveStop:

			gun->acceleration = glm::vec3(0, 0, 0);
			if (glm::length(gun->ver_velocity) >gun->speed) {
				gun->ver_velocity = gun->ver_velocity * gun->damping;
				if (move == 1 ) { //move foward
					gun->trans -=gun->ver_velocity / ofGetFrameRate();
				}
				if (move == 2 ) { //move backward
					gun->trans += gun->ver_velocity / ofGetFrameRate();
				}
				
			}
			if (glm::length(gun->hor_velocity) > gun->speed) {
				gun->hor_velocity = gun->hor_velocity * gun->damping;
				if (move == 3) { //move left
					gun->trans -= gun->hor_velocity / ofGetFrameRate();
				}
				if (move == 4) { //move right
					gun->trans += gun->hor_velocity / ofGetFrameRate();
				}
			}
			break;
		}
		
	}
	
	
	// update explosion effects when an invasion is defeated
	expEmit.update();

	// update explosion effects when gun object is defeated
	expEmitShip.update();

	// update thruster effect in the tail of the ship as flying up 
	thrusterShip.update();
	thrusterShip.setPosition(gun->trans);
	
	// update gun velocity as rotate angle, rate as rate in gui 
	gun->update();
	
	
	// Every 3 level up, the gun rate will increase 15%
	if (level % 3 == 0) {
		if (levelup) {
			gun->rate *= 1.5;
			levelupSound.play(); // a sound effect play whenever the rate in upgrade
			levelup = false;
		}
	}
	else {
		levelup = true;
	}
	
	if (!startAnim) {
		gun->trans.y -= 3;
		if (gun->trans.y <= (ofGetWindowHeight() * 2 / 3)) {
			gun->trans.y = (ofGetWindowHeight() * 2 / 3);
		}
	}
	
	//set angle of the ship object
	//ship.angle = fireAngle;
	
	//start and update base on level of difficulty 
	if (startAnim) {
		if (level <= aliens.size()) {
			for (int i = 0; i < level; i++) {
				Emitter *alien = aliens[i];
				if (alien->started) {
					alien->update();
				}
				else {
					alien->start();
				}
			}
		}
		else {
			for (int i = 0; i < aliens.size(); i++) {
				Emitter *alien = aliens[i];
				if (alien->started) {
					alien->update();
				}
				else {
					alien->start();
				}
			}
		}
	}
	
	
	// check for collisions between missles and invaders
	//  
	checkCollisions();

	
	// we will randomize initial velocity so that not the invaders
	
	
	// invader 1 update
	ofVec3f v = alien1->velocity;
	alien1->setVelocity(ofVec3f(ofRandom(-v.y / 2, v.y / 2), v.y, v.z));
	alien1->trans.x = ofRandom(0, ofGetWindowWidth());
	
	// invader 2 update
	alien2->trans.x = ofRandom(0, ofGetWindowWidth());
	for (int i = 0; i < alien2->sys->sprites.size(); i++) {
		ofVec3f p = alien2->sys->sprites[i].trans;
		alien2->sys->sprites[i].velocity = 60 * (curveEvaly(p.y + 2,150, 4) - curveEvaly(p.y,150, 4));
	}

	// invader 3 update
	alien3->trans.y = ofRandom(0, ofGetWindowHeight() * 3 / 4);
	for (int i = 0; i < alien3->sys->sprites.size(); i++) {
		ofVec3f p = alien3->sys->sprites[i].trans;
		alien3->sys->sprites[i].velocity = 60 * (curveEval(p.x, 100, 4) - curveEval(p.x+2, 100, 4));
	}
	
	// invader 4 update

	for (int i = 0; i < alien4->sys->sprites.size(); i++) {
		ofVec3f p = alien4->sys->sprites[i].trans;
		alien4->sys->sprites[i].velocity = 60 * (curveEvaly(p.y + 2, 50, 10) - curveEvaly(p.y, 50, 10));
	}

	// invader 5 update
	alien5->trans.y = ofRandom(ofGetWindowHeight()/4, ofGetWindowHeight()*3/4);
	for (int i = 0; i < alien5->sys->sprites.size(); i++) {
		ofVec3f p = alien5->sys->sprites[i].trans;
		alien5->sys->sprites[i].velocity = 60 * (curveEval(p.x + 2, 10, level) - curveEval(p.x, 10, level));
	}

	// game runs until all lives of gun run out
	//
	float t = ofGetElapsedTimeMillis();
	if (startAnim) {
		if (gunLife == 0) {
			gunLife = -1;
			// ship explosion
			expEmitShip.setPosition(gun->trans);
			expEmitShip.start();
			explSound.play();
			thrusterShip.stop();
			gun->stop();
			gunSound.stop();

			//set gameOver
			gameOver = true;

			playtime = (t - gameStartTime) / 1000;

			// remove all the invaders		
			aliens.clear();

		}
	}
	

	//check for playing area bound
	if (gun->trans.x < playarea.getLeft()) {
		gun->trans.x = playarea.getLeft();
	}
	else if (gun->trans.x > playarea.getRight()) {
		gun->trans.x = playarea.getRight();
	}
	if (gun->trans.y > playarea.getBottom()) {
		gun->trans.y = playarea.getBottom();
	}
	else if (gun->trans.y < playarea.getTop()) {
		gun->trans.y = playarea.getTop();
	}

	// level is calculate as quotient of 20 
	level = score/10 + 1;
	if (startAnim) {
		t = ofGetElapsedTimeMillis();
		currentplaytime = t - gameStartTime;
		
		//set up bonus life
		life->update();
		life->setPosition(ofVec3f(ofRandom(0, ofGetWindowWidth()), 0, 0));

		//every 20 seconds there will  be a bonus item drop down
		if (int(currentplaytime) % 20000 <= 20) {
			//cout << "current play time" << int(currentplaytime) % 30000 << ";";
			life->start();
			dropSound.play();
		}
	}
		
}


//--------------------------------------------------------------
void ofApp::draw(){
	//draw background image
	ofSetBackgroundColor(ofColor::black);
	ofDisableDepthTest();
	bg.draw();
	ofDisableDepthTest();
	
	//show path of the invaders
	if (showPath) {
		for (int i = 0; i < ofGetHeight(); i++) {
			ofVec3f p = curveEvaly(i, 100, 4);
			//p1.y, 0.5, 10
			ofDrawCircle(p.x, p.y, 1);
		}
		for (int i = 0; i < ofGetWidth(); i++) {
			ofVec3f p = curveEval(i, 100, 4);
			//p1.y, 0.5, 10
			ofDrawCircle(p.x, p.y, 1);
		}
	}
	
	
	// Display instruction for player to start the game
	if (!startAnim) {
		ofSetColor(ofColor::white);
		ofDrawBitmapStringHighlight("Press spacebar to begin!", ofGetWidth() / 2 - 85, ofGetHeight() / 2 - 50);
		ofDrawBitmapStringHighlight("Press i for instruction", ofGetWidth() / 2 - 85, ofGetHeight() / 2 - 30);
	}

	// draw particle emitter to implement the explosion and thruster effect.
	ofSetColor(ofColor::white);
	expEmit.draw();
	expEmitShip.draw();
	thrusterShip.draw();

	
	// draw the invaders
	if (level <= aliens.size()) {
		for (int i = 0; i < level; i++) {
			Emitter *alien = aliens[i];
			alien->draw();
		}
	}
	else {
		for (int i = 0; i < aliens.size(); i++) {
			Emitter *alien = aliens[i];
			alien->draw();
		}
	}

	// The string text for game info
	string scoreText;
	scoreText += "Score: " + std::to_string(score);
	string lifeText;
	lifeText += "Lives: " + std::to_string(gunLife);
	
	// if game is over, draw a label in middle of screen with the High score and level
	//
	if (gameOver) {
		
		ofSetColor(ofColor::white);

		ofDrawBitmapStringHighlight("GAME OVER", ofPoint(ofGetWidth() / 2 - 85, ofGetHeight() / 2 - 50));
		ofDrawBitmapStringHighlight("Your High Score: "+ std::to_string(score), ofGetWidth() / 2 - 85, ofGetHeight() / 2 - 20);
		ofDrawBitmapStringHighlight("Your Level: " + std::to_string(level), ofGetWidth() / 2 - 85, ofGetHeight() / 2 +40);
		ofDrawBitmapStringHighlight("Total play time: " + std::to_string(int(playtime)), ofGetWidth() / 2 - 85, ofGetHeight() / 2 + 60);
		ofDrawBitmapStringHighlight("Press Enter to start again", ofGetWidth() / 2 - 85, ofGetHeight() / 2 + 80);

	}
	else {
		ofSetColor(ofColor::white);
		gun->draw();
		life->draw();
		if (instruction) {
			ofDrawBitmapStringHighlight("Press space bar to fire\nRelease to stop", ofGetWidth() / 2 - 85, ofGetHeight() / 2 - 50);
		}
		
		// draw current score, lives, level
		//
		if (startAnim) {
			ofDrawBitmapStringHighlight(scoreText, ofPoint(10, 20));
			ofDrawBitmapStringHighlight(lifeText, ofPoint(10, 40));
			ofDrawBitmapStringHighlight("Level " + std::to_string(level), ofPoint(10, 60));
		}
		
	}
}

ofVec3f ofApp::curveEval(float x, float scale, float cycles)
{
	// x is in screen coordinates and his in [0, WindowWidth]
	float u = (cycles * x * PI) / ofGetWindowWidth();
	return (ofVec3f(x, -scale * sin(u) + (ofGetWindowHeight() / 2), 0));
}

ofVec3f ofApp::curveEvaly(float y, float scale, float cycles)
{
	// x is in screen coordinates and his in [0, WindowWidth]
	float u = (cycles * y * PI) / ofGetWindowHeight();
	return (ofVec3f(-scale * sin(u) + (ofGetWindowWidth() / 2),y , 0));
}
//--------------------------------------------------------------

//  This is a simple O(M x N) collision check
//  For each missle check to see which invaders you hit and remove them
//
void ofApp::checkCollisions() {

	// find the distance at which the two sprites (missles and invaders) will collide
	// detect a collision when we are within that distance.
	//
	float collisionDist = gun->childHeight / 2 + alien1->childHeight / 2;
	float life_collisionDist = gun->height / 2 + alien1->childHeight / 2;
	float bonus_collisionDist = gun->height / 2 + life->childHeight / 2;

	if (life->sys->removeNear(gun->trans, bonus_collisionDist)) {
		gunLife += 1;
	}

	for (int i = 0; i < aliens.size(); i++) {
		Emitter *alien = aliens[i];
		
		float alien_collisionDist = gun->childHeight / 2 + alien->childHeight / 2;
		float life_alien_collisionDist = gun->height / 2 + alien->childHeight / 2;

		for (int i = 0; i < gun->sys->sprites.size(); i++) {
			//score += alien->sys->removeNear(gun->sys->sprites[i].trans, alien_collisionDist);
			if (alien->sys->removeNear(gun->sys->sprites[i].trans, alien_collisionDist)) {
				score += 1;
				expEmit.setPosition(gun->sys->sprites[i].trans);
				expEmit.start();
			}
			
		}
		if (alien->sys->removeNear(gun->trans, life_alien_collisionDist)) {
			gunLife -= 1;
		}
		//hit += alien->sys->removeNear(gun->trans, life_alien_collisionDist);
	}
	
	
	
	/*
	// Loop through all the missiles, then remove any invaders that are within
	// "collisionDist" of the missiles.  the removeNear() function returns the
	// number of missiles removed.
	//
	for (int i = 0; i < gun->sys->sprites.size(); i++) {
		score += invaders->sys->removeNear(gun->sys->sprites[i].trans, collisionDist);
		
	}

	hit += invaders->sys->removeNear(gun->trans, life_collisionDist);
	*/	

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
	
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	if (!bDrag) return;

	glm::vec3 mousePoint(x, y, 0);

	if (bCtrlKeyDown) {
		gun->rot += x - lastMouse.x;
	}
	else {
		gun->trans += mousePoint - lastMouse;
	}
	lastMouse = mousePoint;
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	bDrag = false;
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

void ofApp::keyPressed(int key) {
	switch (key) {
	case 'F':
	case 'f':
		ofToggleFullscreen();
		break;
	case 'H':
	case 'h':
		bHide = !bHide;
		break;
	case 'i':
		instruction=!instruction;
		break;
	case OF_KEY_CONTROL:
		bCtrlKeyDown = true;
		break;
	case OF_KEY_UP:
		moveDir = MoveUp;
		break;
	case OF_KEY_DOWN:
		moveDir = MoveDown;
		break;
	case OF_KEY_LEFT:
		moveDir = MoveLeft;
		break;
	case OF_KEY_RIGHT:
		moveDir = MoveRight;
		break;
	case ' ':
		if (!startAnim) {
			// Record when the game starts 
			startAnim = true;
			gameStartTime = ofGetElapsedTimeMillis();
			for (int i = 0; i < level; i++) {
				Emitter *alien = aliens[i];
				alien->started = true;
			}
		}
	
		 else if (!gameOver) {
			if (!gun->started) {
				gun->started = true;
				gunSound.play();
			}
		}
		break;
	case OF_KEY_SHIFT:
		showPath = !showPath;
		break;
	case 'x':
		gameOver = true;
		gun->started = false;
		thrusterShip.stop();
		aliens.clear();
		break;
	case OF_KEY_RETURN:
		setup();
		break;
	case 'm':
		life->start();
		dropSound.play();
		//expEmit.sys->reset();
		//expEmit.start();
		//expEmitShip.start();
		//thrusterShip.start();
		break;
	}
}


//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	switch (key) {
	case OF_KEY_LEFT:
	case OF_KEY_RIGHT:
	case OF_KEY_UP:
	case OF_KEY_DOWN:
		moveDir = MoveStop;
		break;
	case OF_KEY_ALT:
		break;
	case OF_KEY_CONTROL:
		break;
	case OF_KEY_SHIFT:
		break;
	case ' ':
		bSpaceDown = false;
		gun->started = false;
		gunSound.stop();
		break;
	}
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

