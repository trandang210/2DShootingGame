#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ParticleEmitter.h"
#include "Particle.h"
#include "ParticleSystem.h"
#include "TransformObject.h"


typedef enum { MoveStop, MoveLeft, MoveRight, MoveUp, MoveDown } MoveDir;

// This is a base object that all drawable object inherit from
// It is possible this will be replaced by ofNode when we move to 3D
//
class BaseObject {
public:
	BaseObject();
	ofVec2f trans, scale;
	float	rot;
	bool	bSelected;
	void setPosition(ofVec3f);
};

//  General Sprite class  (similar to a Particle)
//
class Sprite : public BaseObject {
public:
	Sprite();
	void draw();
	float age();
	void setImage(ofImage);
	float speed;    //   in pixels/sec
	ofVec3f velocity; // in pixels/sec
	ofImage image;
	float birthtime; // elapsed time in ms
	float lifespan;  //  time in ms
	string name;
	bool haveImage = false;
	float width, height;  
	
};

//  Manages all Sprites in a system.  You can create multiple systems
//
class SpriteSystem  {
public:
	void add(Sprite);
	void remove(int);
	void update();
	int removeNear(ofVec3f point, float dist);
	void draw();
	vector<Sprite> sprites;
	ofSoundPlayer collideSound;
	bool haveSound = false;
	//vector<Sprite> emitters;
	int noChild;
	bool setNo = false;
};


//  General purpose Emitter class for emitting sprites
//  This works similar to a Particle emitter
//
class Emitter: public BaseObject {
public:
	Emitter(SpriteSystem *);
	
	void draw();
	void start();
	void stop();
	void setLifespan(float);      // in milliseconds
	void setVelocity(ofVec3f);    // pixel/sec
	void setChildImage(ofImage);
	void setChildSize(float w, float h) { childWidth = w; childHeight = h; }
	void setImage(ofImage);
	void setRate(float);
	void update();
	void integrate();
	float speed;
	SpriteSystem *sys; //pointer 
	float rate;
	//ofVec3f velocity;
	float lifespan;
	bool started;
	float lastSpawned;
	ofImage childImage;
	ofImage image;
	bool drawable;
	bool haveChildImage;
	bool haveImage =false;
	float width, height;
	float childWidth, childHeight;
	int noChild;
	bool setNo= false;
	int count=0;
	glm::vec3 velocity;
	glm::vec3 ver_velocity;
	glm::vec3 hor_velocity;
	glm::vec3 acceleration;
	float damping;
	float angle;
};

class Background {
	
public:
	ofImage home;
	ofImage backgroundImage;
	void draw();
	bool drawable;
	bool haveImage = false;
	bool drawHome = false;
	float width, height;
	ofVec3f position;
	

};


class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();
	void checkCollisions();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);


	Background bg;

	ofImage defaultImage;
	ofImage virus1Image;
	ofImage gunImage;
	ofImage alien1Image;
	ofImage alien2Image;
	ofImage alien3Image;
	ofImage alien4Image;
	ofImage alien5Image;
	ofImage alien6Image;
	ofImage pillImage;
	ofImage missleImage;

	
	ofSoundPlayer gunSound;
	ofSoundPlayer backgroundSound;
	ofSoundPlayer openingSound;
	ofSoundPlayer explSound;
	ofSoundPlayer dropSound;
	ofSoundPlayer levelupSound;
	
	bool missleLoaded;
	bool haveSound = false;
	bool imageLoaded;
	bool instruction;
	bool bHide;
	bool levelup =false;

	ParticleEmitter expEmit;
	ParticleEmitter expEmitShip;
	ParticleEmitter thrusterShip;
	

	// adding forces
	//
	TurbulenceForce *turbForce;
	GravityForce *gravityForce;
	ImpulseRadialForce *radialForce;


	
	glm::vec3 lastMouse;   // location of where mouse moved last (when dragging)

	// UI control data
	//
	bool bFullscreen = false;
	bool bCtrlKeyDown = false;
	bool bSpaceDown = false;
	bool bDrag = false;

	bool startAnim = false;

	
	float headingAngle;

	glm::vec3 origin = glm::vec3(0, -1, 0);
	glm::vec3 head;
	
	
	
	ofTrueTypeFont myfont;
	ofRectangle playarea;
	ofVec3f curveEval(float x, float scale, float cycles);
	ofVec3f curveEvaly(float y, float scale, float cycles);
	bool showPath = false;
	
	Emitter *gun;
	Emitter *life;
	Emitter *alien1, *alien2 , *alien3, *alien4, *alien5, *alien6;
	vector<Emitter *> aliens;
	int hit;
	int score;
	int gunLife;
	int level;
	float playtime;
	float gameStartTime;
	float currentplaytime;
	bool gameOver = false;
	ofVec3f mouseLast;
	MoveDir moveDir;
	int move = 0;

	
	


};
