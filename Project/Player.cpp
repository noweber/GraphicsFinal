#include "Player.h"

Player::Player() {
    posX = 0.0f;
    posY = 4.0f;
    posZ = 0.0f;

    posDX = 0.0f;
    posDZ = 0.0f;

    // velocityX = 0.015;
    velocityX = 0.03;
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
    isJumping = false;
    jumpOffset = 0.0f;
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
    //if(posX > -(cLevel->lWidth/2))
    return true;

}

void Player::update(float dt) {
    // Update the position of the camera
    float newX = 0.0f;
    float newY = 0.0f;
    float newZ = 0.0f;
    float edgeAdjust = 0.4f;
    // Check if the player has moved...
    if(hasMoved) {
        // Now check if that move is valid...
        if(this->canMove()) {
            movePlayer();

            if(movedLeft){
                if(posX > -((cLevel->lWidth/2)*cLevel->xDrawingScale) - edgeAdjust) {
                    posDX = -dt * velocityX;
                    posX += posDX;
                }
            } else if (movedLeft && posX < -((cLevel->lWidth/2)*cLevel->xDrawingScale) - edgeAdjust) {
                posX = -((cLevel->lWidth/2)*cLevel->xDrawingScale) - edgeAdjust;
            }




            if(movedRight){
                if(posX < ((cLevel->lWidth/2)*cLevel->xDrawingScale) + edgeAdjust) {
                    posDX = dt * velocityX;
                    posX += posDX;
                }
            } else if (movedRight && posX > -((cLevel->lWidth/2)*cLevel->xDrawingScale) + edgeAdjust) {
                posX = ((cLevel->lWidth/2)*cLevel->xDrawingScale) + edgeAdjust;
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

    // Animate the turtle
    if(this->moveCt > 0) {
        if(this->moveCt > 30) {

            if(this->moveCt > 45) {
                this->headOffsetX -= 0.01;
                this->leftFootOffsetY += 0.02;
            } else {
                this->headOffsetX += 0.01;
                this->leftFootOffsetY -= 0.02;
            }

            this->shellOffsetY += 0.0008;
            this->headOffsetY += 0.005;

            if(isJumping) {
                jumpOffset += 0.02;
            }


            //this->leftFootOffsetY += 0.01;
            //this->rightFootOffsetY -= 0.01;

        } else {

            if(this->moveCt > 15) {
                this->headOffsetX += 0.01;
                this->rightFootOffsetY += 0.02;
            } else {
                this->headOffsetX -= 0.01;
                this->rightFootOffsetY -= 0.02;
            }

            this->shellOffsetY -= 0.0008;
            this->headOffsetY -= 0.005;

            if(isJumping) {
                jumpOffset -= 0.02;
            }

            //this->leftFootOffsetY -= 0.01;
            //this->rightFootOffsetY += 0.01;
        }
        this->moveCt -= 1;
    } else if (this->moveCt == 0) {
        /// Reset all values
        this->isMoving = false;
        this->isJumping = false;
        jumpOffset = 0;

        this->shellOffsetY = 0;
        this->shellOffsetZ = 0;

        this->headOffsetX = 0;
        this->headOffsetY = 0;

        this->leftFootOffsetY = 0;
        this->rightFootOffsetY = 0;
    }

    // Update player's position on the level
    if(cLevel != NULL) {
        cLevel->playerPosXOnLevel = (int) (posX/cLevel->xDrawingScale) + (cLevel->lWidth/2);
        cLevel->playerFloatX = (posX/cLevel->xDrawingScale) + (cLevel->lWidth/2);
        // Now reverse the positioning for collision corrections
        //int temp = cLevel->playerPosXOnLevel;
        //cLevel->playerPosXOnLevel = cLevel->lWidth - temp;




        //std::cout << "Player Position On Level: " << cLevel->playerPosXOnLevel << "\n";
    }

    // Check for a power up
    if(cLevel->frontLane < cLevel->nLanes) {
        if(cLevel->lanes[cLevel->frontLane].paths[cLevel->playerPosXOnLevel] == 5) {
            cLevel->lanes[cLevel->frontLane].paths[cLevel->playerPosXOnLevel] = 0;
            std::cout << "Power Up!\n";
            velocityX += 0.008f;
        }
    }

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
