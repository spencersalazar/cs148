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

float g_quality = 0.1;
float g_power = 0.9;
bool g_nonlinearize = false;
bool g_softclip = false;


inline float signum(const float &v)
{
    return v/fabsf(v);
}


STColor4ub quantize(STVector3 v, float level, float qfactor)
{
    if(level > 0)
    {
        v.x -= 0.5;
        v.y -= 0.5;
        v.z -= 0.5;

        v.x = v.x / (powf(level, powf(qfactor,3)));
        v.y = v.y / (powf(level, powf(qfactor,3)));
        v.z = v.z / (powf(level, powf(qfactor,3)));
        
        if(g_nonlinearize)
        {
            float pow = g_power;
            v.x = (v.x == 0) ? 0 : signum(v.x)*powf(fabsf(v.x), pow);
            v.y = (v.y == 0) ? 0 : signum(v.y)*powf(fabsf(v.y), pow);
            v.z = (v.z == 0) ? 0 : signum(v.z)*powf(fabsf(v.z), pow);
        }

        if(g_softclip)
        {
            v.x = v.x/(1.1-fabsf(v.x));
            v.y = v.y/(1.1-fabsf(v.y));
            v.z = v.z/(1.1-fabsf(v.z));
        }
        
        v.x += 0.5;
        v.y += 0.5;
        v.z += 0.5;

        if(v.x < 0) { v.x = 0; }
        if(v.x > 1) { v.x = 1; }
        if(v.y < 0) { v.y = 0; }
        if(v.y > 1) { v.y = 1; }
        if(v.z < 0) { v.z = 0; }
        if(v.z > 1) { v.z = 1; }
    }
    
    return STColor4ub(v.x*255, v.y*255, v.z*255, 255);
}


STColor4ub reverse_quantize(STVector3 v, float level, float qfactor)
{
    if(level > 0)
    {
        v.x -= 0.5;
        v.y -= 0.5;
        v.z -= 0.5;
        
        v.x = v.x * (powf(level, powf(qfactor,3)));
        v.y = v.y * (powf(level, powf(qfactor,3)));
        v.z = v.z * (powf(level, powf(qfactor,3)));
        
        if(g_nonlinearize)
        {
            float pow = 1.0/g_power;
            v.x = (v.x == 0) ? 0 : signum(v.x)*powf(fabsf(v.x), pow);
            v.y = (v.y == 0) ? 0 : signum(v.y)*powf(fabsf(v.y), pow);
            v.z = (v.z == 0) ? 0 : signum(v.z)*powf(fabsf(v.z), pow);
        }
        
//        v.x = v.x/(1-fabsf(v.x));
//        v.y = v.y/(1-fabsf(v.y));
//        v.z = v.z/(1-fabsf(v.z));
        
        v.x += 0.5;
        v.y += 0.5;
        v.z += 0.5;

        if(v.x < 0) { v.x = 0; }
        if(v.x > 1) { v.x = 1; }
        if(v.y < 0) { v.y = 0; }
        if(v.y > 1) { v.y = 1; }
        if(v.z < 0) { v.z = 0; }
        if(v.z > 1) { v.z = 1; }
    }

    return STColor4ub(v.x*255, v.y*255, v.z*255, 255);
}


void haar1d_forward(const STVector3 * in, STVector3 * out, int N, int stride)
{
    for(int i = 0; i < N/2; i++)
    {
        // smooth
        out[i*stride] = (in[i*2*stride] + in[i*2*stride+stride])/2.0;
        // detail
        out[N/2*stride+i*stride] = (in[i*2*stride] - in[i*2*stride+stride])/2.0 + STVector3(0.5, 0.5, 0.5);
    }
}


