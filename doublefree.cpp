#include "point.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <GL/freeglut.h>
#include <FreeImage/FreeImage.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

using namespace std;

#define BODY_WIDTH 2
#define BODY_HEIGHT 4
#define BODY_DEPTH 2


// GLOBAL VARIABLES //

void* m_glutFont = GLUT_BITMAP_TIMES_ROMAN_24;

GLuint texID[3]; // Texture ID's for the three textures.

char* textureFileNames[3] = {	// File names for the files from which texture images are loaded
    (char*)"textures/grass.jpg",
    (char*)"textures/Steel.jpg",
    (char*)"textures/Zombie.jpg"
};

static size_t windowWidth = 640;
static size_t windowHeight = 480;
static float aspectRatio;
bool displayWireMesh = false;
bool viewAxis = true;
static float shoulderAngle = 0, elbowAngle = -10, hipAngle = 0, kneeAngle = 10;
float moveX = 0, moveY = 0, moveZ = 0;
int aniSeconds = 0;
int aniState = 0;
bool playAnimation = true;
bool ActivateEaster = false;

float lx = 0.0f, lz = -1.0f;
float x = 0.0f, z = 5.0f;
float angle = 0;


float redHeadRobot = 0.0f;
float greenHeadRobot = 0.0f;
float blueHeadRobot = 1.0f;

float redBodyRobot = 0.0f;
float greenBodyRobot = 0.0f;
float blueBodyRobot = 1.0f;

float RedRobotPartColor = 0.0f;
float GreenRobotPartColor = 0.0f;
float BlueRobotPartColor = 1.0f;

GLint leftMouseButton, rightMouseButton;    //status of the mouse buttons
int mouseX = 0, mouseY = 0;                 //last known X and Y of the mouse
bool sphereOn = false;                      //show the camera radius sphere

enum cameraList { CAMERA_INNER = 0, CAMERA_OUTER = 1 };
enum cameraList currentCamera = CAMERA_OUTER;

#define USING_INNER (currentCamera == CAMERA_INNER)

float M_PI = 3.141592;

Point outerCamTPR;
Point outerCamXYZ;

Point innerCamXYZ;
Point innerCamTPR;
Point innerCamDir;

typedef struct
{
    int X;
    int Y;
    int Z;
    double U;
    double V;
}VERTICES;

const double PI = 3.1415926535897;
const int space = 11;
const int VertexCount = (90 / space) * (360 / space) * 4;
VERTICES VERTEX[VertexCount];

static void QuadBox(GLfloat size, GLenum type)
{
    static GLfloat n[6][3] =
    {
      {-1.0, 0.0, 0.0},
      {0.0, 1.0, 0.0},
      {1.0, 0.0, 0.0},
      {0.0, -1.0, 0.0},
      {0.0, 0.0, 1.0},
      {0.0, 0.0, -1.0}
    };
    static GLint faces[6][4] =
    {
      {0, 1, 2, 3},
      {3, 2, 6, 7},
      {7, 6, 5, 4},
      {4, 5, 1, 0},
      {5, 6, 2, 1},
      {7, 4, 0, 3}
    };
    GLfloat v[8][3];
    GLint i;

    v[0][0] = v[1][0] = v[2][0] = v[3][0] = -size / 2;
    v[4][0] = v[5][0] = v[6][0] = v[7][0] = size / 2;
    v[0][1] = v[1][1] = v[4][1] = v[5][1] = -size / 2;
    v[2][1] = v[3][1] = v[6][1] = v[7][1] = size / 2;
    v[0][2] = v[3][2] = v[4][2] = v[7][2] = -size / 2;
    v[1][2] = v[2][2] = v[5][2] = v[6][2] = size / 2;

    for (i = 5; i >= 0; i--) {
        glBegin(type);
        glNormal3fv(&n[i][0]);
        glTexCoord2f(0.0, 0.0);
        glVertex3fv(&v[faces[i][0]][0]);
        glTexCoord2f(0.0, 1.0);
        glVertex3fv(&v[faces[i][1]][0]);
        glTexCoord2f(1.0, 1.0);
        glVertex3fv(&v[faces[i][2]][0]);
        glTexCoord2f(1.0, 0.0);
        glVertex3fv(&v[faces[i][3]][0]);
        glEnd();
    }
}

