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
    myZ.generateCrimeRandom(100, 0, 100, 0, 100);
    myZ.generateRoad();

    myZ.AHC(10);
    myZ.move2Road();

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
    S.penThick(2);

    S.color(0);
    for (auto &r : myZ.getRoad())
    {
        cxy e1 = r.end1();
        cxy e2 = r.end2();
        S.line({off + scale * e1.x, off + scale * e1.y,
                off + scale * e2.x, off + scale * e2.y});
    }

    S.color(0x0000FF);
    for (auto &c : myZ.getCrime())
    {
        int x, y;
        c.getLocation(x, y);
        S.circle(off + scale * x, off + scale * y, 2);
    }
    S.color(0xFF0000);
    for (auto &c : myZ.getCluster())
    {
        if (c.isDeleted())
            continue;
        cxy rl = c.getRoadLocation();
        S.circle(off + scale * rl.x, off + scale * rl.y, 4);
    }
}