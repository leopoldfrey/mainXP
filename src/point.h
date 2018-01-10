#ifndef point_h
#define point_h

#define MAG(X,Y,Z) sqrt((X * X) + (Y * Y) + (Z * Z));

class point {
public:
    ofVec3f pos;
    ofVec3f vel;
    float life;
    float decay;
    float snd;
    ofFloatColor color;
    
    point() {}
    
    point(ofVec3f p, ofVec3f v, float s, ofFloatColor c) {
        pos = p;
        vel = v;
        snd = s;
        color = c;
        float vmag = MAG(vel.x, vel.y, vel.z);
        //decay ranges from [0.93, 0.97], the faster the link, the faster the decay
        float dec = MAX(MIN(50, 1/vmag), 4);
        decay = (dec-4)/(50-4)*0.2+0.77;
        life = 1.;
        decay = dec;
    }
    
    point(ofVec3f p) {
        pos = p;
    }
    
    point(ofVec3f p, ofVec3f v, ofFloatColor col, float s) {
        pos = p;
        vel = v;
        float vmag = MAG(vel.x, vel.y, vel.z);
        //decay ranges from [0.93, 0.97], the faster the link, the faster the decay
        float dec = MAX(MIN(50, 1/vmag), 4);
        decay = (dec-4)/(50-4)*0.2+0.77;
        life = 1.;
        color = col;
        snd = s;
    }
    
    void smooth(point prev, float smooth) {
        pos.x = smooth*pos.x + (1.0-smooth)*prev.pos.x;
        pos.y = smooth*pos.y + (1.0-smooth)*prev.pos.y;
        pos.z = smooth*pos.z + (1.0-smooth)*prev.pos.z;
    }
    
    ~point() {}
    
};


#endif /* point_h */
