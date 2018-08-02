#include "NPC.h"


void CNPC::Initialize()
{
	IsAwake  = false;
	IsAlive  = true;
	TickTime = 0;
	Enemy    = -1;

}

void CNPC::Close()
{
}