void CreateSphere(double R, double H, double K, double Z) {
    int n;
    double a;
    double b;
    n = 0;
    for (b = 0; b <= 90 - space; b += space) {

        for (a = 0; a <= 360 - space; a += space)
        {
            VERTEX[n].X = R * sin((a) / 180 * PI) * sin((b) / 180 * PI) - H;
            VERTEX[n].Y = R * cos((a) / 180 * PI) * sin((b) / 180 * PI) + K;
            VERTEX[n].Z = R * cos((b) / 180 * PI) - Z;
            VERTEX[n].V = (2 * b) / 360;
            VERTEX[n].U = (a) / 360;

            n++;
            VERTEX[n].X = R * sin((a) / 180 * PI) * sin((b + space) / 180 * PI) - H;
            VERTEX[n].Y = R * cos((a) / 180 * PI) * sin((b + space) / 180 * PI) + K;
            VERTEX[n].Z = R * cos((b + space) / 180 * PI) - Z;
            VERTEX[n].V = (2 * (b + space)) / 360;
            VERTEX[n].U = (a) / 360;
            n++;
            VERTEX[n].X = R * sin((a + space) / 180 * PI) * sin((b) / 180 * PI) - H;
            VERTEX[n].Y = R * cos((a + space) / 180 * PI) * sin((b) / 180 * PI) + K;
            VERTEX[n].Z = R * cos((b) / 180 * PI) - Z;
            VERTEX[n].V = (2 * b) / 360;
            VERTEX[n].U = (a + space) / 360;
            n++;
            VERTEX[n].X = R * sin((a + space) / 180 * PI) * sin((b + space) / 180 * PI) - H;
            VERTEX[n].Y = R * cos((a + space) / 180 * PI) * sin((b + space) / 180 * PI) + K;
            VERTEX[n].Z = R * cos((b + space) / 180 * PI) - Z;
            VERTEX[n].V = (2 * (b + space)) / 360;
            VERTEX[n].U = (a + space) / 360;
            n++;
        }
    }
}

void DisplaySphere(double R)
{

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    int b;
    glScalef(0.0125 * R, 0.0125 * R, 0.0125 * R);
    
    glBegin(GL_TRIANGLE_STRIP);
    for (b = 0; b < VertexCount; b++)
    {
        glTexCoord2f(VERTEX[b].U, VERTEX[b].V);
        glVertex3f(VERTEX[b].X, VERTEX[b].Y, -VERTEX[b].Z);
    }



    for (b = 0; b < VertexCount; b++)
    {

        glTexCoord2f(VERTEX[b].U, -VERTEX[b].V);

        glVertex3f(VERTEX[b].X, VERTEX[b].Y, VERTEX[b].Z);

    }

    glEnd();
}

void drawSphere(double size)
{
    CreateSphere(size, 0, 0, 0);
    DisplaySphere(5);
}

void DrawBox(double w, double h, double l, float red, float green, float blue) {
    glPushMatrix();
    glScalef(w, h, l);
    QuadBox(1, GL_QUADS);
    glPopMatrix();

}


void drawText(const std::string& text, const unsigned int x, const unsigned int y, const float r, const float g, const float b)
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, windowWidth, 0, windowHeight, 0, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glColor3f(r, g, b);
    glRasterPos2i(x, y);
    for (const char c : text)
        glutBitmapCharacter(m_glutFont, (int)c);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}


void draw_cylinder(GLfloat radius,
    GLfloat height,
    float R,
    float G,
    float B)
{
    GLfloat x = 0.0;
    GLfloat y = 0.0;
    GLfloat angle = 0.0;
    GLfloat angle_stepsize = 0.1;

    /** Draw the tube */
    glColor3f(R, G, B);
    glBegin(GL_QUAD_STRIP);
    angle = 0.0;
    while (angle < 2 * PI) {
        x = radius * cos(angle);
        y = radius * sin(angle);
        glVertex3f(x, y, height);
        glVertex3f(x, y, 0.0);
        angle = angle + angle_stepsize;
    }
    glVertex3f(radius, 0.0, height);
    glVertex3f(radius, 0.0, 0.0);
    glEnd();

    /** Draw the circle on top of cylinder */
    glColor3f(R, G, B);

    glBegin(GL_POLYGON);
    angle = 0.0;
    while (angle < 2 * PI) {
        x = radius * cos(angle);
        y = radius * sin(angle);
        glVertex3f(x, y, height);
        angle = angle + angle_stepsize;
    }
    glVertex3f(radius, 0.0, height);
    glEnd();
}

//non-used old functions, so that we can enable textures.

/*void DrawBox(GLdouble width, GLdouble height, GLdouble depth, float red, float green, float blue) {
    if (displayWireMesh)
    {
        glPushMatrix();
        glColor3f(1, 1, 1);
        glScalef(width, height, depth);
        glutWireCube(1.0);
        glPopMatrix();
    }
    else
    {
        glPushMatrix();
        glColor3f(red, green, blue);
        glScalef(width, height, depth);
        glutSolidCube(1.0);
        glPopMatrix();
    }
}*/


/*
void drawSphere(GLdouble width, GLdouble height, GLdouble depth, float red, float green, float blue) {
    if (displayWireMesh)
    {
        glPushMatrix();
        glColor3f(1, 1, 1);
        glScalef(width, height, depth);
        glutWireSphere(1.0f, 10, 10);
        glPopMatrix();
    }
    else
    {
        glPushMatrix();
        glColor3f(red, green, blue);
        glScalef(width, height, depth);
        glutSolidSphere(1.0f, 10, 10);
        glPopMatrix();
    }
}*/

