#ifndef __TBBCrowdSimulation__agent__
#define __TBBCrowdSimulation__agent__

#include <GLUT/GLUT.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <random>
using namespace std;

#define PI 3.14159265


struct vec3 {
    GLfloat x;
    GLfloat y;
    GLfloat z;
    
    
    vec3 ():x(0), y(0), z(0){};
    vec3 (const GLfloat& a, const GLfloat& b, const GLfloat& c):x(a), y(b), z(c){};
    
    GLdouble length() {
        return sqrt((x*x) + (y*y));
    }
    
    vec3 operator * (const GLfloat f) {
        return vec3(x*f, y*f, z*f);
    }
    
    vec3 operator / (const GLfloat f) {
        return vec3(x/f, y/f, z/f);
    }
    
    vec3 operator + (const vec3& c) {
        return vec3(x+c.x, y+c.y, z+c.z);
    }
    
    vec3 operator - (const vec3& c) {
        return vec3(x-c.x, y-c.y, z-c.z);
    }
    
    vec3 cross (const vec3& c)
    {
        return vec3(y*c.z - c.y*z,
                          -x*c.z + c.x*z,
                          x*c.y - c.x*y);
    }
    
    GLfloat dot (const vec3& c)
    {
        return (x*c.x + y*c.y + z*c.z);
    }
    
    bool operator == (const vec3& c) {
        return (abs(x - c.x)<1e-1 && abs(y - c.y)<1e-1 && abs(z - c.z)<1e-1);
    }
    
};



class agent{
    
private:
    vec3 color;
    vec3 dest;
    GLfloat radius;
    GLdouble vMax;
    
    static const GLint checkLength = 5;
    constexpr static const double checkAngle = 1.0472;
    
    bool isBehindMe (agent* a);
    double optimizationEquation(GLdouble rad, GLdouble distance, GLdouble targetAngleRad);
    double distanceToPath(const vec3& projection, const vec3& target);
    double distanceAtAngle(GLdouble angle);
    
public:
    
    static vector<agent*> crowd;
    double curAngle;
    GLint idInCrowd;
    
    double acc;
    
    vec3 curVelocity, curDirection, newVelocity, newDirection, coord;
    
    agent(GLfloat x, GLfloat y, GLfloat z);
    void draw();
    void move();
    

    
    //static random_device rd;
    static mt19937 mt;
    static uniform_int_distribution<GLint> distribution;
};

#endif
