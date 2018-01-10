#include "ofApp.h"

#pragma mark -
#pragma mark MAIN

void ofApp::setup() {
    
	initCam = false;

    //ofLogToFile("log.txt", true);
    ofLogToConsole();
    
    ofTrueTypeFont::setGlobalDpi(72);
    
    verdana14.load("verdana.ttf", 14, true, true);
    verdana14.setLineHeight(18.0f);
    verdana14.setLetterSpacing(1.037);
    
    currentScenario = -1;
    
	ofLog() << "Version " << VERSION;
    localIP = getLocalIP();
    ofLog() << "Local IP Adress " << localIP;
    ofSetEscapeQuitsApp(false);
    
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    
#ifdef TARGET_OPENGLES
    fbo.allocate(WIDTH, HEIGHT, GL_RGBA ); // with alpha, 32 bits red, 32 bits green, 32 bits blue, 32 bits alpha, from 0 to 1 in 'infinite' steps
    ofLog() << "GL_RGBA32F_ARB is not available for OPENGLES.  Using RGBA.";
#else
    fbo.allocate(WIDTH, HEIGHT, GL_RGBA32F_ARB); // with alpha, 32 bits red, 32 bits green, 32 bits blue, 32 bits alpha, from 0 to 1 in 'infinite' steps
#endif
    
    fbo.begin();
    ofClear(0,0,0, 0);
    fbo.end();
    
	cam.setOrientation(ofPoint(-20, 0, 0));


	leap.setReceiveBackgroundFrames(true);
	leap.open();

    initGui();
    initSound();
    
#if USE_ZEROCONF
    initZeroconf();
#else
    resolved = true;
    stringstream ss;
    ss << ip1 << "." << ip2 << "." << ip3 << "." << ip4;
    hostname = ss.str();
    if(user)
    {
        //hostname = "192.168.0.19";
        inPort = 14000;
        outPort = 14001;
    } else {
        //hostname = "192.168.0.20";
        inPort = 14001;
        outPort = 14000;
    }
    receiver.setup(inPort);
    socketOk = false;
    connectSocket();
#endif
    sync.setup((ofParameterGroup&)advancedPanel, inPort+10, hostname, outPort+10);
    
    loadScenari();
    nextScenario();
    
    textFade = 255;
    
    fullscreen = true;
    ofSetFullscreen(true);
    gui.setPosition(ofGetScreenWidth()-210,10);
}

void ofApp::update() {
#if USE_ZEROCONF
    resolveZeroconf();
#endif
    //ofLog() << "UPDATE";
    if(isPlaying)
    {
        playUpdate();
    } else {
        leapUpdate();
        receiveOSC();
    }
    
    ofSoundUpdate();
    getVolume();
    
    
    sync.update();
    
    ofEnableAlphaBlending();
    fbo.begin();
    fboDraw();
    fbo.end();

    if(isRecording)
        record();
    
    if(ofGetFrameNum() == 60){
        receiver.setup(inPort);
    }
}

void ofApp::draw() {

	if (initCam == false)
	{
		initCpt++;
		if (initCpt == 10)
			initCam = true;
	}
		
    if(graphMode)
    {
        graphDraw();
        return;
    }
    
    ofBackground(0,0,0);
    
    fbo.draw(0,0);
    
    if(saveImage)
    {
        ofSaveScreen("capture/"+timestampString()+".jpg");
        saveImage = false;
    }
    
    if(textFade > 0)
    {
        if(isRecording)
            ofSetColor(255,0,0,textFade);
        else if(isPlaying)
            ofSetColor(0,255,0,textFade);
        else
            ofSetColor(255,255,255,textFade);
        
        string name;
        if(currentScenario != -1)
        {
            name = to_string(currentScenario) + " - " + scenari[currentScenario].getName();
            replace( name.begin(), name.end(), '_', ' ');
        } else {
            name = "";
        }
        verdana14.drawString(name, 10, 20);
        textFade--;
    }
    
    if(doListScenari) {
        ofSetColor(255,255,255,127);
        for(int i = 0 ; i < scenari.size() ; i++) {
            string name = to_string(i) + " - " + scenari[i].getName();
            replace( name.begin(), name.end(), '_', ' ');
            if(currentScenario == i)
                ofSetColor(255,255,255,200);
            else
                ofSetColor(127,127,255,200);
            verdana14.drawString(name, 10, 80 + 20 * i);
        }
    }
    
    if(!bHide)
    {
        ofSetColor(127,127,255,255);
        verdana14.drawString("(local)", 10, 40);
        stringstream in;
        in << (user ? "B  " : "A  ") << localIP;// " " << inPort;
        verdana14.drawString(in.str(), 110, 40);
        if(resolved)
        {
            verdana14.drawString("(remote)", 10, 60);
            stringstream out;
            out << (user ? "A  " : "B  ") << hostname;// << " " << outPort;
            verdana14.drawString(out.str(), 110, 60);
        }
        
    }
    
    if(isPlaying)
    {
        ofSetColor(0,200,0,127);
        ofFill();
        ofDrawRectangle(0, ofGetHeight()-4, (curTime/(float)maxTime)*ofGetWidth(), 4);
        ofSetColor(0,255,0,255);
        stringstream in;
        in << shortName << " - " << ofToString(curTime/1000.,2) << " / " << ofToString(maxTime/1000.,2) << "s";
        verdana14.drawString(in.str(), 10, ofGetHeight() - 14);
    }
    
    ofSetColor(255,255,255,255);
    
    /*
    if(doSound)
    {
        int db = atodb(smoothedVol);
        int trans = ofMap(db, -80, -40, 0, 255);
        int size = ofMap(db, -80, -40, 0, 8);
        ofSetColor(127,127,127, trans);
        ofDrawCircle(15, 15, size);
        ofSetColor(0);
        ofDrawCircle(15, 15, size - 2);
        ofSetColor(255,255,255);
    }

    if(isRecording)
    {
        frameCounter++;
        frameCounter %= 64;
        ofSetColor(255,0,0, 255-(frameCounter*4));
        ofDrawCircle(15, 15, 5);
        ofSetColor(255,255,255);
    }//*/
    
    if( !bHide )
        gui.draw();
}

