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
#include <vector>
#include "dnn/DNN2d.h"
using namespace rcsc;
using namespace std;
/*-------------------------------------------------------------------*/
/*!

 */
bool UnMark::execute(PlayerAgent *agent)
{
    const WorldModel & wm = agent->world();
    Vector2D stra =  Strategy::i().getPosition(wm.self().unum());
    if(stra.dist(wm.self().pos()) > 10)
        return false;
    int self_unum = wm.self().unum();
    vector<Vector2D> tm_pos;
    vector<Vector2D> opp_pos;
    Vector2D ball_pos;
    Vector2D ball_vel;
    wm2vector(wm, tm_pos, opp_pos, ball_pos, ball_vel);

    int receiver_tm = 0;
    double max_out = -100000;
    double max_out_self = -100000;
    Vector2D best_pos = wm.self().pos();
    int unum_tmp = self_unum - 1;
    for(double dist = 0; dist <= 4; dist+= 2){
        for(double angle = 0; angle < 360; angle += 45){
            Vector2D tar = wm.self().pos() + Vector2D::polar2vector(dist, angle);
            if(tar.absX() > 52 || tar.absY() > 36 || tar.x > wm.offsideLineX())
                continue;
            tm_pos[unum_tmp] = tar;
            auto features_vec = vector2feature(wm, tm_pos, opp_pos, ball_pos, ball_vel);
            MatrixXd features_mat = vector_to_matrix(features_vec);
            DNN2d::i()->Calculate(features_mat);
            auto out = DNN2d::i()->max_output();
            int tar_receiver_tm = get<0>(out) + 1;
            double tar_max_out = get<1>(out);
            if(tar_receiver_tm == self_unum){
                if(tar_max_out > max_out_self){
                    max_out_self = tar_max_out;
                    best_pos = tar;
                }
            }else{
                if(tar_max_out > max_out){
                    max_out = tar_max_out;
                }
            }
            if(dist<1)
                break;
        }
    }

    if(max_out < max_out_self - 0.1 && max_out_self > 0.5){
        agent->debugClient().addCircle(best_pos, 1.5);
        Body_GoToPoint2010(best_pos, 0.1, 100).execute(agent);
        return true;
    }
    return false;
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

    if(mate_min <= opp_min){
//        if(UnMark().execute(agent))
//            return true;
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
