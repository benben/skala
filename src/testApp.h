#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"

class testApp : public ofBaseApp{

	public:

		void setup();
		void update();
		void draw();
		void exit();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);


        static const int    INPUT_NUM = 5;
        static const int    OUTPUT_NUM = 3;

		ofVideoGrabber 		vidGrabber[INPUT_NUM];
        ofTexture           camPic[INPUT_NUM];
		ofTexture           output[OUTPUT_NUM];

		int 				camWidth, camHeight;
        int                 picWidth, picHeight;
        int                 outputWidth, outputHeight;

        int                 picx, picy;

		int                 io_mapping[OUTPUT_NUM];
		int                 switch_input, switch_output;

		int                 status;
		const char *        status_msg;

		unsigned char * 	videoNull, *videoNoise;

        unsigned long       previousMillis;
        int                 interval, delayed_output;

        int                 activeCam, oldCam, hoppingOutput;

        bool                help, testscreen, notYetHopped;

        ofTexture           noise_preview;

        bool grabit;
};

#endif
