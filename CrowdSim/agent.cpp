#include "agent.h"


// thread safe, unlike rand()
mt19937 agent::mt = mt19937(random_device()());
uniform_int_distribution<GLint> agent::distribution = uniform_int_distribution<GLint>(0,999);

vector<agent*> agent::crowd;


agent::agent(GLfloat x, GLfloat y, GLfloat z):radius((z-100)/2) {
    
    //vMax = distribution(mt)/10000.0;
    vMax = 0.04;
    
    // used for the first 2 cases
    coord.x = x; coord.y = y; coord.z = z;

    //@@@ choose the same destination for everyone
    //dest.x = 40;
    //dest.y = -40;
    
    //@@@ choose a random destination within the borders of the square
    // dest.x = distribution(mt)%100 - 49;
    // dest.y = distribution(mt)%100 - 49;
    
    //@@@ choose a random destination outside the borders of the square
    if (distribution(mt) % 2 == 1) {
        coord.x = 51;
        coord.y = distribution(mt)%100 - 49;
    } else {
        coord.x = distribution(mt)%100 - 49;
        coord.y = 51;
    }
    
    if (distribution(mt) % 2 == 1) {
        dest.x = -51;
        dest.y = distribution(mt)%100 - 49;
    } else {
        dest.x = distribution(mt)%100 - 49;
        dest.y = -51;
    }
    
    dest.z = z;
    
    acc = 0.001;
    
    // choose a random color in order to distinguish between different agents
    color.x = (distribution(mt))/1000.0;
    color.y = (distribution(mt))/1000.0;
    color.z = (distribution(mt))/1000.0;
}



void agent::draw()
{
    // draw the agent
    glPushMatrix();
    glTranslatef(coord.x, coord.y, coord.z);
    glColor3f(color.x, color.y, color.z);
    gluSphere(gluNewQuadric(), radius, 10, 10);
    glPopMatrix();
    
    
    // draw the destinations in white
    glPushMatrix();
    glTranslatef(dest.x, dest.y, dest.z);
    glColor3f(1, 1, 1);
    gluSphere(gluNewQuadric(), radius, 10, 10);
    glPopMatrix();
    
    curVelocity = newVelocity;
}



void agent::move()
{
    if (coord == dest)
    {
        newVelocity.x = newVelocity.y = curVelocity.x = curVelocity.y = 0;
        
        // change the destination of the agent
        switch(distribution(mt)%4) {
            case 0: {
                dest.x = 51;
                dest.y = distribution(mt)%100 - 49;
                break;
            }
                
            case 1: {
                dest.x = distribution(mt)%100 - 49;
                dest.y = 51;
                break;
            }
                
            case 2: {
                dest.x = -51;
                dest.y = distribution(mt)%100 - 49;
                break;
            }
                
            case 3: {
                dest.x = distribution(mt)%100 - 49;
                dest.y = -51;
                break;
            }
        }
    }
    else
    {
        // Get the optimal angle
        vec3 delta;
        delta = dest - coord;
        GLdouble targetAngleRad = atan2(delta.y, delta.x);
        //GLdouble targetAngle = (targetAngleRad / PI) * 180;
        
        
        // Do the heuristic calculations
        //#####################################################################################
        GLdouble closestAgentDistance = 100;
        GLdouble angleToClosestAgent = 0;
        
        GLdouble bestAngleRad = targetAngleRad;
        GLdouble curAngleRad = targetAngleRad - checkAngle;
        GLdouble bestDistance = 1e100;
        
        while (curAngleRad <= targetAngleRad + checkAngle)
        {
            GLdouble closestDistanceAtCurAngle = distanceAtAngle(curAngleRad);
            GLdouble optimizationEquationAtCurAngle = optimizationEquation(curAngleRad, closestDistanceAtCurAngle, targetAngleRad);
            
            
            if (optimizationEquationAtCurAngle < bestDistance)
            {
                bestAngleRad = curAngleRad;
                bestDistance = optimizationEquationAtCurAngle;
            }
            
            if (closestDistanceAtCurAngle < closestAgentDistance)
            {
                closestAgentDistance = closestDistanceAtCurAngle;
                angleToClosestAgent = curAngleRad;
            }
            
            curAngleRad += 0.0523599;
        }
        //#####################################################################################

        // Used for determining the velocity calculations of this loop
        GLdouble tempVelocity = curVelocity.length();
        
        
        
        // Hard Dodge to the right if the agent is coming too close to another one
        //#####################################################################################
        if (closestAgentDistance <= 3*radius && distanceAtAngle(angleToClosestAgent - PI/2) > 2*vMax)
        {
            bestAngleRad = angleToClosestAgent - PI/2;
            tempVelocity = vMax;
        }
        else
        {
            //@@@ uncomment bellow to remove effect of the heuristic algorithm
            // bestAngle = targetAngle;
            if (closestAgentDistance >= 2) {
                tempVelocity = min(vMax, tempVelocity + acc);
            } else {
                tempVelocity = max(0.0, tempVelocity - acc);
            }
            
            tempVelocity =  min(tempVelocity, closestAgentDistance / 2);
        }
        //#####################################################################################

        
        newVelocity.x = tempVelocity * cos(bestAngleRad);
        newVelocity.y = tempVelocity * sin(bestAngleRad);
    }
    
    // update the position
    coord.x += curVelocity.x;
    coord.y += curVelocity.y;
    
    
}

