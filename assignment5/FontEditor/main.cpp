/*
 *  main.cpp
 *  FontEditor
 *
 *  GLUT application to act as basis for a glyph editor.
 *
 */

#include "TemplateLayer.h"
#include "TTFontWriter.h"

#include "st.h"
#include "stglut.h"

#include "UIWidget.h"
#include "UIButton.h"
#include "UILabel.h"
#include "UICurvePoint.h"
#include <list>
#include "loadsave.h"

#include <assert.h>

using namespace std;

inline STPoint2 operator*(const STPoint2 &p, float s)
{
    return STPoint2(p.x*s, p.y*s);
}

inline STPoint2 operator*(float s, const STPoint2 &p)
{
    return STPoint2(p.x*s, p.y*s);
}

inline STPoint2 operator+(const STPoint2 &p1, const STPoint2 &p2)
{
    return STPoint2(p1.x+p2.x, p1.y+p2.y);
}

//
// Globals used by this application.
// As a rule, globals are Evil, but this is a small application
// and the design of GLUT makes it hard to avoid them.
//

MyFont          gMyFont;        // font object 
TemplateLayer  *gTemplate = 0;  // used for drawing the template and guides

// variables to keep track of rendering a preview of the font
bool            gPreviewMode = false;
STFont         *gPreviewFont = 0;

// variables to keep track of mouse position and buttons
int gMousePreviousX;
int gMousePreviousY;
int gMouseActiveButton = -1;

// just a few function prototypes
void CreateTestFont();
void PreparePreview();

static int gWindowSizeX = 0;
static int gWindowSizeY = 0;

list<UIWidget *> gWidgets;
list<UICurvePoint *> gCurvePoints;
UICurvePoint * gSelectedCurvePoint = NULL;
TTContour * gActiveContour = NULL;
int gSelectedContour = -1;
STPoint2 gContourMovePoint = STPoint2(0,0);
TTGlyph * gGlyph = NULL;

UIWidget * gCaptureWidget = NULL;
UIWidget * gEnteredWidget = NULL;

enum
{
    ONLINE_MODE,
    OFFLINE_MODE
}
gPointMode = ONLINE_MODE;

UIButton * gOnOffLineButton = NULL;
UIButton * gSelectModeButton = NULL;


void AddWidget(UIWidget* widget, const UIRectangle& rectangle)
{
    widget->SetRectangle(rectangle);
    gWidgets.push_back(widget);
}

void OnOffLineCallback(UIButton * button)
{
    // toggle
    if(gPointMode == ONLINE_MODE)
        gPointMode = OFFLINE_MODE;
    else
        gPointMode = ONLINE_MODE;
    
    // set button text
    if(gPointMode == ONLINE_MODE)
        button->setText("On-Line");
    else
        button->setText("Off-Line");
    
    glutPostRedisplay();
}


void NewGlyphCallback(UIButton * button)
{
    if(gActiveContour && gActiveContour->NumPoints() == 0)
        return;
    gActiveContour = new TTContour;
    gGlyph->AddContour(gActiveContour);
    
    glutPostRedisplay();
}

void CurvePointSelectCallback(UICurvePoint * curvePoint)
{
    gSelectedCurvePoint = curvePoint;
    
    glutPostRedisplay();
}

void CurvePointMoveCallback(UICurvePoint * curvePoint)
{
    if(gSelectedContour)
    {
        
    }
}


void RenderQuadraticBezier(STPoint2 a, STPoint2 ctl, STPoint2 b)
{
    int NUM_SEGS = 10;
    float advance = 1.0/((float) NUM_SEGS);
    
    glBegin(GL_LINES);
    
    for(float t = 0; t < 1;)
    {
        float t2 = t+advance;
        STPoint2 pt1 = (1-t)*((1-t)*a+t*ctl) + t*((1-t)*ctl+t*b);
        STPoint2 pt2 = (1-t2)*((1-t2)*a+t2*ctl) + t2*((1-t2)*ctl+t2*b);
        while((pt1-pt2).Length() > 1)
        {
            advance /= 2.0;
            t2 = t+advance;
            pt1 = (1-t)*((1-t)*a+t*ctl) + t*((1-t)*ctl+t*b);
            pt2 = (1-t2)*((1-t2)*a+t2*ctl) + t2*((1-t2)*ctl+t2*b);
        }
        
        glVertex2f(pt1.x, pt1.y);
        glVertex2f(pt2.x, pt2.y);
        
        t += advance;
    }
    
    glEnd();
}