//function for easter egg to draw a zombie
void drawZombie()
{
    //body
    glPushMatrix();
    glTranslatef(0, 0, 0);
    DrawBox(BODY_WIDTH, BODY_HEIGHT, BODY_DEPTH, 0, 0, 0.5f);
    glPopMatrix();

    // Head
    glPushMatrix();
    glTranslatef(0, 2.9f, 0);
    DrawBox(2.0, 2.0, 2.0, 0.0, 1.0, 0.0);
    glPopMatrix();

    // Right Hand
    glPushMatrix();
    glTranslatef(1.0f, 1.5f, 0);
    glRotatef((GLfloat)90, 0.0, 1.0, 0);
    glRotatef((GLfloat)90, 1.0, 0.0, 0);
    glRotatef((GLfloat)-shoulderAngle / 4, 0.0, 1.0, 0.0);
    glTranslatef(1.0f, 0, 0);
    DrawBox(2.0, 0.4, 1.0, 0, 1.0, 0); // Upper arm

    //glRotatef(90, 0, 1, 0);
    glTranslatef(1.0f, 0, 0);
    glRotatef((GLfloat)-elbowAngle / 4, 0.0, 1.0, 0.0);
    glTranslatef(1.0f, 0, 0);
    DrawBox(2.0, 0.4, 1.0, 0, 1.0, 0.0); // Lower arm


    glPopMatrix();

    // left Hand
    glPushMatrix();
    glTranslatef(-1.0f, 1.5f, 0);
    glRotatef((GLfloat)90, 0.0, 1.0, 0);
    glRotatef((GLfloat)90, 1.0, 0.0, 0);
    glRotatef((GLfloat)shoulderAngle / 4, 0.0, 1.0, 0.0);
    glTranslatef(1.0f, 0, 0);
    DrawBox(2.0, 0.4, 1.0, 0, 1.0, 0); // Upper arm

    glTranslatef(1.0f, 0, 0);
    glRotatef((GLfloat)-elbowAngle / 4, 0.0, 1.0, 0.0);
    glTranslatef(1.0f, 0, 0);
    DrawBox(2.0, 0.4, 1.0, 0, 1.0, 0); // Lower arm
    glRotatef(-90, 1, 0, 0);
    glPopMatrix();


    // right leg
    glPushMatrix();
    glTranslatef(0.7f, -2.0f, 0);
    glRotatef((GLfloat)-90, 0.0, 0.0, 1.0);
    glRotatef((GLfloat)hipAngle, 0.0, 1.0, 0.0);
    glTranslatef(1.0f, 0, 0);
    DrawBox(2.0, 0.6, 1.0, 0, 1.0, 0); // 

    glTranslatef(1.0f, 0, 0);
    glRotatef((GLfloat)-kneeAngle, 0.0, 1.0, 0.0);
    glTranslatef(1.0f, 0, 0);
    DrawBox(2.0, 0.6, 1.0, 0, 1.0, 0); // Lower arm
    glPopMatrix();


    // right leg
    glPushMatrix();
    glTranslatef(-0.7f, -2.0f, 0);
    glRotatef((GLfloat)-90, 0.0, 0.0, 1.0);
    glRotatef((GLfloat)-hipAngle, 0.0, 1.0, 0.0);
    glTranslatef(1.0f, 0, 0);
    DrawBox(2.0, 0.6, 1.0, 0, 1.0, 0); // 

    glTranslatef(1.0f, 0, 0);
    glRotatef((GLfloat)-kneeAngle, 0.0, 1.0, 0.0);
    glTranslatef(1.0f, 0, 0);
    DrawBox(2.0, 0.6, 1.0, 0, 1.0, 0); // Lower arm
    glPopMatrix();

}

