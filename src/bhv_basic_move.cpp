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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "bhv_basic_move.h"

#include "strategy.h"

#include "bhv_basic_tackle.h"

#include <rcsc/action/basic_actions.h>
#include <rcsc/action/body_go_to_point.h>
#include <rcsc/action/body_intercept.h>
#include <rcsc/action/neck_turn_to_ball_or_scan.h>
#include <rcsc/action/neck_turn_to_low_conf_teammate.h>

#include <rcsc/player/player_agent.h>
#include <rcsc/player/debug_client.h>
#include <rcsc/player/intercept_table.h>

#include <rcsc/common/logger.h>
#include <rcsc/common/server_param.h>

#include "neck_offensive_intercept_neck.h"
#include "bhv_basic_offensive_kick.h"

using namespace rcsc;

/*-------------------------------------------------------------------*/
/*!

 */
UnMark::UnMark(const WorldModel &wm, Vector2D _home_pos) {
    home_pos = _home_pos;
    target = home_pos;
    find_best_target(wm);
}

void UnMark::find_best_target(const WorldModel &wm) {
    cout << "####CYCLE: " << wm.time().cycle() << endl;
    Vector2D tmpos[11], opppos[11], ballpos, ballvel, selfpos;
    int kicker_unum = wm.getTeammateNearestToBall(10)->unum(),
            self_unum = wm.self().unum();
    set_poses(tmpos, opppos, wm);
    ballpos = wm.ball().pos();
    ballvel = wm.ball().vel();
    selfpos = wm.self().pos();

    DNN2d::i("weights.dnn");

    bool finded_pos = false;
    for (int depth = 0; depth < 2; depth++) {
        for (double dist = 2; dist <= 10.0; dist += 2.0) {
            for (double angle = 0; angle < 360.0; angle += 30) {
                Vector2D new_pos = selfpos + Vector2D::polar2vector(dist, angle);
                tmpos[self_unum - 1] = new_pos;
                DNN2d::i()->Calculate(DNN2d::i()->make_input(
                        kicker_unum,
                        tmpos,
                        opppos,
                        ballpos,
                        ballvel
                ));
                int nn_unum = DNN2d::i()->max_output() + 1;
                cout << "nn_unum: " << nn_unum << endl;
                if (nn_unum == self_unum) {
                    finded_pos = true;
                    target = new_pos;
                    break;
                }
            }
            if (finded_pos) break;
        }
        if (finded_pos) break;
        tmpos[self_unum - 1] = selfpos;
        DNN2d::i()->Calculate(DNN2d::i()->make_input(
                kicker_unum,
                tmpos,
                opppos,
                ballpos,
                ballvel
        ));
        kicker_unum = DNN2d::i()->max_output() + 1;
    }
}

void UnMark::set_poses(Vector2D *tmpos, Vector2D *opppos, const WorldModel &wm) {
    for (int i = 1; i <= 11; i++) {
        const AbstractPlayerObject *tm = wm.ourPlayer(i);
        if (tm == NULL || tm->unum() < 1) continue;

        tmpos[i - 1] = tm->pos();
    }
    for (int i = 1; i <= 11; i++) {
        const AbstractPlayerObject *opp = wm.theirPlayer(i);
        if (opp == NULL || opp->unum() < 1) continue;

        opppos[i - 1] = opp->pos();
    }
}

bool
Bhv_BasicMove::execute(PlayerAgent *agent) {
    dlog.addText(Logger::TEAM,
                 __FILE__": Bhv_BasicMove");
    //-----------------------------------------------
    // tackle
    if (Bhv_BasicTackle(0.8, 80.0).execute(agent)) {
        return true;
    }
    const WorldModel &wm = agent->world();
    /*--------------------------------------------------------*/
    // chase ball
    const int self_min = wm.interceptTable()->selfReachCycle();
    const int mate_min = wm.interceptTable()->teammateReachCycle();
    const int opp_min = wm.interceptTable()->opponentReachCycle();

    if (!wm.existKickableTeammate()
        && (self_min <= 3
            || (self_min <= mate_min
                && self_min < opp_min + 3)
        )
            ) {
        dlog.addText(Logger::TEAM,
                     __FILE__": intercept");
        Body_Intercept().execute(agent);
        agent->setNeckAction(new Neck_OffensiveInterceptNeck());

        return true;
    }

    const Vector2D target_point = Strategy::i().getPosition(wm.self().unum());
    const double dash_power = Strategy::get_normal_dash_power(wm);

    double dist_thr = wm.ball().distFromSelf() * 0.1;
    if (dist_thr < 1.0) dist_thr = 1.0;

    dlog.addText(Logger::TEAM,
                 __FILE__": Bhv_BasicMove target=(%.1f %.1f) dist_thr=%.2f",
                 target_point.x, target_point.y,
                 dist_thr);

    dlog.addLine(Logger::DRIBBLE, wm.self().pos(), target_point, 255, 0, 0);

    agent->debugClient().addMessage("BasicMove%.0f", dash_power);
    agent->debugClient().setTarget(target_point);
    agent->debugClient().addCircle(target_point, dist_thr);

    if (!Body_GoToPoint(target_point, dist_thr, dash_power
    ).execute(agent)) {
        Body_TurnToBall().execute(agent);
    }

    if (wm.existKickableOpponent()
        && wm.ball().distFromSelf() < 18.0) {
        agent->setNeckAction(new Neck_TurnToBall());
    } else {
        agent->setNeckAction(new Neck_TurnToBallOrScan());
    }

    return true;
}
