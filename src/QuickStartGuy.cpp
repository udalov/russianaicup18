#include "QuickStartGuy.h"

#include "Const.h"
#include "Static.h"
#include <iostream>

using namespace std;

Move quickStartMove(const State& state, const RobotState& me, int tick, int delta) {
    if (!me.touch) {
        // TODO: use nitro
        return Move(Vec(0, -MAX_ENTITY_SPEED, 0), false);
    }

    auto isMe = isAlly(me.id);
    auto inv = isMe ? 1.0 : -1.0;

    auto ball = state.ball.position;
    auto ballV = state.ball.velocity;
    auto pos = me.position;

    auto jump = ball.sqrDist(pos) < sqr(BALL_RADIUS + ROBOT_MAX_RADIUS) && pos.z*inv < ball.z*inv;

    /*
    auto DEBUG = me.id == 3 && tick == 100 && delta == 0;
    if (DEBUG) {
        cout << me.toString() << " ball.distance(pos)=" << ball.distance(pos) << " pos.z=" << pos.z << " ball.z=" << ball.z << endl;
    }
    */

    auto isAttacker = false;
    if (isMe) {
        // Not the quick start behavior: avoid defending when both bots are at the same distance to the goal,
        // which happens in the --disable-random mode
        isAttacker = true;
        for (auto& robot : state.robots) {
            if (robot.id != me.id && isAlly(robot.id) == isAlly(me.id) && robot.position.z*inv < pos.z*inv) {
                isAttacker = false;
            }
        }
    } else {
        for (auto& robot : state.robots) {
            if (robot.id != me.id && isAlly(robot.id) == isAlly(me.id) && robot.position.z*inv < pos.z*inv) {
                isAttacker = true;
            }
        }
    }

    if (isAttacker) {
        auto increment = ballV * 0.1;
        auto ballPos = ball;
        for (int i = 1; i <= 100; i++) {
            ballPos += increment;

            if (ballPos.z*inv > pos.z*inv && abs(ballPos.x) < ARENA_W / 2 && abs(ballPos.z) < ARENA_D / 2) {
                auto deltaPos = Vec(ballPos.x - pos.x, 0, ballPos.z - pos.z);
                auto deltaPosDist = deltaPos.len();
                auto needSpeed = deltaPosDist * 10.0 / i;
                if (0.5 * ROBOT_MAX_GROUND_SPEED < needSpeed && needSpeed < ROBOT_MAX_GROUND_SPEED) {
                    auto targetVelocity = deltaPos * (10.0 / i);
                    return Move(targetVelocity, jump ? ROBOT_MAX_JUMP_SPEED : 0);
                }
            }
        }
    }

    auto targetPos = Vec(0, 0, -ARENA_D / 2 + ARENA_BR);
    if (ballV.z*inv < -1e-5) {
        auto t = (targetPos.z*inv - ball.z) / ballV.z;
        auto x = ball.x + ballV.x * t;
        if (abs(x) < ARENA_GW / 2) {
            targetPos.x = x;
        }
    }

    auto targetVelocity = Vec(targetPos.x - pos.x, 0, targetPos.z*inv - pos.z);
    targetVelocity *= ROBOT_MAX_GROUND_SPEED;
    return Move(targetVelocity, jump ? ROBOT_MAX_JUMP_SPEED : 0);
}
