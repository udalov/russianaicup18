#include "Vis.h"

#include "Const.h"

using namespace model;
using namespace std;

const double SCALE = 9.0;

Color Color::ME = Color(255, 255, 255);
Color Color::ALLY = Color(150, 150, 255);
Color Color::ENEMY = Color(255, 150, 150);
Color Color::BALL = Color(40, 255, 40);
Color Color::WHITE = Color(255, 255, 255);

Vis::Vis() :
    vis("/Users/udalov/c/russianaicup18/out/log.txt"),
    actions(),
    logs(),
    OXZ_DX(1 + ARENA_GD),
    OXZ_DY(3),
    OXY_DX(1 + ARENA_GD + ARENA_D + ARENA_GD + 1),
    OXY_DY(3),
    OYZ_DX(1 + ARENA_GD),
    OYZ_DY(3 + ARENA_W + 3),
    lastColor(Color(-1, -1, -1)),
    trX(0.0), trY(0.0)
{}

void Vis::drawArena() {
    setColor(Color::WHITE);

    auto D = ARENA_D;
    auto W = ARENA_W;
    auto H = ARENA_H;
    auto cr = ARENA_CR;
    auto tr = ARENA_TR;
    auto br = ARENA_BR;
    auto gw = ARENA_GW;
    auto gd = ARENA_GD;
    auto gh = ARENA_GH;
    auto gsr = ARENA_GSR;
    auto gtr = ARENA_GTR;

    // Oxz projection

    setTranslation(OXZ_DX, OXZ_DY);

    drawArc(cr, cr, cr, 90, 90);
    drawLine(cr, 0, D - cr, 0);
    drawArc(D - cr, cr, cr, 0, 90);
    drawLine(D, cr, D, W/2 - gw/2 - gsr);
    drawLine(D, W/2 + gw/2 + gsr, D, W - cr);
    drawArc(D - cr, W - cr, cr, 270, 90);
    drawLine(D - cr, W, cr, W);
    drawArc(cr, W - cr, cr, 180, 90);
    drawLine(0, W - cr, 0, W/2 + gw/2 + gsr);
    drawLine(0, W/2 - gw/2 - gsr, 0, cr);

    drawArc(D + gsr, W/2 - gw/2 - gsr, gsr, 180, 90);
    drawArc(D + gsr, W/2 + gw/2 + gsr, gsr, 90, 90);
    drawArc(-gsr, W/2 - gw/2 - gsr, gsr, 270, 90);
    drawArc(-gsr, W/2 + gw/2 + gsr, gsr, 0, 90);

    drawLine(-gd + gtr, W/2 - gw/2, -gsr, W/2 - gw/2);
    drawLine(-gd + gtr, W/2 + gw/2, -gsr, W/2 + gw/2);
    drawLine(D + gsr, W/2 - gw/2, D + gd - gtr, W/2 - gw/2);
    drawLine(D + gsr, W/2 + gw/2, D + gd - gtr, W/2 + gw/2);

    drawArc(-gd + gtr, W/2 - gw/2 + gtr, gtr, 90, 90);
    drawArc(-gd + gtr, W/2 + gw/2 - gtr, gtr, 180, 90);
    drawArc(D + gd - gtr, W/2 - gw/2 + gtr, gtr, 0, 90);
    drawArc(D + gd - gtr, W/2 + gw/2 - gtr, gtr, 270, 90);

    drawLine(-gd, W/2 - gw/2 + gtr, -gd, W/2 + gw/2 - gtr);
    drawLine(D + gd, W/2 - gw/2 + gtr, D + gd, W/2 + gw/2 - gtr);


    // Oxy projection

    setTranslation(OXY_DX, OXY_DY);

    drawArc(tr, tr, tr, 90, 90);
    drawLine(tr, 0, W - tr, 0);
    drawArc(W - tr, tr, tr, 0, 90);
    drawLine(W, tr, W, H - br);
    drawArc(W - br, H - br, br, 270, 90);
    drawLine(W - br, H, br, H);
    drawArc(br, H - br, br, 180, 90);
    drawLine(0, H - br, 0, tr);

    setColor(Color::WHITE.alpha(0.4));
    drawArc(W/2 - gw/2 + gtr, H - gh + gtr, gtr, 90, 90);
    drawLine(W/2 - gw/2 + gtr, H - gh, W/2 + gw/2 - gtr, H - gh);
    drawArc(W/2 + gw/2 - gtr, H - gh + gtr, gtr, 0, 90);
    drawLine(W/2 + gw/2, H - gh + gtr, W/2 + gw/2, H - br);
    drawArc(W/2 + gw/2 - gtr, H - br, gtr, 270, 90);
    drawArc(W/2 - gw/2 + gtr, H - br, gtr, 180, 90);
    drawLine(W/2 - gw/2, H - br, W/2 - gw/2, H - gh + gtr);
    setColor(Color::WHITE);
    

    // Oyz projection

    setTranslation(OYZ_DX, OYZ_DY);

    drawRect(0, 0, D, H);


    // Extras for Oxz

    setTranslation(OXZ_DX, OXZ_DY);
    setColor(Color::WHITE.alpha(0.2));
    drawLine(D/2, 0, D/2, W);
    drawCircle(D/2, W/2, gw/2);
    drawLine(0, W/2 - gw/2 - gsr, 0, W/2 + gw/2 + gsr);
    drawLine(D, W/2 - gw/2 - gsr, D, W/2 + gw/2 + gsr);

    setTranslation(0, 0);
    endTurn();
}

