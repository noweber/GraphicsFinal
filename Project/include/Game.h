#ifndef GAME_H
#define GAME_H

class Game {
    public:
        Game();
        ~Game();

    void init();

    // Updates all active game objects based on the amount of time that has passed
    void update(int time);



    protected:
    private:
};

#endif // GAME_H
