
//--------------------------------------------------------------
//
//  Kevin M. Smith
//
//  Student Names: Aminah Jaludi, Mckenzie Lola

#include "ofApp.h"
#include "Util.h"
#include <sstream>
#include <iomanip>

//--------------------------------------------------------------
// setup scene, lighting, state and load geometry
//
void ofApp::setup() {

	//Setup camera
	cam.setDistance(10);
	cam.setNearClip(.1);
	cam.setFov(65.5);
	ofSetVerticalSync(true);
	cam.disableMouseInput();
	ofEnableSmoothing();
	ofEnableDepthTest();

	// setup rudimentary lighting 
	initLightingAndMaterials();

	mars.loadModel("geo/moon-houdini.obj");
	mars.setScaleNormalization(false);

	titleFont.load("Sen-Bold.ttf", 64);
	instructionFont.load("Sen-Regular.ttf", 24);
	backgroundImg.load("background.jpg");

	//Create sliders for testing
	gui.setup();
	gui.add(altitude_toggle.setup("Altitude Info", false));;
	bHide = false;

	//create Octree for testing
	octree.create(mars.getMesh(0), 20);;

	// Initialize the colors for each level of the subdivisions
	levelColors.clear();
	levelColors.push_back(ofColor::red);
	levelColors.push_back(ofColor::green);
	levelColors.push_back(ofColor::blue);
	levelColors.push_back(ofColor::yellow);
	levelColors.push_back(ofColor::purple);
	levelColors.push_back(ofColor::orange);
	levelColors.push_back(ofColor::cyan);
	levelColors.push_back(ofColor::magenta);
	levelColors.push_back(ofColor::limeGreen);
	levelColors.push_back(ofColor::pink);
}

//--------------------------------------------------------------
// incrementally update scene (animation)
void ofApp::update() {
	if (restart) {
		if (bLanderLoaded) {

			//Move lander in accordance to key pressed
			if (w_pressed) {
				moveUp();
			}
			if (s_pressed) {
				moveDown();
			}
			if (d_pressed) {
				rotateRight();
			}
			if (a_pressed) {
				rotateLeft();
			}
			if (up_pressed) {
				moveBackwards();
			}
			if (down_pressed) {
				moveForward();
			}
			if (right_pressed) {
				moveRight();
			}
			if (left_pressed) {
				moveLeft();
			}

			applyExternalForces();
			ship.integrator();

			lander.setPosition(ship.pos.x, ship.pos.y, ship.pos.z);
			lander.setRotation(0, ship.rot, 0, 1, 0);

			calculateAltitude();
		}
		checkCollision();

		if (bCollisionDetected) {
			resolveCollision();
		}
	}
	else {
		lander.clear();
		bLanderLoaded = false;
		bCollisionDetected = false;
	}
}

//--------------------------------------------------------------
void ofApp::draw() {

	ofBackground(ofColor::black);

	// Get window center
	float centerX = ofGetWidth() / 2.0;

	if (!restart) {
		ofDisableLighting();
		ofBackground(20); // dark background
		ofSetColor(255);  // white text

		string title = "3D LANDER GAME";
		string instruction1 = "Press E to Start, and Q to quit";
		string instruction2 = "Use Arrow Keys and AWSD Keys to Move";

		// Measure string widths for centering
		float titleWidth = titleFont.stringWidth(title);
		float instr1Width = instructionFont.stringWidth(instruction1);
		float instr2Width = instructionFont.stringWidth(instruction2);

		// Draw centered
		titleFont.drawString(title, centerX - titleWidth / 2, 200);
		instructionFont.drawString(instruction1, centerX - instr1Width / 2, 300);
		instructionFont.drawString(instruction2, centerX - instr2Width / 2, 400);
	}
	else if (restart && !dragging_mode && !classic_mode) {

		string title = "SELECT GAME MODE";
		string instruction1 = "[1] Classic Mode : fixed lander starting position";
		string instruction2 = "[2] Dragging Mode : drag lander to desired starting position";

		// Measure string widths for centering
		float titleWidth = titleFont.stringWidth(title);
		float instr1Width = instructionFont.stringWidth(instruction1);
		float instr2Width = instructionFont.stringWidth(instruction2);

		// Draw centered
		titleFont.drawString(title, centerX - titleWidth / 2, 200);
		instructionFont.drawString(instruction1, centerX - instr1Width / 2, 300);
		instructionFont.drawString(instruction2, centerX - instr2Width / 2, 400);
	}
	else {
		glDepthMask(false);

		backgroundImg.draw(0, 0, ofGetWidth(), ofGetHeight());
		if (!bHide) gui.draw();
		
		glDepthMask(true);

		if (altitude_toggle) {
			ostringstream oss;
			oss << fixed << setprecision(1) << "Altitude is " << altitude;
			string altitude_str = oss.str();

			float x = ofGetWidth() - 140;
			float y = 20;
			ofSetColor(ofColor::white);
			ofDrawBitmapString(altitude_str, x, y);
		}
		cam.begin();

		ofPushMatrix();

		ofEnableLighting(); // shaded mode
		mars.drawFaces();
		ofMesh mesh;
		if (bLanderLoaded) {
			lander.drawFaces();

			if (bLanderSelected) {
				ofVec3f min = lander.getSceneMin() + lander.getPosition();
				ofVec3f max = lander.getSceneMax() + lander.getPosition();

				Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
				ofSetColor(ofColor::white);
				Octree::drawBox(bounds);
			}
			ship.draw();
		}
		
		ofDisableLighting();
		ofPopMatrix();
		cam.end();
	}

}

