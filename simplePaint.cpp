/**
 * author: Shengyu wu
 * date: 10/21/2018
 */

#include <time.h>
#include "Point.h"
#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <list>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#endif

using namespace std;

/**
 * Globals
 */

float red = 1.0, green = 0.0, blue = 0.0;
int init_x, init_y; // store the first point when shape is line, rectangle or circle
int brushSize = 4;
int eraserSize = 1;
bool isSecond = false;
bool isRandom = false;
bool isEraser = false;
bool isRadial = false;
float width = 800; //initialize window width and height
float height = 600;

int shape = 1; // 1:point, 2:line, 3:rectangle, 4:circle, 5:brush

std::vector<Point> Points; // store all the points until clear
std::list<int> undoMemory; // record for undo, maximum 20 shapes in Memory
std::list<int> redoMemory; // record for redo, maximum 20 shapes in Memory
std::vector<Point> redoPoints; // store the Points after undo temporaly

void display(void)
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glPointSize(2);
	glBegin(GL_POINTS);
	for (unsigned int i = 0; i < Points.size(); i++)
	{
		glColor3f(Points[i].getR(), Points[i].getG(), Points[i].getB());
		glVertex2i(Points[i].getX(), Points[i].getY());
	}
	glEnd();
	glutSwapBuffers();
}

void clear()
{
	Points.clear();
	undoMemory.clear();
	redoPoints.clear();
	redoMemory.clear();
	glClear(GL_COLOR_BUFFER_BIT);
	glutSwapBuffers();
	time_t rawtime;
	struct tm *timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	std::cout << asctime(timeinfo)
			  << "Successfully cleaned window and Memory.\n";
}

void quit()
{
	std::cout << "Thank you for using simple-Paint." << std::endl;
	exit(0);
}

void undo()
{
	if (undoMemory.size() > 0)
	{
		if (undoMemory.back() != Points.size() && redoMemory.back() != Points.size())
		{
			redoMemory.push_back(Points.size());
		}
		int numRemove = Points.size() - undoMemory.back();
		for (int i = 0; i < numRemove; i++)
		{
			redoPoints.push_back(Points.back());
			Points.pop_back();
		}
		redoMemory.push_back(undoMemory.back());
		undoMemory.pop_back();
	}
	else
	{
		time_t rawtime;
		struct tm *timeinfo;
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		std::cout << asctime(timeinfo)
				  << "Failed undo. This is the first record in the Memory.\n";
	}
}

void redo()
{
	if (redoMemory.size() > 1)
	{
		undoMemory.push_back(redoMemory.back());
		redoMemory.pop_back();
		int numRemove = redoMemory.back() - Points.size();
		for (int i = 0; i < numRemove; i++)
		{
			Points.push_back(redoPoints.back());
			redoPoints.pop_back();
		}
	}
	else
	{
		time_t rawtime;
		struct tm *timeinfo;
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		std::cout << asctime(timeinfo)
				  << "Failed redo. This is the last record in the Memory.\n";
	}
}

void drawPoint(int mousex, int mousey)
{
	Point newPoint(mousex, height - mousey, isEraser ? 1.0 : red, isEraser ? 1.0 : green, isEraser ? 1.0 : blue);
	Points.push_back(newPoint);
}

void drawBrush(int x, int y)
{
	for (int i = 0; i < brushSize; i++)
	{
		int randX = rand() % (brushSize + 1) - brushSize / 2 + x;
		int randY = rand() % (brushSize + 1) - brushSize / 2 + y;
		drawPoint(randX, randY);
	}
}

/**
 * Bresenham's line algorithm
 */
void drawLine(int x1, int y1, int x2, int y2)
{
	bool changed = false;
	// Bresenham's line algorithm is only good when |dx| >= |dy|
	// if |dx| < |dy|, change axis x and y
	if (abs(x2 - x1) < abs(y2 - y1))
	{
		int tmp1 = x1;
		x1 = y1;
		y1 = tmp1;
		int tmp2 = x2;
		x2 = y2;
		y2 = tmp2;
		changed = true;
	}
	int dx = x2 - x1;
	int dy = y2 - y1;
	int yi = 1;
	int xi = 1;
	if (dy < 0)
	{
		yi = -1;
		dy = -dy;
	}
	if (dx < 0)
	{
		xi = -1;
		dx = -dx;
	}
	int d = 2 * dy - dx;
	int incrE = dy * 2;
	int incrNE = 2 * dy - 2 * dx;

	int x = x1, y = y1;
	if (changed)
		drawPoint(y, x);
	else
		drawPoint(x, y);
	while (x != x2)
	{
		if (d <= 0)
			d += incrE;
		else
		{
			d += incrNE;
			y += yi;
		}
		x += xi;
		if (changed)
			drawPoint(y, x);
		else
			drawPoint(x, y);
	}
}

