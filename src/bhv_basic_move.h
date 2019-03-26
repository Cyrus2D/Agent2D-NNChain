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

#ifndef BHV_BASIC_MOVE_H
#define BHV_BASIC_MOVE_H

#include <rcsc/geom/vector_2d.h>
#include <rcsc/player/soccer_action.h>
#include <rcsc/player/world_model.h>

#include "dnn/DNN2d.h"


class Bhv_BasicMove
        : public rcsc::SoccerBehavior {
public:
    Bhv_BasicMove() {}

    bool execute(rcsc::PlayerAgent *agent);

private:
    double getDashPower(const rcsc::PlayerAgent *agent);
};


class UnMark {
public:
    rcsc::Vector2D home_pos;
    rcsc::Vector2D target;

    UnMark(const rcsc::WorldModel &wm, rcsc::Vector2D _home_pos);

    void find_best_target(const rcsc::WorldModel &wm);

private:
    void set_poses(rcsc::Vector2D *tmpos, rcsc::Vector2D *opppos, const rcsc::WorldModel &wm);
};

#endif
