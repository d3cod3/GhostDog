#include "ofMain.h"
#include "ofApp.h"

#include "ofAppGLFWWindow.h"

//========================================================================
int main(int argc, char *argv[]){

    ofGLFWWindowSettings settings;
    settings.setGLVersion(4, 1);
    settings.stencilBits = 0;
    settings.setSize(1280,960);
    settings.setPosition(ofVec2f(0,0));
    settings.resizable = true;
    settings.decorated = true;

    shared_ptr<ofAppGLFWWindow> ghostWindow = dynamic_pointer_cast<ofAppGLFWWindow>(ofCreateWindow(settings));
    #ifdef TARGET_LINUX
    ghostWindow->setWindowIcon("images/ghost-dog.png");
    #endif

    shared_ptr<ofApp> ghostApp(new ofApp);

    ofRunApp(ghostWindow, ghostApp);
    ofRunMainLoop();

    // done
    return EXIT_SUCCESS;

}
