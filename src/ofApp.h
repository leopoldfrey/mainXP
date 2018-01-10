#ifndef ofApp_H
#define ofApp_H

#define _DNS_SD_LIBDISPATCH 0
#define USE_ZEROCONF 0

#define VERSION 1.2
#define NUM_SOUNDS 5

#include "ofMain.h"
#include <ofxLeapMotion2\src\ofxLeapMotion2.h>
//#include "ofxLeapMotion2.h"
#include <ofxGui\src\ofxGui.h>
//#include "ofxGui.h"
#include "chain.h"
#include "point.h"
#include "leapPoint.h"
#include "scenario.h"
#include <ofxOsc\src\ofxOscParameterSync.h>
//#include "ofxOscParameterSync.h"
#include "ofXml.h"
#include "mainCam.h"
#include "graph.h"
#if USE_ZEROCONF
#include "ZeroService.h"
#include "NetServiceBrowser.h"
#endif

#if !defined(_WIN32)
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#else
#include <WinSock.h>
#endif

#define WIDTH 1024
#define HEIGHT 768
#define REC_SCALE 10
#define GRAPH_SCALE 0.01
#define PRECISION 0

/*
typedef struct _leapPoint {
    ofPoint palm;
    ofPoint index;
} leapPoint;//*/

#if USE_ZEROCONF
class ofApp : public ofBaseApp, public NetServiceBrowser, public NetServiceBrowserListener
#else
class ofApp : public ofBaseApp
#endif
{
public:
    
#pragma mark -
#pragma mark __OK__ BUG verifier ofxOscsocket exception > gestion exception
#pragma mark __OK__ TODO GUI quand enregistre pas de rond rouge > scenario en rouge
#pragma mark __OK__ TODO GUI pas de visualisation du son
#pragma mark __OK__ TODO fleche gauche / droite
#pragma mark __OK__ TODO fullscreen at start
#pragma mark __OK__ TODO RECORD nombre flottant point ou virgule CSV / CSV2
#pragma mark __OK__ TODO RECORD fichiers separes par scenario
#pragma mark __OK__ TODO RECORD palm+index
#pragma mark __OK__ TODO RECORD replay
#pragma mark __OK__ TODO REAL LOG
#pragma mark __OK__ TODO NOTICE!!!!!
#pragma mark __OK__ TODO graph
#pragma mark __OK__ TODO lister les scenari
#pragma mark __OK__ TODO numero de version dans le nom appli
#pragma mark TODO Z SOUND FX
#pragma mark BUG check window title fullscreen
#pragma mark -
    
#if USE_ZEROCONF
    ofApp() : NetServiceBrowser() {
        setListener(this);
    }
#endif
    
#pragma mark -
#pragma mark MAIN
    
    void setup();
    void update();
    void draw();
    void graphDraw();
    void fboDraw();
    void clearChains();
    void drawChain(chain c);
    void exit();
    
    ofFbo fbo;
    
#pragma mark -
#pragma mark INTERACT
    
    void keyPressed(int key);
    void keyReleased(int key){}
    void mouseMoved(int x, int y ){}
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo){}
    void gotMessage(ofMessage msg){}
    
#pragma mark -
#pragma mark GUI
    
    ofTrueTypeFont verdana14;
    
    void initGui();
    
    bool fullscreen = true;
    bool bHide;
    int textFade;
    ofParameter<bool> user;
    ofParameter<int> ip1;
    ofParameter<int> ip2;
    ofParameter<int> ip3;
    ofParameter<int> ip4;
    
    ofParameter<bool> twoPlayers;
    ofParameter<bool> doSelf;
    ofParameter<bool> doRemote;
    ofParameter<bool> doSound;
    ofParameter<bool> finger;
    ofParameter<bool> grid;
    ofParameter<bool> origin;
    ofParameter<float> snd;
    ofParameter<float> dev;
    ofParameter<float> dec;
    ofParameter<bool> colorMode;
    ofParameter<int> drawMode;
    ofParameter<float> rndColor;
    ofParameter<float> rndPos;
    ofParameter<float> size;
    ofParameter<int> soundA;
    ofParameter<int> soundB;
    ofParameter<float> volume;
    ofParameter<float> trail;
    ofParameter<ofFloatColor> colorA;
    ofParameter<ofFloatColor> colorB;
    ofParameter<bool> camMove;
    
    ofParameter<float> smooth;
    ofParameter<float> smoothCam;
    
    ofParameterGroup generalPanel;
    ofParameterGroup advancedPanel;
    ofParameterGroup expertPanel;
    ofxPanel gui;
    
    ofxOscParameterSync sync;
    
    ofPoint camTarget;
    ofPoint prevCamTarget;
    
    void userChanged(bool & u);
    void ipChanged(int & i);
    void soundChanged(int & i);
    void twoPlayersChanged(bool & u);
    
