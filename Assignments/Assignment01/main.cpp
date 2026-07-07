#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/freeglut.h>
#include <cstdio>

double red = 0.00;

void myDisplay() {
	// clear the viewport
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    //opengl draw calls

    // swap buffers
    glutSwapBuffers();
}

void myKeyboard(int key, int x, int y) {
    
	
    switch (key) {
	case 27:
        glutLeaveMainLoop();
        break;
	}
	// modifiers can be checked with
    // glutGetModifiers()
}

void myMouse(int button, int state, int x, int y) {
	// name of button clicked, up/down, location
    // 1 call for up, 1 call for down
}

void myMouseMotion(int x, int y) {
	// handle mouse motion while a button is down
}

void myMousePassive(int x, int y) {
	// handle mouse motion while mouse is not down
}

void myIdle() {
	// handle animations
    glClearColor(red, 0, 0, 0);
    if (glutGet(GLUT_ELAPSED_TIME) % 20 == 0) {
        if (red<=0.98) {
            red += 0.02;
        }
        else {
            red = 0;
        }
    	
    }

    // tell glut to redraw (it doesnt know that you changed something)
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    // glut initialisations
    glutInit(&argc, argv);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutCreateWindow("Window title!!");
    glutDisplayFunc(myDisplay);
    glutSpecialFunc(myKeyboard);
    glutMouseFunc(myMouse);
    glutMotionFunc(myMouseMotion);
    glutPassiveMotionFunc(myMousePassive);
    glutIdleFunc(myIdle);

    // opengl initialisations
    glClearColor(0, 0, 0, 0);


    glutMainLoop();
    return 0;
}
