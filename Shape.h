#pragma once
#include "ofMain.h"

class Shape {
public:
	Shape() {}
	virtual void draw();
	virtual bool inside(glm::vec3);
	glm::mat4 getTransform();

	ofVec3f pos;
	float rot = 0.0;
	glm::vec3 scale = glm::vec3(1, 1, 1);
	float defaultSize = 20.0;
	vector<glm::vec3> points;
};

void Shape::draw() {
	ofPushMatrix();
	ofMultMatrix(getTransform());
	ofDrawBox(defaultSize);
	ofPopMatrix();
}

bool Shape::inside(glm::vec3 p) {
	return false;
}

glm::mat4 Shape::getTransform()
{
	glm::mat4 T = glm::translate(glm::mat4(1.0), glm::vec3(pos));
	glm::mat4 R = glm::rotate(glm::mat4(1.0), glm::radians(rot), glm::vec3(0, 1, 0));
	glm::mat4 S = glm::scale(glm::mat4(1.0), scale);
	return T * R * S;
}