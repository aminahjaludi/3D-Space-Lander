
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
	//Setup cameras
	cam.setDistance(10);
	cam.setNearClip(.1);
	cam.setFov(65.5);
	
	followCam.setPosition(cameraPosition);

	fixedCam1.setNearClip(0.1);
	fixedCam1.setFarClip(1000);
	fixedCam2.setNearClip(0.1);
	fixedCam2.setFarClip(1000);
	fixedCam3.setNearClip(0.1);
	fixedCam3.setFarClip(1000);

	ofSetVerticalSync(true);
	cam.disableMouseInput();
	ofEnableSmoothing();
	ofEnableDepthTest();

	currentCam = &cam;

	//Setup audio
	ambient.load("ambient.mp3");
	ambient.setVolume(0.7);
	ambient.setLoop(true);
	ambient.play();

	thrust.load("thrust.wav");
	thrust.setVolume(0.2);
	thrust.setLoop(false);

	//set up lighting system
	setupLightSystem();

	terrain.loadModel("geo/terrain-model.obj");
	terrain.setScaleNormalization(false);

	titleFont.load("Sen-Bold.ttf", 64);
	instructionFont.load("Sen-Regular.ttf", 24);
	backgroundImg.load("background.jpg");

	//Create sliders for testing
	gui.setup();
	gui.add(altitude_toggle.setup("Altitude Info", false));;
	bHide = false;

	//create Octree for testing
	octree.create(terrain.getMesh(0), 20);;

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

	ofDisableArbTex();     //Disable rectangular textures

	//Load textures
	if (!ofLoadImage(particleTex, "images/nova_0.png")) {
		cout << "Particle Texture File: images/dot.png not found" << endl;
		ofExit();
	}

	//Load the shader
#ifdef TARGET_OPENGLES
	shader.load("shaders_gles/shader");
	explosionshader.load("shaders_gles/shader");
#else
	shader.load("shaders/shader");
	explosionshader.load("shaders/shader");
#endif

	setupEmitters();
}

//--------------------------------------------------------------

void ofApp::setupEmitters() {
	//Set up exhaust and explosion emitters, render them with shader
	exhaustemitter.setEmitterType(DiskEmitter);
	exhaustemitter.setPosition(ofVec3f(0, 0, 0));	//Set position
	exhaustemitter.setLifespan(1);					//Set particle lifespan

	exhaustemitter.setRate(400);					//Particles per second
	exhaustemitter.setGroupSize(300);				//Number of particles emitted per update
	exhaustemitter.setVelocity(ofVec3f(0, 2, 0));	//Set velocity

	exhaustemitter.setParticleRadius(20);

	explosionemitter.setEmitterType(RadialEmitter);
	explosionemitter.setPosition(ofVec3f(0, 0, 0));
	explosionemitter.setLifespan(10);				//Set particle lifespan
	explosionemitter.setRate(300);					//Particles per second
	explosionemitter.setGroupSize(300);				//Number of particles emitted per update
	explosionemitter.setVelocity(ofVec3f(0, 30, 0));//Set velocity
	explosionemitter.setParticleRadius(30);

	//Set flag to false
	explosiontriggered = false;
}

//--------------------------------------------------------------