#pragma mark -
#pragma mark SCENARIO
    
    ofXml scenariXml;
    int currentScenario;
    vector<scenario> scenari;
    bool doListScenari;
    
    void loadScenari();
    void saveScenari();
    
    void nextScenario();
    void prevScenario();
    void loadScenario(int i);
    void saveScenario();
    void updateScenario();
    void renameScenario();
    void deleteScenario();
    void listScenari();
    
#pragma mark -
#pragma mark LEAP

    void leapUpdate();
    void addPoint(bool left, leapPoint p);
    void addRemotePoint(bool left, leapPoint p);
    
    int64_t leapFrameID;
    vector <int> fingersFound;
    bool enableCamCtrl;
    bool saveImage;
    mainCam cam;
    ofxLeapMotion leap;
    vector <ofxLeapMotionSimpleHand> simpleHands;

    chain leftChain;
    chain rightChain;
    leapPoint lastLeft;
    leapPoint lastRight;
    
    chain remLeftChain;
    chain remRightChain;
    leapPoint remLastLeft;
    leapPoint remLastRight;
    
#pragma mark -
#pragma mark SOUND
    
    void initSound();
    void updateSounds();
    void getVolume();
    
    ofSoundPlayer sounds[NUM_SOUNDS];
    
    ofSoundPlayer  playL;
    ofSoundPlayer  playR;
    ofSoundPlayer  remPlayL;
    ofSoundPlayer  remPlayR;
    
    int lastSemiL = -36;
    int lastSemiR = -36;
    int remLastSemiL = -36;
    int remLastSemiR = -36;
    
    float smoothedVol;
    float remoteSmoothedVol;
    
#pragma mark -
#pragma mark NETWORK
    
    bool socketOk;
    bool resolved;
    string hostname;
    string localIP;
    int outPort;
    int inPort;
    string inName;
    string outName;
    
    ofxOscSender sender;
    ofxOscReceiver receiver;

    bool connectSocket();
    void receiveOSC();
    void sendPoint(bool left, leapPoint p, float vol);
    void sendScenario();

#pragma mark -
#pragma mark ZEROCONF

#if USE_ZEROCONF
    void initZeroconf();
    void resolveZeroconf();
    void stopZeroconf();
    
    ZeroService *zeroServiceIn;
    ZeroService *zeroServiceOut;
    
    set<string> zeroServices;
    CriticalSection zeroCriticalSection;
    
    void zeroSearch(string type, string domain, bool launchThread);
    bool zeroAlreadyExist(const std::string &name);
    void zeroOutputListOfServices();
    void didFindService(NetServiceBrowser* pNetServiceBrowser, NetService *pNetService, bool moreServicesComing);
    void didRemoveService(NetServiceBrowser *pNetServiceBrowser, NetService *pNetService, bool moreServicesComing);
    void didFindDomain(NetServiceBrowser *pNetServiceBrowser, const std::string &domainName, bool moreDomainsComing) { }
    void didRemoveDomain(NetServiceBrowser *pNetServiceBrowser, const std::string &domainName, bool moreDomainsComing) { }
    void willSearch(NetServiceBrowser *pNetServiceBrowser) { /*ofLog() << "[Bro] willSearch";*/ }
    void didNotSearch(NetServiceBrowser *pNetServiceBrowser) { ofLog(OF_LOG_ERROR) << "[Bro] didNotSearch"; }
    void didStopSearch(NetServiceBrowser *pNetServiceBrowser) { /*ofLog() << "[Bro] didStopSearch";*/ }
