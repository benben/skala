#include "ofMain.h"
#include "testApp.h"
#include "ofAppGlutWindow.h"

#define LIVE

//========================================================================
int main( ){

    ofAppGlutWindow window;

    int x = 800;
    int y = 600;

    #ifdef LIVE
        ofSetupOpenGL(&window, x*4,y, OF_WINDOW);
        ofSetWindowPosition(ofGetScreenWidth()-x,0);
    #else
        ofSetupOpenGL(&window, x,y, OF_WINDOW);
        ofSetWindowPosition(ofGetScreenWidth()-x-50,50);
    #endif

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp( new testApp());

}
