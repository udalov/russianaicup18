#include "QuickStartGuy.h"

#include "Const.h"
#include "Solution.h"
#include "Team.h"
#include <iostream>

using namespace std;

namespace {
    constexpr auto radiusSumSqr = sqr(BALL_RADIUS + ROBOT_MAX_RADIUS);
    constexpr auto goalieTargetPos = Vec(0, 0, -ARENA_D / 2 + ARENA_BR);
}

template class QuickStartGuy<false>;
template class QuickStartGuy<true>;

template<bool isMe>
Move QuickStartGuy<isMe>::getMove(const State& state, const RobotState& me, int tick, int delta) {
    if (lastGoalTick <= tick && tick <= lastGoalTick + RESET_TICKS - 2) {
        return Move();
    }

    if (!me.touch) {
        // TODO: use nitro
        return Move(Vec(0, -MAX_ENTITY_SPEED, 0), false);
    }

    constexpr auto inv = isMe ? 1.0 : -1.0;
    constexpr auto iterations = isMe ? 100 : ((TRACK_LEN + 5) / 6);

    auto ball = state.ball.position;
    auto ballV = state.ball.velocity;
    auto pos = me.position;

    auto jump = ball.sqrDist(pos) < radiusSumSqr && pos.z*inv < ball.z*inv;

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
            if (robot.id != me.id && isAlly(robot.id) == isMe && robot.position.z*inv < pos.z*inv) {
                isAttacker = false;
            }
        }
    } else {
        for (auto& robot : state.robots) {
            if (robot.id != me.id && isAlly(robot.id) == isMe && robot.position.z*inv < pos.z*inv) {
                isAttacker = true;
            }
        }
    }

    if (isAttacker) {
        auto increment = ballV * 0.1;
        auto ballPos = ball;
        for (size_t i = 1; i <= iterations; i++) {
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

    if (ballV.z*inv < -1e-5) {
        auto t = (goalieTargetPos.z*inv - ball.z) / ballV.z;
        auto x = ball.x + ballV.x * t;
        if (abs(x) < ARENA_GW / 2) {
            auto targetVelocity = Vec(x - pos.x, 0, goalieTargetPos.z*inv - pos.z);
            targetVelocity *= ROBOT_MAX_GROUND_SPEED;
            return Move(targetVelocity, jump ? ROBOT_MAX_JUMP_SPEED : 0);
        }
    }

    auto targetVelocity = Vec(goalieTargetPos.x - pos.x, 0, goalieTargetPos.z*inv - pos.z);
    targetVelocity *= ROBOT_MAX_GROUND_SPEED;
    return Move(targetVelocity, jump ? ROBOT_MAX_JUMP_SPEED : 0);
}
