#include "Camera.h"

Camera::Camera() {
    cMode = 0;
    cPlayer = NULL;

    posX = 0.0f;
    posY = 4.0f;
    posZ = 0.0f;

    followDistance = 8.0f;
    followHeight = 2.0f;

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

void Camera::adjustForFollowCam() {
    if(cPlayer != NULL) {
            this->posX = cPlayer->posX * 1.5;
            this->posY = cPlayer->posY + followHeight;
            this->posZ = cPlayer->posZ + followDistance;
    }
}

void Camera::update(float dt) {
    // Update the position of the camera
    // cMode 0: Follow Cam
    if(this->cMode == 0) {
        // Update based on Player pointer's position
        adjustForFollowCam();
    }

    // cMode 1: Free Cam
    if(hasMoved && this->cMode == 1) {
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
bool Camera::setPlayer(Player *cP) {
    if(cP == NULL) {
        std::cout << "Failed: Camera::setPlayer()... input parameter was NULL.\n";
        return false;
    }
    cPlayer = cP;
    adjustForFollowCam();
    std::cout << "Success: Camera::setPlayer()... \n";
    return true;
}

void Camera::changeMode() {
    if(cMode == 0) {
        cMode = 1;
    } else if (cMode == 1) {
        ///-- TODO: have this reset camera position relative to player
        cMode = 0;
    }
    std::cout << "Success: Camera::changeMode()... \n";
    return;
}