void RenderLine(const STPoint2 &a, const STPoint2 &b)
{
    glBegin(GL_LINES);
    
    glVertex2f(a.x, a.y);
    glVertex2f(b.x, b.y);
    
    glEnd();
}

void RenderContour(const TTContour *contour)
{
    int len = contour->NumPoints();
    vector<TTPoint> contourPoints;
    for(int i = 0; i < len; i++)
    {
        TTPoint * pt = contour->GetPoint(i);
        TTPoint * nextPt = contour->GetPoint((i+1)%len);

        if(!pt->mOnCurve && i == 0)
        {
            TTPoint * prevPt = contour->GetPoint(len - 1);
            
            if(prevPt->mOnCurve)
                contourPoints.push_back(*prevPt);
            else
            {
                TTPoint synthPt;
                synthPt.mOnCurve = true;
                synthPt.mCoordinates = prevPt->mCoordinates + (pt->mCoordinates - prevPt->mCoordinates)/2.0;
                contourPoints.push_back(synthPt);
            }
        }
        
        contourPoints.push_back(*pt);
        
        if(!pt->mOnCurve && !nextPt->mOnCurve)
        {
            TTPoint synthPt;
            synthPt.mOnCurve = true;
            synthPt.mCoordinates = pt->mCoordinates + (nextPt->mCoordinates - pt->mCoordinates)/2.0;
            contourPoints.push_back(synthPt);
        }
    }
    
    if(len > 0)
        contourPoints.push_back(*contour->GetPoint(0));
    
    len = contourPoints.size();
    for(int i = 0; i < len;)
    {
        TTPoint pt1 = contourPoints[i];
        if(i+1 >= len) break;
        TTPoint pt2 = contourPoints[i+1];
        
        if(pt1.mOnCurve && pt2.mOnCurve)
        {
            // just a line
            RenderLine(pt1.mCoordinates, pt2.mCoordinates);
            i += 1;
        }
        else
        {
            // assumption:
            // any two off curve pts have a on curve pt between them
            if(i+2 >= len) break;
            TTPoint pt3 = contourPoints[i+2];
            assert(pt1.mOnCurve && !pt2.mOnCurve && pt3.mOnCurve);
            RenderQuadraticBezier(pt1.mCoordinates, pt2.mCoordinates, pt3.mCoordinates);
            i += 2;
        }
    }
}

//
// Display event handler
//
void DisplayCallback()
{
    glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT );
    
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // draw the background image and guides first
    gTemplate->Draw();
    
    /*** CS148 TODO
     *   Here is where you will need to draw the control points and curves of
     *   the glyph you are editing, or call a function to perform the drawing.
     */        
    
    // draw the preview mode last, on top of everything else, if it's active
    if (gPreviewMode)
        gTemplate->DrawPreview(gPreviewFont);
    
    for (list<UIWidget *>::iterator ii = gWidgets.begin();
         ii != gWidgets.end(); ++ii) {
        UIWidget* widget = *ii;
        widget->Display();
    }
    
    for(int i = 0; i < gGlyph->NumContours(); i++)
    {
        if(i == gSelectedContour)
            glColor4f(0.95, 0.25, 0.25, 1);
        else
            glColor4f(0, 0, 0, 1);
        RenderContour(gGlyph->GetContour(i));
    }

    glFlush();
}

