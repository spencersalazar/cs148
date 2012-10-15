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

//
// Include headers for UI clases.
//
#include "UIBox.h"
#include "UIButton.h"
#include "UILabel.h"
#include "UIWidget.h"

//
// Include header for parsing config and line editor files
//
#include "parseConfig.h"

#define WIN_WIDTH 512
#define WIN_HEIGHT 512

//
// Globals used by this application.
// As a rule, globals are Evil, but this is a small application
// and the design of GLUT makes it hard to avoid them.
//

// Font to use for text in the UI.
static STFont* gFont = NULL;

// List of widgets being displayed.
static std::vector<UIWidget*> gWidgets;

// Window size so we can properly update the UI.
static int gWindowSizeX = 0;
static int gWindowSizeY = 0;

// Background images
static STImage* gBgIm1;
static STImage* gBgIm2;

#ifndef BUFSIZ
#define BUFSIZ  512
#endif
// Background image filenames parsed from config file
static char gImage1Fname[BUFSIZ];
static char gImage2Fname[BUFSIZ];

// Line editor load/save filenames parsed from config file
static char gSaveFname[BUFSIZ];
static char gLoadFname[BUFSIZ];

/** CS 148 TODO:
*
*   Add your own global variables, functions
*   and classes here as needed to complete
*   the assignment requirements.
*/

UIWidget * gEnteredWidget = NULL;
UIWidget * gCaptureWidget = NULL;

//

//
// Add a widget to the list of widgets in the window.
// Also sets the rectangle of the widget to the one specified.
//
void AddWidget(UIWidget* widget, const UIRectangle& rectangle)
{
    widget->SetRectangle(rectangle);
    gWidgets.push_back(widget);
}

// Creates any widgets or other objects used for displaying lines.
// lineEndpt1 and lineEndpt2 should both be coordindates RELATIVE TO THE IMAGE
// THEY ARE DRAWN TO. This means that if the lower left of one image is at
// (15,15), if lineEndpt1 is (10,10), it would actually be drawn at window
// coordinates (25,25).
//
// The imageChoice parameter tells you whether a line is being drawn to both images,
// or to a single images. When the user is manually adding lines, this parameter
// should be BOTH_IMAGES. When this function is called by loadLineEditorFile
// (already implemented), imageChoice will take on the value of either IMAGE_1
// or IMAGE_2.
void AddNewLine(STPoint2 lineEndpt1, STPoint2 lineEndpt2, ImageChoice imageChoice)
{
    /** CS148 TODO : ONLY REQUIRED FOR EXTRA CREDIT
     *
     * Use this to create any widgets and objects you use to display and edit lines.
     */
}


void MyButtonCallback(UIButton * button)
{
    printf("I did it!\n");
}


//
// Setup routnine.
//
// As you progress with implementing the assignment,
// you will want to modify this routine to create and
// test your new widget types.
//
void CreateWidgets()
{
    /** CS 148 TODO:
    *
    * This function should create any widgets on display
    * when the line editor begins. You should also use
    * it during testing to play with new widget types.
    */
 
    AddWidget(new UIBox(STColor4f(0,0,1,1)),
        UIRectangle(STPoint2(10,20), STPoint2(60, 50)));
    
    AddWidget(new UILabel(gFont, "back it up"),
              UIRectangle(STPoint2(10,20), STPoint2(60, 50)));
    
    AddWidget(new UIButton(gFont, "a button", MyButtonCallback),
              UIRectangle(STPoint2(WIN_WIDTH-120,20), STPoint2(WIN_HEIGHT-10, 50)));
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

    /* CS148 TODO : ONLY FOR EXTRA CREDIT
     *
     * Display both backgrounds for the line editor.
     * You may display them side by side or toggle
     * between the two images.
     */
    gBgIm1->Draw();

    // Loop through all the widgets in the user
    // interface and tell each to display itself.
    int numWidgets = (int) gWidgets.size();
    for (int ii = 0; ii < numWidgets; ++ii) {
        UIWidget* widget = gWidgets[ii];
        widget->Display();
    }

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
    
    UIWidget * receiver = NULL;
    if(gCaptureWidget == NULL)
    {
        int numWidgets = (int) gWidgets.size();
        for (int ii = 0; ii < numWidgets; ++ii)
        {
            UIWidget * widget = gWidgets[ii];
            if(widget->HitTest(position))
                receiver = widget;
        }
    }
    
    if(button == GLUT_LEFT_BUTTON)
    {
        if(state == GLUT_DOWN)
        {
            if(receiver != NULL)
            {
                receiver->HandleMouseDown(position);
                gCaptureWidget = receiver;
                redisplay = true;
            }
        }
        else if(state == GLUT_UP)
        {
            if(gCaptureWidget)
            {
                gCaptureWidget->HandleMouseUp(position);
                gCaptureWidget = NULL;
                redisplay = true;
            }
        }
    }
    
    if(redisplay)
        glutPostRedisplay();
}


