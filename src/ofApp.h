#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include  "ofxAssimpModelLoader.h"
#include "Octree.h"
#include "glm/gtx/intersect.hpp"
#include "../SpacecraftShape.h"
#include "ParticleEmitter.h"
#include "ParticleSystem.h"

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
	void setCameraTarget();
	glm::vec3 ofApp::getMousePointOnPlane(glm::vec3 p, glm::vec3 n);
	void resolveCollision();
	void checkCollision();
	void setUpClassicMode();
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

	void loadVbo();			//vertex buffer for exhaust
	void loadExplosionVbo(); //vertex buffer for explosion

	ParticleEmitter exhaustemitter;
	ParticleEmitter explosionemitter;
	bool explosiontriggered;

	float exhausttimer;
	float explosiontimer;

	// textures
	ofTexture  particleTex;

	// shaders
	ofVbo vbo, explosionvbo;
	ofShader shader, explosionshader;

	ofEasyCam cam;
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

	ofVec3f selectedPoint;
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
};
