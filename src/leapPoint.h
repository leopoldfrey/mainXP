//
//  leapPoint.h
//  Main
//
//  Created by Léopold Frey on 30/03/2017.
//
//

#ifndef leapPoint_h
#define leapPoint_h

class leapPoint {
public:
    ofPoint palm;
    ofPoint index;

    leapPoint() {}
    
    leapPoint(ofPoint p, ofPoint i) {
        palm = p;
        index = i;
    }
    
    leapPoint(leapPoint const &p) {
        palm = ofPoint(p.palm);
        index = ofPoint(p.index);
    }
    
    ~leapPoint() {}
    
    bool operator==( const leapPoint& p ) const {
        return palm == p.palm && index == p.index;
    }
    
    bool operator!=( const leapPoint& p ) const {
        return palm != p.palm || index != p.index;
    }
    
    leapPoint  operator+( const leapPoint& p ) const {
        return leapPoint(palm + p.palm, index + p.index);
    }
    
    leapPoint  operator+( const float f ) const {
        return leapPoint(palm + f, index + f);
    }
    
    leapPoint& operator+=( const leapPoint& p ) {
        palm += p.palm;
        index += p.index;
        return *this;
    }
    
    leapPoint& operator+=( const float f ) {
        palm += f;
        index += f;
        return *this;
    }
    
    leapPoint  operator-( const leapPoint& p ) const {
        return leapPoint(palm - p.palm, index - p.index);
    }
    
    leapPoint  operator-( const float f ) const {
        return leapPoint(palm - f, index - f);
    }
    
    leapPoint  operator-() const;
    leapPoint& operator-=( const leapPoint& p );
    leapPoint& operator-=( const float f );
    leapPoint  operator*( const leapPoint& p ) const  {
        return leapPoint(palm * p.palm, index * p.index);
    }
    
    leapPoint  operator*( const float f ) const {
        return leapPoint(palm * f, index * f);
    }

    leapPoint& operator*=( const leapPoint& p );
    leapPoint& operator*=( const float f );
    leapPoint  operator/( const leapPoint& p ) const {
        return leapPoint(palm / p.palm, index / p.index);
    }
    
    leapPoint  operator/( const float f ) const {
        return leapPoint(palm / f, index / f);
    }
    
    leapPoint& operator/=( const leapPoint& p );
    leapPoint& operator/=( const float f );

    string toString(int precision) {
        return
        ofToString(palm.x,precision) + ", " +
        ofToString(palm.y,precision) + ", " +
        ofToString(palm.z,precision) + ", " +
        ofToString(index.x,precision) + ", " +
        ofToString(index.y,precision) + ", " +
        ofToString(index.z,precision);
    }
    
    float norm(bool finger)
    {
        return finger ? palm.length() : index.length();
    }
};

static leapPoint leapZero() {
    return leapPoint(ofPoint(0,0,0), ofPoint(0,0,0));
}
#endif /* leapPoint_h */
