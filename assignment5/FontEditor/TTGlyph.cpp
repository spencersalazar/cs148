/*
 *  TTGlyph.cpp
 *  FontEditor
 *
 *  Support functions for Glyph, Contour, and Point data structures.
 *  See the accompanying header file for data structure definitions.
 *
 *  Created by Sonny Chan, January 2009.
 *  Copyright 2009 Stanford University. All rights reserved.
 *
 */


#include "TTGlyph.h"

#include <algorithm>
using namespace std;

// --------------------------------------------------------------------------
// TTPoint

ostream &operator<<(ostream &stream, const TTPoint &p)
{
    stream << p.mCoordinates.x << ' '
        << p.mCoordinates.y << ' '
        << p.mOnCurve;
    return stream;
}

istream &operator>>(istream &stream, TTPoint &p)
{
    stream >> p.mCoordinates.x >> p.mCoordinates.y >> p.mOnCurve;
    return stream;
}

// --------------------------------------------------------------------------
// TTContour

void TTContour::InsertPoint(TTPoint *point, int index)
{
    if (0 <= index && index <= int(mPoints.size())) {
        mPoints.push_back(0);
        copy_backward(mPoints.begin()+index, mPoints.end()-1, mPoints.end());
        mPoints[index] = point;
    }
}

void TTContour::RemovePoint(int index)
{
    if (0 <= index && index < int(mPoints.size())) {
        delete mPoints[index];
        copy(mPoints.begin()+index+1, mPoints.end(), mPoints.begin()+index);
        mPoints.pop_back();
    }
}

void TTContour::ClearPoints()
{
    for (vector<TTPoint*>::iterator it = mPoints.begin(); it != mPoints.end(); ++it)
        delete *it;
    mPoints.clear();
}

ostream &operator<<(ostream &stream, const TTContour &c)
{
    stream << c.NumPoints();
    for (int i = 0; i < c.NumPoints(); ++i)
        stream << ' ' << *c.GetPoint(i);
    stream << endl;
    return stream;
}

istream &operator>>(istream &stream, TTContour &c)
{
    int n;
    stream >> n;
    c.ClearPoints();
    for (int i = 0; i < n; ++i) {
        TTPoint *p = new TTPoint();
        stream >> *p;
        c.AddPoint(p);
    }
    return stream;
}

// --------------------------------------------------------------------------
// TTGlyph

TTGlyph::TTGlyph(int lsbearing, int advance)
{
    mLeftSideBearing    = lsbearing;
    mAdvanceWidth       = advance;
}

int TTGlyph::NumPoints() const
{
    int total = 0;
    for (vector<TTContour*>::const_iterator it = mContours.begin(); it != mContours.end(); ++it)
        total += (*it)->NumPoints();
    return total;
}

void TTGlyph::InsertContour(TTContour* contour, int index)
{
    if (0 <= index && index <= int(mContours.size())) {
        mContours.push_back(0);
        copy_backward(mContours.begin()+index, mContours.end()-1, mContours.end());
        mContours[index] = contour;
    }
}

void TTGlyph::RemoveContour(int index)
{
    if (0 <= index && index < int(mContours.size())) {
        delete mContours[index];
        copy(mContours.begin()+index+1, mContours.end(), mContours.begin()+index);
        mContours.pop_back();
    }
}

void TTGlyph::ClearContours()
{
    for (vector<TTContour*>::const_iterator it = mContours.begin(); it != mContours.end(); ++it)
        delete *it;
    mContours.clear();
}

ostream &operator<<(ostream &stream, const TTGlyph &g)
{
    stream << g.GetLeftSideBearing() << ' ' << g.GetAdvanceWidth() << ' '
        << g.NumContours() << endl;
    for (int i = 0; i < g.NumContours(); ++i)
        stream << *g.GetContour(i);
    return stream;
}

istream &operator>>(istream &stream, TTGlyph &g)
{
    int lsb, aw, n;
    stream >> lsb >> aw >> n;
    g.ClearContours();
    g.SetLeftSideBearing(lsb);
    g.SetAdvanceWidth(aw);
    for (int i = 0; i < n; ++i) {
        TTContour *c = new TTContour();
        stream >> *c;
        g.AddContour(c);
    }
    return stream;
}
