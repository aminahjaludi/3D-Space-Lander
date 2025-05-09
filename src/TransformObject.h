#pragma once
#include "ofMain.h"

//  Kevin M. Smith - CS 134 SJSU
//

//  Base class for any object that needs a transform.
//
class TransformObject {
protected:
	TransformObject() {
		position = ofVec3f(0, 0, 0);
		scale = ofVec3f(1, 1, 1);
		rotation = 0;

	}
	ofVec3f position, scale;
	float	rotation;

public:
	ofVec3f getPosition() { return position; }
	void setPosition(const ofVec3f& pos) { position = pos; }

	float getRotation() { return rotation; }
	void setRotation(float rot) { rotation = rot; };
};