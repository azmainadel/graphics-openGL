#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<fstream>
#include<windows.h>
#include<glut.h>
#include<bits/stdc++.h>
#include "bitmap_image.hpp"
using namespace std;

#define pi (2*acos(0.0))
#define theta 0.05

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
int texture;
int drawCB;
double angle;

double screenWidth;
double screenHeight;
double unitWidth;
double unitHeight;

double nearPlaneDistance;
double farPlaneDistance;
double fovY;
double fovX;
double aspectRatio;

int recursionLevel;
int numberOfPixels;

double checkerboardWidth;
double ambientCoef;
double diffuseCoef;
double reflectionCoef;

int numberOfObjects;
int numberOfNormalLight;
int numberOfSpotLight;

double t_min;

struct point{
public:
	double x,y,z;

//    point(){}
//
//	point(double a, double b, double c){
//        x = a;
//        y = b;
//        z = c;
//	}
//    point& operator=(const point& p){
//        x = p.x;
//        y = p.y;
//        z = p.z;
//        return *this;
//    }
};

point pos, u, r, l;
point pointBuffer[1000][1000];

typedef point Color;
typedef point Vector;

double getDotProduct(Vector a, Vector b){
    return a.x*b.x + a.y*b.y + a.z*b.z;
}


Vector getCrossProduct(Vector a, Vector b){
    Vector v = {a.y*b.z - a.z*b.y, b.x*a.z - b.z*a.x, a.x*b.y - a.y*b.x};
    return v;
}


Vector normalize(Vector v){
    double r = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
    Vector u;
    u.x = v.x / r;
    u.y = v.y / r;
    u.z = v.z / r;
    return u;
}


double getDeterminant(double matrix[][3]){
    double x = matrix[0][0] * (matrix[1][1]*matrix[2][2]  - matrix[1][2]*matrix[2][1])
                + matrix[0][1] * (matrix[1][2]*matrix[2][0]  - matrix[1][0]*matrix[2][2])
                + matrix[0][2] * (matrix[1][0]*matrix[2][1]  - matrix[1][1]*matrix[2][0]);
    return x;
}


struct sphere{
public:
    point center;
    double radius;
    Color color;
    double ambientC;
    double diffuseC;
    double specularC;
    double reflectionC;
    double shininess;

    sphere(){}
};


struct pyramid{
public:
    point lowestPoint;
    double width;
    double height;
    Color color;
    double ambientC;
    double diffuseC;
    double specularC;
    double reflectionC;
    double shininess;

    pyramid(){}
};


struct normalLightSource{
public:
    point position;
    double falloffParam;
};


struct spotLightSource{
public:
    point position;
    double falloffParam;
    point look;
    double cutoffAngle;
};

vector<sphere> spheres;
vector<pyramid> pyramids;
vector<normalLightSource> normalLights;
vector<spotLightSource> spotLights;