void ofApp::keyPressed(int key) {

	switch (key) {
	case '1':
		if (restart && !classic_mode && !dragging_mode) {
			classic_mode = true;
			dragging_mode = false;
			setUpClassicMode();
		}
		break;
	case '2':
		if (restart && !dragging_mode && !classic_mode) {
			classic_mode = false;
			dragging_mode = true;
		}
		break;
	case 'q':
	case 'Q':
		quit = true;
		restart = false;
		classic_mode = false;
		dragging_mode = false;
		break;
	case 'e':
	case 'E':
		restart = true;
		quit = false;
		break;
	case OF_KEY_UP:
		up_pressed = true;
		break;
	case OF_KEY_DOWN:
		down_pressed = true;
		break;
	case OF_KEY_RIGHT:
		right_pressed = true;
		break;
	case OF_KEY_LEFT:
		left_pressed = true;
		break;
	case 'w':
	case 'W':
		w_pressed = true;
		break;
	case 's':
	case 'S':
		s_pressed = true;
		break;
	case 'd':
	case 'D':
		d_pressed = true;
		break;
	case 'a':
	case 'A':
		a_pressed = true;
		break;
	case 'C':
	case 'c':
		if (cam.getMouseInputEnabled()) cam.disableMouseInput();
		else cam.enableMouseInput();
		break;
	case 'F':
	case 'f':
		ofToggleFullscreen();
		break;
	case 'r':
		cam.reset();
		break;
	case 't':
		setCameraTarget();
		break;
	case OF_KEY_ALT:
		cam.enableMouseInput();
		break;
	default:
		break;
	}
}

void ofApp::keyReleased(int key) {

	switch (key) {
	case OF_KEY_UP:
		up_pressed = false;
		break;
	case OF_KEY_DOWN:
		down_pressed = false;
		break;
	case OF_KEY_RIGHT:
		right_pressed = false;
		break;
	case OF_KEY_LEFT:
		left_pressed = false;
		break;
	case 'w':
	case 'W':
		w_pressed = false;
		break;
	case 's':
	case 'S':
		s_pressed = false;
		break;
	case 'd':
	case 'D':
		d_pressed = false;
		break;
	case 'a':
	case 'A':
		a_pressed = false;
		break;
	case OF_KEY_ALT:
		cam.disableMouseInput();
		break;
	default:
		break;
	}
}

//--------------------------------------------------------------

void ofApp::mouseMoved(int x, int y) {


}

//--------------------------------------------------------------

void ofApp::mousePressed(int x, int y, int button) {

	// if moving camera, don't allow mouse interaction
	if (cam.getMouseInputEnabled()) return;

	// if moving camera, don't allow mouse interaction
	if (cam.getMouseInputEnabled()) return;

	// if rover is loaded, test for selection
	if (bLanderLoaded) {
		glm::vec3 origin = cam.getPosition();
		glm::vec3 mouseWorld = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
		glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);

		ofVec3f min = lander.getSceneMin() + lander.getPosition();
		ofVec3f max = lander.getSceneMax() + lander.getPosition();

		Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
		bool hit = bounds.intersect(Ray(Vector3(origin.x, origin.y, origin.z), Vector3(mouseDir.x, mouseDir.y, mouseDir.z)), 0, 10000);
		if (hit) {
			bLanderSelected = true;
			mouseDownPos = getMousePointOnPlane(lander.getPosition(), cam.getZAxis());
			bInDrag = true;
		}
		else {
			bLanderSelected = false;
		}
	}
}

//--------------------------------------------------------------

