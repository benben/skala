#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofLogLevel(OF_LOG_VERBOSE);
    ofSetFrameRate(60);

	camWidth 		= 720;	// try to grab at this size.
	camHeight 		= 576;

	picWidth  = 260;
	picHeight = 195;

	outputWidth = 800;
	outputHeight = 600;

	grabit = false;

	//FOR DELAYED SOLO EFFECT
    previousMillis = ofGetElapsedTimeMillis();
    interval = 1000;
    delayed_output = 0;

    for(int i = 0; i < INPUT_NUM; i++) {
        //SETUP ALL INPUTS
        vidGrabber[i].setVerbose(true);
        vidGrabber[i].setDeviceID(i);
        vidGrabber[i].initGrabber(camWidth,camHeight);

        camPic[i].allocate(camWidth,camHeight, GL_RGB);
        //maybe add a blackscreen to the pic here
    }
    //PRINT ALL INPUTS
    vidGrabber[0].listDevices();

    for(int i = 0; i < OUTPUT_NUM; i++) {
        //SETUP ALL OUTPUTS
        output[i].allocate(camWidth,camHeight, GL_RGB);

        //SETUP IO_MAPPING
        //set all outputs to nothing
        io_mapping[i] = 0;
    }

    //SETUP STATUS
    status = 0;

    //SETUP SWITCH VARS
    switch_input = 0;
    switch_output = 0;

    hoppingOutput = 0;

    notYetHopped = false;

    //SETUP A BLACKSCREEN
    videoNull = new unsigned char[camWidth*camHeight*3];

    //SETUP WHITE NOISE
    videoNoise = new unsigned char[camWidth*camHeight];
    noise_preview.allocate(picWidth,picHeight/2, GL_LUMINANCE);
}

//--------------------------------------------------------------
void testApp::update(){

	ofBackground(0,0,0);

    //UPDATE INPUTS
    for(int i = 0; i < INPUT_NUM; i++) {
        vidGrabber[i].grabFrame();
        if (vidGrabber[i].isFrameNew()){
            camPic[i].loadData(vidGrabber[i].getPixels(),camWidth,camHeight,GL_RGB);
        }
    }

    //GENERATE WHITE NOISE
    for(int i = 0;i < camWidth*camHeight; i++) {
        videoNoise[i] = ofRandom(0,255);
    }
    noise_preview.loadData(videoNoise,picWidth,picHeight/2, GL_LUMINANCE);

    //IO MAPPING
    for(int i = 0; i < OUTPUT_NUM; i++) {
        //if mapping zero then no picture should be drawn
        if(io_mapping[i] == 0) {
            //MAKE IT BLACK
            output[i].loadData(videoNull, camWidth,camHeight, GL_RGB);
            //output[i].allocate(camWidth,camHeight, GL_RGB);
        } else if(io_mapping[i] == 7) {
            //WHITE NOISE
            output[i].loadData(videoNoise, camWidth,camHeight, GL_LUMINANCE);
        } else {
            //SET THE OUTPUT TO A INPUT PICTURE
            if (vidGrabber[io_mapping[i]-1].isFrameNew()){
                output[i].loadData(vidGrabber[io_mapping[i]-1].getPixels(),camWidth,camHeight,GL_RGB);
            }
        }
    }

    if(notYetHopped) {
        //if the hoppingpic jumps on the first (virtual) output 0, then the last output (its ancestor) should get is old Campic back
        if(hoppingOutput == 0) {
            io_mapping[OUTPUT_NUM-1] = oldCam;
        } else {
            //the hoppingOutputs ancestor gets his old Campic back
            io_mapping[hoppingOutput-2] = oldCam;
        }
        //save the last active Cam of an Output
        oldCam = io_mapping[hoppingOutput-1];
        io_mapping[hoppingOutput-1] = activeCam;
        printf("hopping is now on %d\n",hoppingOutput);
        notYetHopped = false;
    }

    //STATUS

    switch(status) {
        case 0:
            status_msg = "Select Input!";
            break;
        case 1:
            status_msg = "Select Output!";
            break;
        case 2:
            status_msg = "Press Space to switch!";
            break;
        case 3:
            status_msg = "Please select Input first!";
            break;
        case 4:
            status_msg = "Solo Effect selected! Press Space to switch!";
            break;
        case 5:
            status_msg = "Delayed Solo Effect selected! Press Space to switch!";
            break;
        case 6:
            status_msg = "Delayed Solo Effect is working...";
            break;
    }
}

