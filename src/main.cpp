#include "ofMain.h"
#include "ofApp.h"

int main() {
	ofGLFWWindowSettings settings;
	settings.setGLVersion(4, 6);  // Explicitly request OpenGL 4.6
	settings.setSize(1024, 768);
	ofCreateWindow(settings);

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());
}
