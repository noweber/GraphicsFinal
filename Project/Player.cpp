#include "Player.h"

Player::Player() {
    posX = 0.0f;
    posY = 4.0f;
    posZ = 0.0f;

    posDX = 0.0f;
    posDZ = 0.0f;

    velocityX = 0.02;
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

    // Assign the current level pointer to NULL
    cLevel = NULL;
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

bool Player::canMove() {
    if(this->cLevel == NULL) {
        return false;
    }

    ///-- TODO: Player::canMove()... make this function check for valid moves.
    // Checks all of the boolean move directions flagged from the event handler.
    return true;

}

void Player::update(float dt) {
    // Update the position of the camera
    float newX = 0.0f;
    float newY = 0.0f;
    float newZ = 0.0f;

    // Check if the player has moved...
    if(hasMoved) {
        // Now check if that move is valid...
        if(this->canMove()) {
            if(movedLeft){
                posDX = -dt * velocityX;
                posX += posDX;
            }
            if(movedRight){
                posDX = dt * velocityX;
                posX += posDX;
            }

            if(movedUp) {
                posY += dt * velocityY;
            }
            if(movedDown) {
                posY -= dt * velocityY;
            }

            if(movedForward) {
                //posZ -= dt * velocityZ;
                posDZ = -dt * velocityZ;
                posZ += posDZ;
            }
            if(movedBackward) {
                //posZ += dt * velocityZ;
                posDZ = dt * velocityZ;
                posZ += posDZ;
            }
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

bool Player::changeLevel(Level *cL) {
    if(cL == NULL) {
        std::cout << "Failed: Player::changeLevel()... input parameter was NULL.\n";
        return false;
    }

    this->cLevel = cL;
    this->posX = cL->defaultStartX;
    this->posZ = cL->defaultStartZ;

    std::cout << "Success: Player::changeLevel()... \n";
    return true;
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
