//
//  UICurvePoint.cpp
//  FontEditor
//
//  Created by Spencer Salazar on 10/28/12.
//
//

#include "UICurvePoint.h"


const static float gPointSize = 3;
const static float gSelectSize = 6;


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


void UICurvePoint::HandleMouseDown(const STPoint2& position)
{
    m_mouseDown = true;
}


void UICurvePoint::HandleMouseUp(const STPoint2& position)
{
    m_mouseDown = false;
    if(HitTest(position))
    {
        m_selected = true;
        if(m_callback)
            m_callback(this);
    }
}

void UICurvePoint::HandleMouseMove(const STPoint2& position)
{
    if(m_mouseDown)
    {
        m_point->mCoordinates = position;
        if(m_moveCallback)
            m_moveCallback(this);
    }
}


