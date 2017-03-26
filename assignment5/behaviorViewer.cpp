#include <string>
#include "BehaviorViewer.h"
#include "aBehaviors.h"
#include "GL/glew.h"
#include "GL/glut.h"
#include <algorithm>
#include <AntTweakBar.h>

#pragma warning(disable : 4018)

BehaviorViewer::BehaviorViewer(int numCharacters, int numObstacles)
{
	mFbxTime = 0.0;
    ACamera::gDfltSpeed = 75;
	
	m_numCharacters = numCharacters;
	m_numObstacles = numObstacles;
	
}


BehaviorViewer::~BehaviorViewer()
{

}

bool BehaviorViewer::loadModel(const std::string& filename, const std::string& motionname)
{
	bool result = ABasicViewer::loadModel(filename);  // loads the fbx character and skeleton
	if (result)
	{
		// export skeleton from FBX model
		ASkeleton* pFbxSkeleton = &mFBX->ExportSkeleton();

		// loads motion from bvh file for the common actor that can be used by all agent BVHControllers .
		m_commonActor.getBVHController()->load(motionname);

		// to work properly BvhSkeleton joint names and topology need to be exactly the same as FbxSkeleton
		ASkeleton* pBvhSkeleton = m_commonActor.getBVHController()->getSkeleton();

		// initialize Bvh skeleton pose to time = 0.0
		double time = 0.0;
		m_commonActor.getBVHController()->update(time);

		// set Fbx skeleton pose to match Bvh skeleton pose
		mFBX->SetPose(*pBvhSkeleton);
		mFilename = ABasicViewer::pruneName(motionname);
	}
	return result;
}


void BehaviorViewer::initializeGui()
{
	ABasicViewer::initializeGui();

	TwDefine(" 'File controls' size='200 300' position='5 185' iconified=true fontresizable=false alpha=200");

	m_TwBehaviorBar = TwNewBar("Behavior controls");
	TwDefine(" 'Behavior controls' size='200 200' position='5 185' iconified=false fontresizable=false alpha=200");
	TwEnumVal beTypeEV[] = {
		{ SEEK, "Seek" },
		{ FLEE, "Flee" },
		{ ARRIVAL, "Arrival" },
		{ DEPARTURE, "Departure" },
		{ AVOID, "Avoid" },
		{ ALIGNMENT, "Alignment" },
		{ WANDER, "Wander" },
		{ SEPARATION, "Separation" },
		{ COHESION, "Cohesion" },
		{ FLOCKING, "Flocking" },
		{ LEADER, "Leader" },
		{ CHAOSMARCHING, "ChaosMarching" },
		{ CHASING, "Chasing"},

	};

	m_TwBehaviorType = TwDefineEnum("BehaviorType", beTypeEV, 13);
	TwAddVarCB(m_TwBehaviorBar, "Behavior", m_TwBehaviorType, onSetBehaviorCb, onGetBehaviorCb, this, " ");
	TwAddVarCB(m_TwBehaviorBar, "Num Agents", TW_TYPE_INT32, onSetNumCharactersCb, onGetNumCharactersCb, this, "");
	TwAddVarCB(m_TwBehaviorBar, "Num Obstacles", TW_TYPE_INT32, onSetNumObstaclesCb, onGetNumObstaclesCb, this, "");
	TwAddVarRW(m_TwBehaviorBar, "Max speed", TW_TYPE_DOUBLE, &BehaviorController::gMaxSpeed, "");
	TwAddVarRW(m_TwBehaviorBar, "Max angular", TW_TYPE_DOUBLE, &BehaviorController::gMaxAngularSpeed, "");
	TwAddVarRW(m_TwBehaviorBar, "Neighborhod", TW_TYPE_DOUBLE, &BehaviorController::gKNeighborhood, "");
	TwAddVarRW(m_TwBehaviorBar, "Radius", TW_TYPE_DOUBLE, &BehaviorController::gAgentRadius, "");
	TwAddVarRW(m_TwBehaviorBar, "Debug", TW_TYPE_BOOLCPP, &m_DebugDraw, "");
	TwAddButton(m_TwBehaviorBar, "Reset", onResetCb, this, "");

	//TODO: Add your code here to create additional GUI Variables
	TwAddVarRW(m_TwBehaviorBar, "Mass", TW_TYPE_DOUBLE, &BehaviorController::gMass, "");
	TwAddVarRW(m_TwBehaviorBar, "Max Force", TW_TYPE_DOUBLE, &BehaviorController::gMaxForce, "");
	TwAddVarRW(m_TwBehaviorBar, "Max Torque", TW_TYPE_DOUBLE, &BehaviorController::gMaxTorque, "");
	TwAddVarRW(m_TwBehaviorBar, "KArrival", TW_TYPE_DOUBLE, &BehaviorController::KArrival, "");
	TwAddVarRW(m_TwBehaviorBar, "KDeparture", TW_TYPE_DOUBLE, &BehaviorController::KDeparture, "");
	TwAddVarRW(m_TwBehaviorBar, "KNoise", TW_TYPE_DOUBLE, &BehaviorController::KNoise, "");
	TwAddVarRW(m_TwBehaviorBar, "KWander", TW_TYPE_DOUBLE, &BehaviorController::KWander, "");
	TwAddVarRW(m_TwBehaviorBar, "KAvoid", TW_TYPE_DOUBLE, &BehaviorController::KAvoid, "");
	TwAddVarRW(m_TwBehaviorBar, "TAvoid", TW_TYPE_DOUBLE, &BehaviorController::TAvoid, "");
	TwAddVarRW(m_TwBehaviorBar, "KSeparation", TW_TYPE_DOUBLE, &BehaviorController::KSeparation, "");
	TwAddVarRW(m_TwBehaviorBar, "KAlignment", TW_TYPE_DOUBLE, &BehaviorController::KAlignment, "");
	TwAddVarRW(m_TwBehaviorBar, "KCohesion", TW_TYPE_DOUBLE, &BehaviorController::KCohesion, "");

}


