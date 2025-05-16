
//--------------------------------------------------------------
//
//  Kevin M. Smith
//
//  Student Names: Aminah Jaludi, Mckenzie Lola

#include "ofApp.h"
#include "Util.h"
#include <sstream>
#include <iomanip>

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

	mars.loadModel("geo/terrain-model.obj");
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

	ofDisableArbTex();     // disable rectangular textures

	// load textures
	//
	if (!ofLoadImage(particleTex, "images/nova_0.png")) {
		cout << "Particle Texture File: images/dot.png not found" << endl;
		ofExit();
	}

	// load the shader
	//
#ifdef TARGET_OPENGLES
	shader.load("shaders_gles/shader");
	explosionshader.load("shaders_gles/shader");
#else
	shader.load("shaders/shader");
	explosionshader.load("shaders/shader");
#endif

	//set up exhaust and explosion emitters, render them with shader
	exhaustemitter.setEmitterType(DiskEmitter);
	exhaustemitter.setPosition(ofVec3f(0, 0, 0));  // Set position (adjust as needed)
	exhaustemitter.setLifespan(1);  // Set particle lifespan
	//exhaustemitter.setRate(70);  // Particles per second
	//exhaustemitter.setGroupSize(30);  // Number of particles emitted per update
	exhaustemitter.setRate(400);  // Particles per second
	exhaustemitter.setGroupSize(300);  // Number of particles emitted per update
	exhaustemitter.setVelocity(ofVec3f(0, 2, 0));  // Set velocity
	//exhaustemitter.setParticleRadius(0.04);  // Particle size
	exhaustemitter.setParticleRadius(20);

	explosionemitter.setEmitterType(RadialEmitter);
	explosionemitter.setPosition(ofVec3f(0, 0, 0));
	explosionemitter.setLifespan(10);  // Set particle lifespan
	explosionemitter.setRate(300);  // Particles per second
	explosionemitter.setGroupSize(300);  // Number of particles emitted per update
	explosionemitter.setVelocity(ofVec3f(0, 30, 0));  // Set velocity
	explosionemitter.setParticleRadius(10);

	//set flag to false
	explosiontriggered = false;
}

// load vertex buffer in preparation for rendering the exhaust emitter
//
void ofApp::loadVbo() {
	if (exhaustemitter.sys->particles.size() < 1) return;

	vector<ofVec3f> sizes;
	vector<ofVec3f> points;
	vector<float> lifeRatios;
	for (auto& particle : exhaustemitter.sys->particles) {
		points.push_back(particle.position);
		sizes.push_back(ofVec3f(particle.radius));
		float ratio = particle.age() / particle.lifespan;
		lifeRatios.push_back(ratio);
	}
	// upload the data to the vbo
	//
	int total = (int)points.size();
	vbo.clear();
	vbo.setVertexData(&points[0], total, GL_STATIC_DRAW);
	vbo.setNormalData(&sizes[0], total, GL_STATIC_DRAW);
	vbo.setAttributeData(3, &lifeRatios[0], 1, lifeRatios.size(), GL_STATIC_DRAW);
}

void ofApp::loadExplosionVbo() {
	if (explosionemitter.sys->particles.size() < 1) return;

	vector<ofVec3f> sizes;
	vector<ofVec3f> points;
	vector<float> lifeRatios;
	for (auto& particle : explosionemitter.sys->particles) {
		points.push_back(particle.position);
		sizes.push_back(ofVec3f(particle.radius));
		float ratio = particle.age() / particle.lifespan;
		lifeRatios.push_back(ratio);
	}
	// upload the data to the vbo
	//
	int total = (int)points.size();
	explosionvbo.clear();
	explosionvbo.setVertexData(&points[0], total, GL_STATIC_DRAW);
	explosionvbo.setNormalData(&sizes[0], total, GL_STATIC_DRAW);
	explosionvbo.setAttributeData(3, &lifeRatios[0], 1, lifeRatios.size(), GL_STATIC_DRAW);
}

