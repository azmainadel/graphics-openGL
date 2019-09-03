#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include <windows.h>
#include <glut.h>

#define pi (2*acos(0.0))
#define theta 0.04

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;

double boundaryRadius;
double bubbleRadius;

struct point
{
	double x,y,z;
};

point p1, p2, v1, v2;


void drawCircle(double radius,int segments)
{
    int i;
    struct point points[100];
    //glColor3f(0.7,0.7,0.7);
    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw segments using generated points
    for(i=0;i<segments;i++)
    {
        glBegin(GL_LINES);
        {
			glVertex3f(points[i].x,points[i].y,0);
			glVertex3f(points[i+1].x,points[i+1].y,0);
        }
        glEnd();
    }
}

void bubbleMove(){

    double distanceFromBoundary = sqrt(p1.x*p1.x + p1.y*p1.y);
    double distanceFromBubble = sqrt((p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y));

    if(boundaryRadius-bubbleRadius <= distanceFromBoundary){
        point temp={-p1.x,-p1.y,0};
        temp.x /= distanceFromBoundary;
        temp.y /= distanceFromBoundary;

        double tempx = v1.x;
        v1.x -= 2*(v1.x*temp.x+v1.y*temp.y)*temp.x;
        v1.y -= 2*(tempx*temp.x+v1.y*temp.y)*temp.y;
    }
    else{
        if(bubbleRadius*2 >= distanceFromBubble){
            point temp1;
            temp1.x = (p1.x-p2.x)/distanceFromBubble;
            temp1.y = (p1.y-p2.y)/distanceFromBubble;

            double tempx = v1.x;
            v1.x -= 2*(v1.x*temp1.x+v1.y*temp1.y)*temp1.x;
            v1.y -= 2*(tempx*temp1.x+v1.y*temp1.y)*temp1.y;

            point temp2={-temp1.x,-temp1.y,0};
            tempx = v2.x;
            v2.x -= 2*(v2.x*temp2.x+v2.y*temp2.y)*temp2.x;
            v2.y -= 2*(tempx*temp2.x+v2.y*temp2.y)*temp2.y;
        }
    }

    distanceFromBoundary = sqrt(p2.x*p2.x + p2.y*p2.y);

    if(boundaryRadius-bubbleRadius <= distanceFromBoundary){
        point temp={-p2.x,-p2.y,0};
        temp.x /= distanceFromBoundary;
        temp.y /= distanceFromBoundary;

        double tempx = v2.x;
        v2.x -= 2*(v2.x*temp.x+v2.y*temp.y)*temp.x;
        v2.y -= 2*(tempx*temp.x+v2.y*temp.y)*temp.y;
    }

    p1.x += v1.x/20;
    p1.y += v1.y/20;
    p1.z += v1.z/20;

    p2.x += v2.x/20;
    p2.y += v2.y/20;
    p2.z += v2.z/20;
}
void keyboardListener(unsigned char key, int x,int y){
	point temp;

	switch(key){
		default:
			break;
	}
}

void specialKeyListener(int key, int x,int y){
	point temp;
	switch(key){
		case GLUT_KEY_RIGHT:
            temp={v1.y,-v1.x,0};
            v1.x = v1.x * cos(theta) + temp.x * sin(theta);
            v1.y = v1.y * cos(theta) + temp.y * sin(theta);
			break;
		case GLUT_KEY_LEFT:
		    temp={-v1.y,v1.x,0};
            v1.x = v1.x * cos(theta) + temp.x * sin(theta);
            v1.y = v1.y * cos(theta) + temp.y * sin(theta);
            break;
		default:
			break;
	}
}


void mouseListener(int button, int state, int x, int y){	//x, y is the x-y of the screen (2D)
	switch(button){
		default:
			break;
	}
}