void drawRobot()
{
    //body
    glPushMatrix();
    glTranslatef(0, 0, 0);
    DrawBox(BODY_WIDTH, BODY_HEIGHT, BODY_DEPTH, redBodyRobot, greenBodyRobot, blueBodyRobot);
    glPopMatrix();

    // Head
    glPushMatrix();
    glTranslatef(0, 2.9f, 0);
    drawSphere(20);
    glPopMatrix();

    // Right Hand
    glPushMatrix();
    glTranslatef(1.0f, 1.5f, 0);
    glRotatef((GLfloat)-83, 0.0, 0.0, 1.0);
    glRotatef((GLfloat)-shoulderAngle, 0.0, 1.0, 0.0);
    glTranslatef(1.0f, 0, 0);
    DrawBox(2.0, 0.4, 1.0, RedRobotPartColor, GreenRobotPartColor, BlueRobotPartColor); // Upper arm

    glTranslatef(1.0f, 0, 0);
    glRotatef((GLfloat)-elbowAngle, 0.0, 1.0, 0.0);
    glTranslatef(1.0f, 0, 0);
    DrawBox(2.0, 0.4, 1.0, RedRobotPartColor, GreenRobotPartColor, BlueRobotPartColor); // Lower arm
    glPopMatrix();

    // left Hand
    glPushMatrix();
    glTranslatef(-1.0f, 1.5f, 0);
    glRotatef((GLfloat)-97, 0.0, 0.0, 1.0);
    glRotatef((GLfloat)shoulderAngle, 0.0, 1.0, 0.0);
    glTranslatef(1.0f, 0, 0);
    DrawBox(2.0, 0.4, 1.0, RedRobotPartColor, GreenRobotPartColor, BlueRobotPartColor); // Upper arm

    glTranslatef(1.0f, 0, 0);
    glRotatef((GLfloat)-elbowAngle, 0.0, 1.0, 0.0);
    glTranslatef(1.0f, 0, 0);
    DrawBox(2.0, 0.4, 1.0, RedRobotPartColor, GreenRobotPartColor, BlueRobotPartColor); // Lower arm
    glPopMatrix();


    // right leg
    glPushMatrix();
    glTranslatef(0.7f, -2.0f, 0);
    glRotatef((GLfloat)-90, 0.0, 0.0, 1.0);
    glRotatef((GLfloat)hipAngle, 0.0, 1.0, 0.0);
    glTranslatef(1.0f, 0, 0);
    DrawBox(2.0, 0.6, 1.0, RedRobotPartColor, GreenRobotPartColor, BlueRobotPartColor); // 

    glTranslatef(1.0f, 0, 0);
    glRotatef((GLfloat)-kneeAngle, 0.0, 1.0, 0.0);
    glTranslatef(1.0f, 0, 0);
    DrawBox(2.0, 0.6, 1.0, RedRobotPartColor, GreenRobotPartColor, BlueRobotPartColor); // Lower arm
    glPopMatrix();


    // right leg
    glPushMatrix();
    glTranslatef(-0.7f, -2.0f, 0);
    glRotatef((GLfloat)-90, 0.0, 0.0, 1.0);
    glRotatef((GLfloat)-hipAngle, 0.0, 1.0, 0.0);
    glTranslatef(1.0f, 0, 0);
    DrawBox(2.0, 0.6, 1.0, RedRobotPartColor, GreenRobotPartColor, BlueRobotPartColor); // 

    glTranslatef(1.0f, 0, 0);
    glRotatef((GLfloat)-kneeAngle, 0.0, 1.0, 0.0);
    glTranslatef(1.0f, 0, 0);
    DrawBox(2.0, 0.6, 1.0, RedRobotPartColor, GreenRobotPartColor, BlueRobotPartColor); // Lower arm
    glPopMatrix();

}

void special(int key, int, int)
{
    float fraction = 0.1f;
    switch (key)
    {
    case GLUT_KEY_LEFT:
        angle -= 0.01f;
        lx = sin(angle);
        lz = -cos(angle);
        break;
    case GLUT_KEY_RIGHT:
        angle += 0.01f;
        lx = sin(angle);
        lz = -cos(angle);
        break;
    case GLUT_KEY_UP:
        x += lx * fraction;
        z += lz * fraction;
        break;
    case GLUT_KEY_DOWN:
        x -= lx * fraction;
        z -= lz * fraction;
        break;
    case GLUT_KEY_END: exit(0); break;
    defualt: return;
    }
    glutPostRedisplay();
}

void loadTextures() {
    int i;
    glGenTextures(3, texID); // Get the texture object IDs.
    for (i = 0; i < 3; i++) {
        void* imgData; // Pointer to image color data read from the file.
        int imgWidth; // The width of the image that was read.
        int imgHeight; // The height.
        FREE_IMAGE_FORMAT format = FreeImage_GetFIFFromFilename(textureFileNames[i]);
        if (format == FIF_UNKNOWN) {
            printf("Unknown file type for texture image file %s\n", textureFileNames[i]);
            continue;
        }
        FIBITMAP* bitmap = FreeImage_Load(format, textureFileNames[i], 0); // Read image from file.
        if (!bitmap) {
            printf("Failed to load image %s\n", textureFileNames[i]);
            continue;
        }
        FIBITMAP* bitmap2 = FreeImage_ConvertTo24Bits(bitmap); // Convert to RGB or BGR format
        FreeImage_Unload(bitmap);
        imgData = FreeImage_GetBits(bitmap2); // Grab the data we need from the bitmap.
        imgWidth = FreeImage_GetWidth(bitmap2);
        imgHeight = FreeImage_GetHeight(bitmap2);
        if (imgData) {
            printf("Texture image loaded from file %s, size %dx%d\n",
                textureFileNames[i], imgWidth, imgHeight);
            glBindTexture(GL_TEXTURE_2D, texID[i]); // Will load image data into texture object #i
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_BGR_EXT,
                GL_UNSIGNED_BYTE, imgData);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Required since there are no mipmaps.
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        }
        else {
            printf("Failed to get texture data from %s\n", textureFileNames[i]);
        } 
    } 
} 


void recomputeOrientation(Point& xyz, Point& tpr)
{
    xyz.x = tpr.z * sinf(tpr.x) * sinf(tpr.y);
    xyz.z = tpr.z * -cosf(tpr.x) * sinf(tpr.y);
    xyz.y = tpr.z * -cosf(tpr.y);
    glutPostRedisplay();
}

void resizeWindow(int w, int h)
{
    aspectRatio = w / (float)h;

    windowWidth = w;
    windowHeight = h;

    //update the viewport to fill the window
    glViewport(0, 0, w, h);

    //update the projection matrix with the new window properties
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, aspectRatio, 0.1, 100000);

    glutPostRedisplay();
}

