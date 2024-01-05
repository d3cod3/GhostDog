#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    ofSetEscapeQuitsApp(false);
    ofSetFrameRate(60);
    ofEnableAntiAliasing();
    ofSetWindowTitle("Ghost Dog");
    ofSetVerticalSync(true);

    initDataFolderFromBundle();

    // Load sounds
    shot.load("sounds/shot.wav");
    playerDeath.load("sounds/playerDeath.wav");
    playerExplosion.load("sounds/playerExplosion.wav");
    gameOver.load("sounds/gameOver.wav");
    backgroundLoop.load("sounds/background_loop.wav");
    backgroundLoop.setLoop(OF_LOOP_NORMAL);

    // Set up player sprite
    if (playerImage.load("images/ship.png")) {
        sprite.setImage(playerImage);
    }
    start_point = ofVec3f(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2, 0);
    finish_point = ofVec3f(650, 500);
    sprite.trans.set(start_point);
    sprite.speed = 450;   // in pixels per second (screenspace 1 unit = 1 pixel)
    moveDir = MoveStop;

    // Set up bullet emitter
    gun = new Emitter(new SpriteSystem());
    if (bulletImage.load("images/bullet.png")) {
        gun->setChildImage(bulletImage);
    }
    gun->setPosition(ofVec3f(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2 - 60, 0));
    gun->start();

    // Set up enemy emitters
    enemyEmitter1 = new Emitter(new SpriteSystem());
    if (enemyImage.load("images/enemy.png")) {
        enemyImage.resize(50, 50);
        enemyEmitter1->setChildImage(enemyImage);
    }
    enemyEmitter1->setPosition(ofVec3f(300, -50, 0));
    enemyEmitter1->velocity.set(50, 500, 0);
    enemyEmitter1->setLifespan(5000);
    enemyEmitter1->setRate(1);
    enemyEmitter1->setChildSize(50, 50);
    enemyEmitter1->start();

    enemyEmitter2 = new Emitter(new SpriteSystem());
    if (enemyImage.load("images/enemy2.png")) {
        enemyImage.resize(50, 50);
        enemyEmitter2->setChildImage(enemyImage);
    }
    enemyEmitter2->setPosition(ofVec3f(ofGetWindowWidth() - 300, -50, 0));
    enemyEmitter2->velocity.set(50, 500, 0);
    enemyEmitter2->setLifespan(5000);
    enemyEmitter2->setRate(1);
    enemyEmitter2->setChildSize(50, 50);
    enemyEmitter2->start();

    e1R = 1;
    e2R = 1;

    score = 0;
    lastScore = 0;
    lifes = 3;

    titleFont.load("fonts/ka1.ttf",64);
    mainFont.load("fonts/ArcadepixPlus.ttf",26);
    mainFontBig.load("fonts/ArcadepixPlus.ttf",86);

    blinkTitle = true;
    resetTime = ofGetElapsedTimeMillis();
    blinkTime = 500;
    deathTime = ofGetElapsedTimeMillis();
    reloadTime = ofGetElapsedTimeMillis();
    autopilotTime = ofGetElapsedTimeMillis();

    startAutoKill = false;
    playOnce = false;

    gameState = "start";

    stopAccel();

}

//--------------------------------------------------------------
void ofApp::update(){

    if(gameState == "start") {
        // message to play
        e1R = 1;
        e2R = 1;
        enemyEmitter1->setRate(e1R);
        enemyEmitter2->setRate(e2R);

        score = 0;
        lastScore = 0;
        lifes = 3;

        updateSprite();
        gun->update();
        gun->setRate(5);
        gun->setLifespan(2000);
        gun->setVelocity(ofVec3f(0, -500, 0));
        enemyEmitter1->update();
        enemyEmitter2->update();
        enemyEmitter1->velocity.set(ofRandom(-300,500), 500, 0);
        enemyEmitter2->velocity.set(ofRandom(-500,300), 500, 0);
        checkCollisions();
        autopilot();
    }else if (gameState == "game") {
        // play
        updateSprite();
        gun->setRate(5);
        gun->setLifespan(2000);
        gun->setVelocity(ofVec3f(0, -500, 0));
        gun->update();
        enemyEmitter1->update();
        enemyEmitter2->update();
        checkCollisions();
        checkPlayerDeath();

        if(lastScore != score){
            lastScore = score;
            shot.play();
        }

        // make it interesting over time
        e1R += ofRandom(0.0005,0.002);
        e2R += ofRandom(0.0005,0.002);

        enemyEmitter1->setRate(e1R);
        enemyEmitter2->setRate(e2R);
        enemyEmitter1->velocity.set(ofRandom(-300,500), 500, 0);
        enemyEmitter2->velocity.set(ofRandom(-500,300), 500, 0);
    }else if (gameState == "death") {
        // player death
        if(playOnce){
            playOnce = false;
            playerExplosion.play();
        }

        updateSprite();
        enemyEmitter1->update();
        enemyEmitter2->update();

        if(ofGetElapsedTimeMillis() - deathTime > 3300){
            gameState = "game";
        }
    }else if (gameState == "end") {
        // game over
        updateSprite();
        enemyEmitter1->update();
        enemyEmitter2->update();

        if(ofGetElapsedTimeMillis() - reloadTime > 7000){
            gameState = "start";
        }
    }


}

