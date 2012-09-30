//
//  Geometry.h
//  Mood Globe
//
//  Created by Spencer Salazar on 6/26/12.
//  Copyright (c) 2012 Stanford University. All rights reserved.
//

#ifndef Mood_Globe_Geometry_h
#define Mood_Globe_Geometry_h

#import <math.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

struct GLvertex2f;

struct GLvertex3f
{
    GLfloat x;
    GLfloat y;
    GLfloat z;
    
    GLvertex3f()
    {
        x = y = z = 0;
    }
    
    GLvertex3f(const GLvertex2f &v);
    
    GLvertex3f(GLfloat x, GLfloat y, GLfloat z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }
    
    GLfloat magnitude() { return sqrtf(x*x+y*y+z*z); }
    GLfloat magnitudeSquared() { return x*x+y*y+z*z; }
    
    GLvertex2f toLatLong();
} __attribute__((packed));

GLvertex3f operator+(const GLvertex3f &v1, const GLvertex3f &v2);
GLvertex3f operator-(const GLvertex3f &v1, const GLvertex3f &v2);
GLvertex3f operator*(const GLvertex3f &v, const GLfloat &s);
GLvertex3f operator/(const GLvertex3f &v, const GLfloat &s);

struct GLcolor4f
{
    union
    {
        GLfloat r;
        GLfloat h;
    };
    
    union
    {
        GLfloat g;
        GLfloat s;
    };
    
    union
    {
        GLfloat b;
        GLfloat v;
    };
    
    GLfloat a;
    
    
    GLcolor4f()
    {
        r = g = b = a = 1;
    }
    
    GLcolor4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
    {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }
    
} __attribute__((packed));


GLcolor4f operator+(const GLcolor4f &v1, const GLcolor4f &v2);
GLcolor4f operator-(const GLcolor4f &v1, const GLcolor4f &v2);
GLcolor4f operator*(const GLcolor4f &v, const GLfloat &s);
GLcolor4f operator/(const GLcolor4f &v, const GLfloat &s);


struct GLvertex2f
{
    GLfloat x;
    GLfloat y;
    
    GLvertex2f()
    {
        x = y = 0;
    }
    
    GLvertex2f(GLfloat x, GLfloat y)
    {
        this->x = x;
        this->y = y;
    }
    
    GLvertex2f(const GLvertex2f &v)
    {
        this->x = v.x;
        this->y = v.y;
    }
    
    GLvertex3f operator()()
    {
        return GLvertex3f(this->x, this->y, 0);
    }

    GLfloat magnitude() { return sqrtf(x*x+y*y); }
    GLfloat magnitudeSquared() { return x*x+y*y; }
    GLfloat angle() { return atan2f(y, x); }
    
    static GLvertex2f fromPolar(GLfloat r, GLfloat theta)
    {
        return GLvertex2f(r*cosf(theta), r*sinf(theta));
    }

} __attribute__((packed));

GLvertex2f operator+(const GLvertex2f &v1, const GLvertex2f &v2);
GLvertex2f operator-(const GLvertex2f &v1, const GLvertex2f &v2);
GLvertex2f operator*(const GLvertex2f &v, const GLfloat &s);
GLvertex2f operator/(const GLvertex2f &v, const GLfloat &s);


// geometry primitve, i.e. vertex/normal/color/uv
struct GLgeoprimf
{
//    GLgeoprimf()
//    {
//        vertex = GLvertex3f();
//        normal = GLvertex3f(0, 0, -1);
//        texcoord = GLvertex2f();
//        color = GLcolor4f();
//    }
    
    GLvertex3f vertex;
    GLvertex3f normal;
    GLvertex2f texcoord;
    GLcolor4f color;
} __attribute__((packed));

// triangle primitive -- 3 vertex primitives
struct GLtrif
{
//    GLtrif()
//    {
//        a = GLgeoprimf();
//        b = GLgeoprimf();
//        c = GLgeoprimf();
//    }
    
    GLgeoprimf a;
    GLgeoprimf b;
    GLgeoprimf c;
} __attribute__((packed));


class Geometry
{
public:
    inline static void makeSquare(GLtrif &t1, GLtrif &t2, float width)
    {
        float width_2 = width/2.0;
        
        t1.a.vertex = GLvertex3f(-width_2, -width_2, 0);
        t1.b.vertex = GLvertex3f( width_2, -width_2, 0);
        t1.c.vertex = GLvertex3f(-width_2,  width_2, 0);
        
        t2.a.vertex = GLvertex3f( width_2, -width_2, 0);
        t2.b.vertex = GLvertex3f(-width_2,  width_2, 0);
        t2.c.vertex = GLvertex3f( width_2,  width_2, 0);
        
        t1.a.texcoord = GLvertex2f(0, 0);
        t1.b.texcoord = GLvertex2f(1, 0);
        t1.c.texcoord = GLvertex2f(0, 1);
        
        t2.a.texcoord = GLvertex2f(1, 0);
        t2.b.texcoord = GLvertex2f(0, 1);
        t2.c.texcoord = GLvertex2f(1, 1);
        
        t1.a.color = GLcolor4f(1, 1, 1, 1);
        t1.b.color = GLcolor4f(1, 1, 1, 1);
        t1.c.color = GLcolor4f(1, 1, 1, 1);
        
        t2.a.color = GLcolor4f(1, 1, 1, 1);
        t2.b.color = GLcolor4f(1, 1, 1, 1);
        t2.c.color = GLcolor4f(1, 1, 1, 1);
    }
};

#endif
