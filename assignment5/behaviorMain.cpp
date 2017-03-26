#include "BehaviorViewer.h"

int main(int argc, char** argv)
{
    BehaviorViewer viewer(1,10);
	viewer.init(argc, argv);
    viewer.loadModel("../models/BetaCharacter.fbx", "../motions/Beta/walking.bvh");
	viewer.run();
	return 0;
}

