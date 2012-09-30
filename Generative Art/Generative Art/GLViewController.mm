//
//  GLViewController.m
//  MobileMusic2
//
//  Created by Spencer Salazar on 7/9/12.
//  Copyright (c) 2012 Stanford University. All rights reserved.
//

#import "GLViewController.h"
#import "Geometry.h"
#import <list>
#import <map>
#import "Texture.h"
#import "hsv.h"


using namespace std;


class Random
{
public:
    static float unit()
    {
        return arc4random()/((float) UINT_MAX);
    }
};


class Renderable
{
public:
    virtual void init() = 0;
    virtual void update(float dt, float t) = 0;
    virtual void render() = 0;
};


class FlareCluster : public Renderable
{
public:
    FlareCluster()
    {
        m_mainParticle.init();
    }
    
    void init()
    {
        m_mainParticle.init();
        m_mainParticle.scale = 0.25;
        m_mainParticle.ttl = FLT_MAX;
        m_mainParticle.alpha = 0.8;
        m_tex = loadTexture(@"flare.png");
    }
    
    void update(float dt, float t)
    {
        int num = 1+Random::unit()*2;
        for(int i = 0; i < num; i++)
        {
            Particle p;
            p.init();
            p.scale = m_mainParticle.scale * 0.15;
            p.p = m_mainParticle.p + GLvertex2f(Random::unit()*2-1,Random::unit()*2-1)*p.scale;
            p.v = GLvertex2f(Random::unit()*2-1, -Random::unit())*0.2;
            p.a = GLvertex2f(0, 0.2);
            p.ttl = 1+Random::unit()*0.4;
            p.alpha = 0.15;
            
            m_particles.push_back(p);
        }
        
        m_mainParticle.scale = 0.25 + 0.025*sinf(2*M_PI*0.55*t);
        m_mainParticle.alpha = 0.8 + 0.1*sinf(2*M_PI*0.45*t);
        
        m_mainParticle.update(dt, t);
        
        for(list<Particle>::iterator i = m_particles.begin();
            i != m_particles.end(); i++)
            i->update(dt, t);
        
        for(list<Particle>::iterator i = m_particles.begin();
            i != m_particles.end(); )
        {
            list<Particle>::iterator j = i++;
            if(j->ttl <= 0)
                m_particles.erase(j);
        }
    }
    
    void render()
    {
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        // normal blending
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        // additive blending
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, m_tex);
        
        m_mainParticle.render();
        m_mainParticle.render();
        m_mainParticle.render();
        
        for(list<Particle>::iterator i = m_particles.begin();
            i != m_particles.end(); i++)
        {
            i->render();
            i->render();
            i->render();
        }
    }
    
protected:
    
    class Particle : public Renderable
    {
    public:
        void init()
        {
            Geometry::makeSquare(rect[0], rect[1], 1);
            
            alpha = 1;
            scale = 1;
            
            p = GLvertex2f();
            v = GLvertex2f();
            a = GLvertex2f();
            
            ttl = 0;
        }
        
        void update(float dt, float t)
        {
            p = p + v*dt;
            v = v + a*dt;
            ttl -= dt;
            
            float fadeOutTime = 0.35;
            
            float a = alpha;
            _scale = scale;

            if(ttl < fadeOutTime)
            {
                a *= ttl/fadeOutTime;
                _scale = scale * ttl/fadeOutTime;
            }
            
            rect[0].a.color.a = rect[0].b.color.a = rect[0].c.color.a = a;
            rect[1].a.color.a = rect[1].b.color.a = rect[1].c.color.a = a;
        }
        
        void render()
        {
            glPushMatrix();
            
            glTranslatef(p.x, p.y, 0);
            glScalef(_scale, _scale, _scale);
            
            glVertexPointer(3, GL_FLOAT, sizeof(GLgeoprimf), &rect[0].a.vertex);
            glEnableClientState(GL_VERTEX_ARRAY);
            
            glTexCoordPointer(2, GL_FLOAT, sizeof(GLgeoprimf), &rect[0].a.texcoord);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            
            glColorPointer(4, GL_FLOAT, sizeof(GLgeoprimf), &rect[0].a.color);
            glEnableClientState(GL_COLOR_ARRAY);
            
            glDisableClientState(GL_NORMAL_ARRAY);
            
            glDrawArrays(GL_TRIANGLES, 0, 6);
            
            glPopMatrix();
        }
        
        GLvertex2f p;
        GLvertex2f v;
        GLvertex2f a;
        
        GLtrif rect[2];
        
        float ttl;
        float scale;
        float _scale;
        float alpha;
    };
    
    GLuint m_tex;
    Particle m_mainParticle;
    list<Particle> m_particles;
    
    
};


//------------------------------------------------------------------------------
// name: uiview2gl
// desc: convert UIView coordinates to the OpenGL coordinate space
//------------------------------------------------------------------------------
GLvertex2f uiview2gl(CGPoint p, UIView * view)
{
    GLvertex2f v;
    float aspect = fabsf(view.bounds.size.width / view.bounds.size.height);
    v.x = ((p.x - view.bounds.origin.x)/view.bounds.size.width)*2-1;
    v.y = (((p.y - view.bounds.origin.y)/view.bounds.size.height)*2-1)/aspect;
    return v;
}


class Segment : public Renderable
{
public:
    void init()
    {
        completion = 0;
        paused = false;
        v = 1.5;
    }
    
