// UIButton.h
#ifndef __UIBUTTON_H__
#define __UIBUTTON_H__

/**
* A UIButton is a widget with a simple
* label that responds to mouse events.
*/

#include "UIWidget.h"

class UIButton :
    public UIWidget
{
public:
    // Type of callbacks to handle button-press "fire" events.
    typedef void (*FireCallback)(UIButton* button);

    //
    // Constructor: Initialize a UI button with
    // a given font face and label text.
    //
    UIButton(STFont* font,
             const std::string& text,
             FireCallback callback);

    /** CS 148 TODO:
    *
    *   Your button will need to override some or
    *   all of the virtual functions in UIWidget
    *   in order to display itself and respond
    *   to mouse events.
    */
    
    virtual void Display();
    
    virtual void HandleMouseDown(const STPoint2& position);
    virtual void HandleMouseUp(const STPoint2& position);
    virtual void HandleMouseEnter();
    virtual void HandleMouseLeave();
    
    void setText(std::string text);

private:
    // Font to use for text
    STFont* mFont;

    // Text
    std::string mText;
    float mTextWidth;

    // "Fire" event callback
    FireCallback mCallback;

    /** CS 148 TODO:
    *
    *   Your button will need some member variables
    *   to hold its state.
    */
    enum State
    {
        IDLE_OUTSIDE,
        IDLE_INSIDE,
        PRESSED_INSIDE,
        PRESSED_OUTSIDE,
    };
    
    State mState;
};

#endif // __UIBUTTON_H__