double agent::optimizationEquation(double rad, double distance, double targetAngleRad)
{
    return (
            (checkLength*checkLength)
            +(distance*distance)
            -(4 * checkLength * distance * cos(targetAngleRad - rad))
            );
}




bool agent::isBehindMe (agent* a)
{
    return ((dest-coord).dot(a->coord-coord) < 0);
}

double agent::distanceAtAngle(GLdouble rad)
{
    /*
    double rad = (angle / 180.0) * PI;
    vec3 projection;
    projection.x = cos(rad);
    projection.y = sin(rad);
    projection.z = 0;
    projection = (coord + projection*curVelocity.length());
    
    double d = checkLength;
    
    for (int i = 0; i < crowd.size(); i++)
    {
        if (i == idInCrowd || (crowd[i]->coord - coord).length() > d || isBehindMe(crowd[i])) continue;
        
        double distance = (projection-crowd[i]->coord + crowd[i]->curVelocity).length();
        
        if (distance <= 2*radius)
        {
            double curD = (crowd[i]->coord - coord).length() - (radius * 2);
            if (curD < d) {
                
                if (crowd[i]->curVelocity.length() > 0.01) {
                    d = curD;
                } else {
                    d = checkLength;
                }
            }
        }
    }
    */
    
    
    vec3 end = coord;
    end.x += checkLength*cos(rad);
    end.y += checkLength*sin(rad);

    GLdouble d = checkLength;
    
    for (int i = 0; i < crowd.size(); i++)
    {
        if (i == idInCrowd || (crowd[i]->coord - coord).length() > d || isBehindMe(crowd[i])) continue;
        
        GLdouble distance = distanceToPath(end, crowd[i]->coord + crowd[i]->curVelocity);
        
        // if the agent is coliding with my line
        if (distance <= 2*radius)
        {
            GLdouble curD = (crowd[i]->coord - coord).length() - (radius * 2);
            
            // if it's the closest coliding agent so far
            if (curD < d) {
                // if it's actually moving
                if (crowd[i]->curVelocity.length() > 0.01) {
                    d = curD;
                }
            }
        }
    }
    
    return d;
}


// Calculates the distance between the agent "target" and the line extending from the current position to "end"
double agent::distanceToPath(const vec3& end, const vec3& target) {
     GLdouble distance = (abs(
     ((end.y - coord.y) * target.x)
     -((end.x - coord.x) * target.y)
     +(end.x * coord.y)
     -(end.y * coord.x)))
     /
     sqrt(
     ((end.y - coord.y) * (end.y - coord.y))
     +((end.x - coord.x) * (end.x - coord.x))
     );
     
     return distance;
}