void ofApp::graphDraw() {
    ofBackground(240,240,240);
    ofSetColor(220,220,220,255);
    ofDrawRectangle(0, 0, ofGetWidth(), 32);
    if(shortName != "")
    {
        ofSetColor(0,0,0,255);
        stringstream in;
        in << shortName << " - " << ofToString(maxTime/1000.,2) << "s - Scenario " << currentScenario << " - \"" << scenari[currentScenario].getName() << "\"";
        string s = in.str();
        replace( s.begin(), s.end(), '_', ' ');
                
        verdana14.drawString(s, 10, 20);
    }
   
    int padx = 10;
    int pady = 10;
    int xx = padx;
    int yy = 32 + pady;
    int ww = ofGetWidth() - 2 * padx;
    int hh = (ofGetHeight()-2*yy-pady)/2;
    ofColor c = ofColor::darkBlue;
    gAL.draw(xx, yy, ww, hh, graphStart, graphEnd, graphType, c, verdana14, true);
    c = ofColor::darkGreen;
    gAR.draw(xx, yy, ww, hh, graphStart, graphEnd, graphType, c, verdana14, false);
    c = ofColor::darkBlue;
    gBL.draw(xx, yy + hh + pady, ww, hh, graphStart, graphEnd, graphType, c, verdana14, true);
    c = ofColor::darkGreen;
    gBR.draw(xx, yy + hh + pady, ww, hh, graphStart, graphEnd, graphType, c, verdana14, false);
    
    ofSetColor(220,220,220,255);
    ofDrawRectangle(0, ofGetHeight() - 32, ofGetWidth(), 32);
    
    int xxx = padx;
    int yyy = ofGetHeight() - 32 + pady;
    int www = ofGetWidth() - 2 * padx;
    int hhh = 32 - 2 * pady;
    
    ofSetColor(200,200,200,255);
    ofFill();
    ofDrawRectangle(xxx, yyy, www, hhh);
    ofSetColor(100,100,100,255);
    ofFill();
    
    ofDrawRectangle(ofMap(graphStart,0,1,xxx,xxx+www), yyy, ofMap(graphEnd-graphStart,0,1,0,www), hhh);
    
    if(saveImage)
    {
        ofSaveScreen("capture/"+timestampString()+".jpg");
        saveImage = false;
    }
}

void ofApp::fboDraw() {
    ofFill();
    if(trail == 0)
        ofSetColor(0,0,0, 255);
    else
        ofSetColor(0,0,0, (1.-(trail/5.+0.8))*255.);
    
    ofDrawRectangle(0,0,ofGetWidth(),ofGetHeight());
    
    cam.begin();
    if(camMove)
    {
        float smo = smoothCam / 10. + 0.9;
        ofPoint target;
        target.x = (1.0-smo)*camTarget.x + smo*prevCamTarget.x;
        target.y = (1.0-smo)*camTarget.y + smo*prevCamTarget.y;
        target.z = (1.0-smo)*camTarget.z + smo*prevCamTarget.z;
        cam.setTarget(target);
        prevCamTarget = target;
    } else {
        //ofLog() << "NO HANDS";
		//cam.reset();
        cam.setTarget(ofVec3f(0.,0.,0.));
		//cam.setOrientation(ofPoint(-20, 0, 0));
	}
    if(grid)
    {
        ofPushMatrix();
        ofRotate(90, 0, 0, 1);
        ofSetColor(255, 255, 255, 127);
        ofDrawGridPlane(800, 20, false);
        ofPopMatrix();
    }
    
    
    if(origin)
    {
        fingerType fingerTypes[] = {THUMB, INDEX, MIDDLE, RING, PINKY};
        
        for(int i = 0; i < simpleHands.size(); i++){
            bool isLeft        = simpleHands[i].isLeft;
            ofPoint handPos    = simpleHands[i].handPos;
            ofPoint handNormal = simpleHands[i].handNormal;
            
            if(finger)
            {
                //for (int f=0; f<5; f++) {
                int f = 1;
                ofPoint mcp = simpleHands[i].fingers[ fingerTypes[f] ].mcp;  // metacarpal
                ofPoint pip = simpleHands[i].fingers[ fingerTypes[f] ].pip;  // proximal
                ofPoint dip = simpleHands[i].fingers[ fingerTypes[f] ].dip;  // distal
                ofPoint tip = simpleHands[i].fingers[ fingerTypes[f] ].tip;  // fingertip
                
                ofSetColor(127, 127, 127, 255);
                //ofDrawSphere(mcp.x, mcp.y, mcp.z, 12);
                //ofDrawSphere(pip.x, pip.y, pip.z, 12);
                //ofDrawSphere(dip.x, dip.y, dip.z, 12);
                ofDrawSphere(tip.x, tip.y, tip.z, 2);
                
                ofSetColor(127, 127, 127, 255);
                ofSetLineWidth(2);
                ofDrawLine(mcp.x, mcp.y, mcp.z, pip.x, pip.y, pip.z);
                ofDrawLine(pip.x, pip.y, pip.z, dip.x, dip.y, dip.z);
                ofDrawLine(dip.x, dip.y, dip.z, tip.x, tip.y, tip.z);
                //}
            } else {
                ofSetColor(127, 127, 127, 255);
                ofDrawSphere(handPos.x, handPos.y, handPos.z, 5);
                //ofSetColor(255, 255, 0);
                //ofDrawArrow(handPos, handPos + 100*handNormal);
            }
        }
    }
    
    if(twoPlayers && doRemote)
    {
        drawChain(remLeftChain);
        drawChain(remRightChain);
    }
    if(doSelf || !twoPlayers)
    {
        drawChain(leftChain);
        drawChain(rightChain);
    }

    cam.end();
}

void ofApp::clearChains()
{
    leftChain.clear();
    rightChain.clear();
    remLeftChain.clear();
    remRightChain.clear();
}

void ofApp::drawChain(chain c)
{
    switch(drawMode)
    {
        case 0: // LINES
            ofEnableSmoothing();
            ofSetLineWidth(2);
            for(int i = 0 ; i < c.size()-1 ; i++)
            {
                point p = c.getPoint(i);
                point n = c.getPoint(i+1);
                drawLine(p,n);
            }
            break;
        case 1: // SPHERES
            for(int i = 0 ; i < c.size() ; i++)
            {
                point p = c.getPoint(i);
                ofSetColor(p.color, p.color.a*p.life*255);
                if(snd != 0)
                    ofDrawSphere(p.pos.x, p.pos.y, p.pos.z, p.life*size*p.snd);
                else
                    ofDrawSphere(p.pos.x, p.pos.y, p.pos.z, p.life*size);
                
            }
            break;
        case 2: // DISKS
            ofSetLineWidth(2);
            
            for(int i = 0 ; i < c.size() ; i++)
            {
                point p = c.getPoint(i);
                ofSetColor(p.color, p.color.a*p.life*255);
                if(snd != 0)
                    disk(p, p.life*size*p.snd, 72);
                else
                    disk(p, p.life*size, 72);
            }
            break;
    }
}

void ofApp::exit() {
#if USE_ZEROCONF
    stopZeroconf();
#endif
    leap.close();
    gui.saveToFile("settings.xml");
    ofLog() << "Exit";
}

#pragma mark -
#pragma mark INTERACT

