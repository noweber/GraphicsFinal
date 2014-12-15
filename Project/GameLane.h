#ifndef GAMELANE_H_INCLUDED
#define GAMELANE_H_INCLUDED

class GameLane {

public:
    //Lane(int width, float x, float z);
    GameLane(int width);


    // The width of the lane
    int nPaths;
    // Whether or not this lane has been passed by the player
    bool isPassed;

    // The path slots containing obstacles or openings within each lane
    int *paths;
    // The height of the obstacles used for animated them up and down.
    //float *pathHeights;
    //int *pathTxt;

    // The Lane's position
    float posX;
    float posZ;



    protected:
    private:
};


#endif // GAMELANE_H_INCLUDED
