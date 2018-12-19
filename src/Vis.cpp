#include "Vis.h"

using namespace model;
using namespace std;

const double SCALE = 9.0;

Vis::Vis(const Arena& arena) :
    vis("/Users/udalov/c/russianaicup18/src/out/log.txt"),
    DEPTH(arena.depth),
    WIDTH(arena.width),
    HEIGHT(arena.height),
    GOAL_DEPTH(arena.goal_depth),
    OXZ_DX(1 + GOAL_DEPTH),
    OXZ_DY(3),
    OXY_DX(1 + GOAL_DEPTH + DEPTH + GOAL_DEPTH + 1),
    OXY_DY(3),
    OYZ_DX(1 + GOAL_DEPTH),
    OYZ_DY(3 + WIDTH + 3)
{}

void Vis::drawArena(const Arena& arena) {
    color("white");
    drawRect(OXZ_DX, OXZ_DY, DEPTH, WIDTH);
    drawRect(OXY_DX, OXY_DY, WIDTH, HEIGHT);
    drawRect(OYZ_DX, OYZ_DY, DEPTH, HEIGHT);
    endTurn();
}

void Vis::drawGame(const Robot& me, const Game& game) {
    for (auto& isTeammate : { false, true }) {
        color(isTeammate ? "150 150 255" : "255 150 150");
        for (auto& robot : game.robots) if (robot.is_teammate == isTeammate) {
            drawSphere(robot.x, robot.y, robot.z, robot.radius);
        }
    }

    color("white");
    drawSphere(me.x, me.y, me.z, me.radius);
    color("green");
    drawSphere(game.ball.x, game.ball.y, game.ball.z, game.ball.radius);
    endTurn();
}

void Vis::endTurn() {
    vis << "---" << endl;
}

void Vis::drawSphere(double x, double y, double z, double radius) {
    drawCircle(DEPTH/2 - z + OXZ_DX, WIDTH/2 - x + OXZ_DY, radius);
    drawCircle(WIDTH/2 + x + OXY_DX, HEIGHT - y + OXY_DY, radius);
    drawCircle(DEPTH/2 - z + OYZ_DX, HEIGHT - y + OYZ_DY, radius);
}

void Vis::color(const string& id) {
    vis << "color " << id << "\n";
}

void Vis::drawRect(double x1, double y1, double width, double height) {
    vis << "draw-rect " << x1 * SCALE << " " << y1 * SCALE << " " << width * SCALE << " " << height * SCALE << "\n";
}

void Vis::drawCircle(double x, double y, double radius) {
    vis << "draw-circle " << x * SCALE << " " << y * SCALE << " " << radius * SCALE << "\n";
}