void axis()
{
    glPushMatrix();
    // Draw a red x-axis, a green y-axis and a blue z-axis

    glBegin(GL_LINES);

    glColor3f(1, 0, 0); glVertex3f(0, 0, 0); glVertex3f(10, 0, 0);
    glColor3f(0, 1, 0); glVertex3f(0, 0, 0); glVertex3f(0, 10, 0);
    glColor3f(0, 0, 1); glVertex3f(0, 0, 0); glVertex3f(0, 0, 10);
    glEnd();
    glPopMatrix();
}

void timer(int v)
{

    if (playAnimation)
    {
        if (aniSeconds <= 30 && aniState == 0)
        {
            shoulderAngle += 1;
            hipAngle += 1;

            aniSeconds++;
            if (aniSeconds >= 30)
                aniState = 1;
        }
        else
        {
            shoulderAngle -= 1;
            hipAngle -= 1;
            aniSeconds--;
            if (aniSeconds <= -30)
                aniState = 0;
        }
        if (moveX > -25) {
            moveX = moveX - 0.07;
        }
        else {

            moveX = 0;
        }
    }
    glutPostRedisplay();
    glutTimerFunc(1000 / 60, timer, v);
}

void mouseCallback(int button, int state, int thisX, int thisY)
{
    //update the left and right mouse button states, if applicable
    if (button == GLUT_LEFT_BUTTON)
        leftMouseButton = state;
    else if (button == GLUT_RIGHT_BUTTON)
        rightMouseButton = state;

    //and update the last seen X and Y coordinates of the mouse
    mouseX = thisX;
    mouseY = thisY;
}


void mouseMotion(int x, int y)
{
    if (leftMouseButton == GLUT_DOWN)
    {
        Point* curTPR = (USING_INNER ? &innerCamTPR : &outerCamTPR);      //just for conciseness below
        curTPR->x += (x - mouseX) * 0.005;
        curTPR->y += (USING_INNER ? -1 : 1) * (y - mouseY) * 0.005;

        // make sure that phi stays within the range (0, M_PI)
        if (curTPR->y <= 0)
            curTPR->y = 0 + 0.001;
        if (curTPR->y >= M_PI)
            curTPR->y = M_PI - 0.001;

        //update camera (x,y,z) based on (radius,theta,phi)
        if (USING_INNER)
        {
            recomputeOrientation(innerCamDir, innerCamTPR);
            innerCamDir.normalize();
        }
        else {
            recomputeOrientation(outerCamXYZ, outerCamTPR);
        }
    }
    else if (rightMouseButton == GLUT_DOWN && !USING_INNER) {
        double totalChangeSq = (x - mouseX) + (y - mouseY);

        Point* curTPR = &outerCamTPR;      //just for conciseness below
        curTPR->z += totalChangeSq * 0.01;

        //limit the camera radius to some reasonable values so the user can't get lost
        if (curTPR->z < 2.0)
            curTPR->z = 2.0;
        if (curTPR->z > 10.0 * (currentCamera + 1))
            curTPR->z = 10.0 * (currentCamera + 1);

        //update camera (x,y,z) based on (radius,theta,phi)
        recomputeOrientation(outerCamXYZ, outerCamTPR);
    }

    mouseX = x;
    mouseY = y;
}


void initScene()
{
    glEnable(GL_DEPTH_TEST);

    float lightCol[4] = { 1, 1, 1, 1 };
    float ambientCol[4] = { 0.3, 0.3, 0.3, 1.0 };
    float lPosition[4] = { 10, 10, 10, 1 };
    
    glEnable(GL_POINT_SMOOTH);

    glutPostRedisplay();

}

void drawTreeRight() {
    if (displayWireMesh)
    {
        glPushMatrix();
        glTranslatef(14, 5, 0);
        glRotatef(90, 1, 0, 0);
        draw_cylinder(1.5, 11, 0.4f, 0.3f, 0.2f);
        glPopMatrix();

        glPushMatrix();
        glColor3f(0, 1, 0);
        glTranslatef(14, 12, 0);
        glutWireSphere(7.0, 50, 50);

        glPopMatrix();
    }
    else
    {
        glPushMatrix();
        glTranslatef(14, 5, 0);
        glRotatef(90, 1, 0, 0);
        draw_cylinder(1.5, 11, 0.4f, 0.3f, 0.2f);
        glPopMatrix();

        glPushMatrix();
        glColor3f(0, 1, 0);
        glTranslatef(14, 12, 0);
        glutSolidSphere(7.0, 50, 50);

        glPopMatrix();
    }

}

void drawTreeLeft() {
    if (displayWireMesh)
    {
        glPushMatrix();
        glTranslatef(-14, 5, 0);
        glRotatef(90, 1, 0, 0);
        draw_cylinder(1.5, 11, 0.4f, 0.3f, 0.2f);
        glPopMatrix();

        glPushMatrix();
        glColor3f(0, 1, 0);
        glTranslatef(-14, 12, 0);
        glutWireSphere(7.0, 50, 50);

        glPopMatrix();
    }
    else
    {
        glPushMatrix();
        glTranslatef(-14, 5, 0);
        glRotatef(90, 1, 0, 0);
        draw_cylinder(1.5, 11, 0.4f, 0.3f, 0.2f);
        glPopMatrix();

        glPushMatrix();
        glColor3f(0, 1, 0);
        glTranslatef(-14, 12, 0);
        glutSolidSphere(7.0, 50, 50);

        glPopMatrix();
    }

}

