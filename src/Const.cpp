#define NO_CONST
#include "Const.h"

#include <fstream>

using namespace model;
using namespace std;

void printConst(const Rules& rules) {
    auto arena = rules.arena;

    ofstream out("/Users/udalov/c/russianaicup18/src/Const.h");

    out << "#pragma once" << endl << endl << "#include \"model/Rules.h\"" << endl << endl << "#ifndef NO_CONST" << endl << endl;

    out.precision(2);
    out << fixed;
    out << "#define ROBOT_MIN_RADIUS " << rules.ROBOT_MIN_RADIUS << endl;
    out << "#define ROBOT_MAX_RADIUS " << rules.ROBOT_MAX_RADIUS << endl;
    out << "#define ROBOT_MAX_JUMP_SPEED " << rules.ROBOT_MAX_JUMP_SPEED << endl;
    out << "#define ROBOT_ACCELERATION " << rules.ROBOT_ACCELERATION << endl;
    out << "#define ROBOT_NITRO_ACCELERATION " << rules.ROBOT_NITRO_ACCELERATION << endl;
    out << "#define ROBOT_MAX_GROUND_SPEED " << rules.ROBOT_MAX_GROUND_SPEED << endl;
    out << "#define ROBOT_ARENA_E " << rules.ROBOT_ARENA_E << endl;
    out << "#define ROBOT_RADIUS " << rules.ROBOT_RADIUS << endl;
    out << "#define ROBOT_MASS " << rules.ROBOT_MASS << endl;
    out << "#define TICKS_PER_SECOND " << rules.TICKS_PER_SECOND << endl;
    out << "#define MICROTICKS_PER_TICK " << rules.MICROTICKS_PER_TICK << endl;
    out << "#define RESET_TICKS " << rules.RESET_TICKS << endl;
    out << "#define BALL_ARENA_E " << rules.BALL_ARENA_E << endl;
    out << "#define BALL_RADIUS " << rules.BALL_RADIUS << endl;
    out << "#define BALL_MASS " << rules.BALL_MASS << endl;
    out << "#define MIN_HIT_E " << rules.MIN_HIT_E << endl;
    out << "#define MAX_HIT_E " << rules.MAX_HIT_E << endl;
    out << "#define MAX_ENTITY_SPEED " << rules.MAX_ENTITY_SPEED << endl;
    out << "#define MAX_NITRO_AMOUNT " << rules.MAX_NITRO_AMOUNT << endl;
    out << "#define START_NITRO_AMOUNT " << rules.START_NITRO_AMOUNT << endl;
    out << "#define NITRO_POINT_VELOCITY_CHANGE " << rules.NITRO_POINT_VELOCITY_CHANGE << endl;
    out << "#define NITRO_PACK_X " << rules.NITRO_PACK_X << endl;
    out << "#define NITRO_PACK_Y " << rules.NITRO_PACK_Y << endl;
    out << "#define NITRO_PACK_Z " << rules.NITRO_PACK_Z << endl;
    out << "#define NITRO_PACK_RADIUS " << rules.NITRO_PACK_RADIUS << endl;
    out << "#define NITRO_PACK_AMOUNT " << rules.NITRO_PACK_AMOUNT << endl;
    out << "#define NITRO_PACK_RESPAWN_TICKS " << rules.NITRO_PACK_RESPAWN_TICKS << endl;
    out << "#define GRAVITY " << rules.GRAVITY << endl;
    out << endl;
    out << "#define ARENA_W " << arena.width << endl;
    out << "#define ARENA_H " << arena.height << endl;
    out << "#define ARENA_D " << arena.depth << endl;
    out << "#define ARENA_BR " << arena.bottom_radius << endl;
    out << "#define ARENA_TR " << arena.top_radius << endl;
    out << "#define ARENA_CR " << arena.corner_radius << endl;
    out << "#define ARENA_GTR " << arena.goal_top_radius << endl;
    out << "#define ARENA_GW " << arena.goal_width << endl;
    out << "#define ARENA_GH " << arena.goal_height << endl;
    out << "#define ARENA_GD " << arena.goal_depth << endl;
    out << "#define ARENA_GSR " << arena.goal_side_radius << endl;

    out << endl << "#endif" << endl << endl << "void printConst(const model::Rules& rules);" << endl;
}