void ofApp::setupLightSystem() {
	//Store the positions of the landing spots
	ofVec3f positions[3] = {
		ofVec3f(-2.104, 5.376, -125.309),
		ofVec3f(101.439, -2.432, 96.568),
		ofVec3f(-154.609, 65.377, 130.132)
	};

	//Iterate through all of the landing positions to assign each with a keylight, filllight and rimlight
	for (int i = 0; i < 3; i++) {
		pads[i].position = positions[i];

		//Key Light Setup (Dimmed)
		pads[i].keyLight.setup();
		pads[i].keyLight.enable();
		pads[i].keyLight.setAreaLight(50, 3);
		pads[i].keyLight.setAttenuation(1.0, 0.3, 0.1);
		pads[i].keyLight.setDiffuseColor(ofFloatColor(2, 2, 2)); // More brightness
		pads[i].keyLight.setAmbientColor(ofFloatColor(1.5, 1.5, 1.5)); 
		pads[i].keyLight.setSpecularColor(ofFloatColor(0.2, 0.2, 0.2));   // Softer reflections

		pads[i].keyLight.setPosition(pads[i].position.x + 10, pads[i].position.y + 25, pads[i].position.z + 10);
		pads[i].keyLight.rotate(35, ofVec3f(0, 1, 0));
		pads[i].keyLight.rotate(-45, ofVec3f(1, 0, 0));
		pads[i].keyLight.lookAt(ofVec3f(pads[i].position.x, pads[i].position.y - 10, pads[i].position.z));

		//Fill Light Setup (Dimmed)
		pads[i].fillLight.setup();
		pads[i].fillLight.enable();
		pads[i].fillLight.setPointLight();
		pads[i].fillLight.setAttenuation(0.6, 0.2, 0.05);

		pads[i].fillLight.setPosition(pads[i].position.x + 1, pads[i].position.y + 2, pads[i].position.z + 5);
		pads[i].fillLight.rotate(30, ofVec3f(1, 0, 0));
		pads[i].fillLight.rotate(-45, ofVec3f(0, 1, 0));
		pads[i].fillLight.lookAt(pads[i].position);

		//Rim Light Setup (Dimmed)
		pads[i].rimLight.setup();
		pads[i].rimLight.enable();
		pads[i].rimLight.setSpotlight();
		pads[i].rimLight.setSpotlightCutOff(25); // Narrow glow
		pads[i].rimLight.setAttenuation(0.4, 0.2, 0.1);
		pads[i].rimLight.setDiffuseColor(ofFloatColor(0.4, 0.45, 1)); // Dimmed blue glow
		pads[i].rimLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
		pads[i].rimLight.setSpecularColor(ofFloatColor(1, 1, 1));

		pads[i].rimLight.setPosition(pads[i].position.x, pads[i].position.y + 1, pads[i].position.z - 4);
		pads[i].rimLight.rotate(50, ofVec3f(1, 0, 0));
		pads[i].rimLight.rotate(180, ofVec3f(0, 1, 0));
		pads[i].rimLight.lookAt(pads[i].position);

	}

	//Set up lights for spacecraft
	shiplights.keyLight.setup();
	shiplights.keyLight.enable();

	shiplights.keyLight.setSpotlightCutOff(1); 
	shiplights.keyLight.setDiffuseColor(ofFloatColor(0.8, 0.8, 1));
	shiplights.keyLight.setSpecularColor(ofFloatColor(2.0, 2.0, 2.0)); //More reflective glow on the rocket
	shiplights.fillLight.setAttenuation(0.5, 0.3, .1);
	shiplights.keyLight.setPosition(ship.pos.x, ship.pos.y + 50, ship.pos.z + 10);
	shiplights.keyLight.lookAt(ofVec3f(ship.pos.x, ship.pos.y, ship.pos.z)); //Aim downward at rocket's bottom

}

//--------------------------------------------------------------

//Load vertex buffer in preparation for rendering the exhaust emitter
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

	//Upload the data to the vbo
	int total = (int)points.size();
	vbo.clear();
	vbo.setVertexData(&points[0], total, GL_STATIC_DRAW);
	vbo.setNormalData(&sizes[0], total, GL_STATIC_DRAW);
	vbo.setAttributeData(3, &lifeRatios[0], 1, lifeRatios.size(), GL_STATIC_DRAW);
}

