#ifndef chain_h
#define chain_h

#include "point.h"

class chain {
    
public:
    
    vector<point> points;
    
    void addPoint(point p, float smooth) {
        if(points.size() != 0)
            p.smooth(points[points.size()-1], 1.-smooth);
        points.push_back(p);
    }
    
    void clear() {
        points.clear();
    }
    
    int size() {
        return points.size();
    }
    
    point getPoint(int i) {
        return points[i];
    }
    
    void step(float dev, float dec) {
        for(int i = 0 ; i < points.size() ; i++)
        {
            points[i].pos = points[i].pos + points[i].vel * dev;
            points[i].vel = points[i].vel * 0.95;
            points[i].life = points[i].life*dec;
        }
        
        for(int i = points.size()-1 ; i > 0  ; i--)
        {
            if(points[i].life < 0.01)
                points.erase(points.begin()+i);
        }
    }
        
    chain() {
    }
    
    ~chain() {
        points.clear();
    }
};

#endif /* chain_h */