void ofApp::mouseDragged(int x, int y, int button) {

	// if moving camera, don't allow mouse interaction
	if (cam.getMouseInputEnabled()) return;
	if (restart && classic_mode) return;

	if (bInDrag) {

		glm::vec3 landerPos = lander.getPosition();

		glm::vec3 mousePos = getMousePointOnPlane(landerPos, cam.getZAxis());
		glm::vec3 delta = mousePos - lander.getPosition();

		landerPos += delta;
		lander.setPosition(landerPos.x, landerPos.y, landerPos.z);
		ship.pos = lander.getPosition();

		ofVec3f min = lander.getSceneMin() + lander.getPosition();
		ofVec3f max = lander.getSceneMax() + lander.getPosition();

		Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));

		colBoxList.clear();
		octree.intersect(bounds, octree.root, colBoxList);

		if (colBoxList.size() >= 10) {
			bCollisionDetected = true;
		}
		else {
			bCollisionDetected = false;
		}
	}
}

//--------------------------------------------------------------

void ofApp::mouseReleased(int x, int y, int button) {
	bInDrag = false;
}

//--------------------------------------------------------------

// Set the camera to use the selected point as it's new target
void ofApp::setCameraTarget() {

}

//--------------------------------------------------------------

void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------

void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------

void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------

void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------

// setup basic ambient lighting in GL  (for now, enable just 1 light)
void ofApp::initLightingAndMaterials() {

	static float ambient[] =
	{ .5f, .5f, .5, 1.0f };
	static float diffuse[] =
	{ 1.0f, 1.0f, 1.0f, 1.0f };

	static float position[] =
	{ 5.0, 5.0, 5.0, 0.0 };

	static float lmodel_ambient[] =
	{ 1.0f, 1.0f, 1.0f, 1.0f };

	static float lmodel_twoside[] =
	{ GL_TRUE };


	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, position);


	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	//	glEnable(GL_LIGHT1);
	glShadeModel(GL_SMOOTH);
}

//--------------------------------------------------------------

// Support drag-and-drop of model (.obj) file loading.  when
// model is dropped in viewport, place origin under cursor
void ofApp::dragEvent(ofDragInfo dragInfo) {
	if (restart && dragging_mode) {
		if (lander.loadModel(dragInfo.files[0])) {
			bLanderLoaded = true;
			lander.setScaleNormalization(false);
			lander.setPosition(0, 0, 0);
			//cout << "number of meshes: " << lander.getNumMeshes() << endl;
			bboxList.clear();
			for (int i = 0; i < lander.getMeshCount(); i++) {
				bboxList.push_back(Octree::meshBounds(lander.getMesh(i)));
			}

			// Setup our rays
			glm::vec3 origin = cam.getPosition();
			glm::vec3 camAxis = cam.getZAxis();
			glm::vec3 mouseWorld = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
			glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);
			float distance;

			bool hit = glm::intersectRayPlane(origin, mouseDir, glm::vec3(0, 0, 0), camAxis, distance);
			if (hit) {
				glm::vec3 intersectPoint = origin + distance * mouseDir;

				// Now position the lander's origin at that intersection point
				glm::vec3 min = lander.getSceneMin();
				glm::vec3 max = lander.getSceneMax();
				float offset = (max.y - min.y) / 2.0;
				lander.setPosition(intersectPoint.x, intersectPoint.y - offset, intersectPoint.z);

				ship.pos = lander.getPosition();
				ship.scale = glm::vec3(0.1, 0.2, 0.1);

				// set up bounding box for lander while we are at it
				landerBounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
			}
		}
	}

}

//--------------------------------------------------------------

//  Intersect the mouse ray with the plane normal to the camera 
//  return intersection point.
glm::vec3 ofApp::getMousePointOnPlane(glm::vec3 planePt, glm::vec3 planeNorm) {
	
	glm::vec3 origin = cam.getPosition();
	glm::vec3 camAxis = cam.getZAxis();
	glm::vec3 mouseWorld = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
	glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);
	float distance;

	bool hit = glm::intersectRayPlane(origin, mouseDir, planePt, planeNorm, distance);

	if (hit) {
		glm::vec3 intersectPoint = origin + distance * mouseDir;
		return intersectPoint;
	}
	else return glm::vec3(0, 0, 0);
}

//--------------------------------------------------------------

void ofApp::resolveCollision() {
	glm::vec3 impulseForce = glm::vec3(0, 1, 0);  // World up

	impulseForce *= ship.thrust;
	ship.velocity += impulseForce * 2;
	ship.addForce(impulseForce * 2);

	ship.integrator();
	
	// Update bounding box after moving
	ofVec3f min = lander.getSceneMin() + lander.getPosition();
	ofVec3f max = lander.getSceneMax() + lander.getPosition();
	Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));

	colBoxList.clear();
	octree.intersect(bounds, octree.root, colBoxList);

	if (colBoxList.size() < 10) {
		bCollisionDetected = false;
	}
}

