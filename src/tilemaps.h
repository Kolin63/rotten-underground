#ifndef CAMPFIRE_TILEMAPS_H
#define CAMPFIRE_TILEMAPS_H

#include <stdlib.h>

#include "pos.h"


#define LEVELS_AMOUNT 11

struct rat_spawn {
  struct pos_int pos;
  int rat_type;
};

struct level {
  size_t width;
  size_t height;
  struct pos_int player_spawn;
  struct rat_spawn rat_spawns[32];
};

// TILE KEY
// '2' : Gravel (Floor)
// '4' : Track Left
// '6' : Track Middle
// '7' : Track Right
// '8' : Platform (Walkway)
// '9' : Wall (Obstacle)
// '10' : Rubble (Obstacle)

// clang-format off
static const struct level levels[LEVELS_AMOUNT] = {
    {30, 20, {2, 8},  {},},   // 0: Intro
    {30, 20, {2, 8},  {},},   // 1: Movement Tutorial
    {30, 20, {2, 10}, {},},  // 2: First Combat
    {30, 20, {2, 8},  {},},   // 3: Multiple Rats
    {30, 20, {2, 8},  {},},   // 4: Find the Gun
    {30, 20, {2, 9},  {},},   // 5: Gun Practice
    {30, 20, {2, 8},  {},},   // 6: Both Weapons
    {30, 20, {2, 8},  {},},   // 7: Money Level
    {30, 20, {2, 8},  {},},   // 8: Gauntlet
    {30, 20, {2, 8},  {},},   // 9: Pre-Boss
    {30, 20, {5, 8},  {},},  // 10: Boss Arena
};
// clang-format on

#endif