//--------------------------------------------------------------

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

	//Upload the data to the vbo
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
			shiplights.keyLight.setPosition(ship.pos.x, ship.pos.y + 50, ship.pos.z + 10);

			//Check if explosion has occured, update explosionemitter
			if (explosiontriggered)
			{
				explosionemitter.update();

				float timer = ofGetElapsedTimeMillis() - explosiontimer;

				if (timer > 2000) { 
					explosiontriggered = false;
					explosionemitter.stop();
				}

			}
			if (won) return;
			if (exhaustemitter.started)
			{
				float elapsedtime = ofGetElapsedTimeMillis() - exhausttimer;

				//If elapsedtime is greater than half a second stop the exhaust
				if (elapsedtime > 500) {
					exhaustemitter.stop();
					exhaustemitter.sys->reset();           
					exhaustemitter.sys->particles.clear();  
				}
			}
			
			float x_land = lander.getPosition().x;
			float y_land = lander.getPosition().y;
			float z_land = lander.getPosition().z;

			followCam.lookAt(lander.getPosition());
			
			glm::vec3 shipPos = ship.pos;
			glm::vec3 forward = getHeadingVector(shipRotation);
			glm::vec3 up(0, 1, 0);

			//Chase cam: slightly behind and above
			fixedCam1.setPosition(shipPos - forward * 30.0f + glm::vec3(0, 10, 0));
			fixedCam1.lookAt(shipPos, up);

			//Top-down cam: directly above, looking down
			fixedCam2.setPosition(shipPos + glm::vec3(0, 50, 0));
			fixedCam2.lookAt(shipPos, glm::vec3(0, 0, -1));

			//Angled cam: behind and off to the side
			fixedCam3.setPosition(shipPos - forward * 20.0f + glm::vec3(10, 15, 0));
			fixedCam3.lookAt(shipPos, up);
			
			exhaustemitter.setPosition(glm::vec3(x_land, y_land + 1, z_land));
			
			checkCollision();
			if (bCollisionDetected) {
				resolveCollision();
			}

			calculateAltitude();
			if (thrusting) {
				uint64_t now = ofGetElapsedTimeMillis();
				if (now - lastDecrementTime >= decrementInterval && fuel > 0) { 
					fuel--; //Update fuel
					lastDecrementTime = now;
				}
			}

			if (!lost && fuel > 0) {
				if (w_pressed) { //Move player upwards
					moveUp();
					if (!thrust.isPlaying()) thrust.play();
				}
				if (s_pressed) { //Move player downwards
					moveDown();
					if (!thrust.isPlaying()) thrust.play();
				}
				if (d_pressed) { //Rotate player to the right
					rotateRight();
				}
				if (a_pressed) { //Rotate player to the left
					rotateLeft();
				}
				if (up_pressed) { //Move player backwards
					moveBackwards();
					if (!thrust.isPlaying()) thrust.play();
				}
				if (down_pressed) { //Move player forward
					moveForward();
					if (!thrust.isPlaying()) thrust.play();
				}
				if (right_pressed) { //Move player right
					moveRight();
					if (!thrust.isPlaying()) thrust.play();
				}
				if (left_pressed) { //Move player left
					moveLeft();
					if (!thrust.isPlaying()) thrust.play();
				}
			}

			applyExternalForces(); //Apply turbulence and gravity
			ship.integrator(); //Move using physics
			exhaustemitter.update();

			//Update lander model and ship positions
			lander.setPosition(ship.pos.x, ship.pos.y, ship.pos.z);
			lander.setRotation(0, ship.rot, 0, 1, 0);
		}
			
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
	ofBackground(ofColor::black); //Draw background

	if (won && ofGetElapsedTimef() - game_endt >= 1.5f) {
		displayWin();		//Show winning screen
	}
	if (lost && ofGetElapsedTimef() - game_endt >= 1.5f) {
		displayLoss();		//Show losing screen
	}
	else if (!restart) {
		displayMainMenu();	//Show main menu
	}
	else if (restart && !dragging_mode && !classic_mode) {
		displayModes();		//Show mode selection screen
	}
	else {
		printInfo();		//Print altitude and fuel and gui

		if (fuel == 0) {	//Check fuel
			lost = true;
			won = false;
		}

		//This makes everything look glowy :)
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		ofEnablePointSprites();

		//Begin drawing in the camera
		shader.begin();

		shader.setUniformTexture("tex", particleTex, 0);
		shader.setUniformMatrix4f("modelViewProjectionMatrix", currentCam->getModelViewProjectionMatrix());
		currentCam->begin();

		particleTex.bind();
		vbo.draw(GL_POINTS, 0, (int)exhaustemitter.sys->particles.size());
		
		particleTex.unbind();
		currentCam->end();
		shader.end();

		ofSetColor(255);						//Reset drawing color to white.
		ofEnableBlendMode(OF_BLENDMODE_ALPHA);  //Reset back to normal (alpha) blending.
		glUseProgram(0);						//Ensure no shader is still active.
		ofDisablePointSprites();

		//If explosion has been trigeered, use vertex buffer to draw explosion particles
		if (explosiontriggered)
		{
			ofEnablePointSprites();
			loadExplosionVbo();
			explosionshader.begin();
			explosionshader.setUniformTexture("tex", particleTex, 0);
			explosionshader.setUniformMatrix4f("modelViewProjectionMatrix", currentCam->getModelViewProjectionMatrix());
			currentCam->begin();
			particleTex.bind();

			explosionvbo.draw(GL_POINTS, 0, (int)explosionemitter.sys->particles.size());

			particleTex.unbind();
			currentCam->end();
			explosionshader.end();
			ofSetColor(255);						//Reset drawing color to white.
			ofEnableBlendMode(OF_BLENDMODE_ALPHA);  //Reset back to normal (alpha) blending.
			glUseProgram(0);						//Ensure no shader is still active.
			ofDisablePointSprites();
		}

		currentCam->begin();
		ofPushMatrix();

		ofEnableLighting(); //Shaded mode
		terrain.drawFaces();
		ofMesh mesh;

		if (bLanderLoaded && (currentCam == &followCam || currentCam == &cam)) {
			lander.drawFaces();
			
			if (bLanderSelected) {
				ofVec3f min = lander.getSceneMin() + lander.getPosition();
				ofVec3f max = lander.getSceneMax() + lander.getPosition();

				Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
				ofSetColor(ofColor::white);
				Octree::drawBox(bounds);
			}

			ship.rot = shipRotation - 90;
			ship.draw();
		}
		
		ofDisableLighting();
		ofPopMatrix();
		currentCam->end();
	}
}

