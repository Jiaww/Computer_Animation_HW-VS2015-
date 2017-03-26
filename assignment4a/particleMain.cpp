#include "ParticleViewer.h"

int main(int argc, char** argv)
{
    ParticleViewer viewer;
	viewer.init(argc, argv);
	viewer.run();
	return 0;
}