void ofApp::keyPressed(int key) {
    if (key == 'G') {
        graphMode = !graphMode;
        if(graphMode)
        {
            //if(isRecording)
                stopRec();
            //if(isPlaying)
                stopPlay();
            initGraph();
        }
    } else if (key == 'f') {
        fullscreen = !fullscreen;
        ofSetFullscreen(fullscreen);
        if(fullscreen)
            gui.setPosition(ofGetScreenWidth() - 210, 10);
        else
            gui.setPosition(WIDTH - 210, 10);
    } else if (key == 's') {
        saveImage = true;
    } else if (key == 'o') {
        openFile();
    } else if(graphMode) {
        if (key == 'p') {
            graphFinger = !graphFinger;
            setFinger(graphFinger);
        } else if (key == ' ' || key == 359 || key == 358) {
            graphType++;
            if(graphType > MAX_GRAPH)
                graphType = 0;
        } else if (key == 357 || key == 356) {
            graphType--;
            if(graphType < 0)
                graphType = MAX_GRAPH;
        }
    } else {
        if( key == 'h' ){
            bHide = !bHide;
            localIP = getLocalIP();
        } else if (key == 'r') {
            if(isRecording)
                stopRec();
            else
                startRec();
        } else if (key == 'p') {
            if(isPlaying)
                stopPlay();
            else
                startPlay();
        } else if (key == 'i') {
            textFade = 255;
        } else if (key == 'l') {
            listScenari();
        } else if (key == 'g') {
            grid = !grid;
        } else if (key == 13 || key == 27) {
            bHide = true;
            doListScenari = false;
        } else if(!isPlaying) {
            if (key == ' ' || key == 359 || key == 358) {
                nextScenario();
            } else if (key == 357 || key == 356) {
                prevScenario();
            } else if (key == 'w') {
                saveScenario();
            } else if (key == 'W') {
                saveScenari();
            } else if (key == 'L') {
                loadScenari();
            } else if (key == 'u') {
                updateScenario();
            } else if (key == 'U') {
                renameScenario();
            } else if (key == 'D') {
                deleteScenario();
            } else if (key == 'c') {
                clearChains();
            } else if (key == '0') {
                currentScenario = -1;
                nextScenario();
            } else if (key == '1') {
                currentScenario = 4;
                nextScenario();
            } else if (key == '2') {
                currentScenario = 17;
                nextScenario();
            } else {
                //ofLog() << "KEY " << (int)key;
            }
        }
    }
}

void ofApp::mousePressed(int x, int y, int button) {
    if(graphMode) {
        
        int padx = 10;
        int pady = 10;
        int xxx = padx;
        int yyy = ofGetHeight() - 32 + pady;
        int www = ofGetWidth() - 2 * padx;
        int hhh = 32 - 2 * pady;

        if(y > yyy && y < yyy+hhh)
        {
            if(ofGetElapsedTimeMillis() - clicTime < 200) {
                graphStart = 0;
                graphEnd = 1;
            } else {
                int startPos = xxx + graphStart*www;
                int endPos = xxx + graphEnd*www;
                if(abs(x-startPos) < 10)
                {
                    dragGraphStart = true;
                } else if(abs(x-endPos) < 10) {
                    dragGraphEnd = true;
                } else if(x > startPos && x < endPos) {
                    dragGraph = true;
                }
            }
            clicTime = ofGetElapsedTimeMillis();
            clicX = x;
        }

    }
}

void ofApp::mouseDragged(int x, int y, int button) {
    if(graphMode) {
        
        int padx = 10;
        int pady = 10;
        int xxx = padx;
        int yyy = ofGetHeight() - 32 + pady;
        float www = ofGetWidth() - 2 * padx;
        int hhh = 32 - 2 * pady;
        
        if(dragGraphStart)
        {
            graphStart = ofClamp((x - xxx)/www, 0, 1);
            if(graphStart > graphEnd)
            {
                dragGraphStart = false;
                dragGraphEnd = true;
                float tmp = graphEnd;
                graphEnd = graphStart;
                graphStart = graphEnd;
            }
        }
        if(dragGraphEnd)
        {
            graphEnd = ofClamp((x - xxx)/www, 0, 1);
            if(graphEnd < graphStart)
            {
                dragGraphStart = true;
                dragGraphEnd = false;
                float tmp = graphEnd;
                graphEnd = graphStart;
                graphStart = graphEnd;
            }
        }
        if(dragGraph)
        {
            int dX = x - clicX;
            int startPos = xxx + graphStart*www;
            int endPos = xxx + graphEnd*www;

            graphEnd = ofClamp((endPos + dX - xxx)/www,0.,1.);
            graphStart = ofClamp((startPos + dX - xxx)/www,0.,1.);
            
            clicX = x;
        }
    }
}

void ofApp::mouseReleased(int x, int y, int button) {
    if(graphMode) {
        dragGraph = false;
        dragGraphStart = false;
        dragGraphEnd = false;
    } else {
        switch (button) {
            case OF_MOUSE_BUTTON_1:
                nextScenario();
                break;
            case OF_MOUSE_BUTTON_3:
                if(isRecording)
                    stopRec();
                else
                    startRec();
                break;
            default:
                break;
        }
    }
}


void ofApp::windowResized(int w, int h) {
#ifdef TARGET_OPENGLES
    fbo.allocate(w, h, GL_RGBA ); // with alpha, 32 bits red, 32 bits green, 32 bits blue, 32 bits alpha, from 0 to 1 in 'infinite' steps
    ofLog() << "GL_RGBA32F_ARB is not available for OPENGLES.  Using RGBA.";
#else
    fbo.allocate(w, h, GL_RGBA32F_ARB); // with alpha, 32 bits red, 32 bits green, 32 bits blue, 32 bits alpha, from 0 to 1 in 'infinite' steps
#endif
    fbo.begin();
    ofClear(0,0,0, 0);
    fbo.end();
}

#pragma mark -
#pragma mark SCENARIO

void ofApp::loadScenari() {
    if(scenariXml.load("scenari.xml"))
    {
        scenari.clear();
        currentScenario = -1;
        scenariXml.setTo("scenari");
        int children = scenariXml.getNumChildren();
        for(int i = 0 ; i < children ; i++)
        {
            scenariXml.setToChild(i);
            string name = scenariXml.getAttribute("name");
            string params = scenariXml.getAttribute("params");
            scenario s(name, params);
            scenari.push_back(s);
            scenariXml.reset();
        }
        ofLog() << "Scenari loaded (" << scenari.size() << ")";
    }
}

void ofApp::saveScenari() {
    scenariXml.clear();
    scenariXml.addChild("scenari");
    scenariXml.setTo("scenari");
    for(int i = 0 ; i < scenari.size() ; i++)
    {
        scenario s = scenari[i];
        scenariXml.addChild("scenario");
        scenariXml.setToChild(i);
        scenariXml.setAttribute("name", s.getName());
        scenariXml.setAttribute("params", s.paramsToString());
        scenariXml.reset();
    }
    scenariXml.save("scenari.xml");
    ofLog() << "Scenari saved (" << scenari.size() << ")";
}

void ofApp::nextScenario() {
	//ofLog() << "nextScenario";
	if (isPlaying)
		return;
    if(scenari.size() == 0)
        return;
    currentScenario++;
    if(currentScenario >= scenari.size())
        currentScenario = 0;
    loadScenario(currentScenario);
}

void ofApp::prevScenario() {
    if(scenari.size() == 0)
        return;
    currentScenario--;
    if(currentScenario < 0)
        currentScenario = scenari.size() - 1;
    loadScenario(currentScenario);
}

void ofApp::loadScenario(int i) {
    if(isRecording)
    {
        stopRec();
        startRec();
    }
    if(scenari.size() == 0)
        return;
    if(i < scenari.size() && i >= 0)
    {
        scenario s = scenari[i];
        ofLog() << "Loading scenario " << i << " \"" << s.getName() << "\"";
        
        twoPlayers = s.getParam(0);
        
        drawMode = s.getParam(1);
        
        dev = s.getParam(2);
        dec = s.getParam(3);
        trail = s.getParam(4);
        doSound = s.getParam(5);
        rndPos = s.getParam(6);
        size = s.getParam(7);
        
        colorA = ofFloatColor(s.getParam(8),s.getParam(9),s.getParam(10),s.getParam(11));
        colorB = ofFloatColor(s.getParam(12),s.getParam(13),s.getParam(14),s.getParam(15));
        
        rndColor = s.getParam(16);
        colorMode = s.getParam(17);
        
        snd = s.getParam(18);
        soundA = s.getParam(19);
        soundB = s.getParam(20);
        doSelf = s.getParam(21);
        doRemote = s.getParam(22);
        
        camMove = s.getParam(23);
        
        clearChains();
        updateSounds();
        sync.update();
        sendScenario();
        
        textFade = 255;
        
    }
}

