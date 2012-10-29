//
//  UICurvePoint.h
//  FontEditor
//
//  Created by Spencer Salazar on 10/28/12.
//
//

#ifndef __FontEditor__UICurvePoint__
#define __FontEditor__UICurvePoint__

#include "UIWidget.h"
#include "st.h"
#include "stglut.h"
#include "TTGlyph.h"

class UICurvePoint;

typedef void (*CPSelectionCallback)(UICurvePoint *);
typedef void (*CPMoveCallback)(UICurvePoint *);

class UICurvePoint : public UIWidget
{
public:
    UICurvePoint(TTPoint * pt, CPSelectionCallback callback, CPMoveCallback moveCallback) :
    m_point(pt),
    m_mouseOver(false),
    m_mouseDown(false),
    m_selected(false),
    m_callback(callback),
    m_moveCallback(moveCallback)
    { }
    
    void Display();
    
    bool HitTest(const STPoint2& position);
    
    void HandleMouseEnter();
    void HandleMouseLeave();
    
    virtual void HandleMouseDown(const STPoint2& position);
    virtual void HandleMouseUp(const STPoint2& position);
    virtual void HandleMouseMove(const STPoint2& position);
    
    void Deselect() { m_selected = false; }
    inline TTPoint * GetPoint() { return m_point; }
    
protected:
    
    CPSelectionCallback m_callback;
    CPMoveCallback m_moveCallback;
    
    bool m_selected;
    bool m_mouseDown;
    bool m_mouseOver;
    TTPoint * m_point;
};


#endif /* defined(__FontEditor__UICurvePoint__) */
