#include <iostream>
#include <GLUT/GLUT.h>
#include <tbb/task.h>
#include <tbb/parallel_for.h>
#include <tbb/blocked_range2d.h>
#include <tbb/scalable_allocator.h>
#include <vector>
#include <random>
#include "agent.h"

using namespace std;
using namespace tbb;

void* operator new(size_t size) {
    return scalable_malloc(size);
}

void* operator new[](size_t size) {
    return scalable_malloc(size);
}

void operator delete(void* ptr) {
    scalable_free(ptr);
}

void operator delete[](void* ptr) {
    scalable_free(ptr);
}

vector<agent, scalable_allocator<agent>> agents;

void updateAgents() {
    const unsigned int agentsPerThread = 10;
    
    parallel_for(blocked_range<size_t>(0, agents.size(), agentsPerThread), [](blocked_range<size_t>& range) {
        for (size_t i=range.begin(); i!=range.end(); i++) {
            agents[i].move();
        }
    });
    
    glutPostRedisplay();
}

void init() {
    glClearColor (1.0,1.0,1.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    
    // make it 50 all around if u wanna hide the border
    //***************************************************************
    glOrtho(-50, 50, -50, 50, 1, 200);
    //***************************************************************
    
    gluLookAt(0, 0, 200,
              0, 0, 0,
              0, 1, 0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    
    for (int i=0; i<100; i++) {
        agents.push_back(agent((i/10-5)*2,(i%10+10)*2, 101));
    }

    for (int i=0; i<100; i++) {
        agent::crowd.push_back(&agents[i]);
        agents[i].idInCrowd = i;
    }
    
}

void draw() {
    glClear(GL_COLOR_BUFFER_BIT);
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
    
    glColor3f(0, 0, 0);
    
    glutSolidCube(100);
    
    for (int i=0; i<agents.size(); i++)
        agents[i].draw();
    
    
    glFlush();
}

void mouseHandler(int button, int state, int x, int y) {

}

int main(int argc, char * argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(300, 200);
    glutCreateWindow("Crowd Simulation");
    glutDisplayFunc(draw);
    glutIdleFunc(updateAgents);
    init();
    
    
    glutMouseFunc(mouseHandler);
    
    glutMainLoop();
    
    return 0;
}


