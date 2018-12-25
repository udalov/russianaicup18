#pragma once

#include "model/Game.h"
#include "model/Robot.h"

#include <fstream>
#include <functional>
#include <string>
#include <vector>

struct Color {
    double red, green, blue;
    std::string string;

    Color(double red, double green, double blue);

    Color alpha(double alpha) const;

    bool operator==(const Color& other) const;
    bool operator!=(const Color& other) const;

    static Color ME;
    static Color ALLY;
    static Color ENEMY;
    static Color BALL;
    static Color WHITE;
};

class Vis {
    private:
        std::ofstream vis;
        std::vector<std::function<void(Vis&)>> actions;
        std::vector<std::string> logs;

        double OXZ_DX, OXZ_DY, OXY_DX, OXY_DY, OYZ_DX, OYZ_DY;

        Color lastColor;
        double trX, trY;

    public:
        Vis();

        void drawArena();
        void drawGame(const model::Robot& me, const model::Game& game);
        void drawScores(const model::Game& game);
        void drawText();
        void endTurn();

        void addAction(const std::function<void(Vis&)>& block);
        void addLog(const std::string& block);

        void drawSphere(double x, double y, double z, double radius, const Color& color);

        void setColor(const Color& color);
        void setTranslation(double dx, double dy);
        void drawLine(double x1, double y1, double x2, double y2);
        void drawArc(double x, double y, double radius, double startAngle, double extent);
        void drawRect(double x1, double y1, double width, double height);
        void drawCircle(double x, double y, double radius);
};
