#pragma once

#include <vector>
#include "ShapeIntegrate.h"

class SpacecraftShape : public ShapeIntegrate {
public:
	SpacecraftShape();
	void draw();
	bool inside(glm::vec3);
};


SpacecraftShape::SpacecraftShape() {
	velocity = glm::vec3(0, 0, 0);
	accel = glm::vec3(0, 0, 0);
	linear_damping = 0.97;
	angular_damping = 0.95;
	prev_force = glm::vec3(0, 0, 0);
	points.resize(3);
}

void SpacecraftShape::draw() {
	ofPushMatrix();
	ofMultMatrix(getTransform());

	//Set color and draw triangle
	ofSetColor(ofColor::mediumPurple);
	ofDrawLine(points[0], points[1]);
	ofDrawLine(points[0], points[2]);
	ofDrawLine(points[1], points[2]);

	ofPopMatrix();
}

bool SpacecraftShape::inside(glm::vec3 p0) {
	//Check if a point is inside triangle
	glm::vec3 p = glm::inverse(getTransform()) * glm::vec4(p0, 1);

	glm::vec3 v1 = glm::normalize(points[0] - p);
	glm::vec3 v2 = glm::normalize(points[1] - p);
	glm::vec3 v3 = glm::normalize(points[2] - p);

	float a1 = glm::orientedAngle(v1, v2, glm::vec3(0, 0, -1));
	float a2 = glm::orientedAngle(v2, v3, glm::vec3(0, 0, -1));
	float a3 = glm::orientedAngle(v3, v1, glm::vec3(0, 0, -1));
	if (a1 < 0 && a2 < 0 && a3 < 0) return true;
	else return false;
}