//
// Keyboard event handler
//
void KeyboardCallback(unsigned char key, int x, int y)
{
    bool redisplay = false;
    
    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    int vw = viewport[2], vh = viewport[3];
 
    int modifiers = glutGetModifiers();    

    /*** CS148 TODO
     *   You will probably want to capture various keystrokes to perform
     *   commands for your font editor.  Either add more keys for the switch
     *   statement to recognize, or intercept the events before it goes to
     *   the switch statement below.  If you cast the "key" value to an int
     *   and print it out here, you can see what key codes are generated for
     *   different keys.
     */    

    //
    // Interpret different keys to perform various commands
    //
    switch(key)
    {
        case char(27):
            exit(0);
            break;
            
        case char(127):
            // delete
            if(gSelectedContour >= 0)
            {
                TTContour * c = gGlyph->GetContour(gSelectedContour);
                if(gActiveContour == c)
                    gActiveContour = NULL;
                gGlyph->RemoveContour(gSelectedContour);
                
                for(list<UICurvePoint *>::iterator i = gCurvePoints.begin();
                    i != gCurvePoints.end();)
                {
                    list<UICurvePoint *>::iterator j = i++;
                    if((*j)->GetContour() == c)
                        gCurvePoints.erase(j);
                }
                
                gSelectedContour = -1;
                if(c == gActiveContour)
                    gActiveContour = NULL;
                
                redisplay = true;
            }
            else if(gSelectedCurvePoint)
            {
                int len = gActiveContour->NumPoints();
                for(int i = 0; i < len; i++)
                {
                    if(gActiveContour->GetPoint(i) == gSelectedCurvePoint->GetPoint())
                    {
                        gActiveContour->RemovePoint(i);
                        break;
                    }
                }
                
                gWidgets.remove(gSelectedCurvePoint);
                gCurvePoints.remove(gSelectedCurvePoint);
                delete gSelectedCurvePoint;
                gSelectedCurvePoint = NULL;
                
                redisplay = true;
            }
            
            gCaptureWidget = NULL;
            gEnteredWidget = NULL;
            
            break;
            
            // template image zoom controls
        case '+':
        case '=':
            gTemplate->ScaleImage(1.0f);
            glutPostRedisplay();
            break;
        case '-':
            gTemplate->ScaleImage(-1.0f);
            glutPostRedisplay();
            break;
            
        case 'o':
            OnOffLineCallback(gOnOffLineButton);
            redisplay = true;
            break;
            
            // template image fine zoom controls
        case ']':
            gTemplate->ScaleImage(0.2f);
            glutPostRedisplay();
            break;
        case '[':
            gTemplate->ScaleImage(-0.2f);
            glutPostRedisplay();
            break;
            
            // save, load, and preview
        case 's':
//            gMyFont.SaveData();
            saveGlyph(gGlyph);
            break;
        case 'l':
//            gMyFont.LoadData();
            delete gGlyph;
            gCaptureWidget = NULL;
            gEnteredWidget = NULL;
            for(list<UICurvePoint *>::iterator i = gCurvePoints.begin();
                i != gCurvePoints.end(); i++)
            {
                gWidgets.remove(*i);
                delete *i;
            }
            gCurvePoints.clear();
            gActiveContour = NULL;
            gSelectedCurvePoint = NULL;
            gSelectedContour = -1;
            
            gGlyph = loadGlyph();
            
            for(int c = 0; c < gGlyph->NumContours(); c++)
            {
                TTContour * contour = gGlyph->GetContour(c);
                for(int p = 0; p < contour->NumPoints(); p++)
                {
                    TTPoint * point = contour->GetPoint(p);
                    UICurvePoint * cp = new UICurvePoint(point, contour, CurvePointSelectCallback, CurvePointMoveCallback);
                    gWidgets.push_back(cp);
                    gCurvePoints.push_back(cp);
                }
            }

            glutPostRedisplay();
            break;
            
        case '\t':
            if(gSelectedContour == gGlyph->NumContours()-1)
                gSelectedContour = -1;
            else
                gSelectedContour = gSelectedContour+1;
            redisplay = true;
            
            break;
            
        case 'p':
            gPreviewMode = !gPreviewMode;
            if (gPreviewMode) PreparePreview();
            glutPostRedisplay();
            break;
            
        case 'c':
            if(gSelectedCurvePoint)
            {
                gSelectedCurvePoint->GetPoint()->mOnCurve = !gSelectedCurvePoint->GetPoint()->mOnCurve;
                redisplay = true;
            }
            break;
    }
    
    if(redisplay)
        glutPostRedisplay();
}