//--------------------------------------------------------------
void ofApp::draw(){
    // Set background gradient
    ofColor colorOne(0, 0, 0);
    ofColor colorTwo(15, 32, 39);
    ofBackgroundGradient(colorOne, colorTwo, OF_GRADIENT_LINEAR);

    if(gameState == "start") {
        // message to play
        ofSetColor(255,255,255,200);
        titleFont.drawString("GHOST DOG",ofGetWindowWidth()/2 - 330,ofGetWindowHeight()/2);
        if(ofGetElapsedTimeMillis()-resetTime > blinkTime){
            resetTime = ofGetElapsedTimeMillis();
            blinkTitle = !blinkTitle;
        }
        if(blinkTitle){
            mainFont.drawString("Press any key to play",ofGetWindowWidth()/2 - 160,ofGetWindowHeight()/2 + 60);
        }
        sprite.draw();
        gun->draw();
        enemyEmitter1->draw();
        enemyEmitter2->draw();
    }else if (gameState == "game") {
        // play
        sprite.draw();
        gun->draw();
        enemyEmitter1->draw();
        enemyEmitter2->draw();
    }else if (gameState == "death") {
        // player death
        if(ofGetElapsedTimeMillis()-resetTime > 100){
            resetTime = ofGetElapsedTimeMillis();
            blinkTitle = !blinkTitle;
        }
        if(blinkTitle){
            sprite.draw();
        }
        enemyEmitter1->draw();
        enemyEmitter2->draw();
    }else if (gameState == "end") {
        // game over
        ofPushMatrix();
        ofRotateXDeg(ofGetElapsedTimeMillis()*100);
        sprite.draw();
        ofPopMatrix();
        enemyEmitter1->draw();
        enemyEmitter2->draw();

        if(ofGetElapsedTimeMillis()-resetTime > 400){
            resetTime = ofGetElapsedTimeMillis();
            blinkTitle = !blinkTitle;
        }
        if(blinkTitle){
            mainFontBig.drawString("GAME OVER",ofGetWindowWidth()/2 - 260,ofGetWindowHeight()/2);
        }
    }

    // Draw current score
    string scoreText;
    scoreText += "Score: " + std::to_string(score);
    mainFont.drawString(scoreText,20,60);

    // Draw lifes
    ofSetColor(255,255,255,200);
    for(int i=0;i<lifes;i++){
        playerImage.draw(ofGetWindowWidth()-((i+1)*playerImage.getWidth()*1.3),20);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(gameState != "end"){
        switch (key) {
        case OF_KEY_RIGHT:
            if(sprite.trans.x < ofGetWindowWidth() - sprite.width){
                moveSprite(MoveRight);
            }else{
                stopSprite();
            }
            break;
        case OF_KEY_LEFT:
            if(sprite.trans.x > sprite.width){
                moveSprite(MoveLeft);
            }else{
                stopSprite();
            }
            break;
        case OF_KEY_UP:
            if(sprite.trans.y > sprite.height){
                moveSprite(MoveUp);
            }else{
                stopSprite();
            }
            break;
        case OF_KEY_DOWN:
            if(sprite.trans.y < ofGetWindowHeight() - sprite.height){
                moveSprite(MoveDown);
            }else{
                stopSprite();
            }
            break;
        case OF_KEY_CONTROL:
            startAccel();
            break;
        }
    }

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

    if(gameState == "start") {
        gameState = "game";
        stopSprite();
        sprite.trans.set(ofVec3f(ofGetWindowWidth() / 2, ofGetWindowHeight() -100, 0));
        gun->setPosition(ofVec3f(ofGetWindowWidth() / 2, ofGetWindowHeight() -160, 0));
        backgroundLoop.play();
    }

    if(gameState != "end"){
        switch (key) {
        case OF_KEY_LEFT:
        case OF_KEY_RIGHT:
        case OF_KEY_UP:
        case OF_KEY_DOWN:
            stopSprite();
            stopAccel();
            break;
        case OF_KEY_CONTROL:
            stopAccel();
            break;
        }
    }
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

    ofPoint mouse_cur = ofPoint(x, y);
    ofVec3f delta = mouse_cur - mouse_last;
    sprite.trans += delta;
    gun->trans += delta;
    mouse_last = mouse_cur;
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    mouse_last = ofPoint(x, y);
}

//--------------------------------------------------------------
void ofApp::initDataFolderFromBundle(){
    string _bundleDataPath;

#ifdef TARGET_OSX
    CFURLRef appUrl = CFBundleCopyBundleURL(CFBundleGetMainBundle());
    CFStringRef appPath = CFURLCopyFileSystemPath(appUrl, kCFURLPOSIXPathStyle);

    const CFIndex kCStringSize = 128;
    char temporaryCString[kCStringSize];
    bzero(temporaryCString,kCStringSize);
    CFStringGetCString(appPath, temporaryCString, kCStringSize, kCFStringEncodingUTF8);
    std::string *appPathStr = new std::string(temporaryCString);
    CFRelease(appUrl);
    CFRelease(appPath);

    CFURLRef resourceUrl = CFBundleCopyResourcesDirectoryURL(CFBundleGetMainBundle());
    CFStringRef resourcePath = CFURLCopyFileSystemPath(resourceUrl, kCFURLPOSIXPathStyle);

    bzero(temporaryCString,kCStringSize);
    CFStringGetCString(resourcePath, temporaryCString, kCStringSize, kCFStringEncodingUTF8);
    std::string *resourcePathStr = new std::string(temporaryCString);
    CFRelease(resourcePath);
    CFRelease(resourceUrl);

    _bundleDataPath = *appPathStr + "/" + *resourcePathStr + "/";

    const char *homeDir = getenv("HOME");

    if(!homeDir){
        struct passwd* pwd;
        pwd = getpwuid(getuid());
        if (pwd){
            homeDir = pwd->pw_dir;
        }
    }

    string _GDDataPath(homeDir);
    userHome = _GDDataPath;

    _GDDataPath += "/Documents/GhostDog/data";

    std::filesystem::path tempPath(_GDDataPath.c_str());

    ghostPath = tempPath;

    // Create data directory
    if(!ofDirectory::doesDirectoryExist(ghostPath)){
        ofDirectory::createDirectory(ghostPath,true,true);

        std::filesystem::path dataPath(_bundleDataPath.c_str());

        ofDirectory dataDir(dataPath);
        dataDir.copyTo(ghostPath,true,true);
    }

    ofSetDataPathRoot(ghostPath);

#endif
}


//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    ofSetWindowShape(1280,960);
}

