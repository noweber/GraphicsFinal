#include "Player.h"

Player::Player() {
    posX = 0.0f;
    posY = 4.0f;
    posZ = 8.0f;

    posDX = 0.0f;
    posDY = 0.0f;
    posDZ = 0.0f;

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

    isMoving = false;
    moveCt = 0;

    shellOffsetY = 0;
    shellOffsetZ = 0;

    headOffsetX = 0;
    headOffsetY = 0;

    leftFootOffsetY = 0;
    rightFootOffsetY = 0;
}

Player::~Player() {
    //dtor
}

void Player::movePlayer() {
    if(!this->isMoving) {
        this->isMoving = true;
        this->moveCt = 59;
    }
}

void Player::update(float dt) {
    // Update the position of the camera
    float newX = 0.0f;
    float newY = 0.0f;
    float newZ = 0.0f;

    if(hasMoved) {
        if(movedLeft){
            newX = posX -  dt * velocityX;
            posDX = newX - posX;
            posX = newX;
        }
        if(movedRight){
            newX = posX + dt * velocityX;
            posDX = newX - posX;
            posX = newX;
        }

        if(movedUp) {
            newY = posY + dt * velocityY;
            posDY = newY - posY;
            posY = newY;
        }
        if(movedDown) {
            newY = posY - dt * velocityY;
            posDY = newY - posY;
            posY = newY;
        }

        if(movedForward) {
            newZ = posZ - dt * velocityZ;
            posDZ = newZ - posZ;
            posZ = newZ;
        }
        if(movedBackward) {
            newZ = posZ + dt * velocityZ;
            posDZ = newZ - posZ;
            posZ = newZ;
        }

        // move these inside the if statements to fix?
       /* posDX = newX - posX;
        posX = newX;

        posDY = newY - posY;
        posY = newY;

        posDZ = newZ - posZ;
        posZ = newZ;*/



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

/*
void Player::update(float dt) {
    if(this->moveCt > 0) {
        if(this->moveCt > 30) {

            if(this->moveCt > 45) {
                this->headOffsetX -= 0.01;
                this->leftFootOffsetY += 0.01;
            } else {
                this->headOffsetX += 0.01;
                this->leftFootOffsetY -= 0.01;
            }

            this->shellOffsetY += 0.0008;
            this->headOffsetY += 0.005;


            //this->leftFootOffsetY += 0.01;
            //this->rightFootOffsetY -= 0.01;

        } else {

            if(this->moveCt > 15) {
                this->headOffsetX += 0.01;
                this->rightFootOffsetY += 0.01;
            } else {
                this->headOffsetX -= 0.01;
                this->rightFootOffsetY -= 0.01;
            }

            this->shellOffsetY -= 0.0008;
            this->headOffsetY -= 0.005;

            //this->leftFootOffsetY -= 0.01;
            //this->rightFootOffsetY += 0.01;
        }
        this->moveCt -= 1;
    } else if (this->moveCt == 0) {
        /// Reset all values
        this->isMoving = false;

        this->shellOffsetY = 0;
        this->shellOffsetZ = 0;

        this->headOffsetX = 0;
        this->headOffsetY = 0;

        this->leftFootOffsetY = 0;
        this->rightFootOffsetY = 0;
    }
}
*/