#endif
    
#pragma mark -
#pragma mark RECORDS
    
    int64_t recTime;
    
    void startRec();
    void stopRec();
    void record();
    bool isRecording;
    string recFileName;
    int frameCounter;
    
    //float recScale = 10.;
   
#pragma mark -
#pragma mark REPLAY
    
    int64_t playTime;
    int64_t curTime;
    int64_t nextTime;
    int64_t maxTime;
    bool isPlaying;
    string playFileName;
    string shortName;
    ifstream playFile;
    vector<int> nextLine;

    leapPoint pAL;
    leapPoint pAR;
    leapPoint pBL;
    leapPoint pBR;
    
    void openFile();
    void startPlay();
    void playUpdate();
    void playLine();
    void stopPlay();
    int getDuration();
    
#pragma mark -
#pragma mark GRAPH
    
    int nLines;
    bool graphFinger = true;
    bool graphMode;
    bool graphFinder;
    int graphType;
    Graph gAL, gAR, gBL, gBR;
    float graphStart = 0;
    float graphEnd = 1;
    bool dragGraphStart;
    bool dragGraphEnd;
    bool dragGraph;
    int clicTime;
    int clicX;
    void initGraph();
    void setFinger(bool f);
    
#pragma mark -
#pragma mark UTILS
    
    float toneToRate(int semitone) {
        return pow(pow(2., 1./12.), (float)semitone);
    }
    
    int scaleFilter(int semitone) {
        bool oct = semitone < 0;
        bool oct2 = semitone < -11;
        bool oct3 = semitone >= 12;
        if(oct)
            semitone += 12;
        if(oct2)
            semitone += 12;
        if(oct3)
            semitone -= 12;
        
        switch(semitone)
        {
            case 0:
                semitone = 0;
                break;
            case 1:
            case 2:
                semitone = 2;
                break;
            case 3:
            case 4:
                semitone = 3;
                break;
            case 5:
            case 6:
                semitone = 5;
                break;
            case 7:
                semitone = 7;
                break;
            case 8:
            case 9:
                semitone = 8;
                break;
            case 10:
            case 11:
                semitone = 10;
                break;
            case 12:
                semitone = 12;
                break;
        }
        if(oct)
            semitone -= 12;
        if(oct2)
            semitone -= 12;
        if(oct3)
            semitone += 12;
        
        return semitone;
    }

    float zmap(float v, float ol, float ou, float l, float u) {
        float pente = (u - l) / (float)(ou - ol);
        float ord = l - (pente * ol);
        return clip((float)(pente * v + ord), l, u);
    }
    
    float clip(float v, float l, float u) {
        return v < l ? l : (v > u ? u : v);
    }
    
    void drawLine(point p, point n) {
        glBegin(GL_LINES);
        ofSetColor(p.color, p.color.a*p.life*255);
        glVertex3f(p.pos.x,p.pos.y,p.pos.z);
        ofSetColor(n.color, n.color.a*n.life*255);
        glVertex3f(n.pos.x,n.pos.y,n.pos.z);
        glEnd();
    }
    
    void disk(point p, float r, int num_segments) {
        float theta, x, y;
        
        glBegin(GL_LINE_LOOP);
        for (int i = 0 ; i < num_segments-1 ; i++)
        {
            theta = 2 * 3.1415926 * i / num_segments;
            x = r * cos(theta);
            y = r * sin(theta);
            glVertex3f(x + p.pos.x, y + p.pos.y, p.pos.z);
        }
        glEnd();
    }
    
    string getOscMsgAsString(ofxOscMessage m) {
        string msg_string;
        msg_string = m.getAddress();
        msg_string += ":";
        for(int i = 0; i < m.getNumArgs(); i++){
            // get the argument type
            msg_string += " " + m.getArgTypeName(i);
            msg_string += ":";
            // display the argument - make sure we get the right type
            if(m.getArgType(i) == OFXOSC_TYPE_INT32){
                msg_string += ofToString(m.getArgAsInt32(i));
            }
            else if(m.getArgType(i) == OFXOSC_TYPE_FLOAT){
                msg_string += ofToString(m.getArgAsFloat(i));
            }
            else if(m.getArgType(i) == OFXOSC_TYPE_STRING){
                msg_string += m.getArgAsString(i);
            }
            else{
                msg_string += "unknown";
            }
        }
        return msg_string;
    }

    string getLocalIP()
    {
        
#if !defined(_WIN32)

        char addressBuffer[INET_ADDRSTRLEN];
        
        struct ifaddrs * ifAddrStruct=NULL;
        struct ifaddrs * ifa=NULL;
        void * tmpAddrPtr=NULL;
        
        getifaddrs(&ifAddrStruct);
        
        for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
            if (!ifa->ifa_addr) {
                continue;
            }
            if(ifa->ifa_addr->sa_family == AF_INET) {
                //ofLog() << "IFA NAME " << ifa->ifa_name;
                if(strcmp(ifa->ifa_name, "en0") == 0) {
                    tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
                    inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
                } else if (strcmp(ifa->ifa_name, "en1") == 0){
                    tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
                    inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
                } else if (strcmp(ifa->ifa_name, "en2") == 0){
                    tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
                    inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
                } else if (strcmp(ifa->ifa_name, "en3") == 0){
                    tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
                    inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
                } else if (strcmp(ifa->ifa_name, "en4") == 0){
                    tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
                    inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
                } else if (strcmp(ifa->ifa_name, "en5") == 0){
                    tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
                    inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
                } else if (strcmp(ifa->ifa_name, "en6") == 0){
                    tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
                    inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
                } else if (strcmp(ifa->ifa_name, "en7") == 0){
                    tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
                    inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
                } else if (strcmp(ifa->ifa_name, "en8") == 0){
                    tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
                    inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
                } else if (strcmp(ifa->ifa_name, "en9") == 0){
                    tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
                    inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
                }
            }
        }
        if (ifAddrStruct!=NULL)
            freeifaddrs(ifAddrStruct);
        return addressBuffer;
