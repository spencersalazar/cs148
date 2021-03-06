//
//  Geometry.cpp
//  Mood Globe
//
//  Created by Spencer Salazar on 6/26/12.
//  Copyright (c) 2012 Stanford University. All rights reserved.
//

#include "Geometry.h"
#include <stdio.h>


GLvertex3f::GLvertex3f(const GLvertex2f &v)
{
    x = v.x;
    y = v.y;
    z = 0;
}

GLvertex2f GLvertex3f::toLatLong()
{
    GLvertex2f ll;
    ll.x = (atan2f(y,x) / M_PI + 1.0f) * 0.5f;
    ll.y = asinf(z) / M_PI + 0.5f;
    
    return ll;
}

GLvertex3f operator+(const GLvertex3f &v1, const GLvertex3f &v2)
{
    GLvertex3f v3 = GLvertex3f(v1.x+v2.x, v1.y+v2.y, v1.z+v2.z);
    return v3;
}

GLvertex3f operator-(const GLvertex3f &v1, const GLvertex3f &v2)
{
    GLvertex3f v3 = GLvertex3f(v1.x-v2.x, v1.y-v2.y, v1.z-v2.z);
    return v3;
}

GLvertex3f operator*(const GLvertex3f &v, const GLfloat &s)
{
    GLvertex3f v2 = GLvertex3f(v.x*s, v.y*s, v.z*s);
    return v2;
}

GLvertex3f operator/(const GLvertex3f &v, const GLfloat &s)
{
    GLvertex3f v2 = GLvertex3f(v.x/s, v.y/s, v.z/s);
    return v2;
}

GLvertex2f operator+(const GLvertex2f &v1, const GLvertex2f &v2)
{
    GLvertex2f v3 = GLvertex2f(v1.x+v2.x, v1.y+v2.y);
    return v3;
}

GLvertex2f operator-(const GLvertex2f &v1, const GLvertex2f &v2)
{
    GLvertex2f v3 = GLvertex2f(v1.x-v2.x, v1.y-v2.y);
    return v3;
}

GLvertex2f operator*(const GLvertex2f &v, const GLfloat &s)
{
    GLvertex2f v2 = GLvertex2f(v.x*s, v.y*s);
    return v2;
}

GLvertex2f operator/(const GLvertex2f &v, const GLfloat &s)
{
    GLvertex2f v2 = GLvertex2f(v.x/s, v.y/s);
    return v2;
}


GLcolor4f operator+(const GLcolor4f &v1, const GLcolor4f &v2)
{
    GLcolor4f v3 = GLcolor4f(v1.r+v2.r, v1.g+v2.g, v1.b+v2.b, v1.a+v2.a);
    return v3;
}

GLcolor4f operator-(const GLcolor4f &v1, const GLcolor4f &v2)
{
    GLcolor4f v3 = GLcolor4f(v1.r-v2.r, v1.g-v2.g, v1.b-v2.b, v1.a-v2.a);
    return v3;
}

GLcolor4f operator*(const GLcolor4f &v, const GLfloat &s)
{
    return GLcolor4f(v.r*s, v.g*s, v.b*s, v.a*s);
}

GLcolor4f operator/(const GLcolor4f &v, const GLfloat &s)
{
    return GLcolor4f(v.r/s, v.g/s, v.b/s, v.a/s);
}

