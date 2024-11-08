/*
* ---------------------------
* PATHFINDING FOR CENTURION
* [2019] - [2020] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#include <engine.h>


int main(int numArgs, char* args[])
{
    Engine engine = Engine();
    if (engine.Initialize(args[0]))
    {
        return engine.Launch();
    }
    return 0;
}