/**
 * Using drawLine function to draw the rectangle
 * Top-left corner specified by the first click,
 * and the bottom-right corner specified by a second click
 */
void drawRectangle(int x1, int y1, int x2, int y2)
{
	if (x1 < x2 && y1 < y2)
	{
		drawLine(x1, y1, x2, y1);
		drawLine(x2, y1, x2, y2);
		drawLine(x2, y2, x1, y2);
		drawLine(x1, y2, x1, y1);
	}
	else
	{
		time_t rawtime;
		struct tm *timeinfo;
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		std::cout << asctime(timeinfo)
				  << "The first click should be the top-left corner, the second click should be bottom-right corner.\n";
	}
}

/**
 * Midpoint circle algorithm
 */
void drawCircle(int x1, int y1, int x2, int y2)
{
	int r = sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
	double d;
	int x, y;

	x = 0;
	y = r;
	d = 1.25 - r;

	while (x <= y)
	{
		drawPoint(x1 + x, y1 + y);
		drawPoint(x1 - x, y1 + y);
		drawPoint(x1 + x, y1 - y);
		drawPoint(x1 - x, y1 - y);
		drawPoint(x1 + y, y1 + x);
		drawPoint(x1 - y, y1 + x);
		drawPoint(x1 + y, y1 - x);
		drawPoint(x1 - y, y1 - x);
		x++;
		if (d < 0)
		{
			d += 2 * x + 3;
		}
		else
		{
			y--;
			d += 2 * (x - y) + 5;
		}
	}
}

void drawRadialBrush(int x, int y)
{
	int xc = glutGet(GLUT_WINDOW_WIDTH) / 2;
	int yc = glutGet(GLUT_WINDOW_HEIGHT) / 2;
	int dx, dy;

	dx = xc - x;
	dy = yc - y;

	drawPoint(xc + dx, yc + dy);
	drawPoint(xc - dx, yc + dy);
	drawPoint(xc + dx, yc - dy);
	drawPoint(xc - dx, yc - dy);
	drawPoint(xc + dy, yc + dx);
	drawPoint(xc - dy, yc + dx);
	drawPoint(xc + dy, yc - dx);
	drawPoint(xc - dy, yc - dx);
}

void erase(int x, int y)
{
	for (int i = -eraserSize; i <= eraserSize; i++)
	{
		for (int j = -eraserSize; j <= eraserSize; j++)
		{
			drawPoint(x + i, y + j);
		}
	}
}

void keyboard(unsigned char key, int xIn, int yIn)
{
	isSecond = false;
	switch (key)
	{
	case 'q':
	case 27: 
		quit();
		break;
	case 'c':
		clear();
		break;
	case '+':
		if (shape == 5 && !isEraser)
		{
			if (brushSize < 16)
				brushSize += 4;
			else
			{
				time_t rawtime;
				struct tm *timeinfo;
				time(&rawtime);
				timeinfo = localtime(&rawtime);
				std::cout << asctime(timeinfo)
						  << "[Warning] Airbrush's size cannot be larger. It is already the largest.\n";
			}
		}
		else if (isEraser)
		{
			if (eraserSize < 10)
				eraserSize += 4;
			else
			{
				time_t rawtime;
				struct tm *timeinfo;
				time(&rawtime);
				timeinfo = localtime(&rawtime);
				std::cout << asctime(timeinfo)
						  << "[Warning] Eraser's size cannot be larger. It is already the largest.\n";
			}
		}
		break;
	case '-':
		if (shape == 5 && !isEraser)
		{
			if (brushSize > 4)
				brushSize -= 4;
			else
			{
				time_t rawtime;
				struct tm *timeinfo;
				time(&rawtime);
				timeinfo = localtime(&rawtime);
				std::cout << asctime(timeinfo)
						  << "[Warning] Airbrush's size cannot be smaller. It is already the smallest.\n";
			}
		}
		else if (isEraser)
		{
			if (eraserSize > 2)
				eraserSize -= 4;
			else
			{
				time_t rawtime;
				struct tm *timeinfo;
				time(&rawtime);
				timeinfo = localtime(&rawtime);
				std::cout << asctime(timeinfo)
						  << "[Warning] Eraser's size cannot be smaller. It is already the smallest.\n";
			}
		}
		break;
	case 'u':
		undo();
		break;
	case 'r':
		redo();
		break;
	}
}

