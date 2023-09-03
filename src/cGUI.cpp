#include <string>
#include <sstream>
#include <vector>
#include "cPatrolZone.h"
#include "cGUI.h"

cGUI::cGUI()
    : cStarterGUI(
          "Patrol Zone",
          {50, 50, 1000, 500})
{
    myZ.GenerateCrimeRandom(100, 0, 100, 0, 100);

    myZ.AHC(10);

    myZ.textDisplay();

    fm.events().draw(
        [this](PAINTSTRUCT &ps)
        {
            wex::shapes S(ps);
            draw(S);
        });

    show();
    run();
}
void cGUI::draw(wex::shapes &S)
{
    int scale = 10;
    int off = 10;
    S.penThick(5);

    S.color(0x0000FF);
    for( auto& c : myZ.getCrime() )
    {
        int x,y;
        c.getLocation(x,y);
        S.circle(off + scale * x, off+scale*y,2);
    }
    S.color(0xFF0000);
    for( auto& c : myZ.getCluster() )
    {
        if( c.isDeleted() )
            continue;
        int x,y;
        c.getLocation(x,y);
        S.circle(off + scale * x, off+scale*y,4);
    }
}