void BehaviorViewer::init(int argc, char** argv, int winwidth, int winheight, int winstartx, int winstarty)
{
    ABasicViewer::init(argc, argv, winwidth, winheight, winstartx, winstarty);

	mPause = true; 
	
	m_BehaviorType = ARRIVAL;

	// create AgentList and ObstacleList.  Create Behaviors for each agent and set active behavior
	reset(m_numCharacters, m_numObstacles);
	
	showObstacles(m_BehaviorType == AVOID);
}


void BehaviorViewer::reset(int numCharacters, int numObstacles)
{
	m_numCharacters = numCharacters;
	m_numObstacles = numObstacles;

	m_AgentList.clear();
	m_Obstacles.clear();

	m_AgentList.resize(m_numCharacters);
	m_Obstacles.resize(m_numObstacles);

	// set up behavior controllers
	for (unsigned int i = 0; i < m_numCharacters; i++)
	{
		ASkeleton* pCommmonSkeleton = m_commonActor.getSkeleton();
		m_AgentList[i].setSkeleton(pCommmonSkeleton);
		m_AgentList[i].getBehaviorController()->setTarget(mTarget);
		m_AgentList[i].getBehaviorController()->createBehaviors(m_AgentList, m_Obstacles);
		m_AgentList[i].getBehaviorController()->setActiveBehaviorType(m_BehaviorType);
		if (i == 0)
			m_AgentList[i].getBehaviorController()->setLeader(true);

	}

	// initialize behavior controller states and parameters
	for (unsigned int i = 0; i < m_AgentList.size(); i++)
	{
		m_AgentList[i].getBehaviorController()->reset();
	}

	// create obstacles
	for (int i = 0; i < m_numObstacles; i++)
	{
		m_Obstacles[i].m_Radius = 50 + 200 * (((double)rand()) / RAND_MAX);
		double random = rand();
		double x = ((double)rand()) / RAND_MAX;
		double z = ((double)rand()) / RAND_MAX;

		m_Obstacles[i].m_Center.setLocalTranslation(vec3((x - 0.5)*2000.0, 0.0, (z - 0.5)*2000.0));
	}

}