    virtual void update(float dt, float t)
    {
        if(!paused && completion < 1)
        {
            completion += dt*v;
            if(completion > 1)
                completion = 1;
        }
        
        g[0].vertex = start;
        g[1].vertex = start + (end - start)*completion;
        
        g[0].color = startColor;
        g[1].color = startColor + (endColor - startColor)*completion;
    }
    
    virtual void render()
    {
        glVertexPointer(3, GL_FLOAT, sizeof(GLgeoprimf), &g[0].vertex);
        glEnableClientState(GL_VERTEX_ARRAY);
        
        glColorPointer(4, GL_FLOAT, sizeof(GLgeoprimf), &g[0].color);
        glEnableClientState(GL_COLOR_ARRAY);
        
        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        
        glDrawArrays(GL_LINES, 0, 2);
    }

    bool paused;
    float v;
    float completion;
    GLvertex2f start, end;
    GLcolor4f startColor, endColor;
    GLgeoprimf g[2];
};


const float PATH_LENGTH = 0.25;


@interface GLViewController ()
{
    FlareCluster cluster;
    NSTimeInterval t;
    
    map<UITouch *, GLvertex2f> targets;
    list<Segment> segments;
    list<Segment> activeSegments;
}

@property (strong, nonatomic) EAGLContext *context;

@end

@implementation GLViewController

@synthesize context;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    
    self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
    
    if (!self.context) {
        NSLog(@"Failed to create ES context");
    }
    
    GLKView *view = (GLKView *)self.view;
    view.context = self.context;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    
    [EAGLContext setCurrentContext:self.context];
    
    t = 0;
    
    cluster.init();
    
    float divisions = 3;
    float i = floorf(Random::unit()*divisions);
    Segment newSeg;
    newSeg.init();
    newSeg.startColor = hsv2rgb(GLcolor4f(Random::unit(), Random::unit(), 0.9, 1));
    newSeg.start = GLvertex2f(0,0);
    newSeg.endColor = hsv2rgb(GLcolor4f(Random::unit(), Random::unit(), 0.9, 1));
    newSeg.end = GLvertex2f::fromPolar(PATH_LENGTH, 2*M_PI*i/divisions);
    
    activeSegments.push_back(newSeg);
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
    
    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }
	self.context = nil;
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
	return YES;
}

- (void)update
{
    float dt = self.timeSinceLastUpdate;
    t += dt;
    
    cluster.update(dt, t);
    
    bool paused = targets.size() == 0;
    
    for(list<Segment>::iterator s = activeSegments.begin();
        s != activeSegments.end(); )
    {
        list<Segment>::iterator s2 = s++;
        
        s2->paused = paused;
        s2->update(dt, t);
        
        if(s2->completion >= 1)
        {
            Segment oldSeg = *s2;
            segments.push_back(oldSeg);
            activeSegments.erase(s2);
            
            if(activeSegments.size() < targets.size())
            {
                // add new segment
                float divisions = 2 + targets.size();
                
                Segment newSeg;
                newSeg.init();
                newSeg.startColor = oldSeg.endColor;
                newSeg.start = oldSeg.end;
                newSeg.endColor = hsv2rgb(GLcolor4f(Random::unit(), Random::unit(), 0.9, 1));
                
                // find acceptable angle that is closest to target (touch)
                float closest = FLT_MAX;
                GLvertex2f end;
                GLvertex2f touch = targets.begin()->second;
                for(float i = 0; i < divisions; i++)
                {
                    GLvertex2f v = newSeg.start + GLvertex2f::fromPolar(PATH_LENGTH, 2*M_PI*i/divisions);
                    float magSq = (touch-v).magnitudeSquared();
                    if(magSq < closest && (v-oldSeg.start).magnitudeSquared() > 0.001)
                    {
                        closest = magSq;
                        end = v;
                    }
                }
                
                newSeg.end = end;
                
                activeSegments.push_back(newSeg);
            }
        }
    }

    for(list<Segment>::iterator s = segments.begin(); s != segments.end(); s++)
        s->update(dt, t);
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    /*** clear ***/
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    /*** setup projection matrix ***/
    float aspect = fabsf(self.view.bounds.size.width / self.view.bounds.size.height);
    GLKMatrix4 projectionMatrix = GLKMatrix4MakeOrtho(-1, 1, 1.0/aspect, -1.0/aspect, -1, 100);
    
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(projectionMatrix.m);
    
    /*** set model + view matrix ***/
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_LINE_SMOOTH);
    glLineWidth(2.0);
    glDisable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    for(list<Segment>::iterator s = segments.begin(); s != segments.end(); s++)
        s->render();
    for(list<Segment>::iterator s = activeSegments.begin();
        s != activeSegments.end(); s++)
        s->render();
    
    if(targets.size())
    {
        GLvertex2f touch = targets.begin()->second;
        glTranslatef(touch.x, touch.y, 0);
    }
    
    glScalef(0.5, 0.5, 0.5);
    cluster.render();
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    for(UITouch * touch in touches)
    {
        targets[touch] = uiview2gl([touch locationInView:self.view], self.view);
    }
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    for(UITouch * touch in touches)
    {
        targets[touch] = uiview2gl([touch locationInView:self.view], self.view);
    }
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    for(UITouch * touch in touches)
    {
        targets.erase(touch);
    }
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    [self touchesEnded:touches withEvent:event];
}



@end