void ofApp::updateScenario() {
    if(scenari.size() == 0)
        return;
    if(currentScenario < scenari.size() && currentScenario >= 0)
    {
        vector<float> params;
        
        params.push_back(twoPlayers);
        
        params.push_back(drawMode);
        
        params.push_back(dev);
        params.push_back(dec);
        params.push_back(trail);
        params.push_back(doSound);
        params.push_back(rndPos);
        params.push_back(size);
        
        params.push_back(colorA->r);
        params.push_back(colorA->g);
        params.push_back(colorA->b);
        params.push_back(colorA->a);
        params.push_back(colorB->r);
        params.push_back(colorB->g);
        params.push_back(colorB->b);
        params.push_back(colorB->a);
        
        params.push_back(rndColor);
        params.push_back(colorMode);
        
        params.push_back(snd);
        params.push_back(soundA);
        params.push_back(soundB);
        params.push_back(doSelf);
        params.push_back(doRemote);
        
        params.push_back(camMove);
        params.push_back(-1);
        
        scenari[currentScenario].setParams(params);
        
        ofLog() << "Updating scenario " << currentScenario << " \"" << scenari[currentScenario].getName() << "\"";
        
        saveScenari();
    }
}

void ofApp::deleteScenario() {
    if(scenari.size() == 0)
        return;
    if(currentScenario < scenari.size() && currentScenario >= 0)
    {
        scenari.erase(scenari.begin() + currentScenario);
        currentScenario = -1;
        saveScenari();
    }
}

void ofApp::renameScenario() {
    if(scenari.size() == 0)
        return;
    if(currentScenario < scenari.size() && currentScenario >= 0)
    {
        string name = inputDialog("Rename Scenario", "Enter a new name for this scenario", scenari[currentScenario].getName());
        if(name == "")
            return;
        replace( name.begin(), name.end(), ' ', '_');
        
        scenari[currentScenario].setName(name);
        ofLog() << "Renaming scenario " << currentScenario << " \"" << scenari[currentScenario].getName() << "\"";
        saveScenari();
    }
}

void ofApp::saveScenario() {
    string name = inputDialog("New Scenario", "Enter a name for this scenario", "name");
    if(name == "")
        return;
    
    replace( name.begin(), name.end(), ' ', '_');
    vector<float> params;
    
    params.push_back(twoPlayers);
    
    params.push_back(drawMode);
    
    params.push_back(dev);
    params.push_back(dec);
    params.push_back(trail);
    params.push_back(doSound);
    params.push_back(rndPos);
    params.push_back(size);
    
    params.push_back(colorA->r);
    params.push_back(colorA->g);
    params.push_back(colorA->b);
    params.push_back(colorA->a);
    params.push_back(colorB->r);
    params.push_back(colorB->g);
    params.push_back(colorB->b);
    params.push_back(colorB->a);

    params.push_back(rndColor);
    params.push_back(colorMode);
    
    params.push_back(snd);
    params.push_back(soundA);
    params.push_back(soundB);
    params.push_back(doSelf);
    params.push_back(doRemote);
    
    params.push_back(camMove);
    params.push_back(-1);
    
    scenario s(name, params);
    ofLog() << "New scenario " << s.toString();
    scenari.push_back(s);
    saveScenari();
    
    currentScenario = scenari.size()-1;
}

void ofApp::listScenari() {
    if(isPlaying)
        stopPlay();
    if(isRecording)
        stopRec();
    doListScenari = !doListScenari;
}

#pragma mark -
#pragma mark GUI

void ofApp::initGui() {
    // GUI
    //ofLog() << "gui setup";
    gui.setup();
    gui.setName("Settings");
    
#if !USE_ZEROCONF
    generalPanel.setName("General Settings");
    generalPanel.add(user.set("User A/B", false));
    user.addListener(this,&ofApp::userChanged);
    generalPanel.add(ip1.set("ip1", 192, 0, 255));
    ip1.addListener(this,&ofApp::ipChanged);
    generalPanel.add(ip2.set("ip2", 168, 0, 255));
    ip2.addListener(this,&ofApp::ipChanged);
    generalPanel.add(ip3.set("ip3", 0, 0, 255));
    ip3.addListener(this,&ofApp::ipChanged);
    generalPanel.add(ip4.set("ip4", 19, 0, 255));
    ip4.addListener(this,&ofApp::ipChanged);
    generalPanel.add(volume.set("volume", 1.,0.,5.));
    //ofLog() << "generalPanel";
    
#endif
    advancedPanel.setName("Advanced Settings");
    advancedPanel.add(twoPlayers.set("Two players", false));
    advancedPanel.add(doSelf.set("Self", true));
    advancedPanel.add(doRemote.set("Remote", true));
    twoPlayers.addListener(this, &ofApp::twoPlayersChanged);
    advancedPanel.add(finger.set("Palm/Finger", true));
    advancedPanel.add(grid.set("Grid", false));
    advancedPanel.add(origin.set("Origin", false));
    advancedPanel.add(camMove.set("Move Camera", false));
    advancedPanel.add(drawMode.set("Draw Mode", 0, 0, 2));
    advancedPanel.add(dev.set("Deviation", 0., 0., 1. ));
    advancedPanel.add(dec.set("Decay", 0.9, 0., 1. ));
    advancedPanel.add(trail.set("Trail Length", 0., 0., 1.));
    advancedPanel.add(rndPos.set("Random Position", 0., 0., 1.));
    advancedPanel.add(size.set("Size", 2, 1, 30));
    advancedPanel.add(colorA.set("Color A",ofColor(127,255,255),ofColor(0,0),ofColor(255,255)));
    advancedPanel.add(colorB.set("Color B",ofColor(255,255,127),ofColor(0,0),ofColor(255,255)));
    advancedPanel.add(rndColor.set("Random Color", 0., 0., 1.));
    advancedPanel.add(colorMode.set("Position to Color", 0, 0, 1));
    advancedPanel.add(doSound.set("Sound", true));
    advancedPanel.add(soundA.set("Sound A", 1, 1, NUM_SOUNDS));
    soundA.addListener(this, &ofApp::soundChanged);
    advancedPanel.add(soundB.set("Sound B", 2, 1, NUM_SOUNDS));
    soundB.addListener(this, &ofApp::soundChanged);
    advancedPanel.add(snd.set("Sound Influence", 0., 0., 1.));
    //ofLog() << "advancedPanel";
   
    expertPanel.setName("Expert Settings");
    expertPanel.add(smooth.set("Smooth Leap Data", 0.82,0.,1.));
    expertPanel.add(smoothCam.set("Smooth Camera", 0.96, 0., 1.));
    //ofLog() << "expertPanel";
    
    gui.add(generalPanel);
    gui.add(advancedPanel);
    gui.add(expertPanel);
    
    gui.getGroup("Advanced Settings").minimize();
    gui.getGroup("Expert Settings").minimize();
    
    gui.setFillColor(ofColor::gray);
    gui.setPosition(ofGetWidth() - 210, 10);
    //ofLog() << "load file >>>";
    gui.loadFromFile("settings.xml");
    
    bHide = true;
    //ofLog() << "fin gui";
    
}

