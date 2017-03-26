#include <string>
#include "GL/glew.h"
#include "GL/glut.h"
#include <algorithm>
#include <AntTweakBar.h>

#include "FireworksViewer.h"

#pragma warning(disable : 4018)

FireworksViewer::FireworksViewer()
{

}

FireworksViewer::~FireworksViewer()
{

}

bool FireworksViewer::initializeOpenGL()
{
    // Initialize GLEW.
    GLenum lError = glewInit();
    if (lError != GLEW_OK)
    {
        std::cout << "GLEW Error: %s\n" << glewGetErrorString(lError);
        return false;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glClearColor(0.0, 0.0, 0.0, 1.0);

    // OpenGL 1.5 at least.
    if (!GLEW_VERSION_1_5)
    {
        std::cout << "The OpenGL version should be at least 1.5 to display shaded scene!\n";
        return false;
    }

    glEnable(GL_BLEND);
    glEnable(GL_ALPHA_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glShadeModel(GL_SMOOTH);

    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
    glDisable(GL_CULL_FACE);

    // Setup lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    float AmbientColor[] = { 0.01f, 0.01f, 0.01f, 1.0f };   glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientColor);
    float DiffuseColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };   glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseColor);
    float SpecularColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };   glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularColor);
    float Position[] = { 0.0f, 0.0f, 100.0f, 1.0f };  glLightfv(GL_LIGHT0, GL_POSITION, Position);

    glClearStencil(0); //clear the stencil buffer
    glClearDepth(1.0f);

    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_MULTISAMPLE);
    return true;
}

void FireworksViewer::init(int argc, char** argv, int winwidth, int winheight, int winstartx, int winstarty)
{
    ABasicViewer::init(argc, argv, winwidth, winheight, winstartx, winstarty);
    frameCamera();
	
	mExtForceMode = 0;
	mWindActive = 0;
	mAttractorActive = 0;
	mRepellerActive = 0;
	mRandomActive = 0;
	mDragActive = 0;
	mExtSparkForcesActive = 0;
	mExtRocketForcesActive = 0;

}

void FireworksViewer::onTimer(int value)
{
    double dt = mClock->totalElapsedTime(); // needs to be called before base when clock is reset
    ABasicViewer::onTimer(value);

	int extForceMode = 0;

	if (mExtRocketForcesActive)
		extForceMode = extForceMode | EXT_ROCKETFORCES_ACTIVE;
	if (mExtSparkForcesActive)
		extForceMode = extForceMode | EXT_SPARKFORCES_ACTIVE;
	if (mWindActive)
		extForceMode = extForceMode | WIND_ACTIVE;
	if (mAttractorActive)
		extForceMode = extForceMode | ATTRACTOR_ACTIVE;
	if (mRepellerActive)
		extForceMode = extForceMode | REPELLER_ACTIVE; 
	if (mDragActive)
		extForceMode = extForceMode | DRAG_ACTIVE; 
	if (mRandomActive)
		extForceMode = extForceMode | RANDOM_ACTIVE; 
	
	float deltaT = 0.033;

    if (!mPause)
    {
		double currentTime = mCurrentTime + dt * mTimeScale;
		mCurrentTime = currentTime;
		if (m_Fireworks.getNumParticles() > 0)
		{
			m_Fireworks.update(deltaT, extForceMode);
		}

    }
}

void FireworksViewer::onKeyboard(unsigned char key, int x, int y)
{
    ABasicViewer::onKeyboard(key, x, y);
    if (key == ' ')
    {
		float val = rand();
		float maxVal = RAND_MAX;

		float color[3];
		int rgb = rand() % 3;

		switch (rgb) 
		{
			case 0:
				color[0] = 1.0; // val / maxVal;
				color[1] = 0.0;
				color[2] = 0.0;
				break;
			case 1:
				color[0] = 0.0;
				color[1] = 1.0; //val / maxVal;
				color[2] = 0.0;
				break;
			case 2:
				color[0] = 0.0;
				color[1] = 0.0;
				color[2] = 1.0; // val / maxVal;
				break;
		}

		int width = this->mWindowWidth;
		float r = (float) (rand() % 400); // returns value between 0 to 399
		float posx = r - 200.0;  // posx ranges betwen -200 and 199
	
		m_Fireworks.fireRocket(posx, color);
    }
}