//--------------------------------------------------------------

void ofApp::update() {
	if (restart) {
		if (bLanderLoaded) {
			//check if explosion has occured, update explosionemitter
			if (explosiontriggered)
			{
				//loadExplosionVbo();
				explosionemitter.update();

				float timer = ofGetElapsedTimeMillis() - explosiontimer;
				if (explosionemitter.sys->particles.size() > 0) {
					cout << "Particles exist before clearing: " << explosionemitter.sys->particles.size() << endl;
				}
				else {
					cout << "Particles were never created!" << endl;
				}

				if (timer > 0) { 
					explosiontriggered = false;
					explosionemitter.stop();
				}

			}
			if (won) return;
			if (exhaustemitter.started)
			{
				float elapsedtime = ofGetElapsedTimeMillis() - exhausttimer;

				//if elapsedtime is greater than half a second stop the exhaust
				if (elapsedtime > 500) {
					exhaustemitter.stop();
					exhaustemitter.sys->reset();           
					exhaustemitter.sys->particles.clear();  
				}
			}
			float x_land = lander.getPosition().x;
			float y_land = lander.getPosition().y;
			float z_land = lander.getPosition().z;
			//on every update set the emitters to or near the psotion of the rocket
			exhaustemitter.setPosition(glm::vec3(x_land, y_land + 1, z_land));
			//cout << lander.getPosition() << endl;
			checkCollision();

			if (bCollisionDetected) {
				resolveCollision();
			}

			calculateAltitude();

			//Move lander in accordance to key pressed
			//if (altitude > 0) {
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
				exhaustemitter.update();

				lander.setPosition(ship.pos.x, ship.pos.y, ship.pos.z);
				lander.setRotation(0, ship.rot, 0, 1, 0);
			}
			
		//}
	}
	else {
		lander.clear();
		bLanderLoaded = false;
		bCollisionDetected = false;
		explosionemitter.sys->reset();
		explosionemitter.sys->particles.clear();
	}

	
}

//--------------------------------------------------------------