void ofApp::userChanged(bool & u){
    //ofLog() << "userChanged";
    user = u;
    if(user)
    {
        inPort = 14000;
        outPort = 14001;
    } else {
        inPort = 14001;
        outPort = 14000;
    }
    receiver.setup(inPort);
    socketOk = false;
    connectSocket();
    sync.setup((ofParameterGroup&)advancedPanel, inPort+10, hostname, outPort+10);
    updateSounds();
}

void ofApp::ipChanged(int & i)
{
    stringstream ss;
    ss << ip1 << "." << ip2 << "." << ip3 << "." << ip4;
    if(ss.str() != hostname)
    {
        hostname = ss.str();
        ofLog() << "Setting Hostname \"" << hostname << "\"";
        socketOk = false;
        connectSocket();
        sync.setup((ofParameterGroup&)advancedPanel, inPort+10, hostname, outPort+10);
    }
}

void ofApp::soundChanged(int & i)
{
    updateSounds();
}

void ofApp::twoPlayersChanged(bool & u)
{
    updateSounds();
}

#pragma mark -
#pragma mark LEAP

void ofApp::leapUpdate() {
    //ofLog() << "Leap update";
    fingersFound.clear();
    simpleHands = leap.getSimpleHands();
    
	if (initCam == false)
		return;
    
    if( leap.isFrameNew() )
    {
        if(simpleHands.size())
        {
            leapFrameID = leap.getCurrentFrameID();
            leap.setMappingX(-230, 230, -ofGetWidth()/2, ofGetWidth()/2);
            leap.setMappingY(90, 490, -ofGetHeight()/2, ofGetHeight()/2);
            leap.setMappingZ(-150, 150, -200, 200);
            
            fingerType fingerTypes[] = {THUMB, INDEX, MIDDLE, RING, PINKY};
            
            for(int i = 0; i < simpleHands.size(); i++){
                for (int f=0; f<5; f++) {
                    int id = simpleHands[i].fingers[ fingerTypes[f] ].id;
                    ofPoint mcp = simpleHands[i].fingers[ fingerTypes[f] ].mcp; // metacarpal
                    ofPoint pip = simpleHands[i].fingers[ fingerTypes[f] ].pip; // proximal
                    ofPoint dip = simpleHands[i].fingers[ fingerTypes[f] ].dip; // distal
                    ofPoint tip = simpleHands[i].fingers[ fingerTypes[f] ].tip; // fingertip
                    fingersFound.push_back(id);
                }
                
                leapPoint p;
                p.index = simpleHands[i].fingers[ fingerTypes[INDEX] ].tip;
                p.palm = simpleHands[i].handPos;
               /*if(finger)
                {
                    p = simpleHands[i].fingers[ fingerTypes[INDEX] ].tip;  // fingertip
                } else {
                    p = simpleHands[i].handPos;
                }//*/
                
                if(resolved)
                    sendPoint(simpleHands[i].isLeft, p, smoothedVol);
                
                addPoint(simpleHands[i].isLeft, p);
            }

            switch(simpleHands.size())
            {
                case 1:
                    if(simpleHands[0].isLeft)
                        camTarget = getPoint(lastLeft);
                    else
                        camTarget = getPoint(lastRight);
                    break;
                case 2:
                    camTarget = (getPoint(lastLeft)+getPoint(lastRight))/2.;
                    break;
            }
        } else {
            camTarget = ofPoint(0, 0, 0);
        }
    }
    
    leap.markFrameAsOld();
    
    float decay = dec == 0 ? 0 : dec/10.+0.9;
    leftChain.step(dev, decay);
    rightChain.step(dev, decay);
    remLeftChain.step(dev, decay);
    remRightChain.step(dev, decay);
}

void ofApp::addPoint(bool left, leapPoint _p)
{
    ofPoint p = getPoint(_p);
    p.x += ofRandom(-rndPos, rndPos)*100.;
    p.y += ofRandom(-rndPos, rndPos)*100.;
    p.z += ofRandom(-rndPos, rndPos)*100.;
    
    p.x += snd*smoothedVol*ofRandom(-5000., 5000.);
    p.y += snd*smoothedVol*ofRandom(-5000., 5000.);
    p.z += snd*smoothedVol*ofRandom(-5000., 5000.);
    
    ofFloatColor colLeft, colRight;
    if(!colorMode)
    {
        if(twoPlayers)
        {
            if(user)
            {
                colLeft = colorB;
                colRight = colorB;
            } else {
                colLeft = colorA;
                colRight = colorA;
            }
        } else {
            colLeft = colorA;
            colRight = colorB;
        }
    } else {
        ofFloatColor c = ofFloatColor(zmap(p.x,-400.,400.,0.25,1.), zmap(p.y, -200., 200., 0.25, 1.), 1.-zmap(p.z, -200., 200., 0.25, 1.));
        colLeft = c;
        colRight = c;
    }
    
    float rnd = ofRandom(-rndColor,rndColor);
    colLeft.r += rnd;
    colLeft.g += rnd;
    colLeft.b += rnd;
    rnd = ofRandom(-rndColor,rndColor);
    colRight.r += rnd;
    colRight.g += rnd;
    colRight.b += rnd;
    

    if(left)
    {
        int semitone = scaleFilter((int)zmap(p.x, -400., 400., -12, 12));
        if(semitone != lastSemiL)
        {
            playL.setPan(zmap(p.x,-400.,400.,-1.,1.));
            playL.setSpeed(toneToRate(semitone));
            playL.setVolume(zmap(p.y, -200., 200., 0.1f, 0.3f)*volume);
            if(doSelf && doSound)
                playL.play();
            lastSemiL = semitone;
        }
        
        ofVec3f vel = p - getPoint(lastLeft);
        leftChain.addPoint(point(p, vel, colLeft, smoothedVol*100.), smooth);
        lastLeft = _p;
    } else {
        int semitone = scaleFilter(zmap(p.x, -400., 400., -12, 12));
        if(semitone != lastSemiR)
        {
            playR.setPan(zmap(p.x,-400.,400.,-1.,1.));
            playR.setSpeed(toneToRate(semitone));
            playR.setVolume(zmap(p.y, -200., 200., 0.1f, 0.6f)*volume);
            if(doSelf && doSound)
                playR.play();
            lastSemiR = semitone;
        }
        
        ofVec3f vel = p - getPoint(lastRight);
        rightChain.addPoint(point(p, vel, colRight,smoothedVol*100.), smooth);
        lastRight = _p;
    }
}