void MotionCallback(int x, int y)
{
    STPoint2 position = STPoint2(x, gWindowSizeY - y);
    bool redisplay = false;

    if(gCaptureWidget)
    {
        if(gCaptureWidget->HitTest(position))
        {
            if(gEnteredWidget != gCaptureWidget)
            {
                gCaptureWidget->HandleMouseEnter();
                gEnteredWidget = gCaptureWidget;
            }
        }
        else
        {
            if(gEnteredWidget == gCaptureWidget)
            {
                gEnteredWidget->HandleMouseLeave();
                gEnteredWidget = NULL;
            }
        }
        
        gCaptureWidget->HandleMouseMove(position);
        
        redisplay = true;
    }
    
    if(redisplay)
        glutPostRedisplay();
}


void PassiveMotionCallback(int x, int y)
{
    STPoint2 position = STPoint2(x, gWindowSizeY - y);
    bool redisplay = false;

    UIWidget * receiver = NULL;
    int numWidgets = (int) gWidgets.size();
    for (int ii = 0; ii < numWidgets; ++ii)
    {
        UIWidget * widget = gWidgets[ii];
        if(widget->HitTest(position))
            receiver = widget;
    }
    
    if(receiver != gEnteredWidget)
    {
        if(gEnteredWidget != NULL)
            gEnteredWidget->HandleMouseLeave();
        if(receiver != NULL)
            receiver->HandleMouseEnter();
        gEnteredWidget = receiver;
        redisplay = true;
    }
    
    if(gEnteredWidget != NULL)
    {
        gEnteredWidget->HandleMouseMove(position);
        redisplay = true;
    }
    
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

    gFont = new STFont("resources/arial.ttf", 24);

    CreateWidgets();
}

int main(int argc, char** argv)
{
    //
    // Initialize GLUT.
    //
    glutInit(&argc, argv);
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(20, 20);
    glutInitWindowSize(
        WIN_WIDTH, WIN_HEIGHT);
    glutCreateWindow("CS148 Assignment 3");

    //
    // Initialize the UI.
    //
    Initialize();

    //
    // Parse config file
    //
    parseConfigFile(
        "config.txt",
        gImage1Fname,
        gImage2Fname,
        gSaveFname,
        gLoadFname,
        &gBgIm1,
        &gBgIm2);

    //
    // Register GLUT callbacks and enter main loop.
    //
    glutDisplayFunc(DisplayCallback);
    glutReshapeFunc(ReshapeCallback);

    /** CS148 TODO:
    *
    *   Remember to set up any additional event
    *   callbacks that you create.
    */
    
    glutMouseFunc(MouseCallback);
    glutMotionFunc(MotionCallback);
    glutPassiveMotionFunc(PassiveMotionCallback);

    glutMainLoop();

    //
    // Cleanup code should be called here.
    //
    delete gBgIm1;
    delete gBgIm2;

    return 0;
}
