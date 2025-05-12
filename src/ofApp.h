#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include  "ofxAssimpModelLoader.h"
#include "Octree.h"
#include "glm/gtx/intersect.hpp"
#include "../SpacecraftShape.h"

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
	void drawAxis(ofVec3f);
	void initLightingAndMaterials();
	void savePicture();
	void toggleWireframeMode();
	void togglePointsDisplay();
	void toggleSelectTerrain();
	void setCameraTarget();
	bool mouseIntersectPlane(ofVec3f planePoint, ofVec3f planeNorm, ofVec3f& point);
	bool raySelectWithOctree(ofVec3f& pointRet);
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

	ofEasyCam cam;
	ofxAssimpModelLoader mars, lander;
	ofLight light;
	Box boundingBox, landerBounds;
	vector<Box> colBoxList;
	bool bLanderSelected = false;
	Octree octree;
	TreeNode selectedNode;
	glm::vec3 mouseDownPos; //mouseLastPos;
	bool bInDrag = false;

	ofxPanel gui;
	ofxToggle altitude_toggle;

	ofTrueTypeFont titleFont;
	ofTrueTypeFont instructionFont;
	ofImage backgroundImg;

	bool bAltKeyDown;
	bool bCtrlKeyDown;
	bool bWireframe;
	bool bDisplayPoints;
	bool bHide;
	bool pointSelected = false;
	bool bDisplayLeafNodes = false;
	bool bDisplayOctree = false;
	bool bDisplayBBoxes = false;

	bool bLanderLoaded;
	bool bTerrainSelected;
	bool bCollisionDetected = false;

	ofVec3f selectedPoint;
	ofVec3f intersectPoint;

	vector<Box> bboxList;

	const float selectionRange = 4.0;

	vector<ofColor> levelColors;

	bool up_pressed = false;
	bool down_pressed = false;
	bool left_pressed = false;
	bool right_pressed = false;
	bool w_pressed = false;
	bool s_pressed = false;
	bool d_pressed = false;
	bool a_pressed = false;
	bool quit = false;
	bool restart = false;
	bool classic_mode = false;
	bool dragging_mode = false;
	float landerLoadedTime = 0;
	float altitude;

	SpacecraftShape ship;
};
