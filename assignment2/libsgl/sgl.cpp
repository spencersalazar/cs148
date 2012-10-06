
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


int shadow( Line l ) {
    return (l.a>0) || (l.a == 0 && l.b > 0);
}

int inside( float e, Line l ) {
    return (e == 0) ? !shadow(l) : (e < 0);
}

void makeline( STPoint2& v0, STPoint2& v1, Line& l )
{
    l.a = v1.y - v0.y;
    l.b = v0.x - v1.x;
    l.c = -(l.a * v0.x + l.b * v0.y);
}

void drawOne(Vertex a, Vertex b, Vertex c)
{
    Line l0, l1, l2;
    makeline(a.p, b.p, l2);
    makeline(b.p, c.p, l0);
    makeline(c.p, a.p, l1);
    
    int xmin = 0, xmax = buffer_width;
    int ymin = 0, ymax = buffer_height;
    
    for(int x = xmin; x < xmax; x++)
    {
        for(int y = ymin; y < ymax; y++)
        {
            float e0 = l0.a * x + l0.b * y + l0.c;
            float e1 = l1.a * x + l1.b * y + l1.c;
            float e2 = l2.a * x + l2.b * y + l2.c;
            if(( inside(e0,l0) && inside(e1,l1) && inside(e2,l2) ) ||
               ( !inside(e0,l0) && !inside(e1,l1) && !inside(e2,l2) ))
                img->SetPixel(x, y, STColor4ub(a.c));
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