//--------------------------------------------------------------
void testApp::draw(){
    if(!help) {

    //DRAW INPUTS

    picx = 0;
    picy = 0;
	for(int i = 0; i < INPUT_NUM; i++) {
	    //draw cam pic
        camPic[i].draw(picx*picWidth,picy*picHeight,picWidth,picHeight);
        //draw activeCam Indicator
        if(i == activeCam-1) {
            ofSetColor(0x00ff00);
        } else {
            ofSetColor(0xff0000);
        }
        ofRect(picx*picWidth,picy*picHeight,12,12);
        //draw cam number
        ofSetColor(0x000000);
		ofRect(picx*picWidth+12,picy*picHeight,43,12);
        ofSetColor(0xffffff);
        ofDrawBitmapString("Cam "+ ofToString(i+1),picx*picWidth+14,picy*picHeight+10);
        //printf("drawing cam %d @ %d, %d\n",i,picx*picWidth,picy*picHeight);
        picx = picx + 1;
        //just for manage the drawing of the cam pics
        if(i >= 2) {
            picy = 1;
        }
        if(i == 2) {
            picx = 0;
        }
	}

    //DRAW EFFECT INPUTS
    ofNoFill();
    ofRect(2*picWidth,picHeight,picWidth,picHeight/2);
    ofFill();
    noise_preview.draw(2*picWidth,picHeight*1.5,picWidth,picHeight/2);
    if(activeCam == 0) {
            ofSetColor(0x00ff00);
    } else {
            ofSetColor(0xff0000);
    }
    ofRect(2*picWidth,picHeight,12,12);


    if(activeCam == 7) {
            ofSetColor(0x00ff00);
    } else {
            ofSetColor(0xff0000);
    }
    ofRect(2*picWidth,picHeight*1.5,12,12);
    ofSetColor(0x000000);
    ofRect(2*picWidth+12,picHeight,43,12);
    ofRect(2*picWidth+12,picHeight*1.5,43,12);
    ofSetColor(0xffffff);
    ofDrawBitmapString("Cam 6",2*picWidth+14,picHeight+10);
    ofDrawBitmapString("Cam 7",2*picWidth+14,picHeight*1.5+10);

    } else {
        ofDrawBitmapString("HELP\n\nF1-F5: Camera Inputs\nF6:    Black Screen Input\nF7:    White Noise Input\n\n1-3:   Monitor Outputs\n4:     Solo Effect Monitor Output\n5:     Delayed Solo Effect Monitor Output\n6:     Hopping Effect (use ENTER to switch)\n\nSPACE: Switch\nENTER: Let the selected Cam hop\n\nh:     this help screen\nt:     show a testscreen\n\nRed / Green Boxes: Hopping Effect Indicator",20,20);
    }

    //DRAW FPS AND STATUS

    ofDrawBitmapString("FPS: " + ofToString(ofGetFrameRate()), 10, 2*picHeight+12);
    ofDrawBitmapString(" | Press h for help | Status: ", 145, 2*picHeight+12);

    if(status == 6) {
        if(delayed_output < OUTPUT_NUM) {
            if(ofGetElapsedTimeMillis() - previousMillis > interval) {
                previousMillis = ofGetElapsedTimeMillis();

                io_mapping[delayed_output] = switch_input;
                printf("switching input %d to output %d\n",switch_input,delayed_output+1);
                delayed_output++;
            }
        } else {
            printf("all outputs switched to %d\n",switch_input);
            delayed_output = 0;
            status = 0;
        }
    }

    ofSetColor(0xff0000);
    ofDrawBitmapString(status_msg,380,2*picHeight+12);
    ofSetColor(0xffffff);

    //DRAW OUTPUT

    for(int i = 0; i < OUTPUT_NUM; i++) {
        if(!testscreen) {
            //draw the output preview
            output[i].draw(i*picWidth,ofGetHeight()-picHeight,picWidth,picHeight);
            //draw the output
            output[i].draw((i+1)*outputWidth,0,outputWidth,outputHeight);
        } else {
            ofSetColor(0xff0000);
            //preview
            ofRect(i*picWidth,ofGetHeight()-picHeight,picWidth/3,picHeight);
            //big
            ofRect((i+1)*outputWidth,0,outputWidth/3,outputHeight);
            ofSetColor(0x00ff00);
            //preview
            ofRect(i*picWidth+picWidth/3,ofGetHeight()-picHeight,picWidth/3,picHeight);
            //big
            ofRect((i+1)*outputWidth+outputWidth/3,0,outputWidth/3,outputHeight);
            ofSetColor(0x0000ff);
            //preview
            ofRect(i*picWidth+2*(picWidth/3),ofGetHeight()-picHeight,picWidth/3,picHeight);
            //big
            ofRect((i+1)*outputWidth+2*(outputWidth/3),0,outputWidth/3,outputHeight);
        }
        ofSetColor(0x000000);
        //draw output number
        ofRect(i*picWidth+12,ofGetHeight()-picHeight,68,12);
        ofSetColor(0xffffff);
        ofDrawBitmapString("Output "+ ofToString(i+1),i*picWidth+14,ofGetHeight()-picHeight+10);
    }

    if(grabit) {
        ofSaveScreen(ofToString(ofGetYear()) + ofToString(ofGetMonth()) + ofToString(ofGetDay()) + "_" +ofToString(ofGetHours()) +ofToString(ofGetMinutes()) + ofToString(ofGetSeconds()) + ".png" );
        printf("screen saved!\n");
        grabit = false;
    }
}


