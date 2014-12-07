#include "Camera.h"

Camera::Camera() {
    posX = 0.0f;
    posY = 4.0f;
    posZ = 8.0f;

    velocityX = 0.04;
    velocityY = 0.004;
    velocityZ = 0.04;

    hasMoved = false;
    movedLeft = false;
    movedRight = false;
    movedUp = false;
    movedDown = false;
    movedForward = false;
    movedBackward = false;

    horiAngle = 1.14f;
    vertAngle = 0.0f;
    speed = 0.01f;
    mouseSpeed = 0.005f;

}

void Camera::update(float dt) {
    // Update the position of the camera
    if(hasMoved) {
        if(movedLeft){
            posX -= dt * velocityX;
        }
        if(movedRight){
            posX += dt * velocityX;
        }

        if(movedUp) {
            posY += dt * velocityY;
        }
        if(movedDown) {
            posY -= dt * velocityY;
        }

        if(movedForward) {
            posZ -= dt * velocityZ;
        }
        if(movedBackward) {
            posZ += dt * velocityZ;
        }
    }
    // Reset the movement states
    hasMoved = false;
    movedLeft = false;
    movedRight = false;
    movedUp = false;
    movedDown = false;
    movedForward = false;
    movedBackward = false;

}

