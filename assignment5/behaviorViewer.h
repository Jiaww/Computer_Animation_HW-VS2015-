#pragma once

#include <vector>
#include <map>
#include "aBasicViewer.h"
#include "aBehaviorController.h"
#include "aBVHController.h"
#include "aBehaviors.h" 
#include "aJoint.h"
#include "aSkeleton.h"
#include "aActor.h"



class BehaviorViewer : public ABasicViewer
{
public:
    BehaviorViewer(int numCharacters = 5, int numObstacles = 3);
    virtual ~BehaviorViewer();
    virtual void init(int argc, char** argv,
        int winwidth = DEFAULT_WINDOW_WIDTH,
        int winheight = DEFAULT_WINDOW_HEIGHT,
        int winstartx = DEFAULT_WINDOW_STARTX,
        int winstarty = DEFAULT_WINDOW_STARTY);

    virtual bool loadModel(const std::string& filename, const std::string& motionname);
    
protected:
    virtual void initializeGui();
    virtual void draw3DView();
    virtual void onKeyboard(unsigned char key, int x, int y);
    virtual void onTimer(int value);
    virtual void onMouseMotion(int x, int y);

    virtual void reset(int numActors, int numObstacles);
    virtual void frameCamera();
    virtual void displayGrid();
	virtual void drawFloor() const;
    virtual void onStep();

	// AnTweakBar call back functions
    static void TW_CALL onSetBehaviorCb(const void *value, void *clientData);
    static void TW_CALL onGetBehaviorCb(void *value, void *clientData);
    static void TW_CALL onSetNumCharactersCb(const void *value, void *clientData);
	static void TW_CALL onSetNumObstaclesCb(const void *value, void *clientData);
    static void TW_CALL onGetNumCharactersCb(void *value, void *clientData);
	static void TW_CALL onGetNumObstaclesCb(void *value, void *clientData);
	static void TW_CALL onResetCb(void *clientData);


	virtual void setActiveBehavior(BehaviorType type);
    virtual void showObstacles(bool b) { m_ShowObstacles = b; }

protected: 
	AActor m_commonActor; 
	//ASkeleton* m_pCommonSkeleton;
   // BVHController* m_pBVHController;
	//BVHController* m_pCommonBVHController;
	BehaviorController* m_pBehaviorController;
    std::vector<AActor> m_AgentList;
    std::vector<Obstacle> m_Obstacles; 

    AJoint mTarget;
    bool m_ShowObstacles;
    int m_numCharacters;
	int m_numObstacles;
    bool m_DebugDraw;

	//enum BehaviorType { SEEK, FLEE, ARRIVAL, DEPARTURE, AVOID, WANDER, ALIGNMENT, SEPARATION, COHESION, FLOCKING, LEADER };
	BehaviorType m_BehaviorType;
	
   
	// GUI
    TwBar *m_TwBehaviorBar;
    TwType m_TwBehaviorType;

    // FBX
    double mFbxTime;
};
