#include <iostream>
#include <sstream>
#include <stdlib.h> /* srand, rand */
#include <time.h>   /* time */
#include <vector>
#include <cmath>

#include "cPatrolZone.h"


int main()
{
    cPatrolZone Z;

    Z.GenerateCrimeRandom(100, 0, 100, 0, 100);

    Z.AHC(10);

    Z.textDisplay();

    return 0;
}
