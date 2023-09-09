#include <string>
#include <sstream>
#include <vector>
#include <inputbox.h>
#include "cPatrolZone.h"
#include "cOSM.h"
#include "cGUI.h"

cGUI::cGUI()
    : cStarterGUI(
          "Patrol Zone",
          {50, 50, 1000, 500})
{
    myOSM.set(-45.425, -100000, 75.70, 100000);

    ConstructMenu();

    fm.events().draw(
        [this](PAINTSTRUCT &ps)
        {
            wex::shapes S(ps);
            draw(S);
        });

    show();
    run();
}

void cGUI::ConstructMenu()
{
    wex::menubar mbar(fm);
    wex::menu mfile(fm);
    mfile.append("Calculate",
                 [&](const std::string &title)
                 {
                     try
                     {
                         Calculate();
                     }
                     catch (std::runtime_error &e)
                     {
                         wex::msgbox m(e.what());
                     }
                 });

    mbar.append("Calculate", mfile);

    wex::menu mConfig(fm);
    mConfig.append("Offset,scale",
                   [&](const std::string &title)
                   {
                       SetScaleOffset();
                   });
    mbar.append("Config", mConfig);
}
void cGUI::SetScaleOffset()
{
    double latOff, latScale;
    double lonOff, lonScale;
    myOSM.get(
        latOff, latScale,
        lonOff, lonScale);

    wex::inputbox ib;
    ib.add("Lat Offset", std::to_string(latOff));
    ib.add("Lat Scale", std::to_string(latScale));
    ib.add("Lon Offset", std::to_string(lonOff));
    ib.add("Lon Scale", std::to_string(lonScale));
    ib.showModal();

    myOSM.set(
        atof(ib.value("Lat Offset").c_str()),
        atof(ib.value("Lat Scale").c_str()),
        atof(ib.value("Lon Offset").c_str()),
        atof(ib.value("Lon Scale").c_str()));
}

void cGUI::Calculate()
{
    myOSM.read("../dat/nodeway.txt");

    myZ.generateCrimeRandom(100, 0, 100, 0, 100);
    myZ.generateRoad();

    myZ.AHC(10);
    myZ.move2Road();

    // myZ.textDisplay();

    fm.update();
}
void cGUI::draw(wex::shapes &S)
{
    int scale = 10;
    int off = 10;
    S.penThick(2);

    cxy test  = myOSM.latlon2pixel(45.430, -75.70);
    cxy test2 = myOSM.latlon2pixel(45.435, -75.69);
    S.color(0x0000FF);
    S.circle(test.x, test.y, 2);
    S.circle(test2.x, test2.y, 2);

    S.color(0);

    // loop over way
    for (const auto &w : myOSM.getWays())
    {
        cxy prev(-1, -1);

        // loop over nodes in the way
        for (const auto &n : w.getNodes())
        {
            // get node pixel location
            cxy px = myOSM.getNode(n).getPixel();
            if (prev.x < 0)
            {
                prev = px;
                continue;
            }

            // draw line between nodes
            S.line({(int)prev.x, (int)prev.y,
                    (int)px.x, (int)px.y});

            prev = px;
        }
        S.text(
            w.myName,
            {(int)prev.x, (int)prev.y});
    }

    // S.color(0x0000FF);
    // for (auto &c : myZ.getCrime())
    // {
    //     int x, y;
    //     c.getLocation(x, y);
    //     S.circle(off + scale * x, off + scale * y, 2);
    // }
    S.color(0xFF0000);
    for (auto &c : myZ.getCluster())
    {
        if (c.isDeleted())
            continue;
        cxy rl = c.getRoadLocation();
        S.circle(off + scale * rl.x, off + scale * rl.y, 4);
    }
}