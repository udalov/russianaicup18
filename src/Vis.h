#pragma once

#include "model/Arena.h"
#include "model/Game.h"
#include "model/Robot.h"

#include <fstream>
#include <string>

class Vis {
    private:
        std::ofstream vis;

        double DEPTH, WIDTH, HEIGHT, GOAL_DEPTH;
        double OXZ_DX, OXZ_DY, OXY_DX, OXY_DY, OYZ_DX, OYZ_DY;

    public:
        Vis(const model::Arena& arena);

        void drawArena(const model::Arena& arena);
        void drawGame(const model::Robot& me, const model::Game& game);
        void endTurn();

        void drawSphere(double x, double y, double z, double radius);

        void color(const std::string& id);
        void drawRect(double x1, double y1, double width, double height);
        void drawCircle(double x, double y, double radius);
};