void ofApp::addRemotePoint(bool left, leapPoint _p)
{
    ofPoint p = getPoint(_p);
    p.x += ofRandom(-rndPos, rndPos)*100.;
    p.y += ofRandom(-rndPos, rndPos)*100.;
    p.z += ofRandom(-rndPos, rndPos)*100.;
    
    p.x += snd*remoteSmoothedVol*ofRandom(-5000., 5000.);
    p.y += snd*remoteSmoothedVol*ofRandom(-5000., 5000.);
    p.z += snd*remoteSmoothedVol*ofRandom(-5000., 5000.);
    
    ofFloatColor colLeft, colRight;
    if(!colorMode)
    {
        if(twoPlayers)
        {
            if(user)
            {
                colLeft = colorA;
                colRight = colorA;
            } else {
                colLeft = colorB;
                colRight = colorB;
            }
        } else {
            colLeft = colorA;
            colRight = colorB;
        }
    } else {
        ofFloatColor c = ofFloatColor(zmap(p.x,-400.,400.,0.25,1.), zmap(p.y, -200., 200., 0.25, 1.), 1.-zmap(p.z, -200., 200., 0.25, 1.));
        colLeft = c;
        colRight = c;
    }
    
    float rnd = ofRandom(-rndColor,rndColor);
    colLeft.r += rnd;
    colLeft.g += rnd;
    colLeft.b += rnd;
    rnd = ofRandom(-rndColor,rndColor);
    colRight.r += rnd;
    colRight.g += rnd;
    colRight.b += rnd;
    
    if(left)
    {
        int semitone = scaleFilter((int)zmap(p.x, -400., 400., -12, 12));
        if(semitone != remLastSemiL)
        {
            remPlayL.setPan(zmap(p.x,-400.,400.,-1.,1.));
            remPlayL.setSpeed(toneToRate(semitone));
            remPlayL.setVolume(zmap(p.y, -200., 200., 0.1f, 0.3f)*volume);
            if(doRemote && doSound)
                remPlayL.play();
            remLastSemiL = semitone;
        }
        
        ofVec3f vel = p - getPoint(remLastLeft);
        remLeftChain.addPoint(point(p, vel, colLeft, remoteSmoothedVol*100.), smooth);
        remLastLeft = _p;
    } else {
        int semitone = scaleFilter(zmap(p.x, -400., 400., -12, 12));
        if(semitone != remLastSemiR)
        {
            remPlayR.setPan(zmap(p.x,-400.,400.,-1.,1.));
            remPlayR.setSpeed(toneToRate(semitone));
            remPlayR.setVolume(zmap(p.y, -200., 200., 0.1f, 0.6f)*volume);
            if(doRemote && doSound)
                remPlayR.play();
            remLastSemiR = semitone;
        }
        
        ofVec3f vel = p - getPoint(remLastRight);
        remRightChain.addPoint(point(p, vel, colRight,remoteSmoothedVol*100.), smooth);
        remLastRight = _p;
    }
}

#pragma mark -
#pragma mark SOUND

void ofApp::initSound() {
    
    smoothedVol = 0;
    remoteSmoothedVol = 0;
    
    ofFmodSetBuffersize(512);
    
    sounds[0].load("sounds/piano.aif");
    sounds[1].load("sounds/cello.aif");
    sounds[2].load("sounds/men.aif");
    sounds[3].load("sounds/women.aif");
    sounds[4].load("sounds/timbale.aif");
    
    for(int i = 0 ; i < NUM_SOUNDS ; i++)
        sounds[i].setMultiPlay(true);
    
    updateSounds();
}

void ofApp::getVolume() {
    float vol = ofSoundGetSpectrum(1)[0];
    
    smoothedVol *= 0.8f;
    if (smoothedVol < vol)
        smoothedVol = vol;
}

void ofApp::updateSounds() {
    if(twoPlayers)
    {
        if(user)
        {
            playL = sounds[soundB-1];
            playR = sounds[soundB-1];
            remPlayL = sounds[soundA-1];
            remPlayR = sounds[soundA-1];
        } else {
            playL = sounds[soundA-1];
            playR = sounds[soundA-1];
            remPlayL = sounds[soundB-1];
            remPlayR = sounds[soundB-1];
        }
    } else {
        playL = sounds[soundA-1];
        playR = sounds[soundB-1];
    }

}

#pragma mark -
#pragma mark NETWORK

bool ofApp::connectSocket() {
    if(socketOk)
        return true;
    socketOk = sender.setup(hostname, outPort);
    if(!socketOk)
        return false;
    ofLog() << "Connected to host : " << hostname;
    return true;
}

void ofApp::receiveOSC() {
    while(receiver.hasWaitingMessages())
    {
        ofxOscMessage m;
        if(receiver.getNextMessage(m))
        {
            bool left = true;
            if(m.getAddress() == "/leapLeft" || m.getAddress() == "/leapRight") {
                left = m.getAddress() == "/leapLeft";
                if(m.getNumArgs() == 7)
                {
                    leapPoint p;
                    p.palm = ofPoint(m.getArgAsFloat(0), m.getArgAsFloat(1), m.getArgAsFloat(2));
                    p.index = ofPoint(m.getArgAsFloat(3), m.getArgAsFloat(4), m.getArgAsFloat(5));
                    remoteSmoothedVol = m.getArgAsFloat(6);
                    //ofLog() << "receivePoint(" << p.toString(2) << ")";
                    addRemotePoint(left, p);
                }
            } else if(m.getAddress() == "/scenario") {
                //ofLog() << "Server got message: " << m.getAddress() << " " << m.getArgAsInt(0);
                currentScenario = m.getArgAsInt(0);
                clearChains();
                textFade = 255;
            } else {
                ofLog() << "Server got message: " << m.getAddress();
            }
        }
    }
    
    //this is purely workaround for a mysterious OSCpack bug on 64bit linux
    // after startup, reinit the receivers
    // must be a timing problem, though - in debug, stepping through, it works.
}

void ofApp::sendPoint(bool left, leapPoint p, float vol) {
    
    if(connectSocket())
    {
        //ofLog() << "sendPoint(" << p.toString(2) << ")";
        ofxOscMessage m;
        
        m.setAddress(left ? "/leapLeft" : "/leapRight");
        m.addFloatArg(p.palm.x);
        m.addFloatArg(p.palm.y);
        m.addFloatArg(p.palm.z);
        m.addFloatArg(p.index.x);
        m.addFloatArg(p.index.y);
        m.addFloatArg(p.index.z);
        m.addFloatArg(vol);
        
        sender.sendMessage(m,false);
    }
}

void ofApp::sendScenario() {
    
    if(connectSocket())
    {
        ofxOscMessage m;
        m.setAddress("/scenario");
        m.addIntArg(currentScenario);
        sender.sendMessage(m, false);
    }
}

#pragma mark -
#pragma mark RECORDS

void ofApp::startRec() {
    if(isPlaying)
        stopPlay();
    recFileName = ofToDataPath("records/MainRec_"+timestampString()+"_"+ofToString(currentScenario)+".csv",true);
    playFileName = recFileName;
    ofLog() << "Record started : \"" << recFileName << "\"";
    ofstream recFile(recFileName, ofstream::out | ofstream::app );
    recFile << "Version " << VERSION << endl;
    if(currentScenario < scenari.size() && currentScenario >= 0)
    {
        recFile << "Scenario " << currentScenario << endl;// << " " << scenari[currentScenario].getName() << endl;

    }
    recFile << "time(ms), ";
    recFile << "A_L_pX, A_L_pY, A_L_pZ, ";
    recFile << "A_L_iX, A_L_iY, A_L_iZ, ";
    recFile << "A_R_pX, A_R_pY, A_R_pZ, ";
    recFile << "A_R_iX, A_R_iY, A_R_iZ, ";
    recFile << "B_L_pX, B_L_pY, B_L_pZ, ";
    recFile << "B_L_iX, B_L_iY, B_L_iZ, ";
    recFile << "B_R_pX, B_R_pY, B_R_pZ, ";
    recFile << "B_R_iX, B_R_iY, B_R_iZ";
    recFile << endl;
    isRecording = true;
    recTime = ofGetSystemTime();
    
    textFade = 255;
}

