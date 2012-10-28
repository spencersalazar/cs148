//
//  UICurvePoint.cpp
//  FontEditor
//
//  Created by Spencer Salazar on 10/28/12.
//
//

#include "UICurvePoint.h"


void UICurvePoint::Display()
{
    glColor3f(0, 0, 0);
    
    float size = 3;
    
    if(m_point->mOnCurve)
    {
        glBegin(GL_QUADS);
        
        glVertex2f(m_point->mCoordinates.x+-size, m_point->mCoordinates.y+-size);
        glVertex2f(m_point->mCoordinates.x+ size, m_point->mCoordinates.y+-size);
        glVertex2f(m_point->mCoordinates.x+ size, m_point->mCoordinates.y+ size);
        glVertex2f(m_point->mCoordinates.x+-size, m_point->mCoordinates.y+ size);
    }
    else
    {
        glBegin(GL_LINES);
        
        glVertex2f(m_point->mCoordinates.x+-size, m_point->mCoordinates.y+-size);
        glVertex2f(m_point->mCoordinates.x+ size, m_point->mCoordinates.y+-size);

        glVertex2f(m_point->mCoordinates.x+ size, m_point->mCoordinates.y+-size);
        glVertex2f(m_point->mCoordinates.x+ size, m_point->mCoordinates.y+ size);

        glVertex2f(m_point->mCoordinates.x+ size, m_point->mCoordinates.y+ size);
        glVertex2f(m_point->mCoordinates.x+-size, m_point->mCoordinates.y+ size);
        
        glVertex2f(m_point->mCoordinates.x+-size, m_point->mCoordinates.y+ size);
        glVertex2f(m_point->mCoordinates.x+-size, m_point->mCoordinates.y+-size);
    }
        
    glEnd();
}

