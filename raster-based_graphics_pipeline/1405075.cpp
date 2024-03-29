#include <iostream>
#include <fstream>
#include <cassert>
#include <iomanip>
#include <cmath>
#include <stack>
#include <queue>
#include "bitmap_image.hpp"
using namespace std;

#define pi (2*acos(0.0))
#define epsilon (1.0e-6)

class homogeneous_point
{
public:
    double x, y, z, w;

    // set the three coordinates, set w to 1
    homogeneous_point(double x, double y, double z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = 1;
    }

    /*
    default constructor. does nothing. allows declarations like below:
        matrix m;
    therefore, usage is dangerous
    */
    homogeneous_point() {
    }

    // constructs a homogeneous point with given coordinates. forces w to be 1.0
    // if w is zero, raises error
    homogeneous_point(double x, double y, double z, double w)
    {
        assert (w != 0);
        this->x = x/w;
        this->y = y/w;
        this->z = z/w;
        this->w = 1;
    }

    // adds two points. returns a point forcing w to be 1.0
    homogeneous_point operator+ (const homogeneous_point& point)
    {
        double x = this->x + point.x;
        double y = this->y + point.y;
        double z = this->z + point.z;
        double w = this->w + point.w;
        homogeneous_point p(x, y, z, w);
        return p;
    }

    // subtracts one point from another. returns a point forcing w to be 1.0
    homogeneous_point operator- (const homogeneous_point& point)
    {
        double x = this->x - point.x;
        double y = this->y - point.y;
        double z = this->z - point.z;
        double w = this->w - point.w;
        homogeneous_point p(x, y, z, w);
    }

    // Print the coordinates of a point. exists for testing purpose.
    void print()
    {
        cout << "Point: " << endl;
        cout << x << " " << y << " " << z << " " << w << endl;
    }

};


class Vector
{
public:
    double x, y, z;

    // constructs a vector with given components
    Vector(double x, double y, double z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    // keeps the direction same. recalculates the vector to be unit.
    void normalize()
    {
        double r = sqrt(x*x + y*y + z*z);
        x = x / r;
        y = y / r;
        z = z / r;
    }

    // add two vectors
    Vector operator+(const Vector& v)
    {
        Vector v1(x+v.x, y+v.y, z+v.z);
        return v1;
    }

    // subtract one vector from another
    Vector operator-(const Vector& v)
    {
        Vector v1(x-v.x, y-v.y, z-v.z);
        return v1;
    }

    // scale a vector with a given coefficient
    Vector operator* (double m)
    {
        Vector v(x*m, y*m, z*m);
        return v;
    }

    // get the dot product of two vectors
    static double dot(Vector a, Vector b)
    {
        return a.x*b.x + a.y*b.y + a.z*b.z;
    }

    // get the cross product of two vectors
    static Vector cross(Vector a, Vector b)
    {
        Vector v(a.y*b.z - a.z*b.y, b.x*a.z - b.z*a.x, a.x*b.y - a.y*b.x);
        return v;
    }

    // print a vector. only for testing purposes.
    void print ()
    {
        cout << "Vector" << endl;
        cout << x << " " << y << " " << z << endl;
    }
};


/*
The matrices are forced to be 4x4. This is because in this assignment, we will deal with points in triangles.
Maximum # of points that we will deal with at once is 3. And all the standard matrices are 4x4 (i.e. scale, translation, rotation etc.)
*/
class matrix
{
public:
    double values[4][4];
    int num_rows, num_cols;

    // only set the number of rows and cols
    matrix(int rows, int cols)
    {
        assert (rows <= 4 && cols <= 4);
        num_rows = rows;
        num_cols = cols;
    }

    // prepare an nxn square matrix
    matrix(int n)
    {
        assert (n <= 4);
        num_rows = num_cols = n;
    }

