#include "MyStrategy.h"

#include "Move.h"
#include "Simulation.h"
#include "State.h"
#include "Static.h"
#include "Vec.h"
#include "Vis.h"

#include <cmath>
#include <iostream>
#include <memory>

using namespace model;
using namespace std;

MyStrategy::MyStrategy() { }

unique_ptr<Vis> vis = nullptr;

RobotState createRobot(const Robot& robot) {
    return RobotState(robot.id, Vec(robot.x, robot.y, robot.z), Vec(robot.velocity_x, robot.velocity_y, robot.velocity_z), robot.radius);
}

BallState createBall(const Ball& ball) {
    return BallState(Vec(ball.x, ball.y, ball.z), Vec(ball.velocity_x, ball.velocity_y, ball.velocity_z));
}

void MyStrategy::act(const Robot& me, const Rules& rules, const Game& game, Action& action) {
    if (me.id != 1) return;

    auto tick = game.current_tick;
    if (tick % 100 == 0) cout << "tick " << tick << endl;

    if (tick == 0) {
        vis = make_unique<Vis>(rules.arena);
        vis->drawArena();
        initializeStatic(rules, game);
    }

    /*
    action.target_velocity_x = (game.ball.x - me.x) * 1000.0;
    action.target_velocity_z = (game.ball.z - me.z) * 1000.0;
    */
    Move ans;
    if (abs(me.x) > getArena().width / 2 - getArena().bottom_radius - 2*me.radius) {
        ans = Move(Vec(0, 0, 0), getRules().ROBOT_MAX_JUMP_SPEED);
    } else {
        ans = Move(Vec(-getRules().ROBOT_MAX_GROUND_SPEED, 0, 0), 0);
    }

    vector<RobotState> robots;
    transform(game.robots.begin(), game.robots.end(), back_inserter(robots), createRobot);

    auto ball = createBall(game.ball);

    vis->addLog(createRobot(me).toString());
    vis->addLog(createBall(game.ball).toString());
    vis->addLog(ans.toString());
    vis->addLog("");

    auto state = State(
        move(robots),
        move(ball),
        me.id,
        [&](const RobotState& robot, int tick) {
            return ans;
        }
    );

    simulate(state, 50, *vis);

    action.target_velocity_x = ans.targetVelocity.x;
    action.target_velocity_y = ans.targetVelocity.y;
    action.target_velocity_z = ans.targetVelocity.z;
    action.jump_speed = ans.jumpSpeed;

    if (tick < 500) {
        vis->drawGame(me, game);
    } else {
        terminate();
    }
}