void drawAxes(){
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


void drawSquare(double a){
    //glColor3f(1.0,0.0,0.0);
	glBegin(GL_QUADS);{
		glVertex3f( a, a,0);
		glVertex3f( a,-a,0);
		glVertex3f(-a,-a,0);
		glVertex3f(-a, a,0);
	}glEnd();
}


void drawSphere(double radius, int slices, int stacks){
	struct point points[100][100];
	int i,j;
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


void drawPyramid(point lowestPoint, double width, double height){
    point midPoint = {lowestPoint.x + (width/2.0), lowestPoint.y + (width/2.0), lowestPoint.z};
    point topPoint = {midPoint.x, midPoint.y, midPoint.z + height};

    glPushMatrix();{
        glTranslatef(midPoint.x, midPoint.y, midPoint.z);
        drawSquare(width/2.0);
        }glPopMatrix();

    //glColor3f(.5, .5, .5);
    glPushMatrix();{
        glBegin(GL_TRIANGLES);
        {
            glVertex3f(topPoint.x, topPoint.y, topPoint.z);
            glVertex3f(lowestPoint.x, lowestPoint.y, lowestPoint.z);
            glVertex3f(lowestPoint.x + width, lowestPoint.y, lowestPoint.z);
        }
        glEnd();

        glBegin(GL_TRIANGLES);
        {
            glVertex3f(topPoint.x, topPoint.y, topPoint.z);
            glVertex3f(lowestPoint.x, lowestPoint.y, lowestPoint.z);
            glVertex3f(lowestPoint.x, lowestPoint.y + width, lowestPoint.z);
        }
        glEnd();

        glBegin(GL_TRIANGLES);
        {
            glVertex3f(topPoint.x, topPoint.y, topPoint.z);
            glVertex3f(lowestPoint.x + width, lowestPoint.y, lowestPoint.z);
            glVertex3f(lowestPoint.x + width, lowestPoint.y + width, lowestPoint.z);
        }
        glEnd();

        glBegin(GL_TRIANGLES);
        {
            glVertex3f(topPoint.x, topPoint.y, topPoint.z);
            glVertex3f(lowestPoint.x, lowestPoint.y + width, lowestPoint.z);
            glVertex3f(lowestPoint.x + width, lowestPoint.y + width, lowestPoint.z);
        }
        glEnd();
    }glPopMatrix();
}


void drawLightSource(point position){
    glPushMatrix();{
        glColor3f(1, 1, 1);
        glTranslatef(position.x, position.y, position.z);
        drawSphere(5, 10, 5);
    }glPopMatrix();
}


void generateSpheres(){
    for(int i = 0; i < spheres.size(); i++){
        glPushMatrix();{
            glTranslatef(spheres[i].center.x, spheres[i].center.y, spheres[i].center.z);
            glColor3f(spheres[i].color.x, spheres[i].color.y, spheres[i].color.z);
            drawSphere(spheres[i].radius, 15, 10);
        }glPopMatrix();
    }
}


void generatePyramids(){
    for(int i = 0; i < pyramids.size(); i++){
        glPushMatrix();{
            glColor3f(pyramids[i].color.x, pyramids[i].color.y, pyramids[i].color.z);
            drawPyramid(pyramids[i].lowestPoint, pyramids[i].width, pyramids[i].height);
        }glPopMatrix();
    }
}


void generateLightSources(){
    for(int i = 0; i < normalLights.size(); i++){
        drawLightSource(normalLights[i].position);
    }

    for(int j = 0; j < spotLights.size(); j++){
        drawLightSource(spotLights[j].position);
    }
}


void drawCheckerboard(){
    int blackSquare = 0;

    for(int i = -3*farPlaneDistance; i <= 3*farPlaneDistance; i += checkerboardWidth){
        for(int j = -3*farPlaneDistance; j <= 3*farPlaneDistance; j += checkerboardWidth){
            glPushMatrix();
            if(blackSquare == 0){
                glColor3f(0, 0, 0);
                glTranslatef(i, j, 0);
                drawSquare(checkerboardWidth / 2);
                blackSquare = 1;
            }
            else if(blackSquare == 1){
                glColor3f(1, 1, 1);
                glTranslatef(i, j, 0);
                drawSquare(checkerboardWidth / 2);
                blackSquare = 0;
            }
            glPopMatrix();
        }
    }

}


void calculatePoints(){
    fovX = fovY * aspectRatio;
    screenWidth = 2 * nearPlaneDistance * tan(fovX*0.5*(pi/180));
    screenHeight = 2 * nearPlaneDistance * tan(fovY*0.5*(pi/180));

    point midPoint;
    midPoint.x = pos.x + l.x * nearPlaneDistance;
    midPoint.y = pos.y + l.y * nearPlaneDistance;
    midPoint.z = pos.z + l.z * nearPlaneDistance;

    unitWidth = screenWidth / (double) numberOfPixels;
    unitHeight = screenHeight / (double) numberOfPixels;

    point topLeftPoint;
    topLeftPoint.x = midPoint.x - (screenWidth/2.0) * r.x + u.x * (screenHeight/2.0);
    topLeftPoint.y = midPoint.y - (screenWidth/2.0) * r.y + u.y * (screenHeight/2.0);
    topLeftPoint.z = midPoint.z - (screenWidth/2.0) * r.z + u.z * (screenHeight/2.0);

    for(int i = 0; i < numberOfPixels; i++){
        for(int j = 0; j < numberOfPixels; j++){
            point p;
            p.x = topLeftPoint.x + r.x * j * unitWidth - u.x * i * unitHeight;
            p.y = topLeftPoint.y + r.y * j * unitWidth - u.y * i * unitHeight;
            p.z = topLeftPoint.z + r.z * j * unitWidth - u.z * i * unitHeight;

            pointBuffer[i][j] = p;
        }
    }
}


void generateImage(){
    calculatePoints();

    Color** pixels = new Color*[numberOfPixels];
    for (int i = 0; i < numberOfPixels; i++){
        pixels[i] = new Color[numberOfPixels];
    }

    double t;

    for(int i = 0; i < numberOfPixels; i++){
        for(int j = 0; j < numberOfPixels; j++){
            t_min = farPlaneDistance - nearPlaneDistance;
            point p = pointBuffer[i][j];

            Vector direction;
            direction.x = p.x - pos.x;
            direction.y = p.y - pos.y;
            direction.z = p.z - pos.z;
            direction = normalize(direction);

            //CHECKERBOARD
            t = (-p.z) / direction.z;

            if(t > 0 && t < t_min){
                t_min = t;
                Vector object;
                object.x = p.x + direction.x * t;
                object.y = p.y + direction.y * t;
                object.z = 0;
                Color color;

                int cellX = (int) (object.x + 3 * farPlaneDistance + (checkerboardWidth/2)) / checkerboardWidth;
                int cellY = (int) (object.y + 3 * farPlaneDistance + (checkerboardWidth/2)) / checkerboardWidth;
                int cellZ = (int) (object.z + 3 * farPlaneDistance + (checkerboardWidth/2)) / checkerboardWidth;

                if((cellX + cellY) % 2) color = {1.0 * ambientCoef, 1.0 * ambientCoef, 1.0 * ambientCoef};
                else color = {0.0 * ambientCoef, 0.0 * ambientCoef, 0.0 * ambientCoef};

                pixels[j][i] = color;

            }

            //SPHERE
            for(int k = 0; k < spheres.size(); k++){
                sphere s = spheres[k];
                Vector r_0;
                r_0.x = p.x - s.center.x;
                r_0.y = p.y - s.center.y;
                r_0.z = p.z - s.center.z;

                double a = 1.0;
                double b = 2 * getDotProduct(r_0, direction);
                double c = getDotProduct(r_0, r_0) - s.radius * s.radius;
                double d = b * b - 4.0 * a * c;
                if(d < 0) continue;
                d = sqrt(d);
                double t1 = (-b + d) / (2 * a);
                double t2 = (-b - d) / (2 * a);

                if(t1 >= 0.0 && t2 >= 0.0) t = min(t1, t2);
                else t = max(t1, t2);

                if(t > 0 && t < t_min){
                    t_min = t;
                    Color color = {s.color.x * s.ambientC, s.color.y * s.ambientC, s.color.z * s.ambientC};
                    pixels[j][i] = color;
                }
            }

            //PYRAMID
            for(int l = 0; l < pyramids.size(); l++){
                pyramid py = pyramids[l];

                t = (py.lowestPoint.z - p.z) / direction.z;

                if(t > 0 && t < t_min){
                    Vector object;
                    object.x = p.x + direction.x * t;
                    object.y = p.y + direction.y * t;
                    object.z = p.z + direction.z * t;

                    if((object.x > py.lowestPoint.x && object.x < py.lowestPoint.x + py.width)
                       && (object.y > py.lowestPoint.y && object.y < py.lowestPoint.y + py.width)){
                        t_min = t;
                        Color color = {py.color.x * py.ambientC, py.color.y * py.ambientC, py.color.z * py.ambientC};
                        pixels[j][i] = color;
                    }
                }

                point topPoint = {py.lowestPoint.x + (py.width/2.0), py.lowestPoint.y + (py.width/2.0), py.lowestPoint.z + py.height};
                point a, b = topPoint, c;
                Vector r_0 = p;
                Vector r_d = direction;
                double tempMatrix[3][3];
                double A, beta, gamma, temp;

                {
                a.x = py.lowestPoint.x;
                a.y = py.lowestPoint.y;
                a.z = py.lowestPoint.z;
                c.x = py.lowestPoint.x + py.width;
                c.y = py.lowestPoint.y;
                c.z = py.lowestPoint.z;

                //A
                tempMatrix[0][0] = a.x - b.x;
                tempMatrix[0][1] = a.x - c.x;
                tempMatrix[0][2] = r_d.x;
                tempMatrix[1][0] = a.y - b.y;
                tempMatrix[1][1] = a.y - c.y;
                tempMatrix[1][2] = r_d.y;
                tempMatrix[2][0] = a.z - b.z;
                tempMatrix[2][1] = a.z - c.z;
                tempMatrix[2][2] = r_d.z;
                A = getDeterminant(tempMatrix);
                if(A == 0.0) continue;

                //beta
                tempMatrix[0][0] = a.x - r_0.x;
                tempMatrix[0][1] = a.x - c.x;
                tempMatrix[0][2] = r_d.x;
                tempMatrix[1][0] = a.y - r_0.y;
                tempMatrix[1][1] = a.y - c.y;
                tempMatrix[1][2] = r_d.y;
                tempMatrix[2][0] = a.z - r_0.z;
                tempMatrix[2][1] = a.z - c.z;
                tempMatrix[2][2] = r_d.z;
                beta = getDeterminant(tempMatrix);
                beta = beta / A;

                //gamma
                tempMatrix[0][0] = a.x - b.x;
                tempMatrix[0][1] = a.x - r_0.x;
                tempMatrix[0][2] = r_d.x;
                tempMatrix[1][0] = a.y - b.y;
                tempMatrix[1][1] = a.y - r_0.y;
                tempMatrix[1][2] = r_d.y;
                tempMatrix[2][0] = a.z - b.z;
                tempMatrix[2][1] = a.z - r_0.z;
                tempMatrix[2][2] = r_d.z;
                gamma = getDeterminant(tempMatrix);
                gamma = gamma / A;

                //t
                tempMatrix[0][0] = a.x - b.x;
                tempMatrix[0][1] = a.x - c.x;
                tempMatrix[0][2] = a.x - r_0.x;
                tempMatrix[1][0] = a.y - b.y;
                tempMatrix[1][1] = a.y - c.y;
                tempMatrix[1][2] = a.y - r_0.y;
                tempMatrix[2][0] = a.z - b.z;
                tempMatrix[2][1] = a.z - c.z;
                tempMatrix[2][2] = a.z - r_0.z;
                temp = getDeterminant(tempMatrix);
                t = temp / A;

                if(t > 0 && t < t_min){
                    if ((beta + gamma) < 1.0 && beta > 0.0 && gamma > 0.0){
                        t_min = t;
                        Color color = {py.color.x * py.ambientC, py.color.y * py.ambientC, py.color.z * py.ambientC};
                        pixels[j][i] = color;
                    }
                }
                }

                {
                a.x = py.lowestPoint.x + py.width;
                a.y = py.lowestPoint.y + py.width;
                a.z = py.lowestPoint.z;
                c.x = py.lowestPoint.x + py.width;
                c.y = py.lowestPoint.y;
                c.z = py.lowestPoint.z;

                //A
                tempMatrix[0][0] = a.x - b.x;
                tempMatrix[0][1] = a.x - c.x;
                tempMatrix[0][2] = r_d.x;
                tempMatrix[1][0] = a.y - b.y;
                tempMatrix[1][1] = a.y - c.y;
                tempMatrix[1][2] = r_d.y;
                tempMatrix[2][0] = a.z - b.z;
                tempMatrix[2][1] = a.z - c.z;
                tempMatrix[2][2] = r_d.z;
                A = getDeterminant(tempMatrix);
                if(A == 0.0) continue;

                //beta
                tempMatrix[0][0] = a.x - r_0.x;
                tempMatrix[0][1] = a.x - c.x;
                tempMatrix[0][2] = r_d.x;
                tempMatrix[1][0] = a.y - r_0.y;
                tempMatrix[1][1] = a.y - c.y;
                tempMatrix[1][2] = r_d.y;
                tempMatrix[2][0] = a.z - r_0.z;
                tempMatrix[2][1] = a.z - c.z;
                tempMatrix[2][2] = r_d.z;
                beta = getDeterminant(tempMatrix);
                beta = beta / A;

                //gamma
                tempMatrix[0][0] = a.x - b.x;
                tempMatrix[0][1] = a.x - r_0.x;
                tempMatrix[0][2] = r_d.x;
                tempMatrix[1][0] = a.y - b.y;
                tempMatrix[1][1] = a.y - r_0.y;
                tempMatrix[1][2] = r_d.y;
                tempMatrix[2][0] = a.z - b.z;
                tempMatrix[2][1] = a.z - r_0.z;
                tempMatrix[2][2] = r_d.z;
                gamma = getDeterminant(tempMatrix);
                gamma = gamma / A;

                //t
                tempMatrix[0][0] = a.x - b.x;
                tempMatrix[0][1] = a.x - c.x;
                tempMatrix[0][2] = a.x - r_0.x;
                tempMatrix[1][0] = a.y - b.y;
                tempMatrix[1][1] = a.y - c.y;
                tempMatrix[1][2] = a.y - r_0.y;
                tempMatrix[2][0] = a.z - b.z;
                tempMatrix[2][1] = a.z - c.z;
                tempMatrix[2][2] = a.z - r_0.z;
                temp = getDeterminant(tempMatrix);
                t = temp / A;

                if(t > 0 && t < t_min){
                    if ((beta + gamma) < 1.0 && beta > 0.0 && gamma > 0.0){
                        t_min = t;
                        Color color = {py.color.x * py.ambientC, py.color.y * py.ambientC, py.color.z * py.ambientC};
                        pixels[j][i] = color;
                    }
                }
                }

                {
                a.x = py.lowestPoint.x + py.width;
                a.y = py.lowestPoint.y + py.width;
                a.z = py.lowestPoint.z;
                c.x = py.lowestPoint.x;
                c.y = py.lowestPoint.y + py.width;
                c.z = py.lowestPoint.z;

                //A
                tempMatrix[0][0] = a.x - b.x;
                tempMatrix[0][1] = a.x - c.x;
                tempMatrix[0][2] = r_d.x;
                tempMatrix[1][0] = a.y - b.y;
                tempMatrix[1][1] = a.y - c.y;
                tempMatrix[1][2] = r_d.y;
                tempMatrix[2][0] = a.z - b.z;
                tempMatrix[2][1] = a.z - c.z;
                tempMatrix[2][2] = r_d.z;
                A = getDeterminant(tempMatrix);
                if(A == 0.0) continue;

                //beta
                tempMatrix[0][0] = a.x - r_0.x;
                tempMatrix[0][1] = a.x - c.x;
                tempMatrix[0][2] = r_d.x;
                tempMatrix[1][0] = a.y - r_0.y;
                tempMatrix[1][1] = a.y - c.y;
                tempMatrix[1][2] = r_d.y;
                tempMatrix[2][0] = a.z - r_0.z;
                tempMatrix[2][1] = a.z - c.z;
                tempMatrix[2][2] = r_d.z;
                beta = getDeterminant(tempMatrix);
                beta = beta / A;

                //gamma
                tempMatrix[0][0] = a.x - b.x;
                tempMatrix[0][1] = a.x - r_0.x;
                tempMatrix[0][2] = r_d.x;
                tempMatrix[1][0] = a.y - b.y;
                tempMatrix[1][1] = a.y - r_0.y;
                tempMatrix[1][2] = r_d.y;
                tempMatrix[2][0] = a.z - b.z;
                tempMatrix[2][1] = a.z - r_0.z;
                tempMatrix[2][2] = r_d.z;
                gamma = getDeterminant(tempMatrix);
                gamma = gamma / A;

                //t
                tempMatrix[0][0] = a.x - b.x;
                tempMatrix[0][1] = a.x - c.x;
                tempMatrix[0][2] = a.x - r_0.x;
                tempMatrix[1][0] = a.y - b.y;
                tempMatrix[1][1] = a.y - c.y;
                tempMatrix[1][2] = a.y - r_0.y;
                tempMatrix[2][0] = a.z - b.z;
                tempMatrix[2][1] = a.z - c.z;
                tempMatrix[2][2] = a.z - r_0.z;
                temp = getDeterminant(tempMatrix);
                t = temp / A;

                if(t > 0 && t < t_min){
                    if ((beta + gamma) < 1.0 && beta > 0.0 && gamma > 0.0){
                        t_min = t;
                        Color color = {py.color.x * py.ambientC, py.color.y * py.ambientC, py.color.z * py.ambientC};
                        pixels[j][i] = color;
                    }
                }
                }

                {
                a.x = py.lowestPoint.x;
                a.y = py.lowestPoint.y;
                a.z = py.lowestPoint.z;
                c.x = py.lowestPoint.x;
                c.y = py.lowestPoint.y + py.width;
                c.z = py.lowestPoint.z;

                //A
                tempMatrix[0][0] = a.x - b.x;
                tempMatrix[0][1] = a.x - c.x;
                tempMatrix[0][2] = r_d.x;
                tempMatrix[1][0] = a.y - b.y;
                tempMatrix[1][1] = a.y - c.y;
                tempMatrix[1][2] = r_d.y;
                tempMatrix[2][0] = a.z - b.z;
                tempMatrix[2][1] = a.z - c.z;
                tempMatrix[2][2] = r_d.z;
                A = getDeterminant(tempMatrix);
                if(A == 0.0) continue;

                //beta
                tempMatrix[0][0] = a.x - r_0.x;
                tempMatrix[0][1] = a.x - c.x;
                tempMatrix[0][2] = r_d.x;
                tempMatrix[1][0] = a.y - r_0.y;
                tempMatrix[1][1] = a.y - c.y;
                tempMatrix[1][2] = r_d.y;
                tempMatrix[2][0] = a.z - r_0.z;
                tempMatrix[2][1] = a.z - c.z;
                tempMatrix[2][2] = r_d.z;
                beta = getDeterminant(tempMatrix);
                beta = beta / A;

                //gamma
                tempMatrix[0][0] = a.x - b.x;
                tempMatrix[0][1] = a.x - r_0.x;
                tempMatrix[0][2] = r_d.x;
                tempMatrix[1][0] = a.y - b.y;
                tempMatrix[1][1] = a.y - r_0.y;
                tempMatrix[1][2] = r_d.y;
                tempMatrix[2][0] = a.z - b.z;
                tempMatrix[2][1] = a.z - r_0.z;
                tempMatrix[2][2] = r_d.z;
                gamma = getDeterminant(tempMatrix);
                gamma = gamma / A;

                //t
                tempMatrix[0][0] = a.x - b.x;
                tempMatrix[0][1] = a.x - c.x;
                tempMatrix[0][2] = a.x - r_0.x;
                tempMatrix[1][0] = a.y - b.y;
                tempMatrix[1][1] = a.y - c.y;
                tempMatrix[1][2] = a.y - r_0.y;
                tempMatrix[2][0] = a.z - b.z;
                tempMatrix[2][1] = a.z - c.z;
                tempMatrix[2][2] = a.z - r_0.z;
                temp = getDeterminant(tempMatrix);
                t = temp / A;

                if(t > 0 && t < t_min){
                    if ((beta + gamma) < 1.0 && beta > 0.0 && gamma > 0.0){
                        t_min = t;
                        Color color = {py.color.x * py.ambientC, py.color.y * py.ambientC, py.color.z * py.ambientC};
                        pixels[j][i] = color;
                    }
                }
                }
            }
        }
    }

    bitmap_image image(numberOfPixels, numberOfPixels);
    for (int x = 0; x < numberOfPixels; x++){
        for (int y = 0; y < numberOfPixels; y++){
            image.set_pixel(x, y, pixels[x][y].x*255, pixels[x][y].y*255, pixels[x][y].z*255);
        }
    }
    image.save_image("out.bmp");

    for(int i = 0; i < numberOfPixels; i++){
        delete[] pixels[i];
    }
    delete[] pixels;

}


void generateTexture(){
    bitmap_image b_img ("texture.bmp");
    Color** textureBuffer;
    int height, width;

    height = b_img.height();
    width = b_img.width();
    textureBuffer = new Color* [width];
    for (int i = 0; i < width; i++) {
        textureBuffer[i] = new Color[height];
        for (int j = 0; j < height; j++) {
            unsigned char r, g, b;
            b_img.get_pixel(i, j, r, g, b);
            Color c = {r/255.0, g/255.0, b/255.0};
            textureBuffer[i][j] = c;
        }
    }

    calculatePoints();

    Color** pixels = new Color*[numberOfPixels];
    for (int i = 0; i < numberOfPixels; i++){
        pixels[i] = new Color[numberOfPixels];
    }

    double t;

    for(int i = 0; i < numberOfPixels; i++){
        for(int j = 0; j < numberOfPixels; j++){
            t_min = farPlaneDistance - nearPlaneDistance;
            point p = pointBuffer[i][j];

            Vector direction;
            direction.x = p.x - pos.x;
            direction.y = p.y - pos.y;
            direction.z = p.z - pos.z;
            direction = normalize(direction);

            //CHECKERBOARD
            t = (-p.z) / direction.z;

            if(t > 0 && t < t_min){
                t_min = t;
                Vector object;
                object.x = p.x + direction.x * t;
                object.y = p.y + direction.y * t;
                object.z = 0;
                Color color;

                int cellX = (int) (object.x + 3 * farPlaneDistance + (checkerboardWidth/2)) / checkerboardWidth;
                int cellY = (int) (object.y + 3 * farPlaneDistance + (checkerboardWidth/2)) / checkerboardWidth;
                int cellZ = (int) (object.z + 3 * farPlaneDistance + (checkerboardWidth/2)) / checkerboardWidth;

                object.x -= checkerboardWidth * cellX;
                object.y -= checkerboardWidth * cellY;
                object.x += 3 * farPlaneDistance + (checkerboardWidth/2);
                object.y += 3 * farPlaneDistance + (checkerboardWidth/2);
                cellX = width * object.x / checkerboardWidth;
                cellY = height * object.y / checkerboardWidth;

                pixels[j][i] = textureBuffer[cellX][cellY];
                pixels[j][i].x *= ambientCoef;
                pixels[j][i].y *= ambientCoef;
                pixels[j][i].z *= ambientCoef;

            }

            //SPHERE
            for(int k = 0; k < spheres.size(); k++){
                sphere s = spheres[k];
                Vector r_0;
                r_0.x = p.x - s.center.x;
                r_0.y = p.y - s.center.y;
                r_0.z = p.z - s.center.z;

                double a = 1.0;
                double b = 2 * getDotProduct(r_0, direction);
                double c = getDotProduct(r_0, r_0) - s.radius * s.radius;
                double d = b * b - 4.0 * a * c;
                if(d < 0) continue;
                d = sqrt(d);
                double t1 = (-b + d) / (2 * a);
                double t2 = (-b - d) / (2 * a);

                if(t1 >= 0.0 && t2 >= 0.0) t = min(t1, t2);
                else t = max(t1, t2);

                if(t > 0 && t < t_min){
                    t_min = t;
                    Color color = {s.color.x * s.ambientC, s.color.y * s.ambientC, s.color.z * s.ambientC};
                    pixels[j][i] = color;
                }
            }

            //PYRAMID
            for(int l = 0; l < pyramids.size(); l++){
                pyramid py = pyramids[l];

                t = (py.lowestPoint.z - p.z) / direction.z;

                if(t > 0 && t < t_min){
                    Vector object;
                    object.x = p.x + direction.x * t;
                    object.y = p.y + direction.y * t;
                    object.z = p.z + direction.z * t;

                    if((object.x > py.lowestPoint.x && object.x < py.lowestPoint.x + py.width)
                       && (object.y > py.lowestPoint.y && object.y < py.lowestPoint.y + py.width)){
                        t_min = t;
                        Color color = {py.color.x * py.ambientC, py.color.y * py.ambientC, py.color.z * py.ambientC};
                        pixels[j][i] = color;
                    }
                }

                point topPoint = {py.lowestPoint.x + (py.width/2.0), py.lowestPoint.y + (py.width/2.0), py.lowestPoint.z + py.height};
                point a, b = topPoint, c;
                Vector r_0 = p;
                Vector r_d = direction;
                double tempMatrix[3][3];
                double A, beta, gamma, temp;

                {
                a.x = py.lowestPoint.x;
                a.y = py.lowestPoint.y;
                a.z = py.lowestPoint.z;
                c.x = py.lowestPoint.x + py.width;
                c.y = py.lowestPoint.y;
                c.z = py.lowestPoint.z;

                //A
                tempMatrix[0][0] = a.x - b.x;
                tempMatrix[0][1] = a.x - c.x;
                tempMatrix[0][2] = r_d.x;
                tempMatrix[1][0] = a.y - b.y;
                tempMatrix[1][1] = a.y - c.y;
                tempMatrix[1][2] = r_d.y;
                tempMatrix[2][0] = a.z - b.z;
                tempMatrix[2][1] = a.z - c.z;
                tempMatrix[2][2] = r_d.z;
                A = getDeterminant(tempMatrix);
                if(A == 0.0) continue;

                //beta
                tempMatrix[0][0] = a.x - r_0.x;
                tempMatrix[0][1] = a.x - c.x;
                tempMatrix[0][2] = r_d.x;
                tempMatrix[1][0] = a.y - r_0.y;
                tempMatrix[1][1] = a.y - c.y;
                tempMatrix[1][2] = r_d.y;
                tempMatrix[2][0] = a.z - r_0.z;
                tempMatrix[2][1] = a.z - c.z;
                tempMatrix[2][2] = r_d.z;
                beta = getDeterminant(tempMatrix);
                beta = beta / A;

                //gamma
                tempMatrix[0][0] = a.x - b.x;
                tempMatrix[0][1] = a.x - r_0.x;
                tempMatrix[0][2] = r_d.x;
                tempMatrix[1][0] = a.y - b.y;
                tempMatrix[1][1] = a.y - r_0.y;
                tempMatrix[1][2] = r_d.y;
                tempMatrix[2][0] = a.z - b.z;
                tempMatrix[2][1] = a.z - r_0.z;
                tempMatrix[2][2] = r_d.z;
                gamma = getDeterminant(tempMatrix);
                gamma = gamma / A;

                //t
                tempMatrix[0][0] = a.x - b.x;
                tempMatrix[0][1] = a.x - c.x;
                tempMatrix[0][2] = a.x - r_0.x;
                tempMatrix[1][0] = a.y - b.y;
                tempMatrix[1][1] = a.y - c.y;
                tempMatrix[1][2] = a.y - r_0.y;
                tempMatrix[2][0] = a.z - b.z;
                tempMatrix[2][1] = a.z - c.z;
                tempMatrix[2][2] = a.z - r_0.z;
                temp = getDeterminant(tempMatrix);
                t = temp / A;

                if(t > 0 && t < t_min){
                    if ((beta + gamma) < 1.0 && beta > 0.0 && gamma > 0.0){
                        t_min = t;
                        Color color = {py.color.x * py.ambientC, py.color.y * py.ambientC, py.color.z * py.ambientC};
                        pixels[j][i] = color;
                    }
                }
                }

                {
                a.x = py.lowestPoint.x + py.width;
                a.y = py.lowestPoint.y + py.width;
                a.z = py.lowestPoint.z;
                c.x = py.lowestPoint.x + py.width;
                c.y = py.lowestPoint.y;
                c.z = py.lowestPoint.z;

                //A
                tempMatrix[0][0] = a.x - b.x;
                tempMatrix[0][1] = a.x - c.x;
                tempMatrix[0][2] = r_d.x;
                tempMatrix[1][0] = a.y - b.y;
                tempMatrix[1][1] = a.y - c.y;
                tempMatrix[1][2] = r_d.y;
                tempMatrix[2][0] = a.z - b.z;
                tempMatrix[2][1] = a.z - c.z;
                tempMatrix[2][2] = r_d.z;
                A = getDeterminant(tempMatrix);
                if(A == 0.0) continue;

                //beta
                tempMatrix[0][0] = a.x - r_0.x;
                tempMatrix[0][1] = a.x - c.x;
                tempMatrix[0][2] = r_d.x;
                tempMatrix[1][0] = a.y - r_0.y;
                tempMatrix[1][1] = a.y - c.y;
                tempMatrix[1][2] = r_d.y;
                tempMatrix[2][0] = a.z - r_0.z;
                tempMatrix[2][1] = a.z - c.z;
                tempMatrix[2][2] = r_d.z;
                beta = getDeterminant(tempMatrix);
                beta = beta / A;

                //gamma
                tempMatrix[0][0] = a.x - b.x;
                tempMatrix[0][1] = a.x - r_0.x;
                tempMatrix[0][2] = r_d.x;
                tempMatrix[1][0] = a.y - b.y;
                tempMatrix[1][1] = a.y - r_0.y;
                tempMatrix[1][2] = r_d.y;
                tempMatrix[2][0] = a.z - b.z;
                tempMatrix[2][1] = a.z - r_0.z;
                tempMatrix[2][2] = r_d.z;
                gamma = getDeterminant(tempMatrix);
                gamma = gamma / A;

                //t
                tempMatrix[0][0] = a.x - b.x;
                tempMatrix[0][1] = a.x - c.x;
                tempMatrix[0][2] = a.x - r_0.x;
                tempMatrix[1][0] = a.y - b.y;
                tempMatrix[1][1] = a.y - c.y;
                tempMatrix[1][2] = a.y - r_0.y;
                tempMatrix[2][0] = a.z - b.z;
                tempMatrix[2][1] = a.z - c.z;
                tempMatrix[2][2] = a.z - r_0.z;
                temp = getDeterminant(tempMatrix);
                t = temp / A;

                if(t > 0 && t < t_min){
                    if ((beta + gamma) < 1.0 && beta > 0.0 && gamma > 0.0){
                        t_min = t;
                        Color color = {py.color.x * py.ambientC, py.color.y * py.ambientC, py.color.z * py.ambientC};
                        pixels[j][i] = color;
                    }
                }
                }

                {
                a.x = py.lowestPoint.x + py.width;
                a.y = py.lowestPoint.y + py.width;
                a.z = py.lowestPoint.z;
                c.x = py.lowestPoint.x;
                c.y = py.lowestPoint.y + py.width;
                c.z = py.lowestPoint.z;

                //A
                tempMatrix[0][0] = a.x - b.x;
                tempMatrix[0][1] = a.x - c.x;
                tempMatrix[0][2] = r_d.x;
                tempMatrix[1][0] = a.y - b.y;
                tempMatrix[1][1] = a.y - c.y;
                tempMatrix[1][2] = r_d.y;
                tempMatrix[2][0] = a.z - b.z;
                tempMatrix[2][1] = a.z - c.z;
                tempMatrix[2][2] = r_d.z;
                A = getDeterminant(tempMatrix);
                if(A == 0.0) continue;

                //beta
                tempMatrix[0][0] = a.x - r_0.x;
                tempMatrix[0][1] = a.x - c.x;
                tempMatrix[0][2] = r_d.x;
                tempMatrix[1][0] = a.y - r_0.y;
                tempMatrix[1][1] = a.y - c.y;
                tempMatrix[1][2] = r_d.y;
                tempMatrix[2][0] = a.z - r_0.z;
                tempMatrix[2][1] = a.z - c.z;
                tempMatrix[2][2] = r_d.z;
                beta = getDeterminant(tempMatrix);
                beta = beta / A;

                //gamma
                tempMatrix[0][0] = a.x - b.x;
                tempMatrix[0][1] = a.x - r_0.x;
                tempMatrix[0][2] = r_d.x;
                tempMatrix[1][0] = a.y - b.y;
                tempMatrix[1][1] = a.y - r_0.y;
                tempMatrix[1][2] = r_d.y;
                tempMatrix[2][0] = a.z - b.z;
                tempMatrix[2][1] = a.z - r_0.z;
                tempMatrix[2][2] = r_d.z;
                gamma = getDeterminant(tempMatrix);
                gamma = gamma / A;

                //t
                tempMatrix[0][0] = a.x - b.x;
                tempMatrix[0][1] = a.x - c.x;
                tempMatrix[0][2] = a.x - r_0.x;
                tempMatrix[1][0] = a.y - b.y;
                tempMatrix[1][1] = a.y - c.y;
                tempMatrix[1][2] = a.y - r_0.y;
                tempMatrix[2][0] = a.z - b.z;
                tempMatrix[2][1] = a.z - c.z;
                tempMatrix[2][2] = a.z - r_0.z;
                temp = getDeterminant(tempMatrix);
                t = temp / A;

                if(t > 0 && t < t_min){
                    if ((beta + gamma) < 1.0 && beta > 0.0 && gamma > 0.0){
                        t_min = t;
                        Color color = {py.color.x * py.ambientC, py.color.y * py.ambientC, py.color.z * py.ambientC};
                        pixels[j][i] = color;
                    }
                }
                }

                {
                a.x = py.lowestPoint.x;
                a.y = py.lowestPoint.y;
                a.z = py.lowestPoint.z;
                c.x = py.lowestPoint.x;
                c.y = py.lowestPoint.y + py.width;
                c.z = py.lowestPoint.z;

                //A
                tempMatrix[0][0] = a.x - b.x;
                tempMatrix[0][1] = a.x - c.x;
                tempMatrix[0][2] = r_d.x;
                tempMatrix[1][0] = a.y - b.y;
                tempMatrix[1][1] = a.y - c.y;
                tempMatrix[1][2] = r_d.y;
                tempMatrix[2][0] = a.z - b.z;
                tempMatrix[2][1] = a.z - c.z;
                tempMatrix[2][2] = r_d.z;
                A = getDeterminant(tempMatrix);
                if(A == 0.0) continue;

                //beta
                tempMatrix[0][0] = a.x - r_0.x;
                tempMatrix[0][1] = a.x - c.x;
                tempMatrix[0][2] = r_d.x;
                tempMatrix[1][0] = a.y - r_0.y;
                tempMatrix[1][1] = a.y - c.y;
                tempMatrix[1][2] = r_d.y;
                tempMatrix[2][0] = a.z - r_0.z;
                tempMatrix[2][1] = a.z - c.z;
                tempMatrix[2][2] = r_d.z;
                beta = getDeterminant(tempMatrix);
                beta = beta / A;

                //gamma
                tempMatrix[0][0] = a.x - b.x;
                tempMatrix[0][1] = a.x - r_0.x;
                tempMatrix[0][2] = r_d.x;
                tempMatrix[1][0] = a.y - b.y;
                tempMatrix[1][1] = a.y - r_0.y;
                tempMatrix[1][2] = r_d.y;
                tempMatrix[2][0] = a.z - b.z;
                tempMatrix[2][1] = a.z - r_0.z;
                tempMatrix[2][2] = r_d.z;
                gamma = getDeterminant(tempMatrix);
                gamma = gamma / A;

                //t
                tempMatrix[0][0] = a.x - b.x;
                tempMatrix[0][1] = a.x - c.x;
                tempMatrix[0][2] = a.x - r_0.x;
                tempMatrix[1][0] = a.y - b.y;
                tempMatrix[1][1] = a.y - c.y;
                tempMatrix[1][2] = a.y - r_0.y;
                tempMatrix[2][0] = a.z - b.z;
                tempMatrix[2][1] = a.z - c.z;
                tempMatrix[2][2] = a.z - r_0.z;
                temp = getDeterminant(tempMatrix);
                t = temp / A;

                if(t > 0 && t < t_min){
                    if ((beta + gamma) < 1.0 && beta > 0.0 && gamma > 0.0){
                        t_min = t;
                        Color color = {py.color.x * py.ambientC, py.color.y * py.ambientC, py.color.z * py.ambientC};
                        pixels[j][i] = color;
                    }
                }
                }
            }
        }
    }

    bitmap_image image(numberOfPixels, numberOfPixels);
    for (int x = 0; x < numberOfPixels; x++){
        for (int y = 0; y < numberOfPixels; y++){
            image.set_pixel(x, y, pixels[x][y].x*255, pixels[x][y].y*255, pixels[x][y].z*255);
        }
    }
    image.save_image("out.bmp");

    for(int i = 0; i < numberOfPixels; i++){
        delete[] pixels[i];
    }
    delete[] pixels;

    for(int i = 0; i < width; i++){
        delete[] textureBuffer[i];
    }
    delete[] textureBuffer;

}


void keyboardListener(unsigned char key, int x,int y){
	point temp;

	switch(key){
        case '0':
            cout << "Rendering started" << endl;
            for(int i = 1; i <= 9; i++) cout << "Rendering " << i*10 <<"% complete"<<endl;
            if(texture == 0) generateImage();
            else generateTexture();
            cout << "Rendering complete" << endl;
            break;

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

        case ' ':
            drawCB = 1- drawCB;
            texture = 1 - texture;
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
    //gluLookAt(0,0,200,	0,0,0,	0,1,0);
	gluLookAt(pos.x, pos.y, pos.z, pos.x + l.x, pos.y + l.y, pos.z + l.z, u.x, u.y, u.z);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

	//drawAxes();
	if(drawCB == 1) drawCheckerboard();
	generateSpheres();
	generatePyramids();
	generateLightSources();
	//calculatePoints();
	//generateImage();

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
	drawaxes=0;
	drawCB=1;
	texture=0;
	cameraHeight=150.0;
	cameraAngle=1.0;
	angle=0;

	u = {0, 0, 1};
	r = {-1/sqrt(2), 1/sqrt(2), 0};
	l = {-1/sqrt(2), -1/sqrt(2), 0};
	pos = {200, 200, 100};


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
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance
	gluPerspective(fovY, aspectRatio, nearPlaneDistance, farPlaneDistance);
}


void readFromFile(){
    ifstream infile;
    infile.open("input.txt");

    infile >> nearPlaneDistance >> farPlaneDistance >> fovY >> aspectRatio;
    infile >> recursionLevel >> numberOfPixels;
    infile >> checkerboardWidth;
    infile >> ambientCoef >> diffuseCoef >> reflectionCoef;
    infile >> numberOfObjects;

    string command;

    for(int i = 0; i < numberOfObjects; i++){
        infile >> command;

        if(command == "sphere"){
            sphere s;

            point center;
            infile >> center.x >> center.y >> center.z;
            s.center = center;

            infile >> s.radius;

            Color color;
            infile >> color.x >> color.y >> color.z;
            {
                if(color.x > 1.0 || color.y > 1.0 || color.z > 1.0
                || color.x < 0.0 || color.y < 0.0 || color.z < 0.0) cout<<"RGB value not OK";
                else s.color = color;
            }

            infile >> s.ambientC >> s.diffuseC >> s.specularC >> s.reflectionC;
            infile >> s.shininess;

            spheres.push_back(s);
        }

        else if(command == "pyramid"){
            pyramid p;

            point lowestPoint;
            infile >> lowestPoint.x >> lowestPoint.y >> lowestPoint.z;
            p.lowestPoint = lowestPoint;

            infile >> p.width >> p.height;

            Color color;
            infile >> color.x >> color.y >> color.z;
            {
                if(color.x > 1.0 || color.y > 1.0 || color.z > 1.0
                || color.x < 0.0 || color.y < 0.0 || color.z < 0.0) cout<<"RGB value not OK";
                else p.color = color;
            }

            infile >> p.ambientC >> p.diffuseC >> p.specularC >> p.reflectionC;
            infile >> p.shininess;

            pyramids.push_back(p);
        }
    }

    infile >> numberOfNormalLight;

    for(int i = 0; i < numberOfNormalLight; i++){
        normalLightSource n;

        point position;
        infile >> position.x >> position.y >> position.z;
        n.position = position;

        infile >> n.falloffParam;

        normalLights.push_back(n);
    }

    infile >> numberOfSpotLight;

    for(int j = 0; j < numberOfSpotLight; j++){
        spotLightSource s;

        point position;
        infile >> position.x >> position.y >> position.z;
        s.position = position;

        infile >> s.falloffParam;

        point look;
        infile >> look.x >> look.y >> look.z;
        s.look = look;

        infile >> s.cutoffAngle;

        spotLights.push_back(s);
    }

    infile.close();
}


int main(int argc, char **argv){
	glutInit(&argc,argv);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("1405075_RayTracer");

    readFromFile();

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
