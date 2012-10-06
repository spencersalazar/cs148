
#include "sgl.h"
#include "STImage.h"
#include <list>

using namespace std;
// --- Do not modify this code ---+
#define IMPLEMENT_THIS_FUNCTION printf("Warning: call to unimplimented function!\n")
STImage* img;
int buffer_width;
int buffer_height;
void setBuffer(STImage* ptr) { img = ptr; }
void setBufferSize(int w, int h) { buffer_width = w; buffer_height = h; }
// --- End of do not modify this code ---+


struct Vertex
{
    STPoint2 p;
    STColor4f c;
};

list<Vertex> tris;
STColor4f color;
STTransform3 xform;
list<STTransform3> xformStack;

struct Line
{
    float a, b, c;
};


int shadow(const Line &l)
{
    return (l.a>0) || (l.a == 0 && l.b > 0);
}

int inside(const float e, const Line &l)
{
    return (e == 0) ? !shadow(l) : (e < 0);
}

void makeline(const STPoint2& v0, const STPoint2& v1, Line& l)
{
    l.a = v1.y - v0.y;
    l.b = v0.x - v1.x;
    l.c = -(l.a * v0.x + l.b * v0.y);
}

void interpolate(const STPoint2 &a, const STPoint2 &b, const STPoint2 &c,
                 const float x, const float y,
                 float &a_a, float &a_b, float &a_c)
{
    float T[2][2] =
    {
        { a.x-c.x, b.x-c.x },
        { a.y-c.y, b.y-c.y },
    };
    
    float det_T_1 = 1.0f/(T[0][0]*T[1][1] - T[0][1]*T[1][0]);
    float T_1[2][2] =
    {
        {  T[1][1]*det_T_1, -T[0][1]*det_T_1 },
        { -T[1][0]*det_T_1,  T[0][0]*det_T_1 }
    };
    
    STPoint2 rr3 = STPoint2(x-c.x, y-c.y);
    
    a_a = T_1[0][0]*rr3.x + T_1[0][1]*rr3.y;
    a_b = T_1[1][0]*rr3.x + T_1[1][1]*rr3.y;
    a_c = 1-a_a-a_b;
}

void drawOne(const Vertex &a, const Vertex &b, const Vertex &c)
{
    Line l0, l1, l2;
    makeline(a.p, b.p, l2);
    makeline(b.p, c.p, l0);
    makeline(c.p, a.p, l1);
    
    // bounding box
    int xmin, xmax;
    int ymin, ymax;

    xmin = ceil(min(a.p.x, min(b.p.x, c.p.x)));
    xmax = ceil(max(a.p.x, max(b.p.x, c.p.x)));
    ymin = ceil(min(a.p.y, min(b.p.y, c.p.y)));
    ymax = ceil(max(a.p.y, max(b.p.y, c.p.y)));
    
    for(int x = xmin; x < xmax; x++)
    {
        for(int y = ymin; y < ymax; y++)
        {
            float e0 = l0.a * x + l0.b * y + l0.c;
            float e1 = l1.a * x + l1.b * y + l1.c;
            float e2 = l2.a * x + l2.b * y + l2.c;
            if((inside(e0,l0) && inside(e1,l1) && inside(e2,l2) ) ||
               (!inside(e0,l0) && !inside(e1,l1) && !inside(e2,l2)))
            {
                float a_a, a_b, a_c;
                interpolate(a.p, b.p, c.p, x, y, a_a, a_b, a_c);
                STColor4f color = a_a * a.c + a_b * b.c + a_c * c.c;
                img->SetPixel(x, y, STColor4ub(color));
            }
        }
    }
}

void renderTriangles()
{
    list<Vertex>::iterator v = tris.begin();
    
	Vertex a = *v++;
	Vertex b = *v++;
    
    while(v != tris.end())
    {
        Vertex c = *v++;
        
        drawOne(a, b, c);
        
        a = b;
        b = c;
    }
}

void sglBeginTriangles()
{
    tris.clear();
}

void sglEnd()
{
    renderTriangles();
    tris.clear();
}

void sglLoadIdentity()
{
	xform = STTransform3();
}

void sglScale(SGLfloat xscale, SGLfloat yscale)
{
	xform.scale(xscale, yscale);
}

void sglTranslate(SGLfloat xtrans, SGLfloat ytrans)
{
	xform.translate(xtrans, ytrans);
}

void sglRotate(SGLfloat angle)
{
	xform.rotate(angle);
}

void sglPushMatrix()
{
	xformStack.push_back(xform);
}

void sglPopMatrix()
{
	xform = *(xformStack.end()--);
    xformStack.pop_back();
}

void sglVertex(SGLfloat x, SGLfloat y)
{
    Vertex v;
    v.p = xform * STPoint2(x, y);
    v.c = color;
	tris.push_back(v);
}

void sglColor(SGLfloat r, SGLfloat g, SGLfloat b)
{
	color = STColor4f(r, g, b, 1);
}
