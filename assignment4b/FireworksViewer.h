#ifndef FireworksViewer_H_
#define FireworksViewer_H_

#include "aBasicViewer.h"
#include "aFireworks.h"

#define MAX_FIREWORKS 10

class FireworksViewer : public ABasicViewer
{
public:
    FireworksViewer();
    virtual ~FireworksViewer();
    virtual void init(int argc, char** argv,
        int winwidth = DEFAULT_WINDOW_WIDTH,
        int winheight = DEFAULT_WINDOW_HEIGHT,
        int winstartx = DEFAULT_WINDOW_STARTX,
        int winstarty = DEFAULT_WINDOW_STARTY);
    
protected:

    virtual bool initializeOpenGL();
    virtual void initializeGui();
    virtual void draw3DView();
    virtual void onKeyboard(unsigned char key, int x, int y);
    virtual void onTimer(int value);


protected:

    TwBar *mFireworksEditBar;
	AFireworks m_Fireworks;
	bool mWindActive;
	bool mDragActive;
	bool mAttractorActive;
	bool mRepellerActive;
	bool mRandomActive;
	bool mExtSparkForcesActive;
	bool mExtRocketForcesActive;
	int mExtForceMode;
	vec3 mAttractor;
	vec3 mRepeller;
};

#endif