//--------------------------------------------------------------
void testApp::keyPressed  (int key){
    if (status != 6) {
    ////////////////// INPUTS /////////////////////////
    if (key == OF_KEY_F1){
        //activeCam = 0;
        status = 1;
        switch_input = 1;
	}

    if (key == OF_KEY_F2){
        //activeCam = 0;
        status = 1;
        switch_input = 2;
	}

    if (key == OF_KEY_F3){
        //activeCam = 0;
        status = 1;
        switch_input = 3;
	}

    if (key == OF_KEY_F4){
        //activeCam = 0;
        status = 1;
        switch_input = 4;
	}

    if (key == OF_KEY_F5){
        //activeCam = 0;
        status = 1;
        switch_input = 5;
	}

    if (key == OF_KEY_F6){
        //activeCam = 0;
        status = 1;
        switch_input = 0;
	}

    if (key == OF_KEY_F7){
        //activeCam = 0;
        status = 1;
        switch_input = 7;
	}

	////////////////// OUTPUTS /////////////////////////
	if (key == '1'){
	    if(status == 1) {
            status = 2;
            switch_output = 1;
	    }
	    else {
	        status = 3;
	    }
	}

    if (key == '2'){
	    if(status == 1) {
            status = 2;
            switch_output = 2;
	    }
	    else {
	        status = 3;
	    }
	}

	if (key == '3'){
	    if(status == 1) {
            status = 2;
            switch_output = 3;
	    }
	    else {
	        status = 3;
	    }
	}
    ////////////////// EFFECTS /////////////////////////
    if (key == '4'){
	    if(status == 1) {
            status = 4;
	    }
	    else {
	        status = 3;
	    }
	}
    if (key == '5'){
	    if(status == 1) {
            status = 5;
	    }
	    else {
	        status = 3;
	    }
	}
    if (key == '6'){
	    if(status == 1) {
            activeCam = switch_input;
            hoppingOutput = 0;
            printf("%d gets Output Hopping Effect\n",activeCam);
            status = 0;
	    }
	    else {
	        status = 3;
	    }
	}
	////////////////// SWITCHING /////////////////////////
	if (key == ' '){
	    if(status == 2) {
            io_mapping[switch_output-1] = switch_input;
            printf("io_mapping[%d] = %d\n",switch_output-1,switch_input);
            status = 0;
	    } else if(status == 4) {
	        for(int i = 0; i < OUTPUT_NUM; i++) {
	            io_mapping[i] = switch_input;
	        }
	        printf("all outputs switched to %d\n",switch_input);
	        status = 0;
	    } else if(status == 5) {
            status = 6;
	    }
	}
	if (key == OF_KEY_RETURN) {
	    if(hoppingOutput < OUTPUT_NUM) {
            hoppingOutput++;
	    } else {
	        hoppingOutput = 0;
	    }
	    notYetHopped = true;
	}
	////////////////// HELP /////////////////////////
	if (key == 'h'){
        if(help) {
            help = false;
        } else {
            help = true;
        }
	}
	////////////////// TESTSCREEN /////////////////////////
	if (key == 't'){
        if(testscreen) {
            testscreen = false;
        } else {
            testscreen = true;
        }
	}


    }

    if (key == OF_KEY_F12){
        grabit = true;
	}

}


//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

void testApp::exit() {
    for(int i = 0; i < OUTPUT_NUM; i++) {
        output[i].clear();
    }
}