//--------------------------------------------------------------

void ofApp::keyPressed(int key) {

	switch (key) {
	case '1': //Classic mode selected
		if (restart && !classic_mode && !dragging_mode) {
			classic_mode = true;
			dragging_mode = false;
			setupClassicMode();
		}
		break;
	case '2': //Dragging mode selected
		if (restart && !dragging_mode && !classic_mode) {
			classic_mode = false;
			dragging_mode = true;
		}
		break;
	case 'q':
	case 'Q': //Quit
		won = false;
		lost = false;
		quit = true;
		restart = false;
		classic_mode = false;
		dragging_mode = false;
		disableDragging = false;
		fuel = 120;
		ofResetElapsedTimeCounter();
		break;
	case 'e':
	case 'E': //Mode selection
		restart = true;
		quit = false;
		explosiontriggered = false;
		break;
	case OF_KEY_UP:
		up_pressed = true;
		thrusting = true;
        if (bLanderLoaded) disableDragging = true;
		triggerExhaust();
		break;
	case OF_KEY_DOWN:
		down_pressed = true;
        thrusting = true;
        if (bLanderLoaded) disableDragging = true;
		triggerExhaust();
		break;
	case OF_KEY_RIGHT:
		right_pressed = true;
		thrusting = true;
        if (bLanderLoaded) disableDragging = true;
		triggerExhaust();
		break;
	case OF_KEY_LEFT:
		left_pressed = true;
		thrusting = true;
        if (bLanderLoaded) disableDragging = true;
		triggerExhaust();
		break;
	case 'w':
	case 'W':
		w_pressed = true;
		triggerExhaust();
		thrusting = true;
        if (bLanderLoaded) disableDragging = true;
        break;
	case 's':
	case 'S':
		s_pressed = true;
		triggerExhaust();
		thrusting = true;
		if (bLanderLoaded) disableDragging = true;
		break;
	case 'd':
	case 'D':
		d_pressed = true;
		thrusting = true;
		if (bLanderLoaded) disableDragging = true;
		break;
	case 'a':
	case 'A':
		a_pressed = true;
		thrusting = true;
		if (bLanderLoaded) disableDragging = true;
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
	case 'T':
	case 't': //Update cams
		if (currentCam == &cam) {
			currentCam = &followCam;
		}
		else {
			currentCam = &cam;
		}
		break;
	case 'Y':
	case 'y': //Update on-board cams
		if (currentCam == &fixedCam1) {
			currentCam = &fixedCam2;
		}
		else if (currentCam == &fixedCam2) {
			currentCam = &fixedCam3;
		}
		else {
			currentCam = &fixedCam1;
		}
		break;
	case OF_KEY_ALT:
		cam.enableMouseInput();
		break;
	default:
		break;
	}
}

//--------------------------------------------------------------

void ofApp::triggerExhaust() {
	//Start exhaust emitter
	exhaustemitter.sys->reset();
	if (currentCam == &followCam || currentCam == &cam) exhaustemitter.start();
	exhausttimer = ofGetElapsedTimeMillis();
}

//--------------------------------------------------------------