//  This is a simple O(M x N) collision check
//  For each missle check to see which invaders you hit and remove them
//--------------------------------------------------------------
void ofApp::checkCollisions() {

    // find the distance at which the two sprites (missles and invaders) will collide
    // detect a collision when we are within that distance.
    //
    float collisionDist = gun->childHeight / 2 + enemyEmitter1->childHeight / 2;
    float lifeCollisionDist = sprite.height / 2 + enemyEmitter1->childHeight / 2;

    // Loop through all the missiles, then remove any invaders that are within
    // "collisionDist" of the missiles.  the removeNear() function returns the
    // number of missiles removed.
    //
    for (int i = 0; i < gun->sys->sprites.size(); i++) {
        if(gameState == "game"){
            score += enemyEmitter1->sys->removeNear(gun->sys->sprites[i].trans, collisionDist);
            score += enemyEmitter2->sys->removeNear(gun->sys->sprites[i].trans, collisionDist);
        }else{
            enemyEmitter1->sys->removeNear(gun->sys->sprites[i].trans, collisionDist);
            enemyEmitter2->sys->removeNear(gun->sys->sprites[i].trans, collisionDist);
        }
    }


}

//--------------------------------------------------------------
void ofApp::checkPlayerDeath(){
    for (int e = enemyEmitter1->sys->sprites.size()-1; e >= 0; e--) {
        if (ofDist(sprite.trans.x, sprite.trans.y, enemyEmitter1->sys->sprites[e].trans.x, enemyEmitter1->sys->sprites[e].trans.y) < (enemyEmitter1->sys->sprites[e].height + sprite.height)/2) {
            if(lifes > 1){
                gameState = "death";
                playOnce = true;
                lifes--;
                deathTime = ofGetElapsedTimeMillis();
            }else{
                playerDeath.play();
                backgroundLoop.stop();
                gameOver.play();
                gameState = "end";
                lifes--;
                stopSprite();
                stopAccel();
                reloadTime = ofGetElapsedTimeMillis();
            }
        }
    }

    for (int e = enemyEmitter2->sys->sprites.size()-1; e >= 0; e--) {
        if (ofDist(sprite.trans.x, sprite.trans.y, enemyEmitter2->sys->sprites[e].trans.x, enemyEmitter2->sys->sprites[e].trans.y) < (enemyEmitter2->sys->sprites[e].height + sprite.height)/2) {
            if(lifes > 1){
                gameState = "death";
                playOnce = true;
                lifes--;
                deathTime = ofGetElapsedTimeMillis();
            }else{
                playerDeath.play();
                backgroundLoop.stop();
                gameOver.play();
                gameState = "end";
                lifes--;
                stopSprite();
                stopAccel();
                reloadTime = ofGetElapsedTimeMillis();
            }
        }
    }
}