void ofApp::draw() {
	loadVbo();
	ofBackground(ofColor::black);

	// Get window center
	float centerX = ofGetWidth() / 2.0;

	if (won && ofGetElapsedTimef() >= 1.5) {
		ofDisableLighting();
		ofSetColor(255);  // white text

		string title = "YOU WIN!";
		string instruction1 = "Press Q to quit to Main Menu";

		// Measure string widths for centering
		float titleWidth = titleFont.stringWidth(title);
		float instr1Width = instructionFont.stringWidth(instruction1);

		// Draw centered
		titleFont.drawString(title, centerX - titleWidth / 2, 200);
		instructionFont.drawString(instruction1, centerX - instr1Width / 2, 300);
	}
	if (lost && ofGetElapsedTimef() >= 1.5) {

		ofDisableLighting();
		ofSetColor(255);  // white text

		string title = "GAME OVER";
		string instruction1 = "Press Q to quit to Main Menu";

		// Measure string widths for centering
		float titleWidth = titleFont.stringWidth(title);
		float instr1Width = instructionFont.stringWidth(instruction1);

		// Draw centered
		titleFont.drawString(title, centerX - titleWidth / 2, 200);
		instructionFont.drawString(instruction1, centerX - instr1Width / 2, 300);
	}
	else if (!restart) {
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

		if (dragging_mode) {
			string drag_instr = "Welcome to dragging mode! Drag the lander to your desired starting position.\nThe game will begin once you press arrows or awsd keys and dragging will be disabled.";

			float x = 220;
			float y = 25;
			ofSetColor(ofColor::white);
			ofDrawBitmapString(drag_instr, x, y);
		}

		if (altitude_toggle) {
			ostringstream oss;
			oss << fixed << setprecision(1) << "Altitude is " << altitude;
			string altitude_str = oss.str();

			float x = ofGetWidth() - 140;
			float y = 20;
			ofSetColor(ofColor::white);
			ofDrawBitmapString(altitude_str, x, y);
		}
		ofPushMatrix();
		// this makes everything look glowy :)
		//
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		ofEnablePointSprites();


		// begin drawing in the camera
		//
		shader.begin();
		shader.setUniformTexture("tex", particleTex, 0); 
		shader.setUniformMatrix4f("modelViewProjectionMatrix", cam.getModelViewProjectionMatrix());
		cam.begin();
		particleTex.bind();
		vbo.draw(GL_POINTS, 0, (int)exhaustemitter.sys->particles.size());
		//exhaustemitter.draw();
		particleTex.unbind();
		cam.end();
		shader.end();
	/*	if (exhaustemitter.sys->particles.size() == 0) {
			cout<< "No exhaust particles available.";
		}
		else {
			cout << "Rendering " << exhaustemitter.sys->particles.size() << " exhaust particles.";
		}*/

		//if explosion has been trigeered, use vertex buffer to draw explosion particles
		if (explosiontriggered)
		{
			loadExplosionVbo();
			explosionshader.begin();
			explosionshader.setUniformTexture("tex", particleTex, 0);
			explosionshader.setUniformMatrix4f("modelViewProjectionMatrix", cam.getModelViewProjectionMatrix());
			cam.begin();
			particleTex.bind();
			cout << "Particles before drawing: " << explosionemitter.sys->particles.size() << endl;

			explosionvbo.draw(GL_POINTS, 0, (int)explosionemitter.sys->particles.size());

			particleTex.unbind();
			cam.end();
			explosionshader.end();
		}

		ofDisablePointSprites();
		ofDisableBlendMode();
		ofEnableAlphaBlending();
	
		ofPopMatrix();
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

//--------------------------------------------------------------

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
		won = false;
		lost = false;
		quit = true;
		restart = false;
		classic_mode = false;
		dragging_mode = false;
		disableDragging = false;
		break;
	case 'e':
	case 'E':
		restart = true;
		quit = false;
		explosiontriggered = false;
		break;
	case OF_KEY_UP:
		up_pressed = true;
		//trigger exhaust
		triggerExhaust();
		break;
	case OF_KEY_DOWN:
		down_pressed = true;
		//trigger exhaust
		triggerExhaust();
    disableDragging = true;
		break;
	case OF_KEY_RIGHT:
		right_pressed = true;
		//trigger exhaust
		triggerExhaust();
      disableDragging = true;
		break;
	case OF_KEY_LEFT:
		left_pressed = true;
		//trigger exhaust
		triggerExhaust();
    disableDragging = true;
		break;
	case 'w':
	case 'W':
		w_pressed = true;
		//trigger exhaust
		triggerExhaust();
		disableDragging = true;
		exhaustemitter.sys->reset();
		exhaustemitter.start();
		exhausttimer = ofGetElapsedTimeMillis();
		break;
	case 's':
	case 'S':
		s_pressed = true;
		//trigger exhaust
		//triggerExhaust();
		disableDragging = true;
		break;
	case 'd':
	case 'D':
		d_pressed = true;
		disableDragging = true;
		break;
	case 'a':
	case 'A':
		a_pressed = true;
		disableDragging = true;
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

void ofApp::triggerExhaust() {
	//start exhaust emitter
	exhaustemitter.sys->reset();
	exhaustemitter.start();
	exhausttimer = ofGetElapsedTimeMillis();
}

void ofApp::triggerExplosion(glm::vec3& explosionpoint) {
	//set the position of the explosion emitter to the terrain point
	explosionemitter.setPosition(explosionpoint);
	//start the explosion
	explosionemitter.start();
	cout << "Explosion emitter started: " << explosionemitter.started << endl;
	cout << "Explosion emitter particle system size: " << explosionemitter.sys->particles.size() << endl;
	explosionemitter.update();
	//set flag to true
	explosiontriggered = true;
	//set explosion timer 
	explosiontimer = ofGetElapsedTimeMillis();
	cout << "Explosion triggered at: " << explosionemitter.getPosition() << endl;
}

//--------------------------------------------------------------

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
	case ' ':
		/*cout << "distance: " << cam.getDistance() << endl;
		cout << "pos: " << cam.getPosition() << endl;
		cout << "global pos: " << cam.getGlobalPosition() << endl;
		cout << "global ori: " << cam.getGlobalOrientation() << endl;*/
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
	if (disableDragging) return;
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
	if (ship.prev_force.y < -4 || altitude < 0) { //Loss

    cout << "Collision detected! Force: " << ship.prev_force.y << ", Altitude: " << altitude << endl;

		glm::vec3 collisionarea = ship.pos;
		//call trigger Explosion
		triggerExplosion(collisionarea);
		ofResetElapsedTimeCounter();
		lost = true;
		glm::vec3 impulseForce = glm::vec3(1, 1, 0);

		impulseForce *= ship.thrust;
		ship.velocity += impulseForce * 700;
		ship.addForce(impulseForce * 700);
		ship.integrator();
	}
	else { //Win / bounce back

		bool landed = checkLanding();

		if (landed) {
			ofResetElapsedTimeCounter();
			won = true;
			return;
		}

		while (colBoxList.size() >= 10) {
			glm::vec3 impulseForce = glm::vec3(0, 1, 0);  // World up

			impulseForce *= ship.thrust;
			ship.velocity += impulseForce * 10;
			ship.addForce(impulseForce * 10);
			ship.integrator();

			ofVec3f min = lander.getSceneMin() + lander.getPosition();
			ofVec3f max = lander.getSceneMax() + lander.getPosition();
			Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));

			lander.setPosition(ship.pos.x, ship.pos.y, ship.pos.z);
			lander.setRotation(0, ship.rot, 0, 1, 0);

			colBoxList.clear();
			octree.intersect(bounds, octree.root, colBoxList);
		}
	}
}


