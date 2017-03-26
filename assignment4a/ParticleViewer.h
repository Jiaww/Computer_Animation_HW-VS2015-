#pragma once

#include "aBasicViewer.h"
#include "aParticleSystem.h"

class ParticleViewer : public ABasicViewer
{
public:
    ParticleViewer();
    virtual ~ParticleViewer();
    virtual void init(int argc, char** argv,
        int winwidth = DEFAULT_WINDOW_WIDTH,
        int winheight = DEFAULT_WINDOW_HEIGHT,
        int winstartx = DEFAULT_WINDOW_STARTX,
        int winstarty = DEFAULT_WINDOW_STARTY);
    
protected:

    virtual void initializeGui();
    virtual bool initializeOpenGL();
    virtual void draw3DView();
    virtual void onKeyboard(unsigned char key, int x, int y);
    virtual void onTimer(int value);

    static void TW_CALL onSetStartColorCb(const void *value, void *clientData);
    static void TW_CALL onGetStartColorCb(void *value, void *clientData);
    static void TW_CALL onSetEndColorCb(const void *value, void *clientData);
    static void TW_CALL onGetEndColorCb(void *value, void *clientData);
    static void TW_CALL onSetMaxParticlesCb(const void *value, void *clientData);
    static void TW_CALL onGetMaxParticlesCb(void *value, void *clientData);
    static void TW_CALL ResetCb(void* clientData);

protected:
    AParticleSystem mParticles;
    TwBar *mParticleEditBar;
};