void mouse(int bin, int state, int x, int y)
{
	if (bin == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		if (isRandom)
		{
			srand(time(NULL));
			red = float(rand()) / float(RAND_MAX);
			green = float(rand()) / float(RAND_MAX);
			blue = float(rand()) / float(RAND_MAX);
		}
		if (isEraser)
		{
			undoMemory.push_back(Points.size());
			erase(x, y);
		}
		else
		{
			if (shape == 1)
			{
				undoMemory.push_back(Points.size());
				if (isRadial)
					drawRadialBrush(x, y);
				else
					drawPoint(x, y);
			}
			else if (shape == 5)
			{
				undoMemory.push_back(Points.size());
				drawBrush(x, y);
			}
			else
			{
				if (!isSecond)
				{
					init_x = x;
					init_y = y;
					isSecond = true;
				}
				else
				{
					if (undoMemory.back() != Points.size())
						undoMemory.push_back(Points.size());
					if (shape == 2)
						drawLine(init_x, init_y, x, y);
					else if (shape == 3)
						drawRectangle(init_x, init_y, x, y);
					else if (shape == 4)
						drawCircle(init_x, init_y, x, y);
					isSecond = false;
				}
			}
		}
		if (undoMemory.size() > 20)
		{
			undoMemory.pop_front();
		}
	}
}

void motion(int x, int y)
{
	if (isEraser)
		erase(x, y);
	else
	{
		if (shape == 1)
		{
			if (isRadial)
				drawRadialBrush(x, y);
			else
				drawPoint(x, y);
		}
		if (shape == 5)
			drawBrush(x, y);
	}
}

void reshape(int w, int h)
{
	width = w;
	height = h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h);

	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, w, h);
}

void MainMenu(int value)
{
	switch (value)
	{
	case 0:
		quit();
		break;
	case 1:
		clear();
		break;
	case 2:
		undo();
		break;
	case 3:
		redo();
		break;
	}
}

void BrushSizeMenu(int value)
{
	shape = 5;
	isEraser = false;
	brushSize = value;
	glutSetCursor(GLUT_CURSOR_CROSSHAIR);
}

void ColourMenu(int value)
{
	isSecond = false;
	isEraser = false;

	switch (value)
	{
	case 1: // red
		red = 1.0;
		green = 0.0;
		blue = 0.0;
		break;
	case 2: // green
		red = 0.0;
		green = 1.0;
		blue = 0.0;
		break;
	case 3: // blue
		red = 0.0;
		green = 0.0;
		blue = 1.0;
		break;
	case 4: // purple
		red = 0.5;
		green = 0.0;
		blue = 0.5;
		break;
	case 5: // yellow
		red = 1.0;
		green = 1.0;
		blue = 0.0;
		break;
	case 6: // random
		isRandom = true;
		break;
	}
}

void ShapeMenu(int value)
{
	shape = value;
	isEraser = false;
	isSecond = false;
	isRadial = false;

	switch (shape)
	{
	case 1:
		glutSetCursor(GLUT_CURSOR_RIGHT_ARROW);
		break;
	case 2:
	case 3:
	case 4:
		glutSetCursor(GLUT_CURSOR_CROSSHAIR);
		break;
	}
}

void EraserSizeMenu(int value)
{
	glutSetCursor(GLUT_CURSOR_RIGHT_ARROW);
	eraserSize = value;
	isEraser = true;
}

void RadicalBrushMenu(int value)
{
	isRadial = value == 1 ? true : false;
}