void Vis::drawGame(const Robot& me, const Game& game) {
    for (auto& action : actions) {
        action(*this);
    }

    actions.clear();

    for (auto& isTeammate : { false, true }) {
        for (auto& robot : game.robots) if (robot.is_teammate == isTeammate) {
            drawSphere(robot.x, robot.y, robot.z, robot.radius, isTeammate ? Color::ALLY : Color::ENEMY);
        }
    }

    drawSphere(me.x, me.y, me.z, me.radius, Color::ME);
    drawSphere(game.ball.x, game.ball.y, game.ball.z, game.ball.radius, Color::BALL);
    
    drawScores(game);

    drawText();

    endTurn();
}

void Vis::drawScores(const Game& game) {
    vis << ">\n";
    for (auto& isMe : { true, false }) {
        for (auto& player : game.players) if (player.me == isMe) {
            vis << "Player #" << player.id << (isMe ? " (ME)" : "     ") << "      " << player.score;
            if (player.strategy_crashed) vis << " [CRASHED]";
            vis << "\n";
        }
    }
}

void Vis::drawText() {
    if (logs.empty()) return;

    vis << "-\n";
    for (auto& log : logs) {
        vis << log << "\n";
    }
    logs.clear();
}

void Vis::endTurn() {
    vis << "---" << endl;
}

void Vis::addAction(const function<void(Vis&)>& block) {
    actions.push_back(block);
}

void Vis::addLog(const string& log) {
    logs.push_back(log);
}

void Vis::drawSphere(double x, double y, double z, double radius, const Color& color) {
    setColor(color);
    drawCircle(ARENA_D/2 - z + OXZ_DX, ARENA_W/2 - x + OXZ_DY, radius);
    drawCircle(ARENA_W/2 + x + OXY_DX, ARENA_H - y + OXY_DY, radius);
    drawCircle(ARENA_D/2 - z + OYZ_DX, ARENA_H - y + OYZ_DY, radius);
}

void Vis::setColor(const Color& color) {
    if (lastColor != color) {
        vis << "color " << color.string << "\n";
        lastColor = color;
    }
}

void Vis::setTranslation(double dx, double dy) {
    trX = dx;
    trY = dy;
}

void Vis::drawLine(double x1, double y1, double x2, double y2) {
    vis << "draw-line " << (x1 + trX) * SCALE << " " << (y1 + trY) * SCALE << " " << (x2 + trX) * SCALE << " " << (y2 + trY) * SCALE << "\n";
}

void Vis::drawArc(double x, double y, double radius, double startAngle, double extent) {
    vis << "draw-arc " << (x + trX) * SCALE << " " << (y + trY) * SCALE << " " << radius * SCALE << " " << startAngle << " " << extent << "\n";
}

void Vis::drawRect(double x1, double y1, double width, double height) {
    vis << "draw-rect " << (x1 + trX) * SCALE << " " << (y1 + trY) * SCALE << " " << width * SCALE << " " << height * SCALE << "\n";
}

void Vis::drawCircle(double x, double y, double radius) {
    vis << "draw-circle " << (x + trX) * SCALE << " " << (y + trY) * SCALE << " " << radius * SCALE << "\n";
}


Color::Color(double red, double green, double blue) :
    red(red), green(green), blue(blue),
    string(to_string(red) + " " + to_string(green) + " " + to_string(blue))
{}

Color Color::alpha(double alpha) const {
    return Color(red * alpha, green * alpha, blue * alpha);
}

bool Color::operator==(const Color& other) const {
    return red == other.red && green == other.green && blue == other.blue;
}

bool Color::operator!=(const Color& other) const {
    return !(*this == other);
}
