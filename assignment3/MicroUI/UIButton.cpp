// UIButton.cpp
#include "UIButton.h"

#include "st.h"
#include "stgl.h"

//
// Constructor: Initialize a UI button with
// a given font face and label text.
//
UIButton::UIButton(STFont* font,
                   const std::string& text,
                   FireCallback callback)
    : mFont(font)
    , mText(text)
    , mCallback(callback)
    , mState(IDLE_OUTSIDE)
{
}

/** CS 148 TODO:
*
*   Implement your button here.
*/

void UIButton::setText(std::string text)
{
    mText = text;
}


void UIButton::Display()
{
    float width = GetRectangle().pMax.x - GetRectangle().pMin.x;
    float height = GetRectangle().pMax.y - GetRectangle().pMin.y;
    mTextWidth = mFont->ComputeWidth(mText);
    float b, a;
    
    glPushMatrix();
    
    glTranslatef(GetRectangle().pMin.x, GetRectangle().pMin.y, 0);
    
    // color black if pressed and with slight alpha if mouse is outside
    b = 1;
    a = 1;
    if(mState == PRESSED_INSIDE || mState == PRESSED_OUTSIDE)
        b = 0.5;
    if(mState == PRESSED_OUTSIDE || mState == IDLE_OUTSIDE)
        a = 0.5;
    
    glBegin(GL_QUADS);
    glColor4f(b, b, b, a);
    glVertex2f(0, 0);
    glVertex2f(width, 0);
    glVertex2f(width, height);
    glVertex2f(0, height);
    glEnd();
    
    glPushMatrix();
    
    // center text
    glTranslatef(width/2.0-mTextWidth/2.0, -mFont->GetDescender(), 0);
    
    // color with slight alpha if mouse is outside
    b = 0;
    a = 1;
    if(mState == PRESSED_OUTSIDE || mState == IDLE_OUTSIDE)
        a = 0.5;
    
    mFont->DrawString(mText, STColor4f(b, a));
    
    glPopMatrix();
    glPopMatrix();
}

void UIButton::HandleMouseDown(const STPoint2& position)
{
    mState = PRESSED_INSIDE;
}

void UIButton::HandleMouseUp(const STPoint2& position)
{
    if(mState == PRESSED_INSIDE)
    {
        if(mCallback != NULL)
            mCallback(this);
        mState = IDLE_INSIDE;
    }
    else
    {
        mState = IDLE_OUTSIDE;
    }
}

void UIButton::HandleMouseEnter()
{
    if(mState == PRESSED_OUTSIDE)
        mState = PRESSED_INSIDE;
    else
        mState = IDLE_INSIDE;
}

void UIButton::HandleMouseLeave()
{
    if(mState == PRESSED_INSIDE)
        mState = PRESSED_OUTSIDE;
    else
        mState = IDLE_OUTSIDE;
}
