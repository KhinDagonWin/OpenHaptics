// cannedForceEffect.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#ifdef  _WIN64
#pragma warning (disable:4996)
#endif

#include <stdio.h>
#include <assert.h>

#if defined(WIN32)
# include <conio.h>
#else
# include "conio.h"
#endif

#include <HD/hd.h>
#include <HL/hl.h>

#include <HDU/hduError.h>



void HLCALLBACK buttonCB(HLenum event, HLuint object, HLenum thread,
    HLcache* cache, void* userdata)
{
    if (event == HL_EVENT_1BUTTONDOWN)
    {
        HLuint spring = *((HLuint*)userdata);

        HDdouble anchor[3];
        hlCacheGetDoublev(cache, HL_PROXY_POSITION, anchor);

        hlEffectd(HL_EFFECT_PROPERTY_GAIN, 0.8);
        hlEffectd(HL_EFFECT_PROPERTY_MAGNITUDE, 1.0);
        hlEffectdv(HL_EFFECT_PROPERTY_POSITION, anchor);
        hlStartEffect(HL_EFFECT_SPRING, spring);
    }
    else if (event == HL_EVENT_1BUTTONUP)
    {
        HLuint spring = *((HLuint*)userdata);

        hlStopEffect(spring);
    }
    else if (event == HL_EVENT_2BUTTONDOWN)
    {
        static const HDdouble direction[3] = { 0, 0, 1 };
        static const HDdouble duration = 100; /* ms */

        /* Trigger an impulse by commanding a force with a
           direction and magnitude for a small duration. */
        hlEffectd(HL_EFFECT_PROPERTY_DURATION, duration);
        hlEffectd(HL_EFFECT_PROPERTY_MAGNITUDE, 0.8);
        hlEffectdv(HL_EFFECT_PROPERTY_DIRECTION, direction);
        hlTriggerEffect(HL_EFFECT_CONSTANT);
    }
}



int main(int argc, char* argv[])
{
    HHD hHD;
    HHLRC hHLRC;
    HDErrorInfo error;
    HLuint friction, spring;
    HLerror frameError;

    hHD = hdInitDevice(HD_DEFAULT_DEVICE);
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to initialize haptic device");
        fprintf(stderr, "\nPress any key to quit.\n");
        getch();
        return -1;
    }
    hdMakeCurrentDevice(hHD);

    hHLRC = hlCreateContext(hHD);
    hlMakeCurrent(hHLRC);

    hlDisable(HL_USE_GL_MODELVIEW);

    spring = hlGenEffects(1);

    /* Add a callback to handle button down in the collision thread. */
    hlAddEventCallback(HL_EVENT_1BUTTONDOWN, HL_OBJECT_ANY, HL_CLIENT_THREAD,
        buttonCB, &spring);
    hlAddEventCallback(HL_EVENT_1BUTTONUP, HL_OBJECT_ANY, HL_CLIENT_THREAD,
        buttonCB, &spring);
    hlAddEventCallback(HL_EVENT_2BUTTONDOWN, HL_OBJECT_ANY, HL_CLIENT_THREAD,
        buttonCB, 0);

    /* Start an ambient friction effect. */
    friction = hlGenEffects(1);

    hlBeginFrame();
    hlEffectd(HL_EFFECT_PROPERTY_GAIN, 0.2);
    hlEffectd(HL_EFFECT_PROPERTY_MAGNITUDE, 0.5);
    hlStartEffect(HL_EFFECT_FRICTION, friction);
    hlEndFrame();

    printf("Move around to feel the ambient stick-slip friction.\n\n");
    printf("Press and hold the primary stylus button to feel the spring effect.\n\n");
    printf("Press the second stylus button to trigger an impulse.\n\n");


    while (!_kbhit())
    {
        hlBeginFrame();

        /* Poll for events.  Note that client thread event callbacks get
           dispatched from within a frame here, so we can safely start/stop
           effects from the event callback directly */
        hlCheckEvents();

        hlEndFrame();

        /* Check for any errors. */
        while (HL_ERROR(frameError = hlGetError()))
        {
            fprintf(stderr, "HL Error: %s\n", frameError.errorCode);

            if (frameError.errorCode == HL_DEVICE_ERROR)
            {
                hduPrintError(stderr, &frameError.errorInfo,
                    "Error during haptic rendering\n");
            }
        }
    }

    /* Stop the friction effect. */
    hlBeginFrame();
    hlStopEffect(friction);
    hlEndFrame();

    hlDeleteEffects(friction, 1);
    hlDeleteEffects(spring, 1);

    hlDeleteContext(hHLRC);
    hdDisableDevice(hHD);
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
