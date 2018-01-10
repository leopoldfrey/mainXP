#ifndef graph_h
#define graph_h

#include "leapPoint.h"
#include "ofApp.h"

#define MAX_GRAPH 5

#define GRAPH_X 0
#define GRAPH_Y 1
#define GRAPH_Z 2
#define GRAPH_N 3
#define GRAPH_PI 4
#define GRAPH_SPD 5
#define GRAPH_DSPD 6
// TODO 3D
// TODO VOLUME / AIRE


typedef struct _timePoint {
    int time;
    leapPoint point;
    float normP;
    float normI;
    float dPI;
    float spdP;
    float spdI;
    float dSPD;
} timePoint;

class Graph {
public:
    bool finger;
    string name;
    timePoint minV;
    timePoint maxV;
    timePoint meanV;
    timePoint medianV;
    int maxTime;
    vector<timePoint> histo;
    bool hasData;
    
    void clear() {
        histo.clear();
        maxTime = 0;
        hasData = false;
    }
    
    void setName(string n) {
        name = n;
    }
    
    void setFinger(bool f) {
        finger = f;
    }
    
    void setDuration(int d) {
        maxTime = d;
    }
    
    void add(int t, leapPoint p) {
        float nP = p.norm(false);
        float nI = p.norm(true);
        float dPI = distance(p.palm, p.index);
        //ofLog(OF_LOG_WARNING) << "ADD " << name << " t:" << t << " t-1:" << (histo.size() == 0 ? 0 : histo[histo.size()-1].time);
        float spdP = histo.size() == 0 ? 0 : speed(histo[histo.size()-1], {t,p}, false);
        float spdI = histo.size() == 0 ? 0 : speed(histo[histo.size()-1], {t,p}, true);
        float dSPD = abs(spdI - spdP);
        timePoint val = {t, p, nP, nI, dPI, spdP, spdI, dSPD};
        histo.push_back(val);
        maxV = minV = val;
    }
    
    void calc() {
        
        leapPoint p;
        for(int i = 0 ; i < histo.size() ; i++)
        {
            timePoint p = histo[i];
            
            minV.point.palm.x = min(minV.point.palm.x, p.point.palm.x);
            minV.point.palm.y = min(minV.point.palm.y, p.point.palm.y);
            minV.point.palm.z = min(minV.point.palm.z, p.point.palm.z);
            minV.point.index.x = min(minV.point.index.x, p.point.index.x);
            minV.point.index.y = min(minV.point.index.y, p.point.index.y);
            minV.point.index.z = min(minV.point.index.z, p.point.index.z);
            minV.normP = p.normP == 0 ? minV.normP : min(minV.normP, p.normP);
            minV.normI = p.normI == 0 ? minV.normI : min(minV.normI, p.normI);
            minV.dPI = p.dPI == 0 ? minV.dPI : min(minV.dPI, p.dPI);
            minV.spdP = p.spdP == 0 ? minV.spdP : min(minV.spdP, p.spdP);
            minV.spdI = p.spdI == 0 ? minV.spdI : min(minV.spdI, p.spdI);
            minV.dSPD = p.dSPD == 0 ? minV.dSPD : min(minV.dSPD, p.dSPD);

            maxV.point.palm.x = max(maxV.point.palm.x, p.point.palm.x);
            maxV.point.palm.y = max(maxV.point.palm.y, p.point.palm.y);
            maxV.point.palm.z = max(maxV.point.palm.z, p.point.palm.z);
            maxV.point.index.x = max(maxV.point.index.x, p.point.index.x);
            maxV.point.index.y = max(maxV.point.index.y, p.point.index.y);
            maxV.point.index.z = max(maxV.point.index.z, p.point.index.z);
            maxV.normP = p.normP == 0 ? maxV.normP : max(maxV.normP, p.normP);
            maxV.normI = p.normI == 0 ? maxV.normI : max(maxV.normI, p.normI);
            maxV.dPI = p.dPI == 0 ? maxV.dPI : max(maxV.dPI, p.dPI);
            maxV.spdP = p.spdP == 0 ? maxV.spdP : max(maxV.spdP, p.spdP);
            maxV.spdI = p.spdI == 0 ? maxV.spdI : max(maxV.spdI, p.spdI);
            maxV.dSPD = p.dSPD == 0 ? maxV.dSPD : max(maxV.dSPD, p.dSPD);
        }
        
        hasData = (minV.point != leapZero() || maxV.point != leapZero());
        mean();
        median();
        /*ofLog() << "Calc graph " << name << " data(" << hasData << ")";
        if(hasData)
        {
            ofLog() << "min(" << toString(minV, 2) << ")";
            ofLog() << "max(" << toString(maxV, 2) << ")";
            ofLog() << "mean(" << toString(meanV,2) << ")";
            ofLog() << "median(" << toString(medianV,2) << ")";
        }//*/
    }
    