void BehaviorViewer::onTimer(int value)
{
    if (!mPause) onStep();
    ABasicViewer::onTimer(value);
}

void BehaviorViewer::setActiveBehavior(BehaviorType type)
{
	m_BehaviorType = type;
	for (unsigned int i = 0; i < m_AgentList.size(); i++)
		m_AgentList[i].getBehaviorController()->setActiveBehaviorType(type);

	showObstacles(m_BehaviorType == AVOID);
}

void BehaviorViewer::onStep()
{
	static double lastTime = 0.0;
	double simTimeStep = 0.010;     // desired simulation timestep =  0.010 sec (100Hz)
	
	double dt = mClock->totalElapsedTime(); // needs to be called before base in case clock is reset
	double currentTime = mFbxTime + dt* ABasicViewer::mTimeScale;
    mFbxTime = currentTime;

	//Update Simulation
	double deltaT = currentTime - lastTime;
	//std::cout << "deltaT " << deltaT << " " << std::endl; 
	if (deltaT >= simTimeStep)
	{
	
		// SENSE PHASE - all agents sense the state of the world in the sense phase
		for (unsigned int i = 0; i < m_AgentList.size(); i++)
		{
			BehaviorController* pBehaviorController = m_AgentList[i].getBehaviorController();
			m_AgentList[i].getBehaviorController()->setTarget(mTarget);  // sets target of ith agent
			m_AgentList[i].getBehaviorController()->sense(deltaT);
		}

		// CONTROL PHASE - given the state of the world all agents determine what to do in control phase
		for (unsigned int i = 0; i < m_AgentList.size(); i++)
		{
			m_AgentList[i].getBehaviorController()->control(deltaT);
		}

		// ACT PHASE - control is applied and agent state is updated in act phase
		for (unsigned int i = 0; i < m_AgentList.size(); i++)
		{
			m_AgentList[i].getBehaviorController()->act(deltaT);
		}

		lastTime = currentTime;

	}

}

void BehaviorViewer::onKeyboard(unsigned char key, int x, int y)
{
    ABasicViewer::onKeyboard(key, x, y);
}

void BehaviorViewer::onMouseMotion(int x, int y)
{
	if (mModifierState == GLUT_ACTIVE_CTRL) // move target
	{
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		if (viewport[3] == 0){
			viewport[3] = 1;
		}

		vec3 target(0, 0, 0);
		vec3 current(0, 0, 0);
		mCamera.screenToWorld(mLastX, viewport[3] - mLastY, current);
		mCamera.screenToWorld(x, viewport[3] - y, target);

		vec3 origin = mCamera.getPosition();
		vec3 dir = target - origin;

		// Intersect with XZ plane when?
		float t = -origin[1] / dir[1];
		vec3 intersection = origin + t*dir;

		//NILOG(NIMESSAGE_GENERAL_0, "%f %f %f\n", intersection[0], intersection[1], intersection[2]);
		mTarget.setLocalTranslation(intersection);

	}
	else
	{
		ABasicViewer::onMouseMotion(x, y);
	}
}


void BehaviorViewer::frameCamera()
{
    mCamera.frameVolume(vec3(0, 1000, 0), vec3(1000, 1010, 1000));
    mCamera.gDfltLook = vec3(0, 0, 0);
    mCamera.reset();
}

void BehaviorViewer::draw3DView()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
// new
	//glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE); // Draw the front face only, except for the texts and lights.
	//glEnable(GL_LIGHTING);
