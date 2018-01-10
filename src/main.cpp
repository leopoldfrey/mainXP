#include "ofMain.h"
#include "ofApp.h"

int main( )
{
    
#ifndef _WIN32
    ofAppGlutWindow window;
    window.setGlutDisplayString("rgba double alpha samples>=4");
    window.setWindowTitle("Main");
    ofSetupOpenGL(&window, WIDTH,HEIGHT,OF_WINDOW);
#else
    ofSetupOpenGL(WIDTH,HEIGHT,OF_WINDOW);
#endif
    ofSetWindowTitle("Main");
    ofRunApp(new ofApp());
  //*/
    
/*
    
    ofGLFWWindowSettings settings;
    
    settings.width = WIDTH;
    settings.height = HEIGHT;
    settings.resizable = true;
    shared_ptr<ofAppBaseWindow> mainWindow = ofCreateWindow(settings);
    shared_ptr<ofApp> mainApp(new ofApp);
    ofRunApp(mainWindow, mainApp);
    
    settings.width = 300;
    settings.height = 300;
    settings.resizable = true;
    shared_ptr<ofAppBaseWindow> graphWindow = ofCreateWindow(settings);
    shared_ptr<mainGraph> graphApp(new mainGraph);
    ofRunApp(graphWindow, graphApp);
    
    ofRunMainLoop();//*/

}