void ofApp::moveUp()
{
	glm::vec3 thrustForce = glm::vec3(0, 1, 0);  // World up

	thrustForce *= ship.thrust;
	ship.velocity += thrustForce * 2;
	ship.addForce(thrustForce * 2);
}

//--------------------------------------------------------------

void ofApp::moveDown()
{
	glm::vec3 thrustForce = glm::vec3(0, 1, 0);  // World down

	thrustForce *= ship.thrust;
	ship.velocity -= thrustForce * 2;
	ship.addForce(-thrustForce * 2);
}

//--------------------------------------------------------------

void ofApp::moveRight() {
	glm::vec3 thrustForce = glm::vec3(1, 0, 0);

	thrustForce *= ship.thrust;
	ship.velocity += thrustForce * 2;
	ship.addForce(thrustForce * 2);
}

void ofApp::moveLeft() {
	glm::vec3 thrustForce = glm::vec3(1, 0, 0);

	thrustForce *= ship.thrust;
	ship.velocity -= thrustForce * 2;
	ship.addForce(-thrustForce * 2);
}

void ofApp::moveForward() {
	glm::vec3 thrustForce = glm::vec3(0, 0, 1);

	thrustForce *= ship.thrust;
	ship.velocity += thrustForce * 2;
	ship.addForce(thrustForce * 2);
}

void ofApp::moveBackwards() {
	glm::vec3 thrustForce = glm::vec3(0, 0, 1);

	thrustForce *= ship.thrust;
	ship.velocity -= thrustForce * 2;
	ship.addForce(-thrustForce * 2);
}

//Adds positive torque to rotate the player right
void ofApp::rotateRight()
{
	ship.addTorque(glm::vec3(0, 0, 1000));
}

//--------------------------------------------------------------

//Adds negative torque to rotate the player left
void ofApp::rotateLeft()
{
	ship.addTorque(glm::vec3(0, 0, -1000));
}

void ofApp::checkCollision() {
	ofVec3f min = lander.getSceneMin() + lander.getPosition();
	ofVec3f max = lander.getSceneMax() + lander.getPosition();

	Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));

	colBoxList.clear();
	octree.intersect(bounds, octree.root, colBoxList);

	if (colBoxList.size() >= 10) {
		bCollisionDetected = true;
	}
	else {
		bCollisionDetected = false;
	}
}

void ofApp::setUpClassicMode() {
	if (restart && classic_mode) {
		bLanderLoaded = true;
		lander.setScaleNormalization(false);
		lander.loadModel("geo/lander.obj");

		lander.setPosition(0, 0, 0);

		bboxList.clear();
		for (int i = 0; i < lander.getMeshCount(); i++) {
			bboxList.push_back(Octree::meshBounds(lander.getMesh(i)));
		}

		glm::vec3 min = lander.getSceneMin();
		glm::vec3 max = lander.getSceneMax();
		
		lander.setPosition(cam.getPosition().x, cam.getPosition().y + 10, cam.getPosition().z - 20);

		ship.pos = lander.getPosition();
		ship.scale = glm::vec3(0.1, 0.2, 0.1);

		// set up bounding box for lander while we are at it
		landerBounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
	}
}

void ofApp::calculateAltitude() {
	if (altitude_toggle) {
		ofVec3f min = lander.getSceneMin() + lander.getPosition();
		ofVec3f max = lander.getSceneMax() + lander.getPosition();
		Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));

		Vector3 bottomCenter(
			(bounds.min().x() + bounds.max().x()) / 2.0f,   // center in X
			bounds.min().y(),								// bottom in Y
			(bounds.min().z() + bounds.max().z()) / 2.0f    // center in Z
		);

		TreeNode leaf;
		Ray ray = Ray(bottomCenter, Vector3(0, -1, 0));

		bool intersected = octree.intersect(ray, octree.root, leaf);

		if (intersected) {
			ofDefaultVertexType vert = octree.mesh.getVertex(leaf.points[0]);
			altitude = bottomCenter.y() - vert.y;
		}
	}
}

void ofApp::applyExternalForces() {
	if (!bInDrag) {
		glm::vec3 moonGravity = glm::vec3(0, -1.62f * ship.mass, 0);
		ship.addForce(moonGravity);

		if ((ofGetElapsedTimef() > 2.0f)) {
			glm::vec3 turbulenceForce(
				ofRandom(-5.0f, 5.0f),
				ofRandom(-5.0f, 5.0f),
				ofRandom(-5.0f, 5.0f)
			);

			ship.addForce(turbulenceForce);
		}
	}
}