// end new

	// Set the view to the current camera settings.
	mCamera.draw();

	GLfloat pos[4];
	pos[0] = mCamera.getPosition()[0];
	pos[1] = mCamera.getPosition()[1] + 1.0;
	pos[2] = mCamera.getPosition()[2];
	pos[3] = 0.0;

	// Set the positin of light to be above camera.
	glLightfv(GL_LIGHT0, GL_POSITION, pos);

	// Draw target
	vec3 tpos = mTarget.getLocalTranslation();

	glPushMatrix();
	glColor3f(1, 0, 0);
	glTranslatef(tpos[0], tpos[1], tpos[2]);
	glutSolidSphere(15.0, 10, 10);
	glPopMatrix();

	// Draw Agents
	for (int i = 0; i < m_AgentList.size(); i++)
	{
			
		if (m_DebugDraw)
		{
			m_AgentList[i].getBehaviorController()->display();
			
		}
		else
		{
			//new
			//glDisable(GL_TEXTURE_2D);
			// end new

			// update body pose to current time
			BVHController* pBVHController = m_commonActor.getBVHController();
			pBVHController->update(mFbxTime);

			// override root position and orientation
			ASkeleton* pSkeleton = pBVHController->getSkeleton();
			vec3 rootPos = pSkeleton->getRootNode()->getLocalTranslation();
			rootPos[0] = rootPos[2] = 0;  // set x and z components of root position to zero
			pSkeleton->getRootNode()->setLocalTranslation(rootPos);
			pSkeleton->getRootNode()->setLocalRotation(identity3D);
			//pSkeleton->getRootNode()->setGlobalTranslation(rootPos);
			//pSkeleton->getRootNode()->setGlobalRotation(identity3D);
			mFBX->SetPose(*pSkeleton);

			float RmatGL[16];
			vec3 pos0 = m_AgentList[i].getBehaviorController()->getPosition();  // position of agent in world
			mat3 Rmat = m_AgentList[i].getBehaviorController()->getGuide().getLocalRotation();  // body to world rotation matrix
			Rmat.WriteToGLMatrix(RmatGL);

			glPushMatrix();
			glTranslatef(pos0[0], pos0[1], pos0[2]);
			glMultMatrixf(RmatGL);
			mFBX->Draw(mFbxTime, 1);
			glPopMatrix();
		}
	}

	// Draw Obstacles
	glEnable(GL_LIGHTING);
	if (m_ShowObstacles)
	{
		glColor4f(0.6, 0.8, 0.4, 1.0);
		for (int i = 0; i < m_Obstacles.size(); i++)
		{
			vec3 opos = m_Obstacles[i].m_Center.getLocalTranslation();
			double radius = m_Obstacles[i].m_Radius;

			glPushMatrix();
			glTranslatef(opos[0], opos[1], opos[2]);
			glutSolidSphere(radius, 10, 10);
			glPopMatrix();
		}
	}
	glDisable(GL_LIGHTING);
	//displayGrid();
	drawFloor();

}

void BehaviorViewer::displayGrid()
{
    // Draw a grid 500*500
    if (ACamera::gDfltUp[2] == 1) // Z-UP =>rotate
    {
        glPushMatrix();
        glRotatef(90, 1, 0, 0);
    }
    glColor3f(0.8f, 0.8f, 0.8f);
    glLineWidth(1.0);
    const int hw = 1500;
    const int step = 250;
    const int bigstep = 500;
    int i;

    // Draw Grid
    for (i = -hw; i <= hw; i += step) {

        if (i % bigstep == 0) {
            glLineWidth(2.0);
        }
        else {
            glLineWidth(1.0);
        }
        glColor4f(0.6f, 0.6f, 0.6f, 1.0f);
        if (i == 0) glColor3f(0.3f, 0.3f, 0.7f);
        glBegin(GL_LINES);
        glVertex3i(i, 0, -hw);
        glVertex3i(i, 0, hw);
        glEnd();
        if (i == 0) glColor3f(0.7f, 0.3f, 0.3f);
        glBegin(GL_LINES);
        glVertex3i(-hw, 0, i);
        glVertex3i(hw, 0, i);
        glEnd();

    }
    if (ACamera::gDfltUp[2] == 1) // Z-UP =>rotate
    {
        glPopMatrix();
    }

}