//
// Mouse event handler (two functions - click and move)
//
void MouseCallback(int button, int state, int x, int y)
{
    gMousePreviousX = x;
    gMousePreviousY = y;

    // 
    // note that this "gMouseActiveButton" state machine is a little flawed...
    // feel free to fix it if you so desire
    //
    if (state == GLUT_DOWN) gMouseActiveButton = button;
    else                    gMouseActiveButton = -1;
    
    /*** CS148 TODO
     *   Here, you can capture and use the left mouse button click and release
     *   events to perform various actions for editing glyphs.
     */
    if(button == GLUT_LEFT_BUTTON)
    {
        bool redisplay = false;
        STPoint2 position = STPoint2(x, gWindowSizeY - y);
        
        if(gSelectedContour != -1)
        {
            if(state == GLUT_DOWN)
                gContourMovePoint = position;
        }
        else
        {
            
            UIWidget * receiver = NULL;
            if(gCaptureWidget == NULL)
            {
                for (list<UIWidget *>::iterator ii = gWidgets.begin();
                     ii != gWidgets.end(); ++ii)
                {
                    UIWidget* widget = *ii;
                    if(widget->HitTest(position))
                        receiver = widget;
                }
            }
            
            if(state == GLUT_DOWN)
            {
                if(receiver != NULL)
                {
                    gCaptureWidget = receiver;
                    gCaptureWidget->HandleMouseDown(position);
                    redisplay = true;
                }
                else
                {
                    if(gActiveContour == NULL)
                        NewGlyphCallback(NULL);
                    
                    TTPoint * newPt = new TTPoint;
                    newPt->mOnCurve = (gPointMode == ONLINE_MODE) && !(glutGetModifiers() &GLUT_ACTIVE_SHIFT);
                    newPt->mCoordinates = position;
                    
                    TTContour * contour;
                    if(gSelectedContour != -1)
                        contour = gGlyph->GetContour(gSelectedContour);
                    else
                        contour = gActiveContour;
                    
                    contour->AddPoint(newPt);
                    
                    UICurvePoint * widget = new UICurvePoint(newPt, contour, CurvePointSelectCallback, CurvePointMoveCallback);
                    gWidgets.push_back(widget);
                    gCurvePoints.push_back(widget);
                    
                    redisplay = true;
                }
            }
            else if(state == GLUT_UP)
            {
                if(gSelectedCurvePoint)
                {
                    gSelectedCurvePoint->Deselect();
                    redisplay = true;
                }
                gSelectedCurvePoint = NULL;
                
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
}

void MotionCallback(int x, int y)
{
    int dx = x - gMousePreviousX;
    int dy = y - gMousePreviousY;

    //
    // use the right mouse button to control pan
    //
    if (gMouseActiveButton == GLUT_RIGHT_BUTTON) {
        gTemplate->MoveImage(dx, -dy);
        glutPostRedisplay();
    }
    //
    // use the middle mouse button to control zoom
    // (too bad GLUT is too old to support the mouse wheel!)
    //
    else if (gMouseActiveButton == GLUT_MIDDLE_BUTTON) {
        gTemplate->ScaleImage(0.1f * -dy);
        glutPostRedisplay();
    }
    /*** CS148 TODO
     *   Here, you can capture and use the left mouse button move events to
     *   move control points around.
     */
    else if (gMouseActiveButton == GLUT_LEFT_BUTTON)
    {
        bool redisplay = false;
        STPoint2 position = STPoint2(x, gWindowSizeY - y);

        if(gSelectedContour != -1)
        {
            STVector2 diff = position - gContourMovePoint;
            gContourMovePoint = position;
            TTContour * c = gGlyph->GetContour(gSelectedContour);
            for(int i = 0; i < c->NumPoints(); i++)
            {
                TTPoint * p = c->GetPoint(i);
                p->mCoordinates += diff;
            }
            
            redisplay = true;
        }
        else
        {
            
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
        }
        
        if(redisplay)
            glutPostRedisplay();
    }

    gMousePreviousX = x;
    gMousePreviousY = y;
}


void PassiveMotionCallback(int x, int y)
{
    STPoint2 position = STPoint2(x, gWindowSizeY - y);
    bool redisplay = false;
    
    UIWidget * receiver = NULL;
    for (list<UIWidget *>::iterator ii = gWidgets.begin();
         ii != gWidgets.end(); ++ii)
    {
        UIWidget* widget = *ii;
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
// Reshape is called when the window changes size, and is used
// to compute a projection for the new size/shape.
//
void ReshapeCallback(int w, int h)
{
    glViewport(0, 0, w, h);
    
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    gluOrtho2D(0.0,w,0.0,h);

    STVector2 origin(120, h/2 - 40);
    gTemplate->SetOrigin(origin);
    
    gWindowSizeX = w;
    gWindowSizeY = h;
}

//
// Call this function to fill each basic character with a simple test glyph.
// For testing purposes only -- currently not used!
//
void CreateTestFont()
{
    char basicset[] = 
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789!\"#$%&'()*+,-./:;<=>?";

    for (int i = 0; basicset[i]; ++i) {
        TTContour *contour = new TTContour;
        contour->AddPoint(new TTPoint(50, 32, false));
        contour->AddPoint(new TTPoint(50, 224, false));
        contour->AddPoint(new TTPoint(150, 224, false));
        contour->AddPoint(new TTPoint(150, 32, false));

        TTGlyph *glyph = new TTGlyph(0, 200);
        glyph->AddContour(contour);

        gMyFont.SetGlyphForCharacter(basicset[i], glyph);
    }
}

//
// Writes the font as is to a TTF file, then reads it back using STFont
// to render for a preview
//
void PreparePreview()
{
    // first export the font to a TTF file
    TTFontWriter writer(gMyFont);
    writer.WriteToFile("MyFont.ttf");

    // then load the font to render using STFont
    if (gPreviewFont) delete gPreviewFont;
    gPreviewFont = new STFont("MyFont.ttf", 48);
}

//
// Program entry point
//
int main(int argc, char** argv)
{
    //
    // Initialize GLUT.
    //
    glutInit(&argc, argv);
    glutInitDisplayMode( GLUT_SINGLE | GLUT_RGBA );
    glutInitWindowPosition(20, 20);
    glutInitWindowSize(900, 640);
    glutCreateWindow("My Glyph Editor");
    
    gWindowSizeX = 900;
    gWindowSizeY = 640;

    //
    // Create the template layer that controls the background image (with
    // your handwriting on it) and font metric guide lines
    //
    gTemplate = new TemplateLayer(&gMyFont);
    gTemplate->ScaleImage(6.f);
    gTemplate->MoveImage(1045.f, -750.f);
	
    //
    // Register application callbacks with GLUT.
    //
    glutDisplayFunc(DisplayCallback);
    glutReshapeFunc(ReshapeCallback);
    glutKeyboardFunc(KeyboardCallback);
    glutMouseFunc(MouseCallback);
    glutMotionFunc(MotionCallback);
    glutPassiveMotionFunc(PassiveMotionCallback);
	
    STFont * font = new STFont("resources/arial.ttf", 12);
    
    gGlyph = new TTGlyph;
    gOnOffLineButton = new UIButton(font, "On-Line", OnOffLineCallback);
    AddWidget(gOnOffLineButton,
              UIRectangle(STPoint2(10, 10), STPoint2(110, 30)));
    AddWidget(new UIButton(font, "New Contour", NewGlyphCallback),
              UIRectangle(STPoint2(120, 10), STPoint2(220, 30)));
//    gSelectModeButton = new UIButton(font, "Select: Point", SelectModeCallback);
//    AddWidget(gSelectModeButton,
//              UIRectangle(STPoint2(230, 10), STPoint2(330, 30)));
    
    //
    // Run the application
    //
    glutMainLoop();

    //
    // Clean up
    //
    if (gPreviewFont) delete gPreviewFont;
    delete gTemplate;

    return 0;
}
