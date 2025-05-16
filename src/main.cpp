#include "ofMain.h"
#include "ofApp.h"

int main() {
    ofGLFWWindowSettings settings;
    settings.setGLVersion(4, 6);  // Explicitly request OpenGL 4.6
    settings.setSize(1024, 768);
    ofCreateWindow(settings);

    ofRunApp(new ofApp());
}