void BehaviorViewer::drawFloor() const
{
	const float floorSize = 2000.0f;
	const float gridSize = 200.0f;
	const float lightGrid[] = { 0.6f, 0.6f, 0.6f, 0.8f };
	const float darkGrid[] = { 1.0f, 1.0f, 1.0f, 0.8f };
	const float specularGrid[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	const float black[] = { 0.0f, 0.0f, 0.0f, 1.0f };

	bool row, col;
	row = col = false;
	for (int i = -floorSize - 5 * gridSize; i <= floorSize + 5 * gridSize; i += gridSize){
		for (int j = -floorSize - 5 * gridSize; j <= floorSize + 5 * gridSize; j += gridSize){
			if (i < -floorSize || i > floorSize || j < -floorSize || j > floorSize)
			{
				glColor4fv(black);
			}
			else if (col)
			{
				glColor4fv(lightGrid);
			}
			else
			{
				glColor4fv(darkGrid);
			}

			glBegin(GL_QUADS);
			glVertex3f(float(i) - gridSize / 2.0, 0, float(j) - gridSize / 2.0);
			glVertex3f(float(i) - gridSize / 2.0, 0, float(j) + gridSize / 2.0);
			glVertex3f(float(i) + gridSize / 2.0, 0, float(j) + gridSize / 2.0);
			glVertex3f(float(i) + gridSize / 2.0, 0, float(j) - gridSize / 2.0);
			glEnd();
			col = !col;
		}
		row = !row;
		col = row;
	}
}


void TW_CALL BehaviorViewer::onSetBehaviorCb(const void *value, void *clientData)
{
    BehaviorViewer* viewer = ((BehaviorViewer*)clientData);
    BehaviorType v = *(const BehaviorType *)value;  // for instance
    viewer->setActiveBehavior(v);
}

void TW_CALL BehaviorViewer::onGetBehaviorCb(void *value, void *clientData)
{
    BehaviorViewer* viewer = ((BehaviorViewer*)clientData);
    *static_cast<BehaviorType *>(value) = viewer->m_BehaviorType;
}

void TW_CALL BehaviorViewer::onSetNumCharactersCb(const void *value, void *clientData)
{
	BehaviorViewer* viewer = ((BehaviorViewer*)clientData);
	int v = *(const int *)value;  // for instance
	viewer->reset(v, viewer->m_numObstacles);
}

void TW_CALL BehaviorViewer::onSetNumObstaclesCb(const void *value, void *clientData)
{
	BehaviorViewer* viewer = ((BehaviorViewer*)clientData);
	int v = *(const int *)value;  // for instance
	viewer->reset(viewer->m_numCharacters, v);
}


void TW_CALL BehaviorViewer::onGetNumCharactersCb(void *value, void *clientData)
{
    BehaviorViewer* viewer = ((BehaviorViewer*)clientData);
    *static_cast<int *>(value) = viewer->m_numCharacters;
}

void TW_CALL BehaviorViewer::onGetNumObstaclesCb(void *value, void *clientData)
{
	BehaviorViewer* viewer = ((BehaviorViewer*)clientData);
	*static_cast<int *>(value) = viewer->m_numObstacles;
}


void TW_CALL BehaviorViewer::onResetCb(void *clientData)
{
    BehaviorViewer* viewer = ((BehaviorViewer*)clientData);
	viewer->reset(viewer->m_numCharacters, viewer->m_numObstacles);
}

//TODO: Add your code here to create the corresponding callback functions for each new GUI Variable added