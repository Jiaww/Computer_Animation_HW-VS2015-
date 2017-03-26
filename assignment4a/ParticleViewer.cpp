#include <string>
#include "ParticleViewer.h"
#include "GL/glew.h"
#include "GL/glut.h"
#include <algorithm>
#include <AntTweakBar.h>

#pragma warning(disable : 4018)

ParticleViewer::ParticleViewer() : mParticles()
{
}

ParticleViewer::~ParticleViewer()
{

}

bool ParticleViewer::initializeOpenGL()
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
    float Position[] = { 1.0f, 1.0f, 4.0f, 1.0f };  glLightfv(GL_LIGHT0, GL_POSITION, Position);

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

void ParticleViewer::init(int argc, char** argv, int winwidth, int winheight, int winstartx, int winstarty)
{
    ABasicViewer::init(argc, argv, winwidth, winheight, winstartx, winstarty);
    frameCamera();
    mParticles.reset();
}

void ParticleViewer::onTimer(int value)
{
    double dt = mClock->totalElapsedTime(); // needs to be called before base when clock is reset
    ABasicViewer::onTimer(value);
	float deltaT = 0.033; // dt*mTimeScale;
    if (!mPause)
    {

        mParticles.update(deltaT);
    }
}

void ParticleViewer::onKeyboard(unsigned char key, int x, int y)
{
    ABasicViewer::onKeyboard(key, x, y);
}

void ParticleViewer::initializeGui()
{
    ABasicViewer::initializeGui();

    TwDefine(" 'File controls' size='200 300' position='5 185' iconified=true fontresizable=false alpha=200");

    mParticleEditBar = TwNewBar("Edit System");
    TwDefine(" 'Edit System' size='200 400' position='5 185' iconified=false fontresizable=false alpha=200");
    TwAddButton(mParticleEditBar, "Reset", ResetCb, this, " ");
    TwAddVarRW(mParticleEditBar, "Infinite", TW_TYPE_BOOLCPP, &mParticles.mInfinite, " ");
    TwAddVarCB(mParticleEditBar, "Max particles", TW_TYPE_INT32, onSetMaxParticlesCb, onGetMaxParticlesCb, this, " ");
    TwAddVarRW(mParticleEditBar, "Lifetim", TW_TYPE_DOUBLE, &mParticles.mLifeSpan, " ");
   
    TwAddVarRW(mParticleEditBar, "GravityX", TW_TYPE_DOUBLE, &(mParticles.mGravity[0]), " group='Gravity' label='X'");
    TwAddVarRW(mParticleEditBar, "GravityY", TW_TYPE_DOUBLE, &(mParticles.mGravity[1]), " group='Gravity' label='Y'");
    TwAddVarRW(mParticleEditBar, "GravityZ", TW_TYPE_DOUBLE, &(mParticles.mGravity[2]), " group='Gravity' label='Z'");

    TwAddVarRW(mParticleEditBar, "PosX", TW_TYPE_DOUBLE, &(mParticles.mStartPos[0]), " group='Position' label='X'");
    TwAddVarRW(mParticleEditBar, "PosY", TW_TYPE_DOUBLE, &(mParticles.mStartPos[1]), " group='Position' label='Y'");
    TwAddVarRW(mParticleEditBar, "PosZ", TW_TYPE_DOUBLE, &(mParticles.mStartPos[2]), " group='Position' label='Z'");

    TwAddVarRW(mParticleEditBar, "VelX", TW_TYPE_DOUBLE, &(mParticles.mStartVel[0]), " group='Velocity' label='X'");
    TwAddVarRW(mParticleEditBar, "VelY", TW_TYPE_DOUBLE, &(mParticles.mStartVel[1]), " group='Velocity' label='Y'");
    TwAddVarRW(mParticleEditBar, "VelZ", TW_TYPE_DOUBLE, &(mParticles.mStartVel[2]), " group='Velocity' label='Z'");

    TwAddVarCB(mParticleEditBar, "StartC", TW_TYPE_COLOR3F, onSetStartColorCb, onGetStartColorCb, this, " group='Colors' label='Start'");
    TwAddVarCB(mParticleEditBar, "EndC", TW_TYPE_COLOR3F, onSetEndColorCb, onGetEndColorCb, this, " group='Colors' label='End'");
    TwAddVarRW(mParticleEditBar, "AlphaC", TW_TYPE_DOUBLE, &(mParticles.mStartAlpha), " group='Colors' label='Start alpha'");

    TwAddVarRW(mParticleEditBar, "StartS", TW_TYPE_DOUBLE, &(mParticles.mStartScale), " group='Size' label='Start'");
    TwAddVarRW(mParticleEditBar, "EndS", TW_TYPE_DOUBLE, &(mParticles.mEndScale), " group='Size' label='End'");

}

void ParticleViewer::draw3DView()
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
    pos[1] = mCamera.getPosition()[1]+1.0;
    pos[2] = mCamera.getPosition()[2];
    pos[3] = 0.0;
    glLightfv(GL_LIGHT0, GL_POSITION, pos);

    mParticles.drawOpenGL();

    glDisable(GL_LIGHTING);
    displayGrid();
}


void TW_CALL ParticleViewer::onSetStartColorCb(const void *value, void *clientData)
{
    ParticleViewer* viewer = ((ParticleViewer*)clientData);
    const float* v = (const float *)value;  // for instance
    viewer->mParticles.mStartColor = vec3(v[0], v[1], v[2]);
}

void TW_CALL ParticleViewer::onGetStartColorCb(void *value, void *clientData)
{
    ParticleViewer* viewer = ((ParticleViewer*)clientData);
    vec3 windDir = viewer->mParticles.mStartColor;
    float* dir = static_cast<float *>(value);
    dir[0] = windDir[0];
    dir[1] = windDir[1];
    dir[2] = windDir[2];
}

void TW_CALL ParticleViewer::onSetEndColorCb(const void *value, void *clientData)
{
    ParticleViewer* viewer = ((ParticleViewer*)clientData);
    const float* v = (const float *)value;  // for instance
    viewer->mParticles.mEndColor = vec3(v[0], v[1], v[2]);
}

void TW_CALL ParticleViewer::onGetEndColorCb(void *value, void *clientData)
{
    ParticleViewer* viewer = ((ParticleViewer*)clientData);
    vec3 windDir = viewer->mParticles.mEndColor;
    float* dir = static_cast<float *>(value);
    dir[0] = windDir[0];
    dir[1] = windDir[1];
    dir[2] = windDir[2];
}

void TW_CALL ParticleViewer::onSetMaxParticlesCb(const void *value, void *clientData)
{
    ParticleViewer* viewer = ((ParticleViewer*)clientData);
    int v = *(const int *)value;  // for instance
    viewer->mParticles.mMaxParticles = v;
    viewer->mParticles.reset();
}

void TW_CALL ParticleViewer::onGetMaxParticlesCb(void *value, void *clientData)
{
    ParticleViewer* viewer = ((ParticleViewer*)clientData);
    *static_cast<int *>(value) = viewer->mParticles.mMaxParticles;
}

void TW_CALL ParticleViewer::ResetCb(void* clientData)
{
    ParticleViewer* viewer = ((ParticleViewer*)clientData);
    viewer->mParticles.reset();
}