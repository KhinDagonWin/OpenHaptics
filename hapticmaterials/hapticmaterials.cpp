// hapticmaterials.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <stdio.h>
#include <stdlib.h>

#if defined(WIN32)
# include <windows.h>
#endif

#if defined(WIN32) || defined(linux)
# include <GL/glut.h>
#elif defined(__APPLE__)
# include <GLUT/glut.h>
#endif

// Header files for OpenHaptics.
#include <HL/hl.h>
#include <HDU/hduError.h>
#include <HDU/hdu.h>

// id needed for haptic shape.
HLuint gMyShapeId;
HHLRC hHLRC;
HHD hHD;
#include <iostream>



void exitHandler()
{
    // Deallocate the sphere shape id we reserved in initHL.
    hlDeleteShapes(gMyShapeId, 1);

    // Free up the haptic rendering context.
    hlMakeCurrent(NULL);
    if (hHLRC != NULL)
    {
        hlDeleteContext(hHLRC);
    }

    // Free up the haptic device.
    if (hHD != HD_INVALID_HANDLE)
    {
        hdDisableDevice(hHD);
    }
}

void display(void)
{
    // Start a haptic frame.
    hlBeginFrame();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(1.0, 1.0, 1.0);

    // Start the haptic shape.
    hlBeginShape(HL_SHAPE_DEPTH_BUFFER, gMyShapeId);
    glBegin(GL_POLYGON);
    glVertex3f(0.25, 0.25, 0.0);
    glVertex3f(0.75, 0.25, 0.0);
    glVertex3f(0.75, 0.75, 0.0);
    glVertex3f(0.25, 0.75, 0.0);
    glEnd();

    // End the haptic shape.
    hlEndShape();

    // End the haptic frame.
    hlEndFrame();
}

void init(void)
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);

    // Enable depth buffering to provide depth information for OpenHaptics.
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);

    // OpenHaptics setup follows:

    // Create a haptic device instance.
    HDErrorInfo error;
    hHD = hdInitDevice(HD_DEFAULT_DEVICE);
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to initialize haptic device");
        fprintf(stderr, "Press any key to exit");
        getchar();
        exit(-1);
    }

    if (HD_SUCCESS != hdGetError().errorCode)
    {
        fprintf(stderr, "Erorr initializing haptic device.\nPress any key to exit");
        getchar();
        exit(-1);
    }

    // Create a haptic rendering context and activate it.
    hHLRC = hlCreateContext(hHD);
    hlMakeCurrent(hHLRC);

    // Reserve an id for the shape
    gMyShapeId = hlGenShapes(1);

    // Specify the boundaries for the workspace of the haptic device
    // in millimeters in the cordinates of the haptic device.
    // The haptics engine will map the view volume to this workspace
    hlWorkspace(-80, -80, -70, 80, 80, 20);

    // Specify the haptic view volume (in this case it will be
    // the same as the graphic view volume).
    hlMatrixMode(HL_TOUCHWORKSPACE);
    hlLoadIdentity();
    hlOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
}

void glutMenu(int ID)
{
    switch (ID) {
    case 0:
        exit(0);
        break;
    }
}



int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(250, 250);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Hello Haptics");
    glutCreateMenu(glutMenu);
    glutAddMenuEntry("Quit", 0);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    atexit(exitHandler);
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    std::cout << "Hello World!\n";
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