#else
		WSADATA wsa_Data;
		int wsa_ReturnCode = WSAStartup(0x101, &wsa_Data);
		char szHostName[255];
		gethostname(szHostName, 255);
        struct hostent *host_entry;
		host_entry = gethostbyname(szHostName);
		char * szLocalIP;
		szLocalIP = inet_ntoa(*(struct in_addr *)*host_entry->h_addr_list);
		WSACleanup();
		return szLocalIP;
#endif
         
        
	}
    
    string inputDialog(string title, string message, string defAnswer)
    {
        return ofSystemTextBoxDialog(message);
    }
    
    string timestampString(){
        
        string timeFormat = "%d%m%y_%H%M%S";
        
        return ofGetTimestampString(timeFormat);
    }
    
    float dbtoa(float db)
    {
        if(db <= -96)
            return 0;
        return (float)pow(10, db/20.);
    }
    
    float atodb(float a)
    {
        if(a < 0.000017)
            return -96;
        return (float)(20 * log10(a));
    }
    
    ofPoint getPoint(leapPoint p)
    {
        if(finger)
            return p.index;
        else
            return p.palm;
    }
    
    template<typename Out>
    void split(const std::string &s, char delim, Out result) {
        std::stringstream ss;
        ss.str(s);
        std::string item;
        while (std::getline(ss, item, delim)) {
            *(result++) = item;
        }
    }
    
    std::vector<std::string> split(const std::string &s, char delim) {
        std::vector<std::string> elems;
        split(s, delim, std::back_inserter(elems));
        return elems;
    }
    
    template<typename Out>
    void splitInt(const std::string &s, char delim, Out result) {
        std::stringstream ss;
        ss.str(s);
        int item;
        string strItem;
        while (std::getline(ss, strItem, delim)) {
            item = ofToInt(strItem);
            *(result++) = item;
        }
    }
    
    vector<int> splitInt(const std::string &s, char delim) {
        vector<int> elems;
        splitInt(s, delim, std::back_inserter(elems));
        return elems;
    }
    
    string displayVInt(vector<int> v) {
        stringstream ss;
        vector<int>::iterator vi;
        for(int i = 0 ; i < v.size() ; i++)
        {
            ss << v[i];
            if(i < v.size()-1)
                ss << " ";
        }
        return ss.str();
    }
};

#endif
