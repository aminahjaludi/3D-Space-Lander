#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include  "ofxAssimpModelLoader.h"
#include "Octree.h"
#include "glm/gtx/intersect.hpp"
#include "../SpacecraftShape.h"
#include "ParticleEmitter.h"
#include "ParticleSystem.h"

struct LandingPadLights {
	ofLight keyLight;
	ofLight fillLight;
	ofLight rimLight;
	ofVec3f position; // Landing pad position
};


class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();

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
	void initLightingAndMaterials();
	glm::vec3 ofApp::getMousePointOnPlane(glm::vec3 p, glm::vec3 n);
	void resolveCollision();
	void checkCollision();
	void setupClassicMode();
	void calculateAltitude();
	void applyExternalForces();
	void moveUp();
	void moveDown();
	void rotateRight();
	void rotateLeft();
	void moveRight();
	void moveLeft();
	void moveForward();
	void moveBackwards();
	void triggerExplosion(glm::vec3&);	//triggers explosion when rocket crashes with terrain at a high force
	void triggerExhaust();
	void setupEmitters();
	void loadVbo();			//vertex buffer for exhaust
	void loadExplosionVbo(); //vertex buffer for explosion
	bool checkLanding();
	void displayMainMenu();
	void displayModes();
	void displayWin();
	void displayLoss();
	void printInfo();
	void setupLightSystem(); //function to set up lights

	ParticleEmitter exhaustemitter;
	ParticleEmitter explosionemitter;
	bool explosiontriggered;

	bool raySelectWithOctree(ofVec3f&);
	glm::vec3 getHeadingVector(float);
	glm::vec3 getRightVector(float);

	float exhausttimer;
	float explosiontimer;

	// textures
	ofTexture  particleTex;

	// shaders
	ofVbo vbo, explosionvbo;
	ofShader shader, explosionshader;

	ofCamera* currentCam = nullptr;

	ofEasyCam cam;

	ofCamera followCam; // Camera with view following lander
	glm::vec3 cameraPosition = glm::vec3(-60, 90, 70);  // fixed position of camera

	// The camera attached to the lander
	ofCamera fixedCam1;
	ofCamera fixedCam2;
	ofCamera fixedCam3;

	// ADDED
	bool pointSelected;
	ofVec3f selectedPoint;
	ofVec3f selectPos;

	ofLight light;

	ofxPanel gui;
	ofxToggle altitude_toggle;

	ofxAssimpModelLoader mars, lander;
	SpacecraftShape ship;
	Box boundingBox, landerBounds;

	Octree octree;
	vector<Box> colBoxList;
	vector<Box> bboxList;
	vector<ofColor> levelColors;
	TreeNode selectedNode;

	// heading vector
	glm::vec3 shipHeading = glm::vec3(0, 0, -1); // initial forward direction
	float shipRotation = 0; // rotation angle in degrees

	ofVec3f intersectPoint;
	glm::vec3 mouseDownPos;

	float altitude;

	ofTrueTypeFont titleFont;
	ofTrueTypeFont instructionFont;
	ofImage backgroundImg;

	//Booleans for lander movement
	bool bLanderLoaded = false;
	bool bLanderSelected = false;
	bool bInDrag = false;
	bool bCollisionDetected = false;
	bool up_pressed = false;
	bool down_pressed = false;
	bool left_pressed = false;
	bool right_pressed = false;
	bool w_pressed = false;
	bool s_pressed = false;
	bool d_pressed = false;
	bool a_pressed = false;

	bool bHide = false; //Boolean for hiding gui

	//Booleans for starting screen and choosing mode
	bool quit = false;
	bool restart = false;
	bool classic_mode = false;
	bool dragging_mode = false;
	bool lost = false;
	bool won = false;
	bool disableDragging = false;
	bool thrusting = false;
	
	int fuel = 120; //2 minutes of fuel
	uint64_t lastDecrementTime = 0;
	float game_endt = 0;
	int decrementInterval = 1000; //1 second in milliseconds

	//Sound objects
	ofSoundPlayer ambient;
	ofSoundPlayer thrust;
	//lighting
	LandingPadLights pads[3];
};