void ofApp::triggerExplosion(glm::vec3& explosionpoint) {
	//Set the position of the explosion emitter to the terrain point
	explosionemitter.setPosition(explosionpoint);

	//Start the explosion
	explosionemitter.start();

	//Set flag to true
	explosiontriggered = true;

	//Set explosion timer 
	explosiontimer = ofGetElapsedTimeMillis();
}

//--------------------------------------------------------------

void ofApp::keyReleased(int key) {

	switch (key) {
	case OF_KEY_UP:
		up_pressed = false;
		thrusting = false;
		break;
	case OF_KEY_DOWN:
		down_pressed = false;
		thrusting = false;
		break;
	case OF_KEY_RIGHT:
		right_pressed = false;
		thrusting = false;
		break;
	case OF_KEY_LEFT:
		left_pressed = false;
		thrusting = false;
		break;
	case 'w':
	case 'W':
		w_pressed = false;
		thrusting = false;
		break;
	case 's':
	case 'S':
		s_pressed = false;
		thrusting = false;
		break;
	case 'd':
	case 'D':
		d_pressed = false;
		thrusting = false;
		break;
	case 'a':
	case 'A':
		a_pressed = false;
		thrusting = false;
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

	//If moving camera, don't allow mouse interaction
	if (cam.getMouseInputEnabled()) return;

	//If lander is loaded, test for selection
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
			cam.lookAt(lander.getPosition());
			bInDrag = true;
		}
		else {
			bLanderSelected = false;
		}

		bool octHit = raySelectWithOctree(selectedPoint);
		if (octHit)
		{
			selectPos.x = selectedPoint.x;
			selectPos.y = selectedPoint.y;
			selectPos.z = selectedPoint.z;
			cam.lookAt(selectPos);
		}
	}
}

//--------------------------------------------------------------

bool ofApp::raySelectWithOctree(ofVec3f& pointRet) {
	ofVec3f mouse(mouseX, mouseY);
	ofVec3f rayPoint = cam.screenToWorld(mouse);
	ofVec3f rayDir = rayPoint - cam.getPosition();
	rayDir.normalize();
	Ray ray = Ray(Vector3(rayPoint.x, rayPoint.y, rayPoint.z),
		Vector3(rayDir.x, rayDir.y, rayDir.z));

	pointSelected = octree.intersect(ray, octree.root, selectedNode);

	if (pointSelected) {
		pointRet = octree.mesh.getVertex(selectedNode.points[0]);
	}
	return pointSelected;
}

//--------------------------------------------------------------

