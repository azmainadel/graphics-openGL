#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include <windows.h>
#include <glut.h>

#define pi (2*acos(0.0))
#define theta 0.05

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;

double gunAngleU, gunAngleR, gunAngleUpper, gunAngleRotate;

struct point
{
	double x,y,z;
};

point pos, u, r, l;

void drawAxes()
{
	if(drawaxes==1)
	{
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINES);{
			glVertex3f( 1000,0,0);
			glVertex3f(-1000,0,0);

			glVertex3f(0,-1000,0);
			glVertex3f(0, 1000,0);

			glVertex3f(0,0, 1000);
			glVertex3f(0,0,-1000);
		}glEnd();
	}
}

void drawSquare(double a)
{
    //glColor3f(1.0,0.0,0.0);
	glBegin(GL_QUADS);{
		glVertex3f( a, a,2);
		glVertex3f( a,-a,2);
		glVertex3f(-a,-a,2);
		glVertex3f(-a, a,2);
	}glEnd();
}


void drawSphere(double radius,int slices,int stacks)
{
	struct point points[100][100];
	int i,j;
	int color = 0;
	double h,r=radius;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=2*radius*sin(((double)i/(double)stacks)*(pi/2));
		//r=radius*cos(((double)i/(double)stacks)*(pi/2));
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
			points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
			points[i][j].z=h;
		}
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{

		for(j=0;j<slices;j++)
		{
			if(j%2 == 0) color = 0;
			else color = 1;

			glColor3f(color, color, color);
			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
                //lower hemisphere
                glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
			}glEnd();
		}
	}
}

void drawUpperSphere(double radius,int slices,int stacks)
{
	struct point points[100][100];
	int i,j;
	int color = 0;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=radius*cos(((double)i/(double)stacks)*(pi/2));
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
			points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
			points[i][j].z=h;
		}
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{

		for(j=0;j<slices;j++)
		{
            if(j%2 == 0) color = 0;
			else color = 1;

			glColor3f(color, color, color);
			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);

			}glEnd();
		}
	}
}

void drawLowerSphere(double radius,int slices,int stacks)
{
	struct point points[100][100];
	int i,j;
	double h,r;
	int color = 0;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=radius*cos(((double)i/(double)stacks)*(pi/2));
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
			points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
			points[i][j].z=h;
		}
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{
		for(j=0;j<slices;j++)
		{
            if(j%2 == 0) color = 0;
			else color = 1;

			glColor3f(color, color, color);
			glBegin(GL_QUADS);{
                //lower hemisphere
                glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
			}glEnd();
		}
	}
}

void drawOuterSphere(double radius,int slices,int stacks)
{
	struct point points[100][100];
	int i,j;
	int color = 0;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=2*radius - radius*cos(((double)i/(double)stacks)*(pi/2));

		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
			points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
			points[i][j].z=h;
		}
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{
		for(j=0;j<slices;j++)
		{
		    if(j%2 == 0) color = 0;
			else color = 1;

			glColor3f(color, color, color);
			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);

			}glEnd();
		}
	}
}

