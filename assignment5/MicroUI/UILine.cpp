//
//  UILine.cpp
//  MicroUI
//
//  Created by Spencer Salazar on 10/14/12.
//
//

#include "UILine.h"
#include "st.h"
#include "stgl.h"


bool UILine::s_deleteMode = false;
STFont * UILine::s_font = NULL;


void UILine::Display()
{
    glLineWidth(2.0);
    
    float b = 1, a = 1;
    
    if(!m_finalized) a = 0.5;
    if(m_highlighted) a = 0.5;
    if(m_editing) a = 0.5;
    
    if(s_deleteMode && m_highlighted)
        glColor4f(1, 0, 0, 1);
    else
        glColor4f(b, b, b, a);
    
    glBegin(GL_LINES);
    glVertex2f(m_start.x, m_start.y);
    glVertex2f(m_end.x, m_end.y);
    glEnd();
    
    STPoint2 triL, triR;
    float xTrans = -15, yTrans = 7;
    float theta = -atan2f(m_end.y-m_start.y, m_end.x-m_start.x);
    triL = STPoint2(m_end.x + (xTrans*cosf(theta)+yTrans*sinf(theta)),
                    m_end.y + (xTrans*-sinf(theta)+yTrans*cosf(theta)));
    triR = STPoint2(m_end.x + (xTrans*cosf(theta)+-yTrans*sinf(theta)),
                    m_end.y + (xTrans*-sinf(theta)+-yTrans*cosf(theta)));
    
    glBegin(GL_TRIANGLES);
    glVertex2f(m_end.x, m_end.y);
    glVertex2f(triL.x, triL.y);
    glVertex2f(triR.x, triR.y);
    glEnd();
    
    glPushMatrix();
    
    glTranslatef(m_start.x, m_start.y, 0);
    
    glColor4f(1, 1, 1, 1);
    
    glBegin(GL_QUADS);
    glVertex2f(0, s_font->GetDescender());
    glVertex2f(0, s_font->GetAscender());
    glVertex2f(s_font->ComputeWidth(m_label), s_font->GetAscender());
    glVertex2f(s_font->ComputeWidth(m_label), s_font->GetDescender());
    glEnd();
    
    s_font->DrawString(m_label, STColor4f(0, 0.8));
    
    glPopMatrix();
}

bool UILine::HitTest(const STPoint2& position)
{
    if(m_startRect.Contains(position) ||
       m_endRect.Contains(position))
        return true;
    
    return false;
}

void UILine::HandleMouseDown(const STPoint2& position, int modifiers)
{
    if(s_deleteMode)
    {
        if(m_deleteCallback)
            m_deleteCallback(this);
    }
    else
    {
        if(m_startRect.Contains(position))
        {
            m_modifyPoint = &m_start;
            m_modifyRect = &m_startRect;
            m_editing = true;
        }
        else if(m_endRect.Contains(position))
        {
            m_modifyPoint = &m_end;
            m_modifyRect = &m_endRect;
            m_editing = true;
        }
    }
}

void UILine::HandleMouseUp(const STPoint2& position, int modifiers)
{
    m_modifyPoint = NULL;
    m_editing = false;
    m_highlighted = false;
}

void UILine::HandleMouseEnter()
{
    m_highlighted = true;
}

void UILine::HandleMouseLeave()
{
    m_highlighted = false;
}

void UILine::HandleMouseMove(const STPoint2& position, int modifiers)
{
    if(m_modifyPoint)
    {
        *m_modifyPoint = position;
        *m_modifyRect = UIRectangle(STPoint2(m_modifyPoint->x - m_endPointWidth, m_modifyPoint->y - m_endPointWidth),
                                    STPoint2(m_modifyPoint->x + m_endPointWidth, m_modifyPoint->y + m_endPointWidth));

    }
}
