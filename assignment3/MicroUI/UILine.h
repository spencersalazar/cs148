//
//  UILine.h
//  MicroUI
//
//  Created by Spencer Salazar on 10/14/12.
//
//

#ifndef __MicroUI__UILine__
#define __MicroUI__UILine__

#include "UIWidget.h"
#include "STFont.h"
#include <string>

using namespace std;

class UILine : public UIWidget
{
public:
    typedef void (*DeleteCallback)(UILine *);
    
    UILine(STPoint2 start, STPoint2 end, const string &label, DeleteCallback deleteCallback)
    {
        if(s_font == NULL)
            s_font = new STFont("resources/arial.ttf", 12);
        
        m_label = label;
        m_deleteCallback = deleteCallback;
        
        m_finalized = false;
        m_editing = false;
        m_highlighted = false;
        
        m_endPointWidth = 5;
        m_start = start;
        m_end = end;
        m_modifyPoint = NULL;
        
        m_startRect = UIRectangle(STPoint2(m_start.x - m_endPointWidth,m_start.y - m_endPointWidth),
                                  STPoint2(m_start.x + m_endPointWidth,m_start.y + m_endPointWidth));
        m_endRect = UIRectangle(STPoint2(m_end.x - m_endPointWidth,m_end.y - m_endPointWidth),
                                STPoint2(m_end.x + m_endPointWidth,m_end.y + m_endPointWidth));
    }
    
    void setEndpoint(const STPoint2 &endPoint)
    {
        m_end = endPoint;
        m_endRect = UIRectangle(STPoint2(m_end.x - m_endPointWidth,m_end.y - m_endPointWidth),
                                STPoint2(m_end.x + m_endPointWidth,m_end.y + m_endPointWidth));
    }
    
    void finalize() { m_finalized = true; }
    
    virtual void Display();
    virtual bool HitTest(const STPoint2& position);
    
    virtual void HandleMouseDown(const STPoint2& position);
    virtual void HandleMouseUp(const STPoint2& position);
    virtual void HandleMouseEnter();
    virtual void HandleMouseLeave();
    virtual void HandleMouseMove(const STPoint2& position);
    
    static void SetDeleteMode(bool deleteMode) { s_deleteMode = deleteMode; }
    
private:
    static STFont * s_font;
    static bool s_deleteMode;
    
    string m_label;
    DeleteCallback m_deleteCallback;
    
    bool m_finalized;
    bool m_editing;
    bool m_highlighted;
    float m_endPointWidth;
    
    STPoint2 m_start;
    STPoint2 m_end;
    UIRectangle m_startRect;
    UIRectangle m_endRect;
    
    UIRectangle * m_modifyRect;
    STPoint2 * m_modifyPoint;
};

#endif /* defined(__MicroUI__UILine__) */