    // prepare and return an identity matrix of size nxn
    static matrix make_identity(int n)
    {
        assert (n <= 4);
        matrix m(n);
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                if (i == j)
                    m.values[i][j] = 1;
                else
                    m.values[i][j] = 0;
            }
        }
        return m;
    }

    // print the matrix. exists for testing purposes
    void print()
    {
        cout << "Matrix:" << endl;
        for (int i = 0; i < num_rows; i++)
        {
            for (int j = 0; j < num_cols; j++)
            {
                cout << values[i][j] << "\t";
            }
            cout << endl;
        }
    }

    // add the two matrices. Raise error if dimension mismatches
    matrix operator+ (const matrix& m)
    {
        assert (this->num_rows == m.num_rows);
        assert (this->num_cols == m.num_cols);

        matrix m1(num_rows, num_cols);
        for (int i = 0; i < num_rows; i++)
        {
            for (int j = 0; j < num_cols; j++)
            {
                m1.values[i][j] = values[i][j] + m.values[i][j];
            }
        }
        return m1;
    }

    // subtract a matrix from another. raise error if dimension mismatches
    matrix operator- (const matrix& m)
    {
        assert (this->num_rows == m.num_rows);
        assert (this->num_cols == m.num_cols);

        matrix m1(num_rows, num_cols);
        for (int i = 0; i < num_rows; i++)
        {
            for (int j = 0; j < num_cols; j++)
            {
                m1.values[i][j] = values[i][j] - m.values[i][j];
            }
        }
        return m1;
    }

    // multiply two matrices. allows statements like m1 = m2 * m3; raises error is dimension mismatches
    matrix operator* (const matrix& m)
    {
        assert (this->num_cols == m.num_rows);
        matrix m1(this->num_rows, m.num_cols);

        for (int i = 0; i < m1.num_rows; i++) {
            for (int j = 0; j < m1.num_cols; j++) {
                double val = 0;
                for (int k = 0; k < this->num_cols; k++) {
                    val += this->values[i][k] * m.values[k][j];
                }
                m1.values[i][j] = val;
            }
        }
        return m1;
    }

    // multiply a matrix with a constant
    matrix operator* (double m)
    {
        matrix m1(this->num_rows, this->num_cols);
        for (int i = 0; i < num_rows; i++) {
            for (int j = 0; j < num_cols; j++) {
                m1.values[i][j] = m * this->values[i][j];
            }
        }
        return m1;
    }

    // multiply a 4x4 matrix with a homogeneous point and return the resulting point.
    // usage: homogeneous_point p = m * p1;
    // here, m is a 4x4 matrix, intended to be the transformation matrix
    // p1 is the point on which the transformation is being made
    // p is the resulting homogeneous point
    homogeneous_point operator* (const homogeneous_point& p)
    {
        assert (this->num_rows == this->num_cols && this->num_rows == 4);

        matrix m(4, 1);
        m.values[0][0] = p.x;
        m.values[1][0] = p.y;
        m.values[2][0] = p.z;
        m.values[3][0] = p.w;

        matrix m1 = (*this)*m;
        homogeneous_point p1(m1.values[0][0], m1.values[1][0], m1.values[2][0], m1.values[3][0]);
        return p1;
    }

    // return the transpose of a matrix
    matrix transpose()
    {
        matrix m(num_cols, num_rows);
        for (int i = 0; i < num_rows; i++) {
            for (int j = 0; j < num_cols; j++) {
                m.values[j][i] = values[i][j];
            }
        }
        return m;
    }

};

/*
A simple class to hold the color components, r, g, b of a certain shade.
*/
class color {
public:
    double r, g, b;
    color(double r, double g, double b) {
        this->r = r;
        this->g = g;
        this->b = b;
    }
    color() {
    }
};


double eye_x, eye_y, eye_z;
double look_x, look_y, look_z;
double up_x, up_y, up_z;
double fov_x, fov_y, aspectRatio, near, far;
color backgroud;
int screen_x, screen_y;

vector<color> colorVector;



void scan_convert() {
    ifstream stage3;
    stage3.open("stage3.txt");

    color** pixels = new color*[screen_x];
    double** zs = new double*[screen_x];
    for (int i = 0; i < screen_x; i++) {
        pixels[i] = new color [screen_y];
        for (int j = 0; j < screen_y; j++) {
            pixels[i][j] = backgroud;
        }
        zs[i] = new double [screen_y];
        for (int j = 0; j < screen_y; j++) {
            zs[i][j] = +20; // a very large value intended as +INFINITY
        }
    }

    // perform scan conversion, populate the 2D array pixels
    // the array zs is the z-buffer.


    // the following code generates a bmp image. do not change this.
    bitmap_image image(screen_x, screen_y);
    for (int x = 0; x < screen_x; x++) {
        for (int y = 0; y < screen_y; y++) {
            image.set_pixel(x, y, pixels[x][y].r, pixels[x][y].g, pixels[x][y].b);
        }
    }
    image.save_image("out.bmp");

    // free the dynamically allocated memory

}

Vector R(Vector x, Vector a, double theta){
    double radian = (pi*theta) / 180;

    Vector ans = x * cos(radian) +  a * Vector::dot(x, a) * (1-cos(radian)) + Vector::cross(a, x) * sin(radian);
    return ans;
}

