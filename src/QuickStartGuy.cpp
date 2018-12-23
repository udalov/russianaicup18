#include "QuickStartGuy.h"

#include "Static.h"
#include <iostream>

using namespace std;

Move quickStartMove(const State& state, const RobotState& me, int tick, int delta) {
    if (!me.touch) {
        // TODO: use nitro
        return Move();
    }

    auto inv = isAlly(me.id) ? 1.0 : -1.0;

    auto ball = state.ball.position;
    auto ballV = state.ball.velocity;
    auto pos = me.position;

    auto jump = ball.sqrDist(pos) < sqr(getRules().BALL_RADIUS + getRules().ROBOT_MAX_RADIUS) && pos.y < ball.y;

    auto isAttacker = false;
    for (auto& robot : state.robots) {
        if (robot.id != me.id && isAlly(robot.id) == isAlly(me.id) && robot.position.z*inv < pos.z*inv) {
            isAttacker = true;
        }
    }

    /*
    auto DEBUG = me.id == 4 && tick == 12 && delta == 0;
    if (DEBUG) {
        cout << me.toString() << " isAttacker=" << isAttacker << endl;
    }
    */

    if (isAttacker) {
        for (int i = 0; i < 100; i++) {
            auto t = i * 0.1;
            auto ballPos = ball + ballV * t;

            if (ballPos.z*inv > pos.z*inv && abs(ballPos.x) < getArena().width / 2 && abs(ballPos.z) < getArena().depth / 2) {
                auto deltaPos = Vec(ballPos.x - pos.x, 0, ballPos.z - pos.z);
                auto deltaPosDist = deltaPos.len();
                auto needSpeed = deltaPosDist / t;
                if (0.5 * getRules().ROBOT_MAX_GROUND_SPEED < needSpeed && needSpeed < getRules().ROBOT_MAX_GROUND_SPEED) {
                    auto targetVelocity = deltaPos * (1 / t);
                    return Move(targetVelocity, jump ? getRules().ROBOT_MAX_JUMP_SPEED : 0);
                }
            }
        }
    }

    auto targetPos = Vec(0, 0, -getArena().depth / 2 + getArena().bottom_radius);
    if (ballV.z*inv < -1e-5) {
        auto t = (targetPos.z*inv - ball.z) / ballV.z;
        auto x = ball.x + ballV.x * t;
        if (abs(x) < getArena().goal_width / 2) {
            targetPos.x = x;
        }
    }

    auto targetVelocity = Vec(targetPos.x - pos.x, 0, targetPos.z*inv - pos.z);
    targetVelocity *= getRules().ROBOT_MAX_GROUND_SPEED;
    return Move(targetVelocity, jump ? getRules().ROBOT_MAX_JUMP_SPEED : 0);
}