void EasterEgg()
{
    glDisable(GL_TEXTURE_2D);
    std::string name;
    name = "RUN!!!!";
    drawText(name.data(), windowWidth / 2 - 30, windowHeight / 2 + 20, 1.0, 0.0, 0.0);
    name = "Zombie Mode Activated.";
    drawText(name.data(), windowWidth / 2 - 120, windowHeight / 2 + 200, 1.0, 0.0, 0.0);

    glEnable(GL_TEXTURE_2D);

    glPushMatrix();
    
    //glBindTexture(GL_TEXTURE_2D, texID[1]);
    
    glBegin(GL_QUADS);
    glVertex3f(-100.0f, -7.0f, -100.f);
    glVertex3f(-100.0f, -7.0f, 100.f);
    glVertex3f(100.0f, -7.0f, 100.f);
    glVertex3f(100.0f, -7.0f, -100.f);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, texID[0]);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    glFrontFace(GL_CW);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texID[2]);
    glPushMatrix();
    glTranslatef(moveZ, moveY, moveX);
    glPushMatrix();
    glTranslatef(10, 0, 15);
    drawZombie();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(5, 0, 12);
    drawZombie();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 0, 10);
    drawZombie();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-5, 0, 18);
    drawZombie();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-10, 0, 17);
    drawZombie();
    glPopMatrix();

    glPopMatrix();

    glPushMatrix();
    
    glTranslatef(moveZ, moveY, moveX);
    glDisable(GL_TEXTURE_2D);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texID[1]);
    drawRobot();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    
    
}


void drawSceneElements(void)
{

    glEnable(GL_TEXTURE_2D);
    glClearColor(1, 1, 1, 1);
    if (ActivateEaster)
    {
        EasterEgg();
    }
    else
    {
        if (viewAxis)
        {
            axis();
        }

        if (displayWireMesh)
        {
            glColor3f(1, 1, 1);
            for (int dir = 0; dir < 2; dir++)
            {
                for (int i = -40; i < 46; i++)
                {
                    glBegin(GL_LINE_STRIP);
                    for (int j = -40; j < 46; j++)
                        glVertex3f(dir < 1 ? i : j, -6.0f, dir < 1 ? j : i);
                    glEnd();
                }
            }
        }
        else
        {

            glEnable(GL_TEXTURE_2D);
            glPushMatrix();
            glBindTexture(GL_TEXTURE_2D, texID[0]);
            glColor3f(0, 0.45f, 0);
            glBegin(GL_QUADS);
            glTexCoord2f(0.0, 0.0);
            glVertex3f(-100.0f, -7.0f, -100.0f);
            glTexCoord2f(0.0, 1.0);
            glVertex3f(-100.0f, -7.0f, 100.0f);
            glTexCoord2f(1.0, 1.0);
            glVertex3f(100.0f, -7.0f, 100.0f);
            glTexCoord2f(1.0, 0.0);
            glVertex3f(100.0f, -7.0f, -100.0f);
            glEnd();
            glPopMatrix();
            glDisable(GL_TEXTURE_2D);
        }
        
        glFrontFace(GL_CW);
        

        glEnable(GL_TEXTURE_2D);
        glPushMatrix();
        glBindTexture(GL_TEXTURE_2D, texID[1]);
        
        glTranslatef(moveZ, moveY, moveX);

        drawRobot();
        glPopMatrix();
        glDisable(GL_TEXTURE_2D);

        drawTreeLeft();
        drawTreeRight();

        glPushMatrix();
        glTranslatef(0, 0, -20);
        drawTreeLeft();
        drawTreeRight();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0, 0, -40);
        drawTreeLeft();
        drawTreeRight();
        glPopMatrix();

    }

    glFrontFace(GL_CCW);
    
}