void stage3()
{
    if (near == far) return;
    ifstream stage2;
    ofstream stage3;
    stage2.open ("stage2.txt");
    stage3.open ("stage3.txt");
    stage3 << std::fixed;
    stage3 << std::setprecision(7);

    // process input from stage2 and write to stage3

    fov_x = fov_y * aspectRatio;
    double t = near * tan(pi*(fov_y/2)/180);
    double r = near * tan(pi*(fov_x/2)/180);

    matrix P = matrix::make_identity(4);
    P.values[0][0] = near/r;
    P.values[1][1] = near/t;
    P.values[2][2] = -(far+near)/(far-near);
    P.values[3][3] = 0;
    P.values[2][3] = -2*(far*near)/(far-near);
    P.values[3][2] = -1;

    double x, y, z;
    homogeneous_point arrayhp[3];

    while(true){
        stage2 >> x >> y >> z;
        arrayhp[0] = homogeneous_point(x, y, z);
        stage2 >> x >> y >> z;
        arrayhp[1] = homogeneous_point(x, y, z);
        stage2 >> x >> y >> z;
        arrayhp[2] = homogeneous_point(x, y, z);

        homogeneous_point s = arrayhp[2];
        homogeneous_point p = arrayhp[0];
        int i = 0;
        vector<homogeneous_point> vectorhp;

        while(i < 3){

            if(s.z <= -near && p.z <= -near){
                vectorhp.push_back(p);
            }
            else if(s.z <= -near && p.z > -near){
                double t = (-near - p.z) / (s.z - p.z);
                double x = p.x + (s.x - p.x) * t;
                double y = p.y + (s.y - p.y) * t;

                homogeneous_point point = homogeneous_point(x, y, -near);
                vectorhp.push_back(point);
            }
            else if(s.z > -near && p.z <= -near){
                double t = (-near - p.z) / (s.z - p.z);
                double x = p.x + (s.x - p.x) * t;
                double y = p.y + (s.y - p.y) * t;

                homogeneous_point point = homogeneous_point(x, y, -near);
                vectorhp.push_back(point);
                vectorhp.push_back(p);
            }

            s = arrayhp[i];
            p = arrayhp[i+1];
            i++;
        }

        s = vectorhp[vectorhp.size() - 1];
        p = vectorhp[0];
        int j = 0;
        vector<homogeneous_point> vectorFinal;

        cout << vectorhp.size();

        while(j < vectorhp.size()){
            if(s.z >= -far && p.z >= -far){
                vectorFinal.push_back(p);
            }
            else if(s.z >= -far && p.z < -far){
                double t = (-far - p.z) / (s.z - p.z);
                double x = p.x + (s.x - p.x) * t;
                double y = p.y + (s.y - p.y) * t;

                homogeneous_point point = homogeneous_point(x, y, -far);
                vectorFinal.push_back(point);
            }
            else if(s.z < -far && p.z >= -far){
                double t = (-far - p.z) / (s.z - p.z);
                double x = p.x + (s.x - p.x) * t;
                double y = p.y + (s.y - p.y) * t;

                homogeneous_point point = homogeneous_point(x, y, -far);
                vectorFinal.push_back(point);
                vectorFinal.push_back(p);
            }

            s = vectorhp[j];
            p = vectorhp[j+1];
            j++;
        }

        homogeneous_point firstPoint = vectorFinal[0];

        if(stage2.eof()) break;

        for(int k = 1; k < vectorFinal.size() - 1; k++){
            homogeneous_point t1 = P * firstPoint;
            homogeneous_point t2 = P * vectorFinal[k];
            homogeneous_point t3 = P * vectorFinal[k + 1];

            stage3 << t1.x << " " << t1.y << " " << t1.z;
            stage3 << endl;
            stage3 << t2.x << " " << t2.y << " " << t2.z;
            stage3 << endl;
            stage3 << t3.x << " " << t3.y << " " << t3.z;
            stage3 << endl;
        }
    }

    stage3.close();
    stage2.close();

}

void stage2()
{
    ifstream stage1;
    ofstream stage2;
    stage1.open ("stage1.txt");
    stage2.open ("stage2.txt");
    stage2 << std::fixed;
    stage2 << std::setprecision(7);

    // collect input from stage1 and process, write output to stage2

    Vector look = Vector(look_x, look_y, look_z);
    Vector eye = Vector(eye_x, eye_y, eye_z);
    Vector up = Vector(up_x, up_y, up_z);

    Vector l = look - eye;
    l.normalize();
    Vector r = Vector::cross(l, up);
    r.normalize();
    Vector u = Vector::cross(r, l);

    matrix T = matrix::make_identity(4);
    T.values[0][3] = -eye_x;
    T.values[1][3] = -eye_y;
    T.values[2][3] = -eye_z;

    matrix R = matrix::make_identity(4);
    R.values[0][0] = r.x;
    R.values[0][1] = r.y;
    R.values[0][2] = r.z;
    R.values[1][0] = u.x;
    R.values[1][1] = u.y;
    R.values[1][2] = u.z;
    R.values[2][0] = -l.x;
    R.values[2][1] = -l.y;
    R.values[2][2] = -l.z;

    matrix V = matrix::make_identity(4);
    V = R * T;

    double x, y, z;

    while(true){
        stage1 >> x >> y >> z;
        homogeneous_point hPoint = homogeneous_point(x, y, z);
        homogeneous_point transformedPoint = V * hPoint;

        if(stage1.eof()) break;
        stage2 << transformedPoint.x << " " << transformedPoint.y << " " << transformedPoint.z;
        stage2 << endl;
    }

    stage1.close();
    stage2.close();

}

