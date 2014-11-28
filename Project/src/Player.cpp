

Player::Player() {
    posX = 0.0f;
    posY = 0.0f;
    posZ = 0.0f;

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

void Player::update() {
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
