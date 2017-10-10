
#include "simpletracker.h"
#include "TrackingDrawer.h"
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glut.h>

//TrackingDrawer drawer1, drawer2;
SimpleTracker tracker1, tracker2;

void glutIdle()
{
	glutPostRedisplay();
}
void glutDisplay()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glLoadIdentity();
    tracker1.update();
    tracker2.update();
//     drawer1.Draw();
//     drawer2.Draw();
//     if(tracker1.isTracked()) {
//         drawer1.DrawSkeleton();
//     }
//     if(tracker2.isTracked()) { 
//         drawer2.DrawSkeleton();
//     }
    glutSwapBuffers();
}

int main(int argc, char** argv)
{    
    tracker1.init(0);
    tracker2.init(3);
    std::cout << "load trakcer done" << std::endl;
//     drawer1.Init(tracker1.getFrame(), 0, 0);
//     drawer2.Init(tracker2.getFrame(), GL_WIN_SIZE_X/2, 0);    
//     
    glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(800, 640);//GL_WIN_SIZE_X, GL_WIN_SIZE_Y);
	glutCreateWindow ("Test1");
	//glutFullScreen();
	//glutSetCursor(GLUT_CURSOR_NONE);

	glutDisplayFunc(glutDisplay);
	glutIdleFunc(glutIdle);

	//glDisable(GL_DEPTH_TEST);
	//glEnable(GL_TEXTURE_2D);

	//glEnableClientState(GL_VERTEX_ARRAY);
	//glDisableClientState(GL_COLOR_ARRAY);
    
	glutMainLoop();    
    
    return 0;
}