void drawInnerCamera()
{
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(innerCamXYZ.x, innerCamXYZ.y, innerCamXYZ.z);
    glRotatef(-innerCamTPR.x * 180.0 / M_PI, 0, 1, 0);
    glRotatef(innerCamTPR.y * 180.0 / M_PI, 1, 0, 0);
    glScalef(1, -2, 0.75);

    glColor3f(0, 0, 1);
    glutWireCube(1.0f);

    //draw the reels on top of the camera...
    for (int currentReel = 0; currentReel < 2; currentReel++)
    {
        float radius = 0.25f;
        int resolution = 32;
        Point reelCenter = Point(0, -0.25 + (currentReel == 0 ? 0 : 0.5), -0.5);
        glBegin(GL_LINES);
        Point s = reelCenter - Point(0, 0.25, 0);
        glVertex3f(s.x, s.y, s.z);
        for (int i = 0; i < resolution; i++)
        {
            float ex = -cosf(i / (float)resolution * M_PI);
            float why = sinf(i / (float)resolution * M_PI);
            Point p = Point(0, ex * radius, -why * radius * 3) + reelCenter;
            glVertex3f(p.x, p.y, p.z);  //end of this line...
            glVertex3f(p.x, p.y, p.z);  //and start of the next
        }
        Point f = reelCenter + Point(0, 0.25, 0);
        glVertex3f(f.x, f.y, f.z);
        glEnd();
    }

    //and just draw the lens shield manually because 
    //i don't want to think about shear matrices.
    //clockwise looking from behind the camera:
    float lensOff = 0.3f;
    float lensOut = 0.2f;
    Point v0 = Point(0.5, 0.5, -0.5);
    Point v1 = Point(-0.5, 0.5, -0.5);
    Point v2 = Point(-0.5, 0.5, 0.5);
    Point v3 = Point(0.5, 0.5, 0.5);

    Point l0 = v0 + Point(lensOut, 0, 0) + Point(0, lensOut, 0) + Point(0, 0, -lensOff);
    Point l1 = v1 + Point(-lensOut, 0, 0) + Point(0, lensOut, 0) + Point(0, 0, -lensOff);
    Point l2 = v2 + Point(-lensOut, 0, 0) + Point(0, lensOut, 0) + Point(0, 0, lensOff);
    Point l3 = v3 + Point(lensOut, 0, 0) + Point(0, lensOut, 0) + Point(0, 0, lensOff);

    glBegin(GL_LINE_STRIP);
    l0.glVertex();
    l1.glVertex();
    l2.glVertex();
    l3.glVertex();
    l0.glVertex();
    glEnd();

    //and connect the two
    glBegin(GL_LINES);
    v0.glVertex();  l0.glVertex();
    v1.glVertex();  l1.glVertex();
    v2.glVertex();  l2.glVertex();
    v3.glVertex();  l3.glVertex();
    glEnd();

    if (sphereOn)
    {
        //draw a point at the center of the camera
        glColor3f(1, 0, 0);
        glPointSize(10);
        glBegin(GL_POINTS);
        Point(0, 0, 0).glVertex();
        glEnd();
        glPopMatrix();

        //do the same set of transformations, but without the scale..
        glPushMatrix();
        glTranslatef(innerCamXYZ.x, innerCamXYZ.y, innerCamXYZ.z);
        glRotatef(-innerCamTPR.x * 180.0 / M_PI, 0, 1, 0);
        glRotatef(innerCamTPR.y * 180.0 / M_PI, 1, 0, 0);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glColor4f(1, 1, 1, 0.3);
        glutSolidSphere(1, 32, 32);

        glDisable(GL_DEPTH_TEST);
        glColor3f(1, 0, 0);
        glBegin(GL_LINES);
        glVertex3f(0, 0, 0);
        glVertex3f(0, -1, 0);
        glEnd();
        glColor3f(0, 0, 1);
        glBegin(GL_POINTS);
        glVertex3f(0, -1, 0);
        glEnd();
        glEnable(GL_DEPTH_TEST);

        glDisable(GL_CULL_FACE);
        glDisable(GL_BLEND);


        glPopMatrix();
    }
    else {
        glPopMatrix();
    }


    glPopAttrib();
}

void renderCallback(void)
{
    //clear the render buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float borderWidth = 3;
    //start with the code from the outer camera, which covers the whole screen!
    glViewport(0, 0, windowWidth, windowHeight);
    //glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);    glPushMatrix(); glLoadIdentity();   gluOrtho2D(0, 1, 0, 1);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    if (currentCamera == CAMERA_OUTER)
        glColor3f(1, 0, 0);
    else
        glColor3f(1, 1, 1);

    glBegin(GL_QUADS);
    glVertex2f(0, 0); glVertex2f(0, 1); glVertex2f(1, 1); glVertex2f(1, 0);
    glEnd();
    glViewport(borderWidth, borderWidth, windowWidth - borderWidth * 2, windowHeight - borderWidth * 2);
    glColor3f(0, 0, 0);
    glBegin(GL_QUADS);
    glVertex2f(0, 0); glVertex2f(0, 1); glVertex2f(1, 1); glVertex2f(1, 0);
    glEnd();

    glMatrixMode(GL_PROJECTION);    glPopMatrix();
    //glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);

    //update the modelview matrix based on the camera's position
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(outerCamXYZ.x, outerCamXYZ.y, outerCamXYZ.z,
        0, 0, 0,
        0, 1, 0);

    drawSceneElements();
    drawInnerCamera();

    glDisable(GL_DEPTH_TEST);

    //step 1: set the projection matrix using gluOrtho2D -- but save it first!
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1, 0, 1);

    //step 2: clear the modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //step 3: set the viewport matrix a little larger than needed...
    glViewport(2 * windowWidth / 3.0 - borderWidth, 2 * windowHeight / 3.0 - borderWidth,
        windowWidth / 3.0 + borderWidth, windowHeight / 3.0 + borderWidth);
    //step 3a: and fill it with a white rectangle!
    if (currentCamera == CAMERA_OUTER)
        glColor3f(1, 1, 1);
    else
        glColor3f(1, 0, 0);
    glBegin(GL_QUADS);
    glVertex2f(0, 0); glVertex2f(0, 1); glVertex2f(1, 1); glVertex2f(1, 0);
    glEnd();

    //step 4: trim the viewport window to the size we want it...
    glViewport(2 * windowWidth / 3.0, 2 * windowHeight / 3.0,
        windowWidth / 3.0, windowHeight / 3.0);
    //step 4a: and color it black! the padding we gave it before is now a border.
    glColor3f(0, 0, 0);
    glBegin(GL_QUADS);
    glVertex2f(0, 0); glVertex2f(0, 1); glVertex2f(1, 1); glVertex2f(1, 0);
    glEnd();

    //before rendering the scene in the corner, pop the old projection matrix back
    //and re-enable lighting!
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_LIGHTING);

    ///     begin drawing scene in upper corner //////////////////////////////////////////////

    glViewport(2 * windowWidth / 3.0, 2 * windowHeight / 3.0,
        windowWidth / 3.0, windowHeight / 3.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(x, 1.0f, z,
        x + innerCamDir.x + lx, 1.0f + innerCamDir.y, z + innerCamDir.z + lz,
        0.0f, 1.0f, 0.0f);

    //up vector is (0,1,0) (positive Y)

    glClear(GL_DEPTH_BUFFER_BIT);                   //ensure that the overlay is always on top!


    drawSceneElements();

    //push the back buffer to the screen
    glutSwapBuffers();
}


