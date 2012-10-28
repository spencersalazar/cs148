#include <stdlib.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "sgl.h"
#include "st.h"

using namespace std;

STImage* buff;

int grot = 0;
//int arot;
//int brot;


void drawThing(STColor4f a, STColor4f b)
{
//    sglBeginTriangles();
//    
//    sglVertex(-50, -50);
//    sglVertex( 50, -50);
//    
//    sglColor(c.r, c.g, c.b);
//    
//    sglVertex(-50,  50);
//    sglVertex( 50,  50);
//    
//    sglEnd();
    
    const int TRIS = 20;
    const float RADIUS = 50;
    
    for(int i = 0; i < TRIS; i++)
    {
        float theta = ((float) i)/TRIS*2*M_PI;
        float theta_plus_one = ((float) i+1)/TRIS*2*M_PI;
        
        sglBeginTriangles();
        
        float alpha;
        
        alpha = 0.5;
        sglColor(a.r*(1-alpha) + b.r*alpha,
                 a.g*(1-alpha) + b.g*alpha,
                 a.b*(1-alpha) + b.b*alpha);
        sglVertex(0, 0);
        
        alpha = 0.5+0.5*sinf(theta);
        sglColor(a.r*(1-alpha) + b.r*alpha,
                 a.g*(1-alpha) + b.g*alpha,
                 a.b*(1-alpha) + b.b*alpha);
        sglVertex(RADIUS*cosf(theta), RADIUS*sinf(theta));
        
        alpha = 0.5+0.5*sinf(theta_plus_one);
        sglColor(a.r*(1-alpha) + b.r*alpha,
                 a.g*(1-alpha) + b.g*alpha,
                 a.b*(1-alpha) + b.b*alpha);
        sglVertex(RADIUS*cosf(theta_plus_one), RADIUS*sinf(theta_plus_one));
        
        sglEnd();
    }
}


void display( void )
{
	glClear( GL_COLOR_BUFFER_BIT );
    int width = buff->GetWidth();
    int height = buff->GetHeight();
    for(int i = 0; i < width; i++)
    {
        for(int j = 0; j < height; j++)
        {
            buff->SetPixel(i, j, STColor4ub(0, 0, 0, 1));
        }
    }
    
	// --- Make drawing calls here ---+

    sglLoadIdentity();
    
//    sglRotate(grot++);
//    sglTranslate(200, 200);
//    sglRotate(arot++);
//    sglScale(0.5, 0.5);
//    
//    sglBeginTriangles();
//    
//    sglColor(.8f, .1f, 0.f);
//    sglVertex(-100, -100);
//    sglVertex( 100, -100);
//    
//    sglColor(0.0f, .1f, 0.8f);
//    sglVertex(-100,  100);
//    sglVertex( 100,  100);
    
    const int NUM = 21;
    
    sglLoadIdentity();
    sglTranslate(buff->GetWidth()/2.0, buff->GetHeight()/2.0);
    sglRotate(grot++);
    sglScale(0.5, 0.5);
    sglColor(1, 1, 1);
    drawThing(STColor4f(1, 1, 1), STColor4f(1, 1, 1));

    for(int j = 0; j < 4; j++)
    {
        float xtrans, ytrans;
        if(j == 0 || j ==2) xtrans = 75;
        else xtrans = -75;
        if(j == 0 || j ==1) ytrans = 75;
        else ytrans = -75;
        
        float scale = 0.8;
        STColor4f lastColor(1, 1, 1);
        STColor4f newColor;
        
        sglPushMatrix();
        
        for(int i = 0; i < NUM; i++)
        {
            float frac = i/(float)NUM;
            int mod = i%3;
            
            sglColor(lastColor.r, lastColor.g, lastColor.b);
            
            if(i < 2*NUM/3)
                sglScale(scale, scale);
            else
                sglScale(1.0/scale, 1.0/scale);
            sglTranslate(xtrans, ytrans);
            sglRotate(8*cosf((j+1)*3*grot/180.0*M_PI+j));
            
            newColor = STColor4f(((mod == 0)*frac + (1-frac)*(mod == 1))*(1-frac),
                                 ((mod == 1)*frac + (1-frac)*(mod == 2))*(1-frac),
                                 ((mod == 2)*frac + (1-frac)*(mod == 0))*(1-frac),
                                 1);
            
            drawThing(lastColor, newColor);
            
            lastColor = newColor;
        }
        
        sglPopMatrix();
    }
        
    sglEnd();
    
	// --- End of drawing calls ------+

	buff->Draw();
	glutSwapBuffers();
}

void reshape( int w, int h )
{
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    
    glOrtho( 0., w, 0., h, -1., 1. );
    glViewport( 0, 0, w, h );
    setBufferSize(w, h);
    delete buff;
	buff = new STImage(w, h, STColor4ub(0, 0, 0, 255));
	setBuffer(buff);
    
    glutPostRedisplay();
}

void keyboard( unsigned char key, int x, int y )
{
	switch(key)
	{
	case 27: // Escape key
		exit(0);
		break;
	case 's': // Save
		buff->Save("output.png");
		break;
	}
}

void timer(int val)
{
    glutPostRedisplay();
    glutTimerFunc(1000/30, timer, 0);
}

int main (int argc, char *argv[])
{
	int win_width = 512;
	int win_height = 512;
	buff = new STImage(win_width, win_height, STColor4ub(0, 0, 0, 255));
	setBuffer(buff);

	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE );
	glutInitWindowSize( win_width, win_height );

	glutCreateWindow( "Intro Graphics Assignment 2" );

	glutDisplayFunc( display );
    glutTimerFunc(1000/30, timer, 0);
	glutReshapeFunc( reshape );
	glutKeyboardFunc( keyboard );

	glutMainLoop();

	delete buff;
}