void haar_forward(const STImage * in, STImage * out)
{
    int width = in->GetWidth();
    int height = in->GetHeight();
    int size = width*height;
    
    STVector3 * buf1 = new STVector3[size];
    STVector3 * buf2 = new STVector3[size];
    
    for(int y = 0; y < height; y++)
    {
        for(int x = 0; x < width; x++)
        {
            STColor4ub c = in->GetPixel(x, y);
            buf1[y*width+x] = STVector3(c.r/255.0f, c.g/255.0f, c.b/255.0f);
        }
    }
    
    for(int N = width; N > 1; N /= 2)
    {
        // columns
        for(int i = 0; i < N; i++)
            haar1d_forward(buf1+i, buf2+i, N, width);
        for(int i = 0; i < N; i++)
            memcpy(buf1+i*width, buf2+i*width, N*sizeof(STVector3));
        
        // rows
        for(int i = 0; i < N/2; i++)
            haar1d_forward(buf1+i*width, buf2+i*width, N, 1);
        for(int i = 0; i < N/2; i++)
            memcpy(buf1+i*width, buf2+i*width, N*sizeof(STVector3));
    }
    
    for(int y = 0; y < height; y++)
    {
        for(int x = 0; x < width; x++)
        {
            int level = max(ceilf(log2f(x+1)), ceilf(log2f(y+1)));
            //float level = max((log2f(x+1)), (log2f(y+1)));
            out->SetPixel(x, y, quantize(buf1[y*width+x], level, g_quality));
        }
    }
    
//    out->SetPixel(0, 0, STColor4ub(0, 255, 127));
    
    delete[] buf1;
    delete[] buf2;
}


void haar1d_backward(const STVector3 * in, STVector3 * out, int N, int stride)
{
    for(int i = 0; i < N/2; i++)
    {
        // smooth
        out[i*2*stride] = in[i*stride] + (in[N/2*stride+i*stride] - STVector3(0.5, 0.5, 0.5));
        // detail
        out[i*2*stride+stride] = in[i*stride] - (in[N/2*stride+i*stride] - STVector3(0.5, 0.5, 0.5));
    }
}


void haar_backward(const STImage * in, STImage * out)
{
    int width = in->GetWidth();
    int height = in->GetHeight();
    int size = width*height;
    
    STVector3 * buf1 = new STVector3[size];
    STVector3 * buf2 = new STVector3[size];
    
    for(int y = 0; y < height; y++)
    {
        for(int x = 0; x < width; x++)
        {
            STColor4ub c = in->GetPixel(x, y);
            buf1[y*width+x] = STVector3(c.r/255.0f, c.g/255.0f, c.b/255.0f);
        }
    }
    
    for(int N = 2; N <= width; N *= 2)
    {
        // rows
        for(int i = 0; i < N/2; i++)
            haar1d_backward(buf1+i*width, buf2+i*width, N, 1);
        for(int i = 0; i < N/2; i++)
            memcpy(buf1+i*width, buf2+i*width, N*sizeof(STVector3));
        
        // columns
        for(int i = 0; i < N; i++)
            haar1d_backward(buf1+i, buf2+i, N, width);
        for(int i = 0; i < N; i++)
            memcpy(buf1+i*width, buf2+i*width, N*sizeof(STVector3));
    }
    
    for(int y = 0; y < height; y++)
    {
        for(int x = 0; x < width; x++)
        {
            int level = max(ceilf(log2f(x+1)), ceilf(log2f(y+1)));
            //float level = max((log2f(x+1)), (log2f(y+1)));
            out->SetPixel(x, y, reverse_quantize(buf1[y*width+x], level, g_quality));
        }
    }
    
    delete[] buf1;
    delete[] buf2;
}


void diff(const STImage * a, const STImage * b, STImage * out)
{
    int width = a->GetWidth();
    int height = a->GetHeight();

    for(int y = 0; y < height; y++)
    {
        for(int x = 0; x < width; x++)
        {
            STColor4ub c_a = a->GetPixel(x, y);
            STColor4ub c_b = b->GetPixel(x, y);
            STColor4ub mid = STColor4ub(127, 127, 127, 255);
            STColor4ub dif = STColor4ub(c_b.r-c_a.r+mid.r,
                                        c_b.g+c_a.g+mid.g,
                                        c_b.b+c_a.b+mid.b,
                                        255);
            out->SetPixel(x, y, dif);
        }
    }
}


