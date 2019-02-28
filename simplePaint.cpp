#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>

//Globals
int x; int y; int rad; int xcenter; int ycenter;
int x1; int y1; int x2; int y2;
float red=0.0, blue=0.0, green=0.0;
int choice;

void keyboard(unsigned char key, int xIn, int yIn)
{
	int mod = glutGetModifiers();
	switch (key)
	{
		case 'q':
		case 27:
			exit(0);
			break;
    }
}

void init() {
	glClearColor (1.0, 1.0, 1.0,0);
    glColor3f(red, green, blue);
	glViewport( 0,0, 640, 480 );
	glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
	gluOrtho2D( 0.0, 640.0, 0.0, 480.0 );
}

void display(void){

}

void draw_Points(int x, int y){
    glPointSize(4);
    glColor3f(red, green, blue);
    glBegin(GL_POINTS);
    glVertex2i(x, y);
    glEnd();
    glutSwapBuffers();  
}
//implement bresenham's algorithm
void bre_algo(int x1, int x2, int y1, int y2){
    int dx = x2 - x1; int dy = y2 - y1;
    int d = 2*dy - dx; 
    int incrE = dy*2;
    int incrNE = 2*dy - 2*dx;
    if (x1 < x2){
        x = x2; y = y2; x2 = x1;
    }else{
    x = x1; y = y1;
    }
    draw_Points(x, y);
    while (x < x2){
        x++;
        if (d <= 0){
            d += incrE; 
        }else{
            y++;
            d += incrNE;
        }
    }
    draw_Points(x, y);
}

void draw_Lines(int x, int y){
    glColor3f(red, green, blue);
    bre_algo(x1, x2, y1, y2);
    glutSwapBuffers();
}

void draw_Rectangles(int x, int y){
    glColor3f(red, green, blue);
    bre_algo(x1, x2, y1, y2);
    glVertex2f(-x, 0);
	glVertex2f(x, 0);
	glColor3f(red, green, blue);
	glVertex2f(0, -y); 
	glVertex2f(0, y); 
    bre_algo(x1, x2, y1, y2);
    glutSwapBuffers();
}

void circlePoints(int xcenter, int ycenter, int x, int y){ 
    draw_Points(xcenter + x, ycenter + y);
    draw_Points(xcenter - x, ycenter + y);
    draw_Points(xcenter + x, ycenter - y);
    draw_Points(xcenter - x, ycenter - y);
    draw_Points(xcenter + y, ycenter + x);
    draw_Points(xcenter - y, ycenter + x);
    draw_Points(xcenter + y, ycenter - x);
    draw_Points(xcenter - y, ycenter - x);
}


void draw_Circles(int xcenter, int ycenter,int rad){
    int p;
    x = 0; y = rad;
    circlePoints (xcenter, ycenter, x, y);
    p = 1 - rad;
    while (x < y){
        x++;
        if (p < 0){
        p += 1 + 2*x;
        }else{
            y--;
            p += 1 + 2*(x - y);
        }
        circlePoints(xcenter, ycenter, x, y);
    }
    glutSwapBuffers();
}

void mouse(int btn, int state, int mouse_x, int mouse_y){
//draw points with mouse click
	if (choice == 1 && btn == GLUT_LEFT_BUTTON)
        x = mouse_x; y = 480 - mouse_y;
        draw_Points(x,y);
    if (choice == 2 && btn == GLUT_LEFT_BUTTON)
        draw_Lines(x,y);
    if (choice == 3 && btn == GLUT_LEFT_BUTTON)
        draw_Rectangles(x,y);
    if (choice == 4 && btn == GLUT_LEFT_BUTTON)
        draw_Circles(xcenter,ycenter,rad);
}
 
void reshape(int w, int h){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h); 
}

void callBackInit(){
//registers "display" "keyboard" "mouse" "reshape" as callback function
	glutDisplayFunc(display);	

    glutKeyboardFunc(keyboard);

	glutMouseFunc(mouse);

	glutReshapeFunc(reshape);
}


//change color
void menuProc1(int value)
{
    if (value == 1)
        red = 1.0; green = 0.0; blue = 0.0;
    if (value == 2)
        red = 0.0; green = 1.0; blue = 0.0;
    if (value == 3)
        red = 0.0; green = 0.0; blue = 1.0;
    if (value == 4)
        red = 1.0; green = 0.0; blue = 1.0;
    if (value == 5)
        red = 1.0; green = 1.0; blue = 0.0;
        
}

//change shape
void menuProc2(int value){
    if (value == 6)
        choice = 1;   
    if (value == 7)
        choice = 2;
    if (value == 8)
        choice = 3;
    if (value == 9)
        choice = 4;
}

//exit the program
void menuProc3(int value){
    if (value == 1)
        exit(0);
}

void menuProc(int value){

}

void createMyMenu(){

	int subMenu_1 = glutCreateMenu(menuProc1);
    glutAddMenuEntry("red", 1);
    glutAddMenuEntry("green", 2);
    glutAddMenuEntry("blue", 3);
    glutAddMenuEntry("purple", 4);
    glutAddMenuEntry("yellow", 5); 

    int subMenu_2 = glutCreateMenu(menuProc2);
    glutAddMenuEntry("Point", 6);  
    glutAddMenuEntry("Lines", 7);
    glutAddMenuEntry("Rectangle", 8);
    glutAddMenuEntry("circle", 9);

    int subMenu_3 = glutCreateMenu(menuProc3);
    glutAddMenuEntry("Quit", 1); 

	int main_id = glutCreateMenu(menuProc);
    glutAddSubMenu("Colour", subMenu_1);
    glutAddSubMenu("Shapes", subMenu_2);
    glutAddSubMenu("More", subMenu_3);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}


int main(int argc, char** argv)
{
	//glut initialization stuff:
	// set the window size, display mode, and create the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE );
    glutInitWindowSize(640, 480);
    glutInitWindowPosition(0,0);
    glutCreateWindow("Simple paint program");
    //start the program
    glutDisplayFunc(display);
    init();
    callBackInit();
    createMyMenu();
	glutMainLoop();

}