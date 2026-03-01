#ifndef CAMPFIRE_TILEMAPMAPS_H
#define CAMPFIRE_TILEMAPMAPS_H

#include <stdlib.h>

#include "pos.h"

#define LEVELS_AMOUNT 13

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

// TILE KEY (Manual overrides or special metadata can go here)
// Layouts are loaded from assets/levels/levelN.tmj

// clang-format off
static const struct level levels[LEVELS_AMOUNT] = {
    {30, 20, {0, 17}, {{{0, 17}, 1}}},                               // 0: Intro
    {30, 20, {1, 17}, {}},                                           // 1: Movement Tutorial (0 rats)
    {30, 20, {0, 10}, {{{26, 16}, 1}}},                              // 2: First Combat (1 rat)
    {30, 20, {2, 8}, {}},                                           // 3: Crazy Man (NPC)
    {30, 20, {2, 8}, {{{15, 10}, 1}, {{25, 15}, 1}}},                // 4: Find the Gun
    {30, 20, {2, 9}, {{{10, 8}, 2}, {{15, 12}, 2}, {{20, 18}, 2}}},  // 5: Gun Practice (Stage 2)
    {30, 20, {2, 8}, {}},                                           // 6: Crazy Gal (NPC)
    {30, 20, {2, 8}, {{{8, 12}, 1}, {{22, 12}, 1}}},                 // 7: Money Level
    {30, 20, {2, 8}, {{{10, 5}, 2}, {{15, 12}, 3}, {{20, 5}, 2}, {{25, 15}, 2}}}, // 8: Gauntlet (Stage 3)
    {30, 20, {2, 8}, {}},                                           // 9: Crazy Guy (NPC)
    {30, 20, {5, 8}, {}},                                            // 10: Boss Arena
    {30, 20, {15, 10}, {}},                                          // 11: end credits
    {30, 20, {15, 10}, {}},                                          // 12: death screen
};
// clang-format on

#endif
