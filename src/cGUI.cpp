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
    mfile.append("Open Street Map Download file",
                   [&](const std::string &title)
                   {
                       try
                       {
                           wex::filebox fb(fm);
                           myOSMfile = fb.open();
                           myOSM.read(myOSMfile);
                           Calculate();
                       }
                       catch (std::runtime_error &e)
                       {
                           wex::msgbox m(e.what());
                       }
                   });
    mfile.append("Download Open Street Map",
                 [&](const std::string &title)
                 {
                     try
                     {
                         myOSM.download();
                         Calculate();
                     }
                     catch (std::runtime_error &e)
                     {
                         wex::msgbox m(e.what());
                     }
                 });

    mbar.append("Calculate", mfile);

    wex::menu mConfig(fm);
    mConfig.append("Open Street Map Download Bounding Box",
                   [&](const std::string &title)
                   {
                       double north, west, south, east;
                       myOSM.getBBox(north, west, south, east);
                       wex::inputbox ib;
                       ib.labelWidth(70);
                       ib.add("North Lat", std::to_string(north));
                       ib.add("West Lon", std::to_string(west));
                       ib.add("South Lat", std::to_string(south));
                       ib.add("East Lon", std::to_string(east));
                       ib.showModal();
                       myOSM.setBBox(
                           atof(ib.value("North Lat").c_str()),
                           atof(ib.value("West Lon").c_str()),
                           atof(ib.value("South Lat").c_str()),
                           atof(ib.value("East Lon").c_str()));
                   });

    mbar.append("Config", mConfig);
}

void cGUI::Calculate()
{
    myOSM.calculateBBox();
    myOSM.calculatePixel();

    myZ.generateCrimeRandom(100, 5, 95, 5, 95);
    myZ.generateRoad();

    myZ.AHC(10);

    // myZ.move2Road();
    move2Way();

    // myZ.textDisplay();

    fm.update();
}

void cGUI::move2Way()
{
    for (auto &cluster : myZ.getCluster())
    {
        int x, y;
        cluster.getLocation(x, y);
        cxy p(
            10 + 10 * x,
            10 + 10 * y);
        cluster.setRoadLocation(
            myOSM.closestOnWay(p));
    }
}
void cGUI::draw(wex::shapes &S)
{
    int scale = 10;
    int off = 10;
    S.penThick(2);

    drawOSM(S);

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
        // S.circle(off + scale * rl.x, off + scale * rl.y, 4);
        S.circle(rl.x, rl.y, 4);
    }
}

void cGUI::drawOSM(wex::shapes &S)
{
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
        // display the way name
        S.text(
            w.myName,
            {(int)prev.x, (int)prev.y});
    }
}