//--------------------------------------------------------------

void ofApp::moveUp()
{
	glm::vec3 thrustForce = glm::vec3(0, 1, 0);  // World up

	thrustForce *= ship.thrust;
	ship.velocity += thrustForce * 10;
	ship.addForce(thrustForce * 10);
}

//--------------------------------------------------------------

void ofApp::moveDown()
{
	glm::vec3 thrustForce = glm::vec3(0, 1, 0);  // World down

	thrustForce *= ship.thrust;
	ship.velocity -= thrustForce * 10;
	ship.addForce(-thrustForce * 10);
}

//--------------------------------------------------------------

void ofApp::moveRight() {
	glm::vec3 thrustForce = glm::vec3(1, 0, 0);

	thrustForce *= ship.thrust;
	ship.velocity += thrustForce * 10;
	ship.addForce(thrustForce * 10);
}

//--------------------------------------------------------------

void ofApp::moveLeft() {
	glm::vec3 thrustForce = glm::vec3(1, 0, 0);

	thrustForce *= ship.thrust;
	ship.velocity -= thrustForce * 10;
	ship.addForce(-thrustForce * 10);
}

//--------------------------------------------------------------

void ofApp::moveForward() {
	glm::vec3 thrustForce = glm::vec3(0, 0, 1);

	thrustForce *= ship.thrust;
	ship.velocity += thrustForce * 10;
	ship.addForce(thrustForce * 10);
}

//--------------------------------------------------------------

void ofApp::moveBackwards() {
	glm::vec3 thrustForce = glm::vec3(0, 0, 1);

	thrustForce *= ship.thrust;
	ship.velocity -= thrustForce * 10;
	ship.addForce(-thrustForce * 10);
}

//--------------------------------------------------------------

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

//--------------------------------------------------------------

