#include "MyStrategy.h"
#include "Vis.h"

#include <cmath>
#include <iostream>
#include <memory>

using namespace model;
using namespace std;

MyStrategy::MyStrategy() { }

unique_ptr<Vis> vis = nullptr;

void MyStrategy::act(const Robot& me, const Rules& rules, const Game& game, Action& action) {
    if (me.id != 1) return;

    auto tick = game.current_tick;
    if (tick % 100 == 0) cout << "tick " << tick << endl;

    if (tick == 0) {
        vis = make_unique<Vis>(rules.arena);
        vis->drawArena(rules.arena);
    }

    action.target_velocity_x = (game.ball.x - me.x) * 1000.0;
    action.target_velocity_z = (game.ball.z - me.z) * 1000.0;

    if (tick < 3000) {
        vis->drawGame(me, game);
    } else {
        terminate();
    }
}