float compute_psnr(const STImage * a, const STImage * b)
{
    int width = a->GetWidth();
    int height = a->GetHeight();
    
    float mse = 0;
    float I_max = 0;
    
    for(int y = 0; y < height; y++)
    {
        for(int x = 0; x < width; x++)
        {
            STColor4ub c_a = a->GetPixel(x, y);
            STColor4ub c_b = b->GetPixel(x, y);
            float I_a = .299*(c_a.r/255.0f) + .587*(c_a.g/255.0f) + .114*(c_a.b/255.0f);
            float I_b = .299*(c_b.r/255.0f) + .587*(c_b.g/255.0f) + .114*(c_b.b/255.0f);
            mse += (I_b - I_a)*(I_b - I_a);
            if(I_a > I_max)
                I_max = I_a;
        }
    }
    
    mse /= (width*height);
    
    return 10*log10f(I_max*I_max/mse);
}


float compute_entropy(const STImage * i)
{
    int width = i->GetWidth();
    int height = i->GetHeight();
    float bin[256];
    memset(bin, 0, sizeof(float)*256);
    
    for(int y = 0; y < height; y++)
    {
        for(int x = 0; x < width; x++)
        {
            STColor4ub c = i->GetPixel(x, y);
            bin[c.r]++;
            bin[c.g]++;
            bin[c.b]++;
        }
    }
    
    float entropy = 0;
    
    for(int i = 0; i < 256; i++)
    {
        bin[i] /= 3*width*height;
        if(bin[i] > 0)
            entropy += bin[i] * log2f(bin[i]);
    }
    
    return -entropy;
}


//
// Display the UI, including all widgets.
//
void DisplayCallback()
{
    haar_forward(g_original, g_xformquantized);
    haar_backward(g_xformquantized, g_reconstructed);
    
    diff(g_original, g_reconstructed, g_difference);
    
    printf("PSNR: %f\n", compute_psnr(g_original, g_reconstructed));
    printf("entropy: %f\n", compute_entropy(g_xformquantized));

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


void KeyCallback(unsigned char key, int x, int y)
{
    bool redisplay = false;

    switch(key)
    {
        case 'a':
            g_quality -= 0.01;
            if(g_quality < 0) g_quality = 0;
            printf("quality: %f\n", g_quality);
            redisplay = true;
            break;
        case 'd':
            g_quality += 0.01;
            if(g_quality > 1) g_quality = 1;
            printf("quality: %f\n", g_quality);
            redisplay = true;
            break;
            
        case 'z':
            g_power -= 0.01;
            if(g_power < 0) g_power = 0;
            printf("power: %f\n", g_power);
            redisplay = true;
            break;
        case 'c':
            g_power += 0.01;
            if(g_power > 1) g_power = 1;
            printf("power: %f\n", g_power);
            redisplay = true;
            break;
            
        case 'l':
            g_nonlinearize = !g_nonlinearize;
            printf("nonlinearize: %i\n", g_nonlinearize);
            redisplay = true;
            break;
            
        case 'k':
            g_softclip = !g_softclip;
            printf("softclip: %i\n", g_softclip);
            redisplay = true;
            break;
    }
    
    if(redisplay)
        glutPostRedisplay();
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
    g_reconstructed = new STImage(g_original->GetWidth(), g_original->GetHeight());
    g_difference = new STImage(g_original->GetWidth(), g_original->GetHeight());
    
    
    {
        float qualities[] = { 0, 0.1, 0.2, 0.3, 0.5, 0.75, 1 };
        
        for(int i = 0; i < sizeof(qualities)/sizeof(float); i++)
        {
            g_quality = qualities[i];
            
            haar_forward(g_original, g_xformquantized);
            haar_backward(g_xformquantized, g_reconstructed);
            
            printf("quality: %f\n", g_quality);
            printf("PSNR: %f\n", compute_psnr(g_original, g_reconstructed));
            printf("entropy: %f\n", compute_entropy(g_xformquantized));
        }
    }
    
    g_quality = 0.5;
    
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
    glutKeyboardFunc(KeyCallback);

    glutMainLoop();

    //
    // Cleanup code should be called here.
    //
    
    return 0;
}