void ofApp::checkCollision() {
	ofVec3f min = lander.getSceneMin() + lander.getPosition();
	ofVec3f max = lander.getSceneMax() + lander.getPosition();

	Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));

	colBoxList.clear();
	octree.intersect(bounds, octree.root, colBoxList);

	if (colBoxList.size() >= 10 && !bCollisionDetected) {
		bCollisionDetected = true;
	}
	else {
		bCollisionDetected = false;
	}
}

//--------------------------------------------------------------

void ofApp::setUpClassicMode() {
	if (restart && classic_mode) {
		bLanderLoaded = true;
		lander.setScaleNormalization(false);
		lander.loadModel("geo/atlantis-orbiter.obj");

		//lander.setPosition(0, 0, 0);
		lander.setPosition(0, 133, 0);

		bboxList.clear();
		for (int i = 0; i < lander.getMeshCount(); i++) {
			bboxList.push_back(Octree::meshBounds(lander.getMesh(i)));
		}

		glm::vec3 min = lander.getSceneMin();
		glm::vec3 max = lander.getSceneMax();
		
		//lander.setPosition(cam.getPosition().x, cam.getPosition().y + 10, cam.getPosition().z - 20);

		ship.pos = lander.getPosition();
		ship.scale = glm::vec3(0.1, 0.2, 0.1);

		// set up bounding box for lander while we are at it
		landerBounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
	}
}

//--------------------------------------------------------------

void ofApp::calculateAltitude() {
	//if (altitude_toggle) {
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
		//}
}

//--------------------------------------------------------------

void ofApp::applyExternalForces() {
	if (!bInDrag) {
		glm::vec3 moonGravity = glm::vec3(0, -1.62f * ship.mass, 0);
		ship.addForce(moonGravity);

		if ((ofGetElapsedTimef() > 2.0f)) {
			glm::vec3 turbulenceForce(
				ofRandom(-5.0f, 5.0f),
				ofRandom(-1.0f, 1.0f),
				ofRandom(-5.0f, 5.0f)
			);

			ship.addForce(turbulenceForce);
		}
	}
}


//--------------------------------------------------------------

bool ofApp::checkLanding() {
	ofVec3f min = lander.getSceneMin() + lander.getPosition();
	ofVec3f max = lander.getSceneMax() + lander.getPosition();
	Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));

	Vector3 bottomCenter(
		(bounds.min().x() + bounds.max().x()) / 2.0f,   // center in X
		bounds.min().y(),								// bottom in Y
		(bounds.min().z() + bounds.max().z()) / 2.0f    // center in Z
	);

	float landingRadius = 25.699;

	//Landing spot positions (XZ components only)
	ofVec3f disk1(-2.104, 5.376, -125.309);
	ofVec3f disk2(101.439, -2.432, 96.568);
	ofVec3f disk3(-154.609, 65.377, 130.132);

	// Check distance on XZ plane for Disk 1
	float dx1 = bottomCenter.x() - disk1.x;
	float dz1 = bottomCenter.z() - disk1.z;
	float distance1 = sqrt(dx1 * dx1 + dz1 * dz1);
	bool landedOnDisk1 = distance1 <= landingRadius;

	//Check distance on XZ plane for Disk 2
	float dx2 = bottomCenter.x() - disk2.x;
	float dz2 = bottomCenter.z() - disk2.z;
	float distance2 = sqrt(dx2 * dx2 + dz2 * dz2);
	bool landedOnDisk2 = distance2 <= landingRadius;

	//Check distance on XZ plane for Disk 3
	float dx3 = bottomCenter.x() - disk3.x;
	float dz3 = bottomCenter.z() - disk3.z;
	float distance3 = sqrt(dx3 * dx3 + dz3 * dz3);
	bool landedOnDisk3 = distance3 <= landingRadius;

	return landedOnDisk1 || landedOnDisk2 || landedOnDisk3;
}