    void draw(int x, int y, int w, int h, float start, float end, int mode, ofColor c, ofTrueTypeFont f, bool bg) {
        
        float min, max;
        
        switch(mode) {
            default:
                min = -120;
                max = 120;
                break;
            case GRAPH_N:
                min = 0;
                max = 150;
                break;
            case GRAPH_PI:
                min = 0;
                max = 40;
                break;
            case GRAPH_SPD:
            case GRAPH_DSPD:
                min = 0;
                max = 10;
                break;
        }
        
        if(bg)
        {
            // AXES
            ofSetColor(255,255,255,255);
            ofFill();
            ofDrawRectangle(x, y, w, h);
            
            ofSetColor(50,50,50,255);
            
            glBegin(GL_LINES);
            glVertex2f(x, ofMap(0., min, max, y+h, y));
            glVertex2f(x+w, ofMap(0., min, max, y+h, y));
            glEnd();//*/
            
            // HISTO
            
            f.drawString(getModeName(mode), x+10, y+20);
        }
        
        ofSetColor(c);
        int padx = 200;
        f.drawString("min:"+ofToString(getValue(minV,mode),2), x+padx, bg ? y+20 : y+40);
        f.drawString("max:"+ofToString(getValue(maxV,mode),2), x+padx+100, bg ? y+20 : y+40);
        f.drawString("mean:"+ofToString(getValue(meanV,mode),2), x+padx+200, bg ? y+20 : y+40);
        //f.drawString("median:"+ofToString(getValue(medianV,mode),2), x+padx+300, bg ? y+20 : y+40);

        f.drawString(name, x+w-130, bg ? y+20 : y+40);
        f.drawString(hasData ? "" : "(no data)", x+w-75, bg ? y+20 : y+40);
        
        if(!hasData)
            return;
        
        ofEnableSmoothing();
        
        ofSetLineWidth(2);
        glBegin(GL_LINE_STRIP);
        float v, curV;
        int t, curT, prevT;
        leapPoint p;
        timePoint val;
        bool first = true;
        ofPoint prevPt;
        ofPoint curPt;
        int startTime = start*maxTime;
        int endTime = end*maxTime;
        for(int i = 0 ; i < histo.size() ; i++) {
            t = histo[i].time;
            if(t >= startTime && t <= endTime)
            {
                v = getValue(histo[i], mode);
                
                curT = ofMap(t, startTime, endTime, x, x+w);
                curV = ofMap(v, min, max, y+h, y);
                curPt = finger ? histo[i].point.index : histo[i].point.palm;
                if(i==0)
                    prevPt = curPt;
                
                //ofLog() << "prev:(" << prevPt << ") cur:(" << curPt << ") dT:" << (t - prevT);
                
                if(curPt.x != prevPt.x || curPt.y != prevPt.y || curPt.z != prevPt.z)
                {
                    if(first)
                    {
                        first = false;
                        glBegin(GL_LINE_STRIP);
                    }
                    glVertex2f(curT, curV);
                    prevT = t;
                } else {
                    if(!first)
                    {
                        if(t - prevT > 100)
                        {
                            glEnd();
                            first = true;
                        } else {
                            glVertex2f(curT, curV);
                        }
                    }
                }
                prevPt = curPt;
            }
        }
        glEnd();
        
        /*
        ofSetColor(ofColor::lightPink);
        ofSetLineWidth(1);
        glBegin(GL_LINE_STRIP);
        for(int i = 0 ; i < histo.size() ; i++) {
            t = histo[i].time;
            v = getValue(histo[i], mode);
            
            curT = ofMap(t, 0, maxTime, x, x+w);
            curV = ofMap(v, min, max, y+h, y);
            
            glVertex2f(curT, curV);

        }
        glEnd();//*/
        
        ofSetLineWidth(1);
        ofFill();
    }
    
    void setValue(timePoint p, int mode, float v) {
        switch(mode) {
            default:
            case GRAPH_X:
                if(finger)
                    p.point.index.x = v;
                else
                    p.point.palm.x = v;
            case GRAPH_Y:
                if(finger)
                    p.point.index.y = v;
                else
                    p.point.palm.y = v;
            case GRAPH_Z:
                if(finger)
                    p.point.index.z = v;
                else
                    p.point.palm.z = v;
            case GRAPH_N:
                if(finger)
                    p.normI = v;
                else
                    p.normP = v;
            case GRAPH_PI:
                p.dPI = v;
                break;
            case GRAPH_SPD:
                if(finger)
                    p.spdI = v;
                else
                    p.spdP = v;
                break;
            case GRAPH_DSPD:
                p.dSPD = v;
                break;
        }
    }
    
