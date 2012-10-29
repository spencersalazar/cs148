//
//  loadsave.cpp
//  FontEditor
//
//  Created by Spencer Salazar on 10/28/12.
//
//

#include "loadsave.h"


static const char *gFileName = "glyph.txt";


void saveGlyph(TTGlyph * g)
{
    FILE * f = fopen(gFileName, "w");
    
    if(!f)
    {
        fprintf(stderr, "saveGlyph: unable to open file for writing\n");
        return;
    }
    
    for(int c = 0; c < g->NumContours(); c++)
    {
        fprintf(f, "contour\n");
        
        TTContour * contour = g->GetContour(c);
        for(int p = 0; p < contour->NumPoints(); p++)
        {
            TTPoint * point = contour->GetPoint(p);
            fprintf(f, "point %i %f %f\n", point->mOnCurve,
                    point->mCoordinates.x, point->mCoordinates.y);
        }
        
        fprintf(f, "\n");
    }
    
    fclose(f);
}


TTGlyph * loadGlyph()
{
    int BUF_SIZE = 256;
    char buf[BUF_SIZE];
    TTGlyph * g = new TTGlyph;
    TTContour * c = NULL;
    
    FILE * f = fopen(gFileName, "r");
    if(!f)
    {
        fprintf(stderr, "loadGlyph: unable to open file for reading\n");
        return g;
    }
    
    while(fgets(buf, BUF_SIZE, f))
    {
        if(strncmp(buf, "contour", sizeof("contour")-1) == 0)
        {
            c = new TTContour;
            g->AddContour(c);
        }
        else if(strncmp(buf, "point", sizeof("point")-1) == 0)
        {
            if(c != NULL)
            {
                float x = 0, y = 0;
                int onCurve = 1;
                if(sscanf(buf, "point %i %f %f", &onCurve, &x, &y) == 3)
                {
                    TTPoint * p = new TTPoint;
                    p->mOnCurve = onCurve;
                    p->mCoordinates = STPoint2(x, y);
                    c->AddPoint(p);
                }
            }
        }
    }
    return g;
}
    