void ofApp::stopRec() {
    //recFile.close();
    isRecording = false;
    ofLog() << "Record stopped";
    textFade = 255;
}

void ofApp::record()
{
    leapPoint lL = lastLeft * REC_SCALE;
    leapPoint lR = lastRight * REC_SCALE;
    leapPoint rlL = remLastLeft * REC_SCALE;
    leapPoint rlR = remLastRight * REC_SCALE;
    ofstream recFile(recFileName, ofstream::out | ofstream::app);
    recFile << std::fixed << std::setprecision(PRECISION);
    recFile << (ofGetSystemTime() - recTime) << ", ";
    if(user)
    {
        recFile << rlL.toString(PRECISION) << ", ";
        recFile << rlR.toString(PRECISION) << ", ";
        recFile << lL.toString(PRECISION) << ", ";
        recFile << lR.toString(PRECISION);
    } else {
        recFile << lL.toString(PRECISION) << ", ";
        recFile << lR.toString(PRECISION) << ", ";
        recFile << rlL.toString(PRECISION) << ", ";
        recFile << rlR.toString(PRECISION);
    }
    recFile << endl;
}

#pragma mark -
#pragma mark RECORDS

void ofApp::openFile() {
    if(isRecording)
        stopRec();
    if(isPlaying)
        stopPlay();
    ofFileDialogResult r = ofSystemLoadDialog("Main Player", false, ofToDataPath("records"));
    if(r.bSuccess)
    {
        ofLog() << "Opening file : \"" << r.getName() << "\"";
        playFileName = r.getPath();
        shortName = r.getName();
        if(graphMode)
            initGraph();
        else
            startPlay();
    }

}

void ofApp::startPlay() {
    if(isRecording)
        stopRec();
    if(playFileName != "")
    {
        maxTime = getDuration();
        shortName = ofFile(playFileName).getFileName();
        playFile = ifstream(playFileName, ofstream::in);
        
        if (playFile.is_open())
        {
            string line;
            
            if (! playFile.eof() )
            {
                getline (playFile,line);
                float versionValue = ofToFloat(split(line, ' ')[1]);
                
                if (! playFile.eof() )
                {
                    getline (playFile,line);
                    int scenarioValue = ofToInt(split(line, ' ')[1]);
					ofLog() << "scenario " << scenarioValue;
                    
                    if(scenarioValue >= 0 && scenarioValue < scenari.size())
                    {
                        currentScenario = scenarioValue;
                        loadScenario(currentScenario);
                    }
                    if (! playFile.eof() )
                    {
                        getline (playFile,line); // ignored
                        
                        if (! playFile.eof() )
                        {
                            nextLine.clear();
                            getline(playFile, line);
                            nextLine = splitInt(line,' ');
                            if(nextLine.size() > 0)
                            {
                                nextTime = nextLine[0];
                                ofLog() << "Start playing : \"" << shortName << "\" Version " << versionValue << " Scenario " << scenarioValue << " Duration " << fixed << setprecision(3) << maxTime/1000. << "s";
                                isPlaying = true;
                                playTime = ofGetSystemTime();
                                clearChains();
                            }
                        }
                    }
                }
            }
        }
        else
            ofLog() << "Unable to open file " << playFileName;
        textFade = 255;
    }
}

void ofApp::playUpdate() {
	if (!isPlaying)
		return;
    if(playFile.is_open())
    {
        curTime = ofGetSystemTime() - playTime;
        string line;
        while(curTime > nextTime && isPlaying) {
            playLine();
            if(!playFile.eof()) {
                nextLine.clear();
                getline(playFile, line);
                nextLine = splitInt(line,' ');
                if(nextLine.size() > 0 )
                {
                    nextTime = nextLine[0];
                } else {
                    stopPlay();
                    return;
                }
            } else {
                stopPlay();
                return;
            }
        }
    } else {
        ofLog() << "Unable to open file " << playFileName;
    }
    camTarget = (getPoint(lastLeft)+getPoint(lastRight))/2.;
    float decay = dec == 0 ? 0 : dec/10.+0.9;
    leftChain.step(dev, decay);
    rightChain.step(dev, decay);
    remLeftChain.step(dev, decay);
    remRightChain.step(dev, decay);
}

void ofApp::playLine() {
    leapPoint AL, AR, BL, BR;
    if(user)
    {
        BL = leapPoint(ofPoint(nextLine[1],nextLine[2],nextLine[3]), ofPoint(nextLine[4],nextLine[5],nextLine[6]))/REC_SCALE;
        BR = leapPoint(ofPoint(nextLine[7],nextLine[8],nextLine[9]), ofPoint(nextLine[10],nextLine[11],nextLine[12]))/REC_SCALE;
        AL = leapPoint(ofPoint(nextLine[13],nextLine[14],nextLine[15]), ofPoint(nextLine[16],nextLine[17],nextLine[18]))/REC_SCALE;
        AR = leapPoint(ofPoint(nextLine[19],nextLine[20],nextLine[21]), ofPoint(nextLine[22],nextLine[23],nextLine[24]))/REC_SCALE;
    } else {
        AL = leapPoint(ofPoint(nextLine[1],nextLine[2],nextLine[3]), ofPoint(nextLine[4],nextLine[5],nextLine[6]))/REC_SCALE;
        AR = leapPoint(ofPoint(nextLine[7],nextLine[8],nextLine[9]), ofPoint(nextLine[10],nextLine[11],nextLine[12]))/REC_SCALE;
        BL = leapPoint(ofPoint(nextLine[13],nextLine[14],nextLine[15]), ofPoint(nextLine[16],nextLine[17],nextLine[18]))/REC_SCALE;
        BR = leapPoint(ofPoint(nextLine[19],nextLine[20],nextLine[21]), ofPoint(nextLine[22],nextLine[23],nextLine[24]))/REC_SCALE;
    }

    if(AL != leapZero() && AL != pAL)
    {
        addPoint(true, AL);
        pAL = AL;
    }
    if(AR != leapZero() && AR != pAR)
    {
        addPoint(false, AR);
        pAR = AR;
    }
    if(BL != leapZero() && BL != pBL)
    {
        addRemotePoint(true, BL);
        pBL = BL;
    }
    if(BR != leapZero() && BR != pBR)
    {
        addRemotePoint(true, BR);
        pBR = BR;
    }
}

void ofApp::stopPlay() {
    playFile.close();
    isPlaying = false;
	remPlayL.stop();
	remPlayR.stop();
	playL.stop();
	playR.stop();
    ofLog() << "Stop playing";
    textFade = 255;
}