    float getValue(timePoint tp, int mode) {
        switch(mode) {
            default:
            case GRAPH_X:
                return finger ? tp.point.index.x : tp.point.palm.x;
            case GRAPH_Y:
                return finger ? tp.point.index.y : tp.point.palm.y;
            case GRAPH_Z:
                return finger ? tp.point.index.z : tp.point.palm.z;
            case GRAPH_N:
                return finger ? tp.normI : tp.normP;
            case GRAPH_PI:
                return tp.dPI;
            case GRAPH_SPD:
                return finger ? tp.spdI : tp.spdP;
            case GRAPH_DSPD:
                return tp.dSPD;
        }
    }
    
    string getModeName(int mode) {
        string s = finger ? "Index " : "Palm ";
        switch(mode) {
			default:
            case GRAPH_X:
                return s + "X";
            case GRAPH_Y:
                return s + "Y";
            case GRAPH_Z:
                return s + "Z";
            case GRAPH_N:
                return s + "Norm";
            case GRAPH_PI:
                return "Distance(palm,index)";
            case GRAPH_SPD:
                return s + "Speed";
            case GRAPH_DSPD:
                return "SpdDiff(palm,index)";
        }
    }
    
    void median()
    {
        size_t size = histo.size();
        if (size == 0)
        {
            meanV.point = leapZero();
            return;
        }
        
        for(int mode = 0 ; mode <= MAX_GRAPH ; mode++)
        {
            vector<float> vals;
            for(int i = 0 ; i < size ; i++)
                vals.push_back(getValue(histo[i],mode));
            
            setValue(medianV, mode, median(vals));
        }
    }
    
    /*
    template <typename It>
    typename std::iterator_traits<It>::value_type Median(It begin, It end)
    {
        auto size = std::distance(begin, end);
        std::nth_element(begin, begin + size / 2, end);
        return *std::next(begin, size / 2);
    }//*/
    
    float median(vector<float> v)
    {
        vector<float> v2 = v;
        size_t size = v2.size();
        double median;
        
        sort(v2.begin(), v2.end());
            
        if (size  % 2 == 0)
        {
            median = (v2[size / 2 - 1] + v2[size / 2]) / 2;
        } else  {
            median = v2[size / 2];
        }
            
        return median;
    }//*/
    
    void mean() {
        typedef vector<int>::size_type vec_sz;
        
        vec_sz size = histo.size();
        if (size == 0)
        {
            meanV.point = leapZero();
            return;
        }
        leapPoint sum;
        float normPsum, normIsum, dPIsum, spdPsum, spdIsum, dSPDsum;
		normPsum = 0;
		dPIsum = 0;
		spdPsum = 0;
		spdIsum = 0;
		dSPDsum = 0;
		normIsum = 0;
        for(int i = 0 ; i < size ; i++)
        {
            sum += histo[i].point;
            normPsum += histo[i].normP;
            normIsum += histo[i].normI;
            dPIsum += histo[i].dPI;
            spdPsum += histo[i].spdP;
            spdIsum += histo[i].spdI;
            dSPDsum += histo[i].dSPD;
        }
        
        meanV.point = sum / size;
        meanV.normP = normPsum / size;
        meanV.normI = normIsum / size;
        meanV.dPI = dPIsum / size;
        meanV.spdP = spdPsum / size;
        meanV.spdI = spdIsum / size;
        meanV.dSPD = dSPDsum / size;
    }
    
    string toString(timePoint tp, int p) {
        return tp.point.toString(p) + ", " + ofToString(tp.normP, p) + ", " + ofToString(tp.normI, p) + ", " + ofToString(tp.dPI, p) + ", " + ofToString(tp.spdP, p) + ", " + ofToString(tp.spdI, p);
    }
    
    float distance(ofPoint palm, ofPoint index) {
        ofPoint d = palm - index;
        d.x = abs(d.x);
        d.y = abs(d.y);
        d.z = abs(d.z);
        return d.length();
    }
    
    float speed(timePoint p1, timePoint p2, bool finger) {
        if (p1.time == p2.time)
            return 0;
        if(finger) {
            if (p1.point.index == p2.point.index)
                return 0;
            return (p2.point.index - p1.point.index).length() / (p2.time - p1.time);
        } else {
            if (p1.point.palm == p2.point.palm)
                return 0;
            return (p2.point.palm - p1.point.palm).length() / (p2.time - p1.time);
        }
    }
};

#endif /* graph_h */