void keyboardListener(unsigned char key, int x,int y){
	point temp;

	switch(key){
		case '1':
		    temp = l;
            l.x = l.x * cos(theta) - r.x * sin(theta);
            l.y = l.y * cos(theta) - r.y * sin(theta);
            l.z = l.z * cos(theta) - r.z * sin(theta);

            r.x = r.x * cos(theta) + temp.x * sin(theta);
            r.y = r.y * cos(theta) + temp.y * sin(theta);
            r.z = r.z * cos(theta) + temp.z * sin(theta);
			break;

		case '2':
		    temp = l;
            l.x = l.x * cos(theta) + r.x * sin(theta);
            l.y = l.y * cos(theta) + r.y * sin(theta);
            l.z = l.z * cos(theta) + r.z * sin(theta);

            r.x = r.x * cos(theta) - temp.x * sin(theta);
            r.y = r.y * cos(theta) - temp.y * sin(theta);
            r.z = r.z * cos(theta) - temp.z * sin(theta);
			break;

        case '3':
            temp = u;

            u.x = u.x * cos(theta) - l.x * sin(theta);
            u.y = u.y * cos(theta) - l.y * sin(theta);
            u.z = u.z * cos(theta) - l.z * sin(theta);


            l.x = l.x * cos(theta) + temp.x * sin(theta);
            l.y = l.y * cos(theta) + temp.y * sin(theta);
            l.z = l.z * cos(theta) + temp.z * sin(theta);
            break;

        case '4':
            temp = u;

            u.x = u.x * cos(theta) + l.x * sin(theta);
            u.y = u.y * cos(theta) + l.y * sin(theta);
            u.z = u.z * cos(theta) + l.z * sin(theta);

            l.x = l.x * cos(theta) - temp.x * sin(theta);
            l.y = l.y * cos(theta) - temp.y * sin(theta);
            l.z = l.z * cos(theta) - temp.z * sin(theta);
            break;

        case '5':
            temp = u;

            u.x = u.x * cos(theta) + r.x * sin(theta);
            u.y = u.y * cos(theta) + r.y * sin(theta);
            u.z = u.z * cos(theta) + r.z * sin(theta);

            r.x = r.x * cos(theta) - temp.x * sin(theta);
            r.y = r.y * cos(theta) - temp.y * sin(theta);
            r.z = r.z * cos(theta) - temp.z * sin(theta);
            break;

        case '6':
            temp = u;

            u.x = u.x * cos(theta) - r.x * sin(theta);
            u.y = u.y * cos(theta) - r.y * sin(theta);
            u.z = u.z * cos(theta) - r.z * sin(theta);

            r.x = r.x * cos(theta) + temp.x * sin(theta);
            r.y = r.y * cos(theta) + temp.y * sin(theta);
            r.z = r.z * cos(theta) + temp.z * sin(theta);
            break;

        case 'q':
            gunAngleU+=0.3;
            break;
        case 'w':
            gunAngleU-=0.3;
            break;
        case 'e':
            gunAngleR-=0.3;
            break;
        case 'r':
            gunAngleR+=0.3;
            break;
        case 'a':
            gunAngleUpper-=0.4;
            break;
        case 's':
            gunAngleUpper+=0.4;
            break;
        case 'd':
            gunAngleRotate-=0.5;
            break;
        case 'f':
            gunAngleRotate+=0.5;
            break;

		default:
			break;
	}
}


void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
            pos.x -= l.x;
            pos.y -= l.y;
            pos.z -= l.z;
            break;
		case GLUT_KEY_UP:		// up arrow key
            pos.x += l.x;
            pos.y += l.y;
            pos.z += l.z;
			break;

		case GLUT_KEY_RIGHT:
            pos.x += r.x;
            pos.y += r.y;
            pos.z += r.z;
			break;
		case GLUT_KEY_LEFT:
            pos.x -= r.x;
            pos.y -= r.y;
            pos.z -= r.z;
			break;

		case GLUT_KEY_PAGE_UP:
            pos.x += u.x;
            pos.y += u.y;
            pos.z += u.z;
			break;
		case GLUT_KEY_PAGE_DOWN:
            pos.x -= u.x;
            pos.y -= u.y;
            pos.z -= u.z;
			break;

		case GLUT_KEY_INSERT:
			break;

		case GLUT_KEY_HOME:
			break;
		case GLUT_KEY_END:
			break;

		default:
			break;
	}
}


void mouseListener(int button, int state, int x, int y){	//x, y is the x-y of the screen (2D)
	switch(button){
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN){		// 2 times?? in ONE click? -- solution is checking DOWN or UP
				drawaxes=1-drawaxes;
			}
			break;

		case GLUT_RIGHT_BUTTON:
			//........
			break;

		case GLUT_MIDDLE_BUTTON:
			//........
			break;

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
//	gluLookAt(0,0,200,	0,0,0,	0,1,0);
	gluLookAt(pos.x, pos.y, pos.z, pos.x + l.x, pos.y + l.y, pos.z + l.z, u.x, u.y, u.z);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

	drawAxes();

    glRotatef(-90,1,0,0);
    glPushMatrix();
    {
        glRotatef(gunAngleU, 0,1,0);
        glRotatef(gunAngleR, 1,0,0);
        glTranslatef(0,0,20);
        drawLowerSphere(20,20,20);
        glTranslatef(0,0,40);
        drawSphere(20,20,20);
        glTranslatef(0,0,40);
        drawUpperSphere(20,20,20);

        glRotatef(gunAngleUpper, 1,0,0);
        glRotatef(gunAngleRotate,0,0,1);

        glTranslatef(0,0,30);
        drawLowerSphere(10,20,20);
        glTranslatef(0,0,20);
        drawSphere(10,20,20);
        glTranslatef(0,0,20);
        drawOuterSphere(10,20,20);

    }glPopMatrix();






	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


void animate(){
	angle+=0.05;
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init(){
	//codes for initialization
	drawgrid=0;
	drawaxes=1;
	cameraHeight=150.0;
	cameraAngle=1.0;
	angle=0;
	gunAngleR=0;
	gunAngleU=0;
	gunAngleRotate=0;
	gunAngleUpper=0;

	u = {0, 0, 1};
	r = {-1/sqrt(2), 1/sqrt(2), 0};
	l = {-1/sqrt(2), -1/sqrt(2), 0};
	pos = {400,100,100};

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