void stage1()
{
    ifstream scene;
    ofstream stage1;
    scene.open ("scene.txt");
    stage1.open ("stage1.txt");
    stage1 << std::fixed;
    stage1 << std::setprecision(7);

    string command;

    scene >> eye_x >> eye_y >> eye_z;
    scene >> look_x >> look_y >> look_z;
    scene >> up_x >> up_y >> up_z;
    scene >> fov_y >> aspectRatio >> near >> far;
    scene >> screen_x >> screen_y;
    scene >> backgroud.r >> backgroud.g >> backgroud.b;

    // take other commands as input from scene in a loop
    // process accordingly
    // write to stage1


    stack<matrix> S;

    matrix identityMatrix = matrix::make_identity(4);
    S.push(identityMatrix);

    while(true){

        scene >> command;

        if(command == "triangle"){
            double x, y, z;
            for(int i = 0; i < 3; i++){
                scene >> x >> y >> z;
                homogeneous_point hPoint = homogeneous_point(x, y, z);
                homogeneous_point transformedPoint = S.top() * hPoint;

                stage1 << transformedPoint.x << " " << transformedPoint.y << " " << transformedPoint.z;
                stage1 << endl;
            }
            stage1 << endl;

            double r, g, b;
            scene >> r >> g >> b;
            color clr = color(r, g, b);
            colorVector.push_back(clr);
        }

        else if(command == "translate"){
            double tx, ty, tz;
            scene >> tx >> ty >> tz;

            matrix translationMatrix = matrix::make_identity(4);
            translationMatrix.values[0][3] = tx;
            translationMatrix.values[1][3] = ty;
            translationMatrix.values[2][3] = tz;

            matrix top = S.top();
            S.pop();
            S.push(translationMatrix*top);
        }

        else if(command == "scale"){
            double sx, sy, sz;
            scene >> sx >> sy >> sz;

            matrix scalingMatrix = matrix::make_identity(4);
            scalingMatrix.values[0][0] = sx;
            scalingMatrix.values[1][1] = sy;
            scalingMatrix.values[2][2] = sz;

            matrix top = S.top();
            S.pop();
            S.push(scalingMatrix*top);
        }

        else if(command == "rotate"){
            double angle, ax, ay, az;
            scene >> angle >> ax >> ay >> az;
            Vector i = Vector(1, 0, 0);
            Vector j = Vector(0, 1, 0);
            Vector k = Vector(0, 0, 1);

            Vector a = Vector(ax, ay, az);
            a.normalize();

            Vector c1 = R(i, a, angle);
            Vector c2 = R(j, a, angle);
            Vector c3 = R(k, a, angle);

            matrix rotationMatrix = matrix::make_identity(4);
            rotationMatrix.values[0][0] = c1.x;
            rotationMatrix.values[0][1] = c2.x;
            rotationMatrix.values[0][2] = c3.x;
            rotationMatrix.values[1][0] = c1.y;
            rotationMatrix.values[1][1] = c2.y;
            rotationMatrix.values[1][2] = c3.y;
            rotationMatrix.values[2][0] = c1.z;
            rotationMatrix.values[2][1] = c2.z;
            rotationMatrix.values[2][2] = c3.z;

            matrix top = S.top();
            S.pop();
            S.push(rotationMatrix*top);

        }

        else if(command == "push"){
            S.push(S.top());
        }

        else if(command == "pop") S.pop();

        else if(command == "end") break;

    }
//    initialize empty stack S
//        S.push(identity matrix)
//        while true
//        input command
//        if command = �triangle�
//        input three points and the color of the triangle
//        for each of these three point P
//        P� <- transformPoint(S.top,P)
//        output P�
//        else if command = �translate�
//        input translation amounts
//        generate the corresponding translation matrix T
//        S.push(product(S.top,T))
//        else if command = �scale�
//        input scaling factors
//        generate the corresponding scaling matrix T
//        S.push(product(S.top,T))
//        else if command = �rotate�
//        input rotation angle and axis
//        generate the corresponding rotation matrix T
//        S.push(product(S.top,T))
//        else if command = �push�
//        //do it yourself
//        else if command = �pop�
//        //do it yourself
//        else if command = �end�
//        break

    scene.close();
    stage1.close();

}

int main()
{
    cout << std::fixed;
    cout << std::setprecision(4);

    stage1();
    stage2();
    stage3();
    scan_convert();

    return 0;
}
