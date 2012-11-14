// main.cpp
//
// Starter code for CS 148 Assignment 3.
//

//
// Include libst headers for using OpenGL and GLUT.
//
#include "st.h"
#include "stgl.h"
#include "stglut.h"

#include <assert.h>
//
// Include headers for UI clases.
//

//
// Include header for parsing config and line editor files
//

#include <list>

using namespace std;

//
// Globals used by this application.
// As a rule, globals are Evil, but this is a small application
// and the design of GLUT makes it hard to avoid them.
//

// Window size so we can properly update the UI.
static int gWindowSizeX = 0;
static int gWindowSizeY = 0;

static STImage * g_original = NULL;
static STImage * g_xformquantized = NULL;
static STImage * g_reconstructed = NULL;
static STImage * g_difference = NULL;


void haar1d_forward(const STVector3 * in, STVector3 * out, int N, int stride)
{
    for(int i = 0; i < N/2; i++)
    {
        // smooth
        out[i*stride] = (in[i*2*stride] + in[i*2*stride+stride])/2.0;
        // detail
        out[N/2*stride+i*stride] = (in[i*2*stride] - in[i*2*stride+stride])/2.0 + 0.5;
//        out[i*2] = in[i*2];
//        out[i*2+1] = in[i*2+1];
    }
}


void haar_forward(const STImage * in, STImage * out)
{
    int size = in->GetWidth()*in->GetHeight();
    
    STVector3 * buf1 = new STVector3[size];
    STVector3 * buf2 = new STVector3[size];
    
    for(int y = 0; y < in->GetHeight(); y++)
    {
        for(int x = 0; x < in->GetWidth(); x++)
        {
            STColor4ub c = in->GetPixel(x, y);
            buf1[y*in->GetWidth()+x] = STVector3(c.r/255.0f, c.g/255.0f, c.b/255.0f);
        }
    }
    
    for(int N = in->GetWidth(); N > in->GetWidth()/2; N /= 2)
    {
        for(int i = 0; i < N; i++)
            haar1d_forward(buf1+i*N, buf2+i*N, N, 1);
        memcpy(buf1, buf2, N*N);
        
//        for(int i = 0; i < N; i++)
//            haar1d_forward(buf1+i, buf2+i, N, N);
//        memcpy(buf1, buf2, N*N);
    }
    
    for(int y = 0; y < in->GetHeight(); y++)
    {
        for(int x = 0; x < in->GetWidth(); x++)
        {
            STVector3 v = buf1[y*in->GetWidth()+x];
            out->SetPixel(x, y, STColor4ub(v.x*255, v.y*255, v.z*255, 255));
        }
    }
    
    delete[] buf1;
    delete[] buf2;
}


//
// Display the UI, including all widgets.
//
void DisplayCallback()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Clear the window.
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw background
    glEnable(GL_TEXTURE_2D);

    glTranslatef(0, gWindowSizeY/2, 0);
    g_original->Draw();
    
    glTranslatef(gWindowSizeX/2, 0, 0);
    g_xformquantized->Draw();
    
    glTranslatef(-gWindowSizeX/2, -gWindowSizeY/2, 0);
    g_reconstructed->Draw();
    
    glTranslatef(gWindowSizeX/2, 0, 0);
    g_difference->Draw();
    
    // Loop through all the widgets in the user
    // interface and tell each to display itself.

    glutSwapBuffers();
}

//
// Reshape the window and record the size so
// that we can use it in the mouse callbacks.
//
void ReshapeCallback(int w, int h)
{
    gWindowSizeX = w;
    gWindowSizeY = h;

    glViewport(0, 0, gWindowSizeX, gWindowSizeY);

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho(0, gWindowSizeX, 0, gWindowSizeY, -1., 1.);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


void MouseCallback(int button, int state, int x, int y)
{
    STPoint2 position = STPoint2(x, gWindowSizeY - y);
    bool redisplay = false;
    
    if(redisplay)
        glutPostRedisplay();
}


void MotionCallback(int x, int y)
{
    STPoint2 position = STPoint2(x, gWindowSizeY - y);
    bool redisplay = false;
    
    if(redisplay)
        glutPostRedisplay();
}


void PassiveMotionCallback(int x, int y)
{
    STPoint2 position = STPoint2(x, gWindowSizeY - y);
    bool redisplay = false;
    
    if(redisplay)
        glutPostRedisplay();
}


//
// Initialize the application, loading resources,
// setting state, and creating widgets.
//
void Initialize()
{
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_SMOOTH);
}

int main(int argc, char** argv)
{
    if(argc == 2)
        g_original = new STImage(argv[1]);
    else
        g_original = new STImage("assignment7images/MonaLisa.png");
    
    gWindowSizeX = g_original->GetWidth() * 2;
    gWindowSizeY = g_original->GetHeight() * 2;
    
    assert(gWindowSizeX == gWindowSizeY && log2f(gWindowSizeX)==floorf(log2f(gWindowSizeX)));
    
    g_xformquantized = new STImage(g_original->GetWidth(), g_original->GetHeight());
    g_reconstructed = g_original;
    g_difference = g_original;
    
    haar_forward(g_original, g_xformquantized);

    //
    // Initialize GLUT.
    //
    glutInit(&argc, argv);
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(20, 20);
    glutInitWindowSize(gWindowSizeX, gWindowSizeY);
    glutCreateWindow("wavelet");
    
    //
    // Initialize the UI.
    //
    Initialize();

    //
    // Register GLUT callbacks and enter main loop.
    //
    glutDisplayFunc(DisplayCallback);
    glutReshapeFunc(ReshapeCallback);

    glutMouseFunc(MouseCallback);
    glutMotionFunc(MotionCallback);
    glutPassiveMotionFunc(PassiveMotionCallback);

    glutMainLoop();

    //
    // Cleanup code should be called here.
    //
    
    return 0;
}