void createOurMenu()
{
	int colourMenu = glutCreateMenu(ColourMenu);
	glutAddMenuEntry("red", 1);
	glutAddMenuEntry("Green", 2);
	glutAddMenuEntry("Blue", 3);
	glutAddMenuEntry("Purple", 4);
	glutAddMenuEntry("Yellow", 5);
	glutAddMenuEntry("Random", 6);

	int sizeMenu = glutCreateMenu(BrushSizeMenu);
	glutAddMenuEntry("4px", 4);
	glutAddMenuEntry("8px", 8);
	glutAddMenuEntry("12px", 12);
	glutAddMenuEntry("16px", 16);

	int shapeMenu = glutCreateMenu(ShapeMenu);
	glutAddMenuEntry("Point", 1);
	glutAddMenuEntry("Line", 2);
	glutAddMenuEntry("Rectangle", 3);
	glutAddMenuEntry("Circle", 4);
	glutAddSubMenu("Airbrush", sizeMenu);

	int eraserSizeMenu = glutCreateMenu(EraserSizeMenu);
	glutAddMenuEntry("Small", 2);
	glutAddMenuEntry("Medium", 6);
	glutAddMenuEntry("Large", 10);

	int radicalBrushMenu = glutCreateMenu(RadicalBrushMenu);
	glutAddMenuEntry("True", 1);
	glutAddMenuEntry("False", 2);

	int main_id = glutCreateMenu(MainMenu);
	glutAddSubMenu("Colour", colourMenu);
	glutAddSubMenu("Shapes", shapeMenu);
	glutAddSubMenu("Radical Paint Brush", radicalBrushMenu);
	glutAddSubMenu("Eraser", eraserSizeMenu);
	glutAddMenuEntry("Undo", 2);
	glutAddMenuEntry("redo", 3);
	glutAddMenuEntry("Clear", 1);
	glutAddMenuEntry("Quit", 0);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void init(void)
{
	//background color
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glColor3f(red, green, blue);

	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0, width, 0.0, height);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void FPS(int val)
{
	glutPostRedisplay();
	glutTimerFunc(0, FPS, 0);
}

void callbackInit()
{
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutTimerFunc(17, FPS, 0);
}

void Information()
{
	std::cout << "----------------------Welcome to Simple paint Program!----------------------\n"
			  << "A list of commands:\n"
			  << "Right click -> show menu\n"
			  << "Left click -> choose option\n"
			  << "Menu \"Colour\"\t"
			  << "-> Change colour. Default is red.\n"
			  << "Menu \"Shapes\"\t"
			  << "-> The default shape is Point.\n"
			  << "\tPoint\t\t"
			  << "-> draw a Point at the point clicked with the mouse. Clicking and dragging will draw points constantly like free-form drawing.\n"
			  << "\tLine\t\t"
			  << "-> draw a linebetween two subseauently clicked points. (Bresenham's algorithm is used here.)\n"
			  << "\tRectangle\t"
			  << "-> draw a rectangle with top-left corner specified by the first click and the bottom-right corner specified by a second click. If the second click is bottom-left, top-right or top-left comparing to the first click, a warning will show in the console.\n"
			  << "\tCircle\t\t"
			  << "-> draw a circle centered at the position of the first click, with its radius set by a second click.\n"
			  << "\tAirbrush\t"
			  << "-> draw multiple points as brush around the clicked point. There are four options of size.\n"
			  << "Menu \"Radical\""
			  << "-> draw multiple points as brush around the centre point of the document. You can choose True or False. Only useful when shape is point. The default option is false. After choosing point in shape, it becomes False.\n"
			  << "Menu \"Eraser\"\t"
			  << "-> erase the points by clicking and dragging.\n"
			  << "Menu \"Undo\"\t"
			  << "-> undo, the Memory can keep maximum 20 records.\n"
			  << "Menu \"redo\"\t"
			  << "->  redo, the Memory can keep maximum 20 records.\n"
			  << "Menu \"Clear\"\t"
			  << "-> clear all the points and clear the Memory.\n"
			  << "Menu \"Quit\"\t"
			  << "-> close the window.\n"
			  << "Keyboard 'q' or 'esc' -> close the window.\n"
			  << "Keyboard 'c' -> clear all.\n"
			  << "Keyboard '+' -> larger size.\n"
			  << "Keyboard '-' -> smaller size.\n"
			  << "Keyboard 'u' -> undo.\n"
			  << "Keyboard 'r' -> redo." << std::endl;
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Paint");
	callbackInit();
	init();
	Information();
	createOurMenu();
	glutMainLoop();
	return (0);
}