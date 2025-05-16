#pragma once
#include "Shape.h"

class ShapeIntegrate : public Shape {
public:
    void integrator();
    void addTorque(glm::vec3);
    void addForce(glm::vec3);

    glm::vec3 velocity = glm::vec3(0, 0, 0);
    glm::vec3 velocity_angle = glm::vec3(0, 0, 0);

    glm::vec3 accel = glm::vec3(0, 0, 0);
    glm::vec3 accel_angle = glm::vec3(0, 0, 0);

    float linear_damping = 0.98f;   //Reduce drift
    float angular_damping = 0.90f;  //Reduce spin

    float thrust = 1;
    float mass = 1;
    float inertia = 0.9;

    glm::vec3 torque = glm::vec3(0, 0, 0);
    glm::vec3 force = glm::vec3(0, 0, 0);

    glm::vec3 prev_force = glm::vec3(0, 0, 0);
    glm::vec3 prev_velocity = glm::vec3(0, 0, 0);

};


void ShapeIntegrate::integrator() {

    //Gets seconds since last frame
    float dt = ofGetLastFrameTime();

    //Move shape depending on velocity magnitude and time
    pos += velocity * dt;

    //Update velocity depending on acceleration and linear damping
    velocity += accel * dt;
    velocity *= linear_damping;

    //Calculate angular acceleration from torque and inertia
    accel_angle.z = torque.z / inertia;

    //Calculate the rotation speed of the shape using angular acceleration
    velocity_angle.z += accel_angle.z * dt;

    //Factor in angular damping into angular velocity
    velocity_angle.z *= angular_damping;

    //Calculate rotation based on angular velocity and time
    rot += velocity_angle.z * dt;

    //Calculate acceleration (newton's law)
    accel = force / mass;

    prev_force = force;
    prev_velocity = velocity;

    //Reset torque and force
    torque = glm::vec3(0, 0, 0);
    force = glm::vec3(0, 0, 0);
}

void ShapeIntegrate::addTorque(glm::vec3 new_torque) {
    torque += new_torque;
}

void ShapeIntegrate::addForce(glm::vec3 new_force) {
    force += new_force;
}