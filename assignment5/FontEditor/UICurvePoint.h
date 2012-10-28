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


class UICurvePoint : public UIWidget
{
public:
    UICurvePoint(const TTPoint * pt) :
    m_point(pt)
    { }
    
    void Display();
    
protected:
    
    const TTPoint * m_point;
};


#endif /* defined(__FontEditor__UICurvePoint__) */
