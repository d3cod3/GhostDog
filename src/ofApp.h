#pragma once

#include "ofMain.h"

#if !defined(TARGET_WIN32)
#include <pwd.h>
#endif

#include "Emitter.h"

typedef enum { MoveStop, MoveLeft, MoveRight, MoveUp, MoveDown } MoveDir;

class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);

    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);

    void windowResized(int w, int h);

    void parseArgs();


    void    initDataFolderFromBundle();


    void    checkCollisions();
    void    checkPlayerDeath();
    void    updateSprite();
    void    moveSprite(MoveDir);
    void    stopSprite();
    void    startAccel();
    void    stopAccel();
    float   modulateAccel(float);
    void    autopilot();


    std::filesystem::path           ghostPath;
    string                          userHome;


    ofTrueTypeFont  titleFont;
    ofTrueTypeFont  mainFont;
    ofTrueTypeFont  mainFontBig;
    string          gameState;
    string          scrollingMessage;
    bool            blinkTitle;
    size_t          resetTime;
    size_t          blinkTime;
    size_t          deathTime;
    size_t          reloadTime;
    size_t          autopilotTime;
    bool            startAutoKill;

    Sprite          sprite;
    Emitter         *enemyEmitter1, *enemyEmitter2, *gun;
    ofVec3f         start_point;
    ofVec3f         finish_point;
    float           start_time;
    float           finish_time;
    ofVec3f         mouse_last;
    MoveDir         moveDir;
    bool            accel;
    ofVec3f         startAccelPoint;
    ofImage         playerImage;
    ofImage         bulletImage;
    ofImage         enemyImage;

    ofSoundPlayer   shot;
    ofSoundPlayer   playerDeath;
    ofSoundPlayer   playerExplosion;
    ofSoundPlayer   gameOver;
    ofSoundPlayer   backgroundLoop;
    bool            playOnce;

    float           e1R, e2R;

    int             score;
    int             lastScore;
    int             lifes;


private:


};
