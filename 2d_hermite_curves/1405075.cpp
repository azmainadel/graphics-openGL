#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<iostream>

#include <windows.h>
#include <glut.h>

#define pi (2*acos(0.0))
#define MAX_D 2000

struct point
{
	double x,y,z;
};

struct point2d
{
    double x, y;
};

struct point2d cp[200];
int cpidx;

int showVectors;
int showCurve;
int showPoint;

struct point2d curve[10000];
int curveidx;
int curveDrawn;

int ptidx;

int update;
struct point2d updatingPoint;
int idx = -999;

void drawSquare()
{
    glBegin(GL_QUADS);
    {
        glVertex3d( 3,  3, 0);
        glVertex3d( 3, -3, 0);
        glVertex3d(-3, -3, 0);
        glVertex3d(-3,  3, 0);
    }
    glEnd();
}

void drawCircle(double radius,int slices,int stacks)
{
	struct point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		//h=radius*sin(((double)i/(double)stacks)*(pi/2));
		h=0.0;
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
        //glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
		for(j=0;j<slices;j++)
		{
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

void drawArrow(point2d p1, point2d p2){

    point2d dirVector = {p2.x-p1.x, p2.y-p1.y};
    double dirVectorMagnitude = sqrt(pow(dirVector.x,2) + pow(dirVector.y,2));
    point2d unitVector = {dirVector.x/dirVectorMagnitude, dirVector.y/dirVectorMagnitude};
    point2d unitVectorPerp = {-unitVector.y, unitVector.x};

    glColor3f(1,0,0);
    glBegin(GL_TRIANGLES);
    {
        glVertex3f(p2.x, p2.y, 0);
        glVertex3f(p2.x - dirVectorMagnitude*0.1*unitVector.x + 7*unitVectorPerp.x,
                   p2.y - dirVectorMagnitude*0.1*unitVector.y + 7*unitVectorPerp.y, 0);
        glVertex3f(p2.x - dirVectorMagnitude*0.1*unitVector.x - 7*unitVectorPerp.x,
                   p2.y - dirVectorMagnitude*0.1*unitVector.y - 7*unitVectorPerp.y, 0);
    }
    glEnd();

}

void keyboardListener(unsigned char key, int x,int y){
	switch(key){
        case 'g':
            showVectors = 1 - showVectors;
			break;
        case 'a':
            if(curveDrawn == 1){
                showPoint = 1 - showPoint;
                ptidx = 0;
            }
            break;
        case 'u':
            if(curveDrawn == 1){
                update = 1;
                showPoint = 0;
                ptidx = 0;
            }
            break;

		default:
			break;
	}
}

void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
			break;
		case GLUT_KEY_UP:		// up arrow key
			break;

		case GLUT_KEY_RIGHT:
			break;
		case GLUT_KEY_LEFT:
			break;

		case GLUT_KEY_PAGE_UP:
			break;
		case GLUT_KEY_PAGE_DOWN:
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


void mouseListener(int button, int state, int x, int y){
	switch(button){
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN){
                if(showCurve == 0){
//                    std::cout << x << " " << y << std::endl;
                    cp[cpidx].x = (double)x;
                    cp[cpidx].y = (double)(600 - y);
                    cpidx++;
                }
                else if(update == 1){
                    updatingPoint.x = (double)x;
                    updatingPoint.y = (double)(600 - y);
                    update = 2;
                }
                else if(update == 2 && idx != -999){
                    cp[idx].x = (double)x;
                    cp[idx].y = (double)(600 - y);
                    update = 0;
                    idx = -999;
                    curveDrawn = 0;
                    curveidx = 0;
                }
			}
			break;

		case GLUT_RIGHT_BUTTON:
			if(state == GLUT_DOWN){
                if(cpidx > 2 && cpidx%2 == 0){
                    showCurve = 1;
                    showPoint = 0;
                    ptidx = 0;
                }
			}
			break;

		case GLUT_MIDDLE_BUTTON:
			//........
			break;

		default:
			break;
	}
}


void drawVectors(){

    for(int i = 0; i < cpidx; i++){

        if(i%2 == 0) glColor3f(0,1,0);
        else glColor3f(1,1,0);

        glPushMatrix();
        {
            glTranslatef(cp[i].x, cp[i].y, 0);
            drawSquare();
        }
        glPopMatrix();

        point2d p1, p2;

        if(i%2 == 1){
            p1 = {cp[i-1].x, cp[i-1].y};
            p2 = {cp[i].x, cp[i].y};

            drawArrow(p1, p2);

            glColor3f(1, 1, 1);
            glBegin(GL_LINES);
            {
                glVertex3f(p1.x, p1.y, 0);
                glVertex3f(p2.x, p2.y, 0);
            }
            glEnd();
        }
    }
}

void drawCurve(){

    if(curveDrawn == 0){
        curveDrawn = 1;

        point2d p1, p4, r1, r4;

        double n = 700.0;

        for(int i = 0; i < cpidx; i += 2){
            p1 = cp[i];
            p4 = cp[(i+2)%cpidx];
            r1 = {cp[i+1].x - cp[i].x, cp[i+1].y - cp[i].y};
            r4 = {cp[(i+3)%cpidx].x - cp[(i+2)%cpidx].x, cp[(i+3)%cpidx].y - cp[(i+2)%cpidx].y};

            double ax = 2*p1.x - 2*p4.x + r1.x + r4.x;
            double ay = 2*p1.y - 2*p4.y + r1.y + r4.y;

            double bx = -3*p1.x + 3*p4.x - 2*r1.x - r4.x;
            double by = -3*p1.y + 3*p4.y - 2*r1.y - r4.y;

            double cx = r1.x;
            double cy = r1.y;

            double dx = p1.x;
            double dy = p1.y;

            double del = 1.0/n;

            double fx = dx;
            double fy = dy;

            double delfx = ax*del*del*del + bx*del*del + cx*del;
            double delfy = ay*del*del*del + by*del*del + cy*del;

            double delfx2 = 6*ax*del*del*del + 2*bx*del*del;
            double delfy2 = 6*ay*del*del*del + 2*by*del*del;

            double delfx3 = 6*ax*del*del*del;
            double delfy3 = 6*ay*del*del*del;

            curve[curveidx] = {fx, fy};
            curveidx++;

            for(int j = 0; j < n; j++){
                fx += delfx;
                delfx += delfx2;
                delfx2 += delfx3;
                fy += delfy;
                delfy += delfy2;
                delfy2 += delfy3;

                curve[curveidx] = {fx, fy};
                curveidx++;
            }
        }
    }

    glColor3f(1,1,1);

    for(int i = 0; i < curveidx; i++){
        glBegin(GL_LINES);
        {
            glVertex3f(curve[i].x, curve[i].y, 0);
            glVertex3f(curve[(i+1)%curveidx].x, curve[(i+1)%curveidx].y, 0);

            //std::cout<<"DRAWTING"<<std::endl;
        }
        glEnd();
    }
    //curveidx = 0;

}

void drawPoint(){

    point2d pointPos = curve[ptidx % curveidx];
    ptidx += 1;

    glColor3f(0, 1, 1);
    glPushMatrix();
    {
        glTranslatef(pointPos.x, pointPos.y, 0);
        drawCircle(5, 20, 20);
    }
    glPopMatrix();

}

void updatePoint(){

    double minDist = MAX_D;
    double dist;
    //int idx;

    for(int i = 0; i < cpidx; i++){
        dist = sqrt((cp[i].x-updatingPoint.x)*(cp[i].x-updatingPoint.x) + (cp[i].y-updatingPoint.y)*(cp[i].y-updatingPoint.y));

        if(dist < minDist){
            minDist = dist;
            idx = i;
        }
    }

    glColor3f(0, 1, 1);
    glPushMatrix();
    {
        glTranslatef(cp[idx].x, cp[idx].y, 0);
        drawCircle(7, 20, 20);
    }
    glPopMatrix();

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
	//gluLookAt(150*cos(cameraAngle), 150*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
	gluLookAt(0,0,0,	0,0,-1,	0,1,0);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

    if(showVectors == 1) drawVectors();

    glEnable(GL_LINE_SMOOTH);
    if(showCurve == 1) drawCurve();
    glDisable(GL_LINE_SMOOTH);

    if(showPoint == 1) drawPoint();

    if(update == 2) updatePoint();
	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


void animate(){


	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init(){
	//codes for initialization

	cpidx = 0;
	curveidx = 0;
	ptidx = 0;
	showVectors = 1;
	showCurve = 0;
	curveDrawn = 0;
	showPoint = 0;
	update = 0;

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
	gluOrtho2D(0, 800, 0, 600);
	//gluPerspective(80,	1,	1,	1000.0);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance
}

int main(int argc, char **argv){
	glutInit(&argc,argv);
	glutInitWindowSize(800, 600);
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