void normalKeys(unsigned char key, int x, int y)
{

    switch (key)
    {
    case '1':
        displayWireMesh = !displayWireMesh;
        break;
    case '2':
        viewAxis = !viewAxis;
        break;
    case 'a':
        playAnimation = !playAnimation;
        break;
    case 'i':
        currentCamera = CAMERA_INNER;
        break;
    case 'o':
        currentCamera = CAMERA_OUTER;
        break;
    case 'e':
        ActivateEaster = !ActivateEaster;


        break;
    case 27:
        exit(0);
        break;
    }


    float movementConstant = 0.3;
    if (key == 'w' || key == 'W')
    {
        if (USING_INNER)
        {
            innerCamXYZ += innerCamDir * movementConstant * (key == 'w' ? 1 : 3);
        }
        else {
            outerCamTPR.z -= 1.0f * movementConstant;

            //limit the camera radius to some reasonable values so the user can't get lost
            if (outerCamTPR.z < 2.0)
                outerCamTPR.z = 2.0;
            if (outerCamTPR.z > 10.0 * (currentCamera + 1))
                outerCamTPR.z = 10.0 * (currentCamera + 1);
            recomputeOrientation(outerCamXYZ, outerCamTPR);
        }
        glutPostRedisplay();
    }

    if (key == 's' || key == 'S')
    {
        if (USING_INNER)
        {
            innerCamXYZ -= innerCamDir * movementConstant * (key == 's' ? 1 : 3);
        }
        else {
            outerCamTPR.z += 1.0f * movementConstant;

            //limit the camera radius to some reasonable values so the user can't get lost
            if (outerCamTPR.z < 2.0)
                outerCamTPR.z = 2.0;
            if (outerCamTPR.z > 10.0 * (currentCamera + 1))
                outerCamTPR.z = 10.0 * (currentCamera + 1);
            recomputeOrientation(outerCamXYZ, outerCamTPR);
        }
        glutPostRedisplay();
    }


}


int main(int argc, char** argv)
{
    //create a double-buffered GLUT window at (50,50) with predefined windowsize
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(50, 50);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("(Karim Amr Mohamed Talaat Mohamed Hassan) (811238841)");

    printf("\n\
----------------------------------------------------------------------------\n\
Key Instructions:\n\
- '1': toggle (wire/solid) to display wireframe or solid display of the scene.   \n\
- '2': toggle on/off to display the three axes    \n\
- 'a': robot animation toggle ON/OFF  \n\
- 'i' : select the inner camera    \n\
- 'o' : select the outer camera    \n\
- four arrow keys: move the fly camera (inner camera) without changing the orientation determined by the mouse.  \n\
- ESC to quit \n\
----------------------------------------------------------------------------\n");
    //give the camera a 'pretty' starting point!
    innerCamXYZ = Point(5, 5, 5);
    innerCamTPR = Point(-M_PI / 4.0, M_PI / 4.0, 1);
    recomputeOrientation(innerCamDir, innerCamTPR);
    innerCamDir.normalize();

    outerCamTPR = Point(1.50, 2.0, 14.0);
    outerCamXYZ = Point(0, 0, 0);
    recomputeOrientation(outerCamXYZ, outerCamTPR);

    //do some basic OpenGL setup
    initScene();
    loadTextures();

    //register callback functions...
    glutSetKeyRepeat(GLUT_KEY_REPEAT_ON);
    glutKeyboardFunc(normalKeys);
    glutDisplayFunc(renderCallback);
    glutTimerFunc(0, timer, 0);
    glutReshapeFunc(resizeWindow);
    glutMouseFunc(mouseCallback);
    glutSpecialFunc(special);
    glutMotionFunc(mouseMotion);


    //and enter the GLUT loop, never to exit.
    glutMainLoop();

    return(0);
}