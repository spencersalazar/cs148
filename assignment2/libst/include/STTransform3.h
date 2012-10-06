// STColor3f.h
#ifndef __STTRANSFORM3_H__
#define __STTRANSFORM3_H__

// Forward-declare libst types.
#include "stForward.h"
#include "STPoint2.h"
#include <assert.h>

STTransform3 operator*(const STTransform3 &A, const STTransform3 &B);

/**
*  Class representing a 3x3 matrix that can be used to
*  apply transforms to 2D points and vectors
*/
class STTransform3
{
	// Fill in your class definition here. STTransform3 should implement a 3x3 matrix that
	// you can use to modify 2D vertices with homogenous coordinates (hence the 3x3 size
	// rather than 2x2). This class doesn't need to be complicated. Other than basic 
	// accessor functions, you'll probably want to implement multiplication between transforms.
	// You have a lot of flexibility as to how you implement this class, but you will need to
	// follow the conventions used in the rest of libst.
public:
    inline STTransform3()
    {
        // init as identity
        
        m[0] = m[1] = m[2] = 0;
        m[3] = m[4] = m[5] = 0;
        m[6] = m[7] = m[8] = 0;
        
        m[0] = 1;
        m[4] = 1;
        m[8] = 1;
    }
    
    inline const float* operator[](size_t r) const
    {
        assert(r >= 0 && r < 3);
        return (m+r*3);
    }
    
    inline float* operator[](size_t r)
    {
        assert(r >= 0 && r < 3);
        return (m+r*3);
    }
    
    inline void scale(float x, float y)
    {
        STTransform3 s;
        s[0][0] = x;
        s[1][1] = y;
        
        *this = *this * s;
    }
    
    inline void rotate(float theta)
    {
        // convert degrees to radions
        theta = theta / 180.0f * M_PI;
        STTransform3 s;
        s[0][0] = cosf(theta);
        s[0][1] = -sinf(theta);
        s[1][0] = sinf(theta);
        s[1][1] = cosf(theta);
        
        *this = *this * s;
    }
    
    inline void translate(float x, float y)
    {
        STTransform3 s;
        s[0][2] = x;
        s[1][2] = y;
        
        *this = *this * s;
    }
    
    float m[9];
    // m[0] m[1] m[2]
    // m[3] m[4] m[5]
    // m[6] m[7] m[8]
    
    
protected:
};


inline STPoint2 operator*(const STTransform3 &A, const STPoint2 &p)
{
    STPoint2 p2;
    p2.x = p.x*A[0][0] + p.y*A[0][1] + 1*A[0][2];
    p2.y = p.x*A[1][0] + p.y*A[1][1] + 1*A[1][2];
    
    return p2;
}

inline STTransform3 operator*(const STTransform3 &A, const STTransform3 &B)
{
    STTransform3 C;
    
    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            C[i][j] = A[i][0]*B[0][j] + A[i][1]*B[1][j] + A[i][2]*B[2][j];
        }
    }
    
    return C;
}


#endif