void display(){

	//clear the display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0,0,0,0);	//color black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/********************
	/ set-up camera here
	********************/
	//load the correct matrix -- MODEL-VIEW matrix
	glMatrixMode(GL_MODELVIEW);

	//initialize the matrix
	glLoadIdentity();

	//now give three info
	//1. where is the camera (viewer)?
	//2. where is the camera looking?
	//3. Which direction is the camera's UP direction?

	//gluLookAt(100,100,100,	0,0,0,	0,0,1);
	//gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
	gluLookAt(0,0,200,	0,0,0,	0,1,0);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

	//drawAxes();
	//drawGrid();

    //glColor3f(1,0,0);
    //drawSquare(10);

    //drawSS();

    glColor3f(0.7,0.7,0.7);
    drawCircle(boundaryRadius,40);

    glPushMatrix();
    glTranslatef(p1.x,p1.y,p1.z);
    glColor3f(1, 1, 0);
    drawCircle(bubbleRadius,20);
    glPopMatrix();

    glColor3f(1,1,1);
    glBegin(GL_LINES);{
        glVertex3f(p1.x,p1.y,p1.z);
        glVertex3f(p1.x+v1.x*bubbleRadius,p1.y+v1.y*bubbleRadius,p1.z);
    }glEnd();

    point temp={p1.x+v1.x*(bubbleRadius-3),p1.y+v1.y*(bubbleRadius-3),p1.z};
    point perp={-v1.y,v1.x,0};

    glColor3f(1,0,0);
    glBegin(GL_TRIANGLES);{
            glVertex3f(p1.x+v1.x*bubbleRadius,p1.y+v1.y*bubbleRadius,p1.z);
			glVertex3f(temp.x+perp.x*3,temp.y+perp.y*3,0);
			glVertex3f(temp.x-perp.x*3,temp.y-perp.y*3,0);
        }
    glEnd();


    glPushMatrix();
    glTranslatef(p2.x,p2.y,p2.z);
    glColor3f(0, 1, 0);
    drawCircle(bubbleRadius,20);
    glPopMatrix();

    glColor3f(1,1,1);
    glBegin(GL_LINES);{
        glVertex3f(p2.x,p2.y,p2.z);
        glVertex3f(p2.x+v2.x*bubbleRadius,p2.y+v2.y*bubbleRadius,p2.z);
    }glEnd();

    temp={p2.x+v2.x*(bubbleRadius-3),p2.y+v2.y*(bubbleRadius-3),p2.z};
    perp={-v2.y,v2.x,0};

    glColor3f(1,0,0);
    glBegin(GL_TRIANGLES);{
            glVertex3f(p2.x+v2.x*bubbleRadius,p2.y+v2.y*bubbleRadius,p2.z);
			glVertex3f(temp.x+perp.x*3,temp.y+perp.y*3,0);
			glVertex3f(temp.x-perp.x*3,temp.y-perp.y*3,0);
        }
    glEnd();

    //drawTriangle();

    //drawCone(20,50,24);

	//drawSphere(30,24,20);

	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


void animate(){
	angle+=0.05;
	bubbleMove();
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init(){
	//codes for initialization
	drawgrid=1;
	drawaxes=1;
	cameraHeight=150.0;
	cameraAngle=1.0;
	angle=0;
	boundaryRadius = 120;
	bubbleRadius = boundaryRadius/10;

	p1={-25,10,0};
	p2={20,-35,0};

	v1={1,0,0};
	v2={0,1,0};

	//clear the screen
	glClearColor(0,0,0,0);

	/************************
	/ set-up projection here
	************************/
	//load the PROJECTION matrix
	glMatrixMode(GL_PROJECTION);

	//initialize the matrix
	glLoadIdentity();

	//give PERSPECTIVE parameters
	gluPerspective(80,	1,	1,	1000.0);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance
}

int main(int argc, char **argv){
	glutInit(&argc,argv);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("1405075");

	init();

	glEnable(GL_DEPTH_TEST);	//enable Depth Testing

	glutDisplayFunc(display);	//display callback function
	glutIdleFunc(animate);		//what you want to do in the idle time (when no drawing is occuring)

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);
	glutMouseFunc(mouseListener);

	glutMainLoop();		//The main loop of OpenGL

	return 0;
}