//--------------------------------------------------------------
float ofApp::modulateAccel(float dist) {
    return sin(dist * PI) * 5.0 + 1.0;
}

//--------------------------------------------------------------
void ofApp::updateSprite() {

    // Calculate distance to travel for this update
    float dist = sprite.speed * 1 / 60;
    ofVec3f dir;
    ofRectangle r = ofGetWindowRect();

    //  If the accelerator modifer key is pressed, accelerate and
    //  deacclerate sprite from starting position to window edge
    if (accel) {
        switch (moveDir) {
        case MoveUp:
        {
            float totalDist = startAccelPoint.y;
            float frac = sprite.trans.y / totalDist;
            dir = ofVec3f(0, -dist * modulateAccel(frac), 0);
        }
            break;
        case MoveDown:
        {
            float totalDist = r.getMaxY() - startAccelPoint.y;
            float frac = sprite.trans.y / totalDist;
            dir = ofVec3f(0, dist * modulateAccel(frac), 0);
        }
            break;
        case MoveLeft:
        {
            float totalDist = startAccelPoint.x;
            float frac = sprite.trans.x / totalDist;
            dir = ofVec3f(-dist * modulateAccel(frac), 0, 0);
        }
            break;
        case MoveRight:
        {
            float totalDist = r.getMaxX() - startAccelPoint.x;
            float frac = sprite.trans.x / totalDist;
            dir = ofVec3f(dist * modulateAccel(frac), 0, 0);
            break;
        }
        case MoveStop:
        {
            break;
        }
        }
    } else {
        switch (moveDir) {
        case MoveUp:
            dir = ofVec3f(0, -dist, 0);
            break;
        case MoveDown:
            dir = ofVec3f(0, dist, 0);
            break;
        case MoveLeft:
            dir = ofVec3f(-dist, 0, 0);
            break;
        case MoveRight:
            dir = ofVec3f(dist, 0, 0);
            break;
        case MoveStop:
            break;
        }
    }

    // Translate sprite and bullet emitter together
    sprite.trans += dir;
    gun->trans += dir;
}

//--------------------------------------------------------------
void ofApp::moveSprite(MoveDir dir) {
    moveDir = dir;
}

//--------------------------------------------------------------
void ofApp::stopSprite() {
    moveDir = MoveStop;
}

//--------------------------------------------------------------
void ofApp::startAccel() {
    startAccelPoint = sprite.trans;
    accel = true;
}

//--------------------------------------------------------------
void ofApp::stopAccel() {
    accel = false;
}

//--------------------------------------------------------------
void ofApp::autopilot() {

    if(!startAutoKill){
        moveSprite(MoveDown);
        if(sprite.trans.y > ofGetWindowHeight()-100){
            stopSprite();
            startAutoKill = true;
        }
    }

    if(ofGetElapsedTimeMillis()-autopilotTime > 800 && startAutoKill){
        autopilotTime = ofGetElapsedTimeMillis();
        if(ofRandomuf() < 0.5){
            if(sprite.trans.x > 360){
                moveSprite(MoveLeft);
            }else{
                moveSprite(MoveRight);
            }

        }else{
            if(sprite.trans.x < ofGetWindowWidth() - 300){
                moveSprite(MoveRight);
            }else{
                moveSprite(MoveLeft);
            }
        }
    }
}
