//
//  UICurvePoint.cpp
//  FontEditor
//
//  Created by Spencer Salazar on 10/28/12.
//
//

#include "UICurvePoint.h"
#include <assert.h>
#include <math.h>


const static float gPointSize = 3;
const static float gSelectSize = 6;


template <typename T>
T wrapTo(T v, const T &max)
{
    while(v < 0) v += max;
    while(v >= max) v -= max;
    return v;
}

static bool Intersection(const STPoint2 &a1, const STPoint2 &b1,
                         const STPoint2 &a2, const STPoint2 &b2,
                         STPoint2 &i)
{
    float A1 = b1.y - a1.y;
    float B1 = a1.x - b1.x;
    float C1 = A1*a1.x + B1*a1.y;
    float A2 = b2.y - a2.y;
    float B2 = a2.x - b2.x;
    float C2 = A2*a2.x + B2*a2.y;
    
    float det = A1*B2 - A2*B1;
    
    if(det == 0)
    {
        return false;
    }
    else
    {
        i.x = (B2*C1 - B1*C2)/det;
        i.y = (A1*C2 - A2*C1)/det;
        return true;
    }
}


void UICurvePoint::Display()
{
    if(m_mouseOver || m_selected)
    {
        if(m_selected)
            glColor4f(0.5, 0.5, 1, 1.0);
        else
            glColor4f(0.5, 0.5, 1, 0.5);
        
        glBegin(GL_QUADS);
        
        glVertex2f(m_point->mCoordinates.x+-gSelectSize, m_point->mCoordinates.y+-gSelectSize);
        glVertex2f(m_point->mCoordinates.x+ gSelectSize, m_point->mCoordinates.y+-gSelectSize);
        glVertex2f(m_point->mCoordinates.x+ gSelectSize, m_point->mCoordinates.y+ gSelectSize);
        glVertex2f(m_point->mCoordinates.x+-gSelectSize, m_point->mCoordinates.y+ gSelectSize);
        
        glEnd();
    }

    glColor3f(0, 0, 0);
    
    if(m_point->mOnCurve)
    {
        glBegin(GL_QUADS);
        
        glVertex2f(m_point->mCoordinates.x+-gPointSize, m_point->mCoordinates.y+-gPointSize);
        glVertex2f(m_point->mCoordinates.x+ gPointSize, m_point->mCoordinates.y+-gPointSize);
        glVertex2f(m_point->mCoordinates.x+ gPointSize, m_point->mCoordinates.y+ gPointSize);
        glVertex2f(m_point->mCoordinates.x+-gPointSize, m_point->mCoordinates.y+ gPointSize);
    }
    else
    {
        glBegin(GL_LINES);
        
        glVertex2f(m_point->mCoordinates.x+-gPointSize, m_point->mCoordinates.y+-gPointSize);
        glVertex2f(m_point->mCoordinates.x+ gPointSize, m_point->mCoordinates.y+-gPointSize);

        glVertex2f(m_point->mCoordinates.x+ gPointSize, m_point->mCoordinates.y+-gPointSize);
        glVertex2f(m_point->mCoordinates.x+ gPointSize, m_point->mCoordinates.y+ gPointSize);

        glVertex2f(m_point->mCoordinates.x+ gPointSize, m_point->mCoordinates.y+ gPointSize);
        glVertex2f(m_point->mCoordinates.x+-gPointSize, m_point->mCoordinates.y+ gPointSize);
        
        glVertex2f(m_point->mCoordinates.x+-gPointSize, m_point->mCoordinates.y+ gPointSize);
        glVertex2f(m_point->mCoordinates.x+-gPointSize, m_point->mCoordinates.y+-gPointSize);
    }
        
    glEnd();
}


bool UICurvePoint::HitTest(const STPoint2& position)
{
    UIRectangle r = UIRectangle(STPoint2(m_point->mCoordinates.x+-gPointSize, m_point->mCoordinates.y+-gPointSize),
                                STPoint2(m_point->mCoordinates.x+ gPointSize, m_point->mCoordinates.y+ gPointSize));
    if(r.Contains(position))
        return true;
    
    return false;
}


void UICurvePoint::HandleMouseEnter()
{
    m_mouseOver = true;
}



void UICurvePoint::HandleMouseLeave()
{
    m_mouseOver = false;
}


void UICurvePoint::HandleMouseDown(const STPoint2& position, int modifiers)
{
    m_mouseDown = true;
    
    m_lastMouseLocation = position;
}


void UICurvePoint::HandleMouseUp(const STPoint2& position, int modifiers)
{
    m_mouseDown = false;
    if(HitTest(position))
    {
        m_selected = true;
        if(m_callback)
            m_callback(this);
    }
}

