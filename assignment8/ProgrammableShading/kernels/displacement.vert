// displacement.vert

/*
 This GLSL vertex shader performs displacement mapping
 using an analytical displacement function.
 */

// This 'varying' vertex output can be read as an input
// by a fragment shader that makes the same declaration.
varying vec2 texPos;
varying vec3 modelPos;
varying vec3 lightSource;
varying vec3 normal;

varying float displacement;

uniform float t;

float PI = 3.14159265358979323846264;
float TWOPI = 2.0*PI;

float rand(vec2 co)
{
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

//float disp(in float x, in float y)
//{
//    float A = 0.2;
//    float f = 4.0;
//    float A_mod = 0.5;
//    float f_mod = f*0.025;
//    float f_t = 0.125;
//    float scale_t = 0.1;
//    
//    float x_mod = A_mod*(cos(f_mod*TWOPI*(x+t*scale_t)));
//    float y_mod = A_mod*(cos(f_mod*TWOPI*(y+t*scale_t)));
//    
//    return A*
//    cos((f+x_mod)*TWOPI*(x+t*scale_t+0.001*rand(vec2(t,t+0.1))))*
//    cos((f+y_mod)*TWOPI*(y-t*scale_t+0.001*rand(vec2(t,t-0.1))))*
//    sin(f_t*TWOPI*t)*sin(f_t*TWOPI*t);
//}


float disp(in float x, in float y)
{
    float x_sym = x*2.0-1.0;
    float y_sym = y*2.0-1.0;
    
    float A = -0.1;
    float f = 8.0;
    
    float r = sqrt(x_sym*x_sym+y_sym*y_sym);
    
    return A*sin(f*TWOPI*(r-0.175*t))*exp(-r*4.0)*exp(-t*0.25);
}

void main()
{
    // Tell the fragment shader we have done vertex displacement
    displacement = 1.0;
    
	normal = gl_Normal.xyz;
	modelPos = gl_Vertex.xyz;
    
    // Copy the standard OpenGL texture coordinate to the output.
    texPos = gl_MultiTexCoord0.xy;
    
	/* CS 148 TODO: Modify 'modelPos' and 'normal' using your displacment function */
    
    float d = disp(texPos.x, texPos.y);
    modelPos = modelPos + normal*d;
    
    float delta = 0.0001;
    vec3 t1 = normalize(vec3(1, (disp(texPos.x+delta, texPos.y)-d)/delta, 0));
    vec3 t2 = normalize(vec3(0, -(disp(texPos.x, texPos.y+delta)-d)/delta, -1));
    normal = cross(t1, t2);
    
    // Render the shape using modified position.
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix *  vec4(modelPos,1);
    
    // we may need this in the fragment shader...
    modelPos = (gl_ModelViewMatrix * vec4(modelPos,1)).xyz;
    
	// send the normal to the fragment shader
	normal = (gl_NormalMatrix * normal);
    
    // pass the light source position to the fragment shader
    lightSource = gl_LightSource[0].position.xyz;
}