int ofApp::getDuration() {
    if(playFileName != "")
    {
        ifstream file = ifstream(playFileName, ofstream::in);
        if (file.is_open())
        {
            nLines = 0;
            string prevLine;
            string line;
            while (! file.eof() )
            {
                prevLine = line;
                getline (file,line);
                nLines++;
            }
            vector<int> s = splitInt(prevLine,' ');
            if(s.size() > 0)
                return s[0];
            return 0;
        }
    }
    return 0;
}

#pragma mark -
#pragma mark GRAPH

void ofApp::initGraph() {
    if(playFileName != "")
    {
        graphStart = 0;
        graphEnd = 1;
        maxTime = getDuration();
        ofLog() << "Init Graph " << shortName << " - " << ofToString(maxTime/1000., 2) << "s";
        gAL.clear();
        gAL.setName("A Left");
        gAR.clear();
        gAR.setName("A Right");
        gBL.clear();
        gBL.setName("B Left");
        gBR.clear();
        gBR.setName("B Right");
        
        setFinger(graphFinger);
        
        int t;
        leapPoint p;
        ifstream file = ifstream(playFileName, ofstream::in);
        if (file.is_open())
        {
            string prevLine;
            string line;
            
            if (! file.eof() )
            {
                getline (file,line);
                float versionValue = ofToFloat(split(line, ' ')[1]);
                
                if (! file.eof() )
                {
                    getline (file,line);
                    int scenarioValue = ofToInt(split(line, ' ')[1]);
                    
                    if(scenarioValue >= 0 && scenarioValue < scenari.size())
                    {
                        currentScenario = scenarioValue;
                        loadScenario(currentScenario);
                    }
                    if (! file.eof() )
                    {
                        getline (file,line); // ignored
                    }
                }
            }
            while (! file.eof() )
            {
                prevLine = line;
                getline (file,line);
                vector<int> s = splitInt(line,' ');
                if(s.size() == 25)
                {
                    t = s[0];
                    //ofLog(OF_LOG_WARNING) << "ADD POINT t:" << t;
                    
                    p.palm.x = s[1];
                    p.palm.y = s[2];
                    p.palm.z = s[3];
                    p.index.x = s[4];
                    p.index.y = s[5];
                    p.index.z = s[6];
                    gAL.add(t, (p*GRAPH_SCALE));
                    
                    p.palm.x = s[7];
                    p.palm.y = s[8];
                    p.palm.z = s[9];
                    p.index.x = s[10];
                    p.index.y = s[11];
                    p.index.z = s[12];
                    gAR.add(t, (p*GRAPH_SCALE));
                    
                    p.palm.x = s[13];
                    p.palm.y = s[14];
                    p.palm.z = s[15];
                    p.index.x = s[16];
                    p.index.y = s[17];
                    p.index.z = s[18];
                    gBL.add(t, (p*GRAPH_SCALE));
                    
                    p.palm.x = s[19];
                    p.palm.y = s[20];
                    p.palm.z = s[21];
                    p.index.x = s[22];
                    p.index.y = s[23];
                    p.index.z = s[24];
                    gBR.add(t, (p*GRAPH_SCALE));//*/
                }
            }
            
            gAL.setDuration(maxTime);
            gAL.calc();
            gAR.setDuration(maxTime);
            gAR.calc();
            gBL.setDuration(maxTime);
            gBL.calc();
            gBR.setDuration(maxTime);
            gBR.calc();
            
        }
        
        file.close();
    }
}

void ofApp::setFinger(bool f) {
    gAL.setFinger(f);
    gAR.setFinger(f);
    gBL.setFinger(f);
    gBR.setFinger(f);
}

#pragma mark -
#pragma mark ZEROCONF

#if USE_ZEROCONF
void ofApp::initZeroconf() {
    //ofLog() << "Init Zeroconf";
    zeroSearch("_osc._udp", "local.", true);
    this_thread::sleep_for(chrono::milliseconds(200));
    
    if(zeroAlreadyExist("mainA"))
    {
        user = true;
        inPort = 14001;
        inName = "mainB";
        outPort = 14000;
        outName = "mainA";
        
        //ofLog() << "[initZeroconf] Have to resolve : " << outName;
        
    } else {
        user = false;
        inPort = 14000;
        inName = "mainA";
        outPort = 14001;
        outName = "mainB";
    }

    zeroServiceIn = new ZeroService("local.", "_osc._udp", inName, inPort);
    zeroServiceIn->publish(false);

    zeroServiceOut = new ZeroService("local.", "_osc._udp", outName);
    zeroServiceOut->resolveWithTimeout(2.);
    
    receiver.setup(inPort);
}

void ofApp::resolveZeroconf() {
    //ofLog() << "RESOLVE ZEROCONF";
    if(zeroServiceOut->_resolved)
    {
        resolved = true;
        hostname = zeroServiceOut->_hostname;
        socketOk = sender.setup(hostname, outPort);

        //ofLog() << "ServiceOut Resolved host:\"" << hostname << "\" port:\"" << outPort << "\"";
    } else {
        //ofLog() << "ServiceOut NOT Resolved";
    }
}

void ofApp::stopZeroconf() {
    if(zeroServiceIn)
        delete zeroServiceIn;
    if(zeroServiceOut)
        delete zeroServiceOut;
    stop();
}

void ofApp::zeroSearch(string type, string domain, bool launchThread) {
    if(!type.empty())
    {
        stop();
        {
            ScopedLock lock(zeroCriticalSection);
            zeroServices.clear();
        }
        searchForServicesOfType(type, domain, launchThread);
    }
}

bool ofApp::zeroAlreadyExist(const std::string &name) {
    return zeroServices.find(name) != zeroServices.end();
}

void ofApp::zeroOutputListOfServices() {
    ofLog() << "[Bro] List of Services :";
    ScopedLock lock(zeroCriticalSection);
    for(std::set<std::string>::iterator it=zeroServices.begin(); it != zeroServices.end(); ++it)
    {
        ofLog() << "[Bro] \t\t - " << it->c_str();
    }
}

void ofApp::didFindService(NetServiceBrowser* pNetServiceBrowser, NetService *pNetService, bool moreServicesComing) {
    {
        ScopedLock lock(zeroCriticalSection);
        zeroServices.insert(pNetService->getName());
    }
    
    string name = pNetService->getName();
    //ofLog() << "[Bro] Service found : " << name;
    if(name == inName)
    {
        //ofLog() << "[Bro] Published OK";
    } else if(name == outName) {
        //ofLog() << "[didFindService] Have to resolve : " << name;
        zeroServiceOut->resolveWithTimeout(2.);
    }
}

void ofApp::didRemoveService(NetServiceBrowser *pNetServiceBrowser, NetService *pNetService, bool moreServicesComing) {
    
    string name = pNetService->getName();
    {
        set<string>::iterator it = zeroServices.find(name);
        if(it != zeroServices.end())
        {
            ScopedLock lock(zeroCriticalSection);
            zeroServices.erase(it);
        }
    }
    
    //ofLog() << "[Bro] Service Removed " << name;
    if(name == inName)
    {
        //ofLog() << "[Bro] Removed OK";
    } else if(name == outName) {
        //ofLog() << "[Bro] Have to remove : " << name;
        zeroServiceOut->_resolved = false;
        resolved = false;
    }
}
#endif