void UICurvePoint::HandleMouseMove(const STPoint2& position, int modifiers)
{
    if(m_mouseDown)
    {
        if(modifiers & GLUT_ACTIVE_CTRL)
        {
            if(m_point->mOnCurve)
            {
                int numPts = m_contour->NumPoints();
                
                // find index of this point in the contour
                int ptIndex = -1;
                for(int i = 0; i < numPts; i++)
                {
                    if(m_point == m_contour->GetPoint(i))
                    {
                        ptIndex = i;
                        break;
                    }
                }
                
                // this point should exist in the contour
                // otherwise theres a problem!
                assert(ptIndex != -1);
                
                float theta = (position.y - m_lastMouseLocation.y)/180.0*M_PI;
                
                TTPoint * ptM1 = m_contour->GetPoint(wrapTo(ptIndex-1, numPts));
                if(!ptM1->mOnCurve)
                {
                    // translate to origin
                    STVector2 trM1 = ptM1->mCoordinates - m_point->mCoordinates;
                    // rotate about origin
                    trM1 = STVector2(STVector2::Dot(trM1, STVector2(cosf(theta), sinf(theta))),
                                     STVector2::Dot(trM1, STVector2(-sinf(theta), cosf(theta))));
                    // translate back
                    ptM1->mCoordinates = m_point->mCoordinates + trM1;

                    TTPoint * ptM2 = m_contour->GetPoint(wrapTo(ptIndex-2, numPts));
                    if(ptM2->mOnCurve)
                    {
                        TTPoint * ptM3 = m_contour->GetPoint(wrapTo(ptIndex-3, numPts));
                        Intersection(m_point->mCoordinates, ptM1->mCoordinates,
                                     ptM2->mCoordinates, ptM3->mCoordinates,
                                     ptM1->mCoordinates);
                    }
                }
                
                TTPoint * ptP1 = m_contour->GetPoint(wrapTo(ptIndex+1, numPts));
                if(!ptP1->mOnCurve)
                {
                    // translate to origin
                    STVector2 trP1 = ptP1->mCoordinates - m_point->mCoordinates;
                    // rotate about origin
                    trP1 = STVector2(STVector2::Dot(trP1, STVector2(cosf(theta), sinf(theta))),
                                     STVector2::Dot(trP1, STVector2(-sinf(theta), cosf(theta))));
                    // translate back
                    ptP1->mCoordinates = m_point->mCoordinates + trP1;
                    
                    TTPoint * ptP2 = m_contour->GetPoint(wrapTo(ptIndex+2, numPts));
                    if(ptP2->mOnCurve)
                    {
                        TTPoint * ptP3 = m_contour->GetPoint(wrapTo(ptIndex+3, numPts));
                        Intersection(m_point->mCoordinates, ptP1->mCoordinates,
                                     ptP2->mCoordinates, ptP3->mCoordinates,
                                     ptP1->mCoordinates);
                    }
                }
            }
        }
        else
        {
            m_point->mCoordinates = position;
            if(m_moveCallback)
                m_moveCallback(this);
        }
    }
    
    m_lastMouseLocation = position;
}


void UICurvePoint::ForceContinuousTanget()
{
    if(m_point->mOnCurve)
    {
        int numPts = m_contour->NumPoints();
        
        // find index of this point in the contour
        int ptIndex = -1;
        for(int i = 0; i < numPts; i++)
        {
            if(m_point == m_contour->GetPoint(i))
            {
                ptIndex = i;
                break;
            }
        }
        
        // this point should exist in the contour
        // otherwise theres a problem!
        assert(ptIndex != -1);
        
        TTPoint * ptM1 = m_contour->GetPoint(wrapTo(ptIndex-1, numPts));
        TTPoint * ptP1 = m_contour->GetPoint(wrapTo(ptIndex+1, numPts));
        if(!ptM1->mOnCurve && !ptP1->mOnCurve)
        {
            // translate to origin
            STVector2 trM1 = ptM1->mCoordinates - m_point->mCoordinates;
            // translate to origin
            STVector2 trP1 = ptP1->mCoordinates - m_point->mCoordinates;
            // normalize
            trP1.Normalize();
            // point previous tangent in opposite direction with same length
            trM1 = STVector2(-trP1.x, -trP1.y) * trM1.Length();
            // translate back
            ptM1->mCoordinates = m_point->mCoordinates + trM1;
            
            // don't disturb tangents of other points
            TTPoint * ptM2 = m_contour->GetPoint(wrapTo(ptIndex-2, numPts));
            if(ptM2->mOnCurve)
            {
                TTPoint * ptM3 = m_contour->GetPoint(wrapTo(ptIndex-3, numPts));
                Intersection(m_point->mCoordinates, ptM1->mCoordinates,
                             ptM2->mCoordinates, ptM3->mCoordinates,
                             ptM1->mCoordinates);
            }
        }
    }
}