void ofApp::mouseDragged(int x, int y, int button) {

	//If moving camera, don't allow mouse interaction
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

// Support drag-and-drop of model (.obj) file loading.  when
// model is dropped in viewport, place origin under cursor
void ofApp::dragEvent(ofDragInfo dragInfo) {
	if (restart && dragging_mode) {
		if (lander.loadModel(dragInfo.files[0])) {
			bLanderLoaded = true;
			lander.setScaleNormalization(false);
			lander.setPosition(0, 0, 0);

			bboxList.clear();
			for (int i = 0; i < lander.getMeshCount(); i++) {
				bboxList.push_back(Octree::meshBounds(lander.getMesh(i)));
			}

			//Setup our rays
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

				// Set up bounding box for lander while we are at it
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
		glm::vec3 collisionarea = ship.pos;
		triggerExplosion(collisionarea);

		game_endt = ofGetElapsedTimef();
		lost = true;
		glm::vec3 impulseForce = glm::vec3(1, 1, 0);

		//Apply force the shoot the lander away uncontrollably
		impulseForce *= ship.thrust;
		ship.velocity += impulseForce * 700;
		ship.addForce(impulseForce * 700);
		ship.integrator();
	}
	else { //Win or bounce off terrain

		bool landed = checkLanding(); //Check if landing on pad was a success

		if (landed) { //End the game if yes
			game_endt = ofGetElapsedTimef();
			won = true;
			return;
		}

		while (colBoxList.size() >= 10) { //Otherwise bounce off
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
	glm::vec3 thrustForce = glm::vec3(0, 1, 0);

	thrustForce *= ship.thrust;
	ship.velocity += thrustForce * 10;
	ship.addForce(thrustForce * 10);
}

//--------------------------------------------------------------

void ofApp::moveDown()
{
	glm::vec3 thrustForce = glm::vec3(0, 1, 0);

	thrustForce *= ship.thrust;
	ship.velocity -= thrustForce * 10;
	ship.addForce(-thrustForce * 10);
}

//--------------------------------------------------------------

void ofApp::moveRight() {
	glm::vec3 thrust = getRightVector(shipRotation);
	thrust *= ship.thrust;
	ship.addForce(thrust * 10.0f);
	ship.velocity += thrust * 10.0f;
}

//--------------------------------------------------------------

void ofApp::moveLeft() {
	glm::vec3 thrust = -getRightVector(shipRotation);
	thrust *= ship.thrust;
	ship.addForce(thrust * 10.0f);
	ship.velocity += thrust * 10.0f;
}

//--------------------------------------------------------------

void ofApp::moveForward() {
	glm::vec3 thrust = -getHeadingVector(shipRotation);
	thrust *= ship.thrust;
	ship.addForce(thrust * 10.0f);
	ship.velocity += thrust * 10.0f;
}

//--------------------------------------------------------------

void ofApp::moveBackwards() {
	glm::vec3 thrust = getHeadingVector(shipRotation);
	thrust *= ship.thrust;
	ship.addForce(thrust * 10.0f);
	ship.velocity += thrust * 10.0f;
}

//--------------------------------------------------------------

void ofApp::rotateRight() {
	shipRotation -= 5;

	float radians = glm::radians(shipRotation);
	shipHeading.x = sin(radians);
	shipHeading.z = -cos(radians);
	shipHeading.y = 0;
	shipHeading = glm::normalize(shipHeading);
}

//--------------------------------------------------------------

void ofApp::rotateLeft() {
	shipRotation += 5;

	float radians = glm::radians(shipRotation);
	shipHeading.x = sin(radians);
	shipHeading.z = -cos(radians);
	shipHeading.y = 0;
	shipHeading = glm::normalize(shipHeading);
}

//--------------------------------------------------------------

glm::vec3 ofApp::getHeadingVector(float degrees) {
	float radians = glm::radians(degrees);
	return glm::vec3(-sin(radians), 0, -cos(radians));
}

//--------------------------------------------------------------

glm::vec3 ofApp::getRightVector(float degrees) {
	float radians = glm::radians(degrees);
	return glm::vec3(cos(radians), 0, -sin(radians));
}

//--------------------------------------------------------------

//Check for lander collision with terrain
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

//Set up lander for classic mode 
void ofApp::setupClassicMode() {
	if (restart && classic_mode) {
		bLanderLoaded = true;
		lander.setScaleNormalization(false);
		lander.loadModel("geo/atlantis-orbiter.obj");

		lander.setPosition(0, 133, 0);

		bboxList.clear();
		for (int i = 0; i < lander.getMeshCount(); i++) {
			bboxList.push_back(Octree::meshBounds(lander.getMesh(i)));
		}

		glm::vec3 min = lander.getSceneMin();
		glm::vec3 max = lander.getSceneMax();

		ship.pos = lander.getPosition();
		ship.scale = glm::vec3(0.1, 0.2, 0.1);

		//Set up bounding box for lander while we are at it
		landerBounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
	}
}

//--------------------------------------------------------------

void ofApp::calculateAltitude() {

	ofVec3f min = lander.getSceneMin() + lander.getPosition();
	ofVec3f max = lander.getSceneMax() + lander.getPosition();
	Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));

	//Get the bottom center position of the lander's box
	Vector3 bottomCenter(
		(bounds.min().x() + bounds.max().x()) / 2.0f,   // center in X
		bounds.min().y(),								// bottom in Y
		(bounds.min().z() + bounds.max().z()) / 2.0f    // center in Z
	);

	TreeNode leaf;
	Ray ray = Ray(bottomCenter, Vector3(0, -1, 0));

	//Use intersection with ray to get leaf involved in intersection
	bool intersected = octree.intersect(ray, octree.root, leaf);

	if (intersected) {
		ofDefaultVertexType vert = octree.mesh.getVertex(leaf.points[0]);
		altitude = bottomCenter.y() - vert.y; //Use leaf to find altitude
	}

}

//--------------------------------------------------------------

void ofApp::applyExternalForces() {
	if (!bInDrag) {
		glm::vec3 moonGravity = glm::vec3(0, -1.62f * ship.mass, 0); //Apply the moon's gravity
		ship.addForce(moonGravity);

		//Apply random turbulence
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

	//Landing spot positions
	ofVec3f disk1(-2.104, 5.376, -125.309);
	ofVec3f disk2(101.439, -2.432, 96.568);
	ofVec3f disk3(-154.609, 65.377, 130.132);

	//Check distance on XZ plane for Disk 1
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

	//Return whether the lander landed on either of the 3 pads
	return landedOnDisk1 || landedOnDisk2 || landedOnDisk3;
}

//--------------------------------------------------------------

void ofApp::displayMainMenu() {
	ofBackground(ofColor::black);
	float centerX = ofGetWidth() / 2.0;

	ofDisableLighting();
	ofBackground(20);
	ofSetColor(255);

	string title = "3D LANDER GAME";
	string instruction1 = "Press E to Start, and Q to quit";
	string instruction2 = "Use Arrow Keys and AWSD Keys to Move";
	string instruction3 = "Use T, Y, and C for camera controls.";

	//Measure string widths for centering
	float titleWidth = titleFont.stringWidth(title);
	float instr1Width = instructionFont.stringWidth(instruction1);
	float instr2Width = instructionFont.stringWidth(instruction2);
	float instr3Width = instructionFont.stringWidth(instruction3);

	//Draw centered
	titleFont.drawString(title, centerX - titleWidth / 2, 200);
	instructionFont.drawString(instruction1, centerX - instr1Width / 2, 300);
	instructionFont.drawString(instruction2, centerX - instr2Width / 2, 400);
	instructionFont.drawString(instruction3, centerX - instr3Width / 2, 500);
}

void ofApp::displayModes() {
	ofBackground(ofColor::black);
	float centerX = ofGetWidth() / 2.0;
	string title = "SELECT GAME MODE";
	string instruction1 = "[1] Classic Mode : fixed lander starting position";
	string instruction2 = "[2] Dragging Mode : drag lander to desired starting position";

	//Measure string widths for centering
	float titleWidth = titleFont.stringWidth(title);
	float instr1Width = instructionFont.stringWidth(instruction1);
	float instr2Width = instructionFont.stringWidth(instruction2);

	//Draw centered
	titleFont.drawString(title, centerX - titleWidth / 2, 200);
	instructionFont.drawString(instruction1, centerX - instr1Width / 2, 300);
	instructionFont.drawString(instruction2, centerX - instr2Width / 2, 400);
}

void ofApp::displayWin() {
	ofBackground(ofColor::black);
	float centerX = ofGetWidth() / 2.0;
	ofDisableLighting();
	ofSetColor(255);

	string title = "YOU WIN!";
	string instruction1 = "Press Q to quit to Main Menu";

	//Measure string widths for centering
	float titleWidth = titleFont.stringWidth(title);
	float instr1Width = instructionFont.stringWidth(instruction1);

	//Draw centered
	titleFont.drawString(title, centerX - titleWidth / 2, 200);
	instructionFont.drawString(instruction1, centerX - instr1Width / 2, 300);
}

void ofApp::displayLoss() {
	ofBackground(ofColor::black);
	float centerX = ofGetWidth() / 2.0;
	ofDisableLighting();
	ofSetColor(255);

	string title = "GAME OVER";
	string loss_reason;

	//Display different message depending on the loss reason
	if (fuel == 0) {
		loss_reason = "Ran out of fuel!";
	}
	else {
		loss_reason = "Landing was too forceful!";
	}

	string instruction1 = "Press Q to quit to Main Menu";

	//Measure string widths for centering
	float titleWidth = titleFont.stringWidth(title);
	float instr1Width = instructionFont.stringWidth(instruction1);
	float lossRWidth = instructionFont.stringWidth(loss_reason);

	//Draw centered
	titleFont.drawString(title, centerX - titleWidth / 2, 200);
	instructionFont.drawString(loss_reason, centerX - lossRWidth / 2, 300);
	instructionFont.drawString(instruction1, centerX - instr1Width / 2, 400);
}

void ofApp::printInfo() {
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

	std::string fuel_info = "Current fuel: " + to_string(fuel);
	int textWidth = 8 * fuel_info.length();
	int textHeight = 12;

	int x = ofGetWidth() - textWidth - 10;
	int y = ofGetHeight() - 10;
	ofSetColor(ofColor::white);
	ofDrawBitmapString(fuel_info, x, y);
}