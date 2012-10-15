// UIWidget.cpp
#include "UIWidget.h"

#include "st.h"
#include "stgl.h"

//
// Constructor: Initialize a UI widget.
//
UIWidget::UIWidget()
{
}

//
// Destructor: Finalize a UI widget.
//
UIWidget::~UIWidget()
{
}

//
// Display the widget to the current OpenGL context.
//
void UIWidget::Display()
{
}

//
// Determine if this widget or any sub-widget
// is under the specified point.
//
bool UIWidget::HitTest(const STPoint2& position)
{
    return GetRectangle().Contains(position);
}

void UIWidget::HandleMouseDown(const STPoint2& position) {}

void UIWidget::HandleMouseUp(const STPoint2& position) {}

void UIWidget::HandleMouseEnter() {}

void UIWidget::HandleMouseLeave() {}

void UIWidget::HandleMouseMove(const STPoint2& position) {}
