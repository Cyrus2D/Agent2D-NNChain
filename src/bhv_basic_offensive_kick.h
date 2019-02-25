// -*-c++-*-

/*
 *Copyright:

 Copyright (C) Hidehisa AKIYAMA

 This code is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3, or (at your option)
 any later version.

 This code is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this code; see the file COPYING.  If not, write to
 the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

 *EndCopyright:
 */

/////////////////////////////////////////////////////////////////////

#ifndef TOKYOTEC_BHV_BASIC_OFFENSIVE_KICK_H
#define TOKYOTEC_BHV_BASIC_OFFENSIVE_KICK_H

#include <rcsc/player/soccer_action.h>
#include <iostream>


class Bhv_BasicOffensiveKick
        : public rcsc::SoccerBehavior {
private:

public:

    bool execute(rcsc::PlayerAgent *agent);
};

class OffenseConfig {
public:
    OffenseConfig(std::string _team_name) {
        dir = "formations-dt";
        if (_team_name.find("HF") != std::string::npos)
            dir = "formations-dt-helios";
        if(_team_name.find("NN") != std::string::npos)
            dnn = true;
        if(_team_name.find("VEL") != std::string::npos)
            vel = true;

    }

    static OffenseConfig *ins;

    static OffenseConfig *i(std::string _teamName = "") {
        if (ins == NULL)
            ins = new OffenseConfig(_teamName);
        return ins;
    }

    bool dnn = false;
    bool vel = false;
    std::string dir;
};

#endif