void FireworksViewer::initializeGui()
{
    ABasicViewer::initializeGui();
    TwDefine(" 'File controls' size='200 300' position='5 185' iconified=true fontresizable=false alpha=200");

	mFireworksEditBar = TwNewBar("Fireworks Controls");
	TwDefine(" 'Fireworks Controls' size='200 280' position='5 185' valueswidth=45 iconified=false fontresizable=false alpha=200");
	TwAddVarRW(mFireworksEditBar, "Ext. Rocket Forces", TW_TYPE_BOOLCPP, &mExtRocketForcesActive, " ");
	TwAddVarRW(mFireworksEditBar, "Ext. Spark Forces", TW_TYPE_BOOLCPP, &mExtSparkForcesActive, " ");

	TwAddVarRW(mFireworksEditBar, "Active Wind", TW_TYPE_BOOLCPP, &mWindActive, " group='Wind Force' ");
	TwAddVarRW(mFireworksEditBar, "Wind Force X", TW_TYPE_DOUBLE, &(m_Fireworks.m_windForce[0]), " group='Wind Force' label='X'");
	TwAddVarRW(mFireworksEditBar, "Wind Force Y", TW_TYPE_DOUBLE, &(m_Fireworks.m_windForce[1]), " group='Wind Force' label='Y'");
	
	TwAddVarRW(mFireworksEditBar, "Active Attract", TW_TYPE_BOOLCPP, &mAttractorActive, " group='Attractor Force' ");
	TwAddVarRW(mFireworksEditBar, "Attractor Force X", TW_TYPE_DOUBLE, &(m_Fireworks.m_attractorPos[0]), " group='Attractor Force' label='X'");
	TwAddVarRW(mFireworksEditBar, "Attractor Force Y", TW_TYPE_DOUBLE, &(m_Fireworks.m_attractorPos[1]), " group='Attractor Force' label='Y'");

	TwAddVarRW(mFireworksEditBar, "Active Repel", TW_TYPE_BOOLCPP, &mRepellerActive, " group='Repeller Force' ");
	TwAddVarRW(mFireworksEditBar, "Repeller Force X", TW_TYPE_DOUBLE, &(m_Fireworks.m_repellerPos[0]), " group='Repeller Force' label='X'");
	TwAddVarRW(mFireworksEditBar, "Repeller Force Y", TW_TYPE_DOUBLE, &(m_Fireworks.m_repellerPos[1]), " group='Repeller Force' label='Y'");

	TwAddVarRW(mFireworksEditBar, "Active Drag", TW_TYPE_BOOLCPP, &mDragActive, "  ");
	TwAddVarRW(mFireworksEditBar, "Random Force", TW_TYPE_BOOLCPP, &mRandomActive, " ");


}


void FireworksViewer::draw3DView()
{
    glViewport(0, 0, (GLsizei)mWindowWidth, (GLsizei)mWindowHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//*** SHL remove 
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

	glOrtho(-400, 400, -1, 800, -1000, 1000);
    gluLookAt(0,0,100,0,0,0,0,1,0);

	/// new
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE); // Draw the front face only, except for the texts and lights.
	glEnable(GL_LIGHTING);

	GLfloat pos[4];
	pos[0] = 0.0;  
	pos[1] = 500.0;
	pos[2] = 200.0;
	pos[3] = 1.0;
	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glDisable(GL_LIGHTING);


	// end remove
//*/

/*** SHL add

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE); // Draw the front face only, except for the texts and lights.
	glEnable(GL_LIGHTING);

	// Set the view to the current camera settings.
	mCamera.draw();

	GLfloat pos[4];
	pos[0] = mCamera.getPosition()[0];
	pos[1] = mCamera.getPosition()[1];
	pos[2] = mCamera.getPosition()[2];
	pos[3] = 1.0;
	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glDisable(GL_LIGHTING);
	// end add
*/
	m_Fireworks.display();
}

/*


void FireworksViewer::draw3DView()
{
	glViewport(0, 0, (GLsizei)mWindowWidth, (GLsizei)mWindowHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE); // Draw the front face only, except for the texts and lights.
	glEnable(GL_LIGHTING);

	// Set the view to the current camera settings.
	mCamera.draw();

	GLfloat pos[4];
	pos[0] = mCamera.getPosition()[0];
	pos[1] = mCamera.getPosition()[1] + 1.0;
	pos[2] = mCamera.getPosition()[2];
	pos[3] = 0.0;
	glLightfv(GL_LIGHT0, GL_POSITION, pos);

	mParticles.drawOpenGL();

	glDisable(GL_LIGHTING);
	displayGrid();
}

*/
