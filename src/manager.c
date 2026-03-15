#include "manager.h"

#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "bullet.h"
#include "controller.h"
#include "enemy.h"
#include "player.h"
#include "render.h"
#include "tile.h"
#include "tilemaps.h"

#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 640

static struct manager* global_manager;

static bool cleanup_done = false;

void manager_init() {
  srand(time(NULL));
  global_manager = malloc(sizeof(struct manager));
  global_manager->game_should_run = true;
  global_manager->player = malloc(sizeof(struct player));
  global_manager->player->pos.x = (float)(levels[0].player_spawn.x * TILE_SIZE);
  global_manager->player->pos.y = (float)(levels[0].player_spawn.y * TILE_SIZE);

  global_manager->camera.offset.x = SCREEN_WIDTH / 2.0;
  global_manager->camera.offset.y = SCREEN_HEIGHT / 2.0;
  global_manager->camera.target.x = 0;
  global_manager->camera.target.y = 0;
  global_manager->camera.rotation = 0.0f;
  global_manager->camera.zoom = 1.0f;

  global_manager->current_level = 0;
  global_manager->intro_step = 0;
  global_manager->intro_timer = 0;
  global_manager->title_alpha = 0.0f;
  global_manager->money = 0;
  global_manager->lives = 6;
  global_manager->has_gun = false;
  global_manager->active_weapon = 0;
  global_manager->crowbar_cooldown = 0.0f;
  global_manager->gun_cooldown = 0.0f;
  global_manager->player_invincibility_timer = 0.0f;
  global_manager->contact_timer = 0.0f;
  global_manager->boss_active = false;
  global_manager->boss_hp = 100;

  for (int i = 0; i < MAX_MONEY; i++) {
    global_manager->money_items[i].active = false;
  }
  global_manager->boss_stage = 1;

  tilemap_init();
  tilemap_load_level(0);

  global_manager->dialog = malloc(sizeof(struct dialog_box));
  dialog_init(global_manager->dialog);

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Rotten Underground");
  SetTargetFPS(60);

  global_manager->font = LoadFont("assets/Jersey15-Regular.ttf");
  global_manager->dialog->font = global_manager->font;

  global_manager->player_tex = LoadTexture("assets/player.png");
  for (int i = 0; i < 7; i++) {
    char path[64];
    sprintf(path, "assets/ratStage1/1rat_%d.png", i + 1);
    global_manager->rat_anim[i] = LoadTexture(path);
  }
  global_manager->rat2_anim[0] = LoadTexture("assets/ratStage2/2rat_1.png");
  global_manager->rat2_anim[1] = LoadTexture("assets/ratStage2/2rat_2.png");
  global_manager->gravel_tex = LoadTexture("assets/gravel.png");
  global_manager->track_left_tex = LoadTexture("assets/trackLeft.png");
  global_manager->track_middle_tex = LoadTexture("assets/trackMiddle.png");
  global_manager->track_right_tex = LoadTexture("assets/trackRight.png");
  global_manager->platform_tex = LoadTexture("assets/platform.png");
  global_manager->rubble_tex = LoadTexture("assets/rubble.png");
  global_manager->wall_tex = LoadTexture("assets/wall.png");
  global_manager->labfloor_tex = LoadTexture("assets/labFloor.png");
  global_manager->goofloor_tex = LoadTexture("assets/gooOnLabFloor.png");
  global_manager->coin_tex = LoadTexture("assets/coin.png");
  global_manager->moneyicon_tex = LoadTexture("assets/moneyIcon.png");
  global_manager->heart_tex = LoadTexture("assets/heart.png");
  global_manager->halfheart_tex = LoadTexture("assets/halfHeart.png");
  global_manager->boss_tex[0] = LoadTexture("assets/boss_1.png");
  global_manager->boss_tex[1] = LoadTexture("assets/boss_2.png");
  global_manager->boss_tex[2] = LoadTexture("assets/boss_3.png");
  global_manager->boss_head_knocked_tex =
      LoadTexture("assets/boss_head_knocked.png");
  global_manager->bullet_tex = LoadTexture("assets/bullet.png");
  global_manager->goo_tex = LoadTexture("assets/goo.png");
  global_manager->goolabfloor_tex = LoadTexture("assets/gooOnLabFloor.png");
  for (int i = 0; i < 4; i++) {
    char npc_path[64];
    sprintf(npc_path, "assets/npc%d.png", i + 1);
    global_manager->npc_tex[i] = LoadTexture(npc_path);
  }
  InitAudioDevice();
  global_manager->death_snd = LoadSound("assets/death.mp3");
  global_manager->bgm =
      LoadMusicStream("assets/audio/nullnel-mossy-sewer-169049.mp3");
  PlayMusicStream(global_manager->bgm);

  global_manager->game_should_run = true;

  for (int i = 0; i < MAX_BULLETS; i++) {
    bullet(&global_manager->bullets[i]);
  }

  enemies_init(global_manager->enemies);
  enemies_spawn(global_manager->enemies,
                tilemap_get_current_level()->rat_spawns);
}

void manager_cleanup() {
  if (cleanup_done) return;
  cleanup_done = true;
  printf("Cleaning up...\n");
  UnloadTexture(global_manager->player_tex);
  for (int i = 0; i < 7; i++) {
    UnloadTexture(global_manager->rat_anim[i]);
  }
  UnloadTexture(global_manager->rat2_anim[0]);
  UnloadTexture(global_manager->rat2_anim[1]);
  UnloadTexture(global_manager->gravel_tex);
  UnloadTexture(global_manager->track_left_tex);
  UnloadTexture(global_manager->track_middle_tex);
  UnloadTexture(global_manager->track_right_tex);
  UnloadTexture(global_manager->platform_tex);
  UnloadTexture(global_manager->rubble_tex);
  UnloadTexture(global_manager->wall_tex);
  UnloadTexture(global_manager->labfloor_tex);
  UnloadTexture(global_manager->goofloor_tex);
  UnloadTexture(global_manager->coin_tex);
  UnloadTexture(global_manager->moneyicon_tex);
  UnloadTexture(global_manager->heart_tex);
  UnloadTexture(global_manager->halfheart_tex);
  for (int i = 0; i < 3; i++) {
    UnloadTexture(global_manager->boss_tex[i]);
  }
  UnloadTexture(global_manager->boss_head_knocked_tex);
  UnloadTexture(global_manager->bullet_tex);
  UnloadTexture(global_manager->goo_tex);
  UnloadTexture(global_manager->goolabfloor_tex);
  for (int i = 0; i < 4; i++) {
    UnloadTexture(global_manager->npc_tex[i]);
  }
  UnloadSound(global_manager->death_snd);
  UnloadMusicStream(global_manager->bgm);
  UnloadFont(global_manager->font);
  CloseAudioDevice();
  CloseWindow();
  free(global_manager->dialog);
  free(global_manager->player);
  free(global_manager);
}

static void update_level_script(float dt) {
  if (global_manager->current_level == 0) {
    if (!global_manager->dialog->active) {
      if (global_manager->intro_step == 0) {
        // Trigger bite
        global_manager->intro_step = 1;
        global_manager->intro_timer = 2.4f;
        static const char* bite[] = {"Agh! These pesky rats...."};
        dialog_show(global_manager->dialog, "Johnny", bite, 1);
      } else if (global_manager->intro_step == 1) {
        if (global_manager->intro_timer > 0) {
          global_manager->intro_timer -= dt;
          // Player runs
          if (global_manager->intro_timer > 0.4f) {
            global_manager->player->pos.x += 300 * dt;
          }
          if (global_manager->enemies[0].active) {
            global_manager->enemies[0].anim_timer += dt;
            if (global_manager->enemies[0].anim_timer >= 0.1f) {
              global_manager->enemies[0].anim_timer = 0;
              global_manager->enemies[0].frame =
                  (global_manager->enemies[0].frame + 1) % 7;
            }
            global_manager->enemies[0].pos.x += 600 * dt;
            global_manager->enemies[0].rotation = 180.0f;
          }

          if (global_manager->intro_timer <= 0.4f &&
              global_manager->enemies[0].active) {
            struct tilemap* tm = tilemap_get_current();
            int p_tile_x = (int)(global_manager->player->pos.x / TILE_SIZE);
            for (size_t y = 0; y < tm->height; y++) {
              // Collapse closely behind
              for (int x = 0; x < p_tile_x - 1 && x < (int)tm->width; x++) {
                tm->tiles[y * tm->width + x] = TILE_RUBBLE;
              }
            }
            // Deactivate rat
            global_manager->enemies[0].active = false;
            static const char* collapse[] = {"Holy- What just happened?!"};
            dialog_show(global_manager->dialog, "Johnny", collapse, 1);
          }
        } else {
          global_manager->intro_step = 2;
        }
      } else if (global_manager->intro_step == 2) {
        global_manager->intro_step = 3;
        // Face left
        global_manager->player->pos.x -= 2.0f;

        // Re-activate rat for fade out context (invisible offscreen)
        global_manager->enemies[0].active = true;
        global_manager->enemies[0].pos.x = 2000.0f;

        static const char* sealed[] = {"It's way too high, I'm sealed in.",
                                       "I guess there's only one way left."};
        dialog_show(global_manager->dialog, "Johnny", sealed, 2);
      } else if (global_manager->intro_step == 3) {
        if (!global_manager->dialog->active) {
          global_manager->intro_step = 4;
          global_manager->intro_timer = 2.5f;
          global_manager->title_alpha = 0.0f;
        }
      } else if (global_manager->intro_step == 4) {
        if (global_manager->intro_timer > 0) {
          global_manager->intro_timer -= dt;
          global_manager->title_alpha += dt / 1.0f;
          if (global_manager->title_alpha > 1.0f)
            global_manager->title_alpha = 1.0f;
        } else {
          // Start fading out scene
          global_manager->intro_step = 5;
          global_manager->intro_timer = 2.0f;
        }
      } else if (global_manager->intro_step == 5) {
        if (global_manager->intro_timer > 0) {
          global_manager->intro_timer -= dt;
          global_manager->player->pos.x += 200 * dt;
          global_manager->enemies[0].pos.x += 200 * dt;
        }
      }
    }
  } else if (global_manager->current_level == 1) {
    if (global_manager->intro_step == 0) {
      global_manager->intro_step = 1;
      global_manager->lives = 6;
      static const char* tut[] = {"WASD or Arrow Keys to move."};
      dialog_show(global_manager->dialog, "Graywater Subway PA System", tut, 1);
    }
  } else if (global_manager->current_level == 2) {
    if (global_manager->intro_step == 0) {
      global_manager->intro_step = 1;
      static const char* crowbar[] = {"To use your crowbar, left click.",
                                      "Kill the rat before continuing!"};
      dialog_show(global_manager->dialog, "Graywater Subway PA System", crowbar,
                  2);
    } else if (global_manager->intro_step == 1 &&
               !global_manager->dialog->active) {
      // Check if all enemies are dead for post-kill dialogue
      bool rats_alive = false;
      for (int i = 0; i < MAX_ENEMIES; i++) {
        if (global_manager->enemies[i].active) {
          rats_alive = true;
          break;
        }
      }
      if (!rats_alive) {
        global_manager->intro_step = 2;
        static const char* post[] = {
            "I swear, there's been so many more rats lately,",
            "well at least I have my trusty crowbar."};
        dialog_show(global_manager->dialog, "Johnny", post, 2);
      }
    }
  } else if (global_manager->current_level == 3) {
    if (global_manager->intro_step == 0) {
      global_manager->intro_step = 1;
      global_manager->lives = 6;
      static const char* l3a[] = {"Hey! Are you stuck here too?"};
      dialog_show(global_manager->dialog, "Crazy Man", l3a, 1);
    } else if (global_manager->intro_step == 1 &&
               !global_manager->dialog->active) {
      global_manager->intro_step = 2;
      static const char* l3b[] = {
          "Yeah, the tunnels collapsed. Classic Graywater..."};
      dialog_show(global_manager->dialog, "Johnny", l3b, 1);
    } else if (global_manager->intro_step == 2 &&
               !global_manager->dialog->active) {
      global_manager->intro_step = 3;
      static const char* l3c[] = {
          "Hey man, be careful. There's some crazy rats in here."};
      dialog_show(global_manager->dialog, "Crazy Man", l3c, 1);
    } else if (global_manager->intro_step == 3 &&
               !global_manager->dialog->active) {
      global_manager->intro_step = 4;
      static const char* l3d[] = {
          "Don't worry, I'm in these tunnels all the time, I've seen it "
          "all."};
      dialog_show(global_manager->dialog, "Johnny", l3d, 1);
    } else if (global_manager->intro_step == 4 &&
               !global_manager->dialog->active) {
      global_manager->intro_step = 5;
      static const char* l3e[] = {"Don't say I didn't warn you..."};
      dialog_show(global_manager->dialog, "Crazy Man", l3e, 1);
    }
  } else if (global_manager->current_level == 4) {
    if (global_manager->intro_step == 0) {
      global_manager->intro_step = 1;
      static const char* l4a[] = {"HELP! HELP!"};
      dialog_show(global_manager->dialog, "Helpless Person", l4a, 1);
    } else if (global_manager->intro_step == 1 &&
               !global_manager->dialog->active) {
      global_manager->intro_step = 2;
      global_manager->has_gun = true;
      static const char* l4b[] = {
          "Quick! Switch to your gun with right click, aim with your mouse, "
          "and left click to shoot!"};
      dialog_show(global_manager->dialog, "Helpless Person", l4b, 1);
    } else if (global_manager->intro_step == 2) {
      // Check if all enemies are dead for post-save dialogue
      bool rats_alive = false;
      for (int i = 0; i < MAX_ENEMIES; i++) {
        if (global_manager->enemies[i].active) {
          rats_alive = true;
          break;
        }
      }
      if (!rats_alive && !global_manager->dialog->active) {
        global_manager->intro_step = 3;
        static const char* l4c[] = {
            "Oh my god! Thank you so much!",
            "Quick tip, you can swap between your weapons with right click."};
        dialog_show(global_manager->dialog, "Helpless Person", l4c, 2);
      }
    }
  } else if (global_manager->current_level == 5) {
    if (global_manager->intro_step == 0) {
      // Check if all enemies are dead for post-fight dialogue
      bool rats_alive = false;
      for (int i = 0; i < MAX_ENEMIES; i++) {
        if (global_manager->enemies[i].active) {
          rats_alive = true;
          break;
        }
      }
      if (!rats_alive && !global_manager->dialog->active) {
        global_manager->intro_step = 1;
        static const char* l5[] = {
            "Strange... These rats are really big.",
            "I guess that crazy guy wasn't so crazy after all."};
        dialog_show(global_manager->dialog, "Johnny", l5, 2);
      }
    }
  } else if (global_manager->current_level == 6) {
    if (global_manager->intro_step == 0) {
      global_manager->intro_step = 1;
      global_manager->lives = 6;
      static const char* l6a[] = {
          "Hey! Hey! You've seen those rats, I know it!",
          "The government is releasing them to hide something down in the "
          "tunnels!"};
      dialog_show(global_manager->dialog, "Crazy Gal", l6a, 2);
    } else if (global_manager->intro_step == 1 &&
               !global_manager->dialog->active) {
      global_manager->intro_step = 2;
      static const char* l6b[] = {
          "Sure lady. And I'm here 'cause I love it here."};
      dialog_show(global_manager->dialog, "Johnny", l6b, 1);
    } else if (global_manager->intro_step == 2 &&
               !global_manager->dialog->active) {
      global_manager->intro_step = 3;
      static const char* l6c[] = {"I know I'm right!"};
      dialog_show(global_manager->dialog, "Crazy Gal", l6c, 1);
    }
  } else if (global_manager->current_level == 7) {
    if (global_manager->intro_step == 0) {
      global_manager->intro_step = 1;
      static const char* l7[] = {
          "Man, people just keep getting crazier and crazier...."};
      dialog_show(global_manager->dialog, "Johnny", l7, 1);
    }
  } else if (global_manager->current_level == 8) {
    if (global_manager->intro_step == 0) {
      global_manager->intro_step = 1;
      static const char* l8a[] = {"What's that smell, smells like sh-"};
      dialog_show(global_manager->dialog, "Johnny", l8a, 1);
    } else if (global_manager->intro_step == 1 &&
               !global_manager->dialog->active) {
      global_manager->intro_step = 2;
      static const char* l8b[] = {"WHAT THE HELL IS THAT"};
      dialog_show(global_manager->dialog, "Johnny", l8b, 1);
    } else if (global_manager->intro_step == 2) {
      // Check if all enemies are dead for post-kill dialogue
      bool rats_alive = false;
      for (int i = 0; i < MAX_ENEMIES; i++) {
        if (global_manager->enemies[i].active) {
          rats_alive = true;
          break;
        }
      }
      if (!rats_alive && !global_manager->dialog->active) {
        global_manager->intro_step = 3;
        static const char* l8c[] = {"Ok seriously... What is going on?"};
        dialog_show(global_manager->dialog, "Johnny", l8c, 1);
      }
    }
  } else if (global_manager->current_level == 9) {
    if (global_manager->intro_step == 0) {
      global_manager->intro_step = 1;
      global_manager->lives = 6;
      static const char* l9[] = {
          "Aliens are real! They're real and they're sending out",
          "their army of alien rats!"};
      dialog_show(global_manager->dialog, "Crazy Guy", l9, 2);
    }
  } else if (global_manager->current_level == 10) {
    if (!global_manager->boss_active) {
      if (global_manager->intro_step == 0) {
        global_manager->intro_step = 1;
        static const char* b1[] = {"What the... what is that?", "Is that... green goo?"};
        dialog_show(global_manager->dialog, "Johnny", b1, 2);
      } else if (global_manager->intro_step == 1 && !global_manager->dialog->active) {
        global_manager->intro_step = 2;
        static const char* b2[] = {"Who are you?"};
        dialog_show(global_manager->dialog, "The Rat King", b2, 1);
      } else if (global_manager->intro_step == 2 && !global_manager->dialog->active) {
        global_manager->intro_step = 3;
        static const char* b3[] = {"I'm just a subway maintenance worker.", "More importantly, how the hell can you talk?"};
        dialog_show(global_manager->dialog, "Johnny", b3, 2);
      } else if (global_manager->intro_step == 3 && !global_manager->dialog->active) {
        global_manager->intro_step = 4;
        static const char* b4[] = {"Uhh. Dw about it. But wait a second...", "If you're a maintenance worker, that means you're the one who's been fighting my army!"};
        dialog_show(global_manager->dialog, "The Rat King", b4, 2);
      } else if (global_manager->intro_step == 4 && !global_manager->dialog->active) {
        global_manager->intro_step = 5;
        static const char* b5[] = {"Hey man, I'm just doin my job and trying to get home."};
        dialog_show(global_manager->dialog, "Johnny", b5, 1);
      } else if (global_manager->intro_step == 5 && !global_manager->dialog->active) {
        global_manager->intro_step = 6;
        static const char* b6[] = {"You kill my rats, I kill you!"};
        dialog_show(global_manager->dialog, "The Rat King", b6, 1);
      } else if (global_manager->intro_step == 6 && !global_manager->dialog->active) {
        global_manager->intro_step = 10;
        global_manager->boss_active = true;
        global_manager->boss_hp = 300;
        global_manager->boss_stage = 1;
      }
    } else {
      if (global_manager->boss_hp <= 0 && global_manager->boss_stage == 3) {
        if (global_manager->intro_step == 30) {
           global_manager->intro_step = 31;
           static const char* dead1[] = {"You filthy, disgusting, rodents don't belong here!"};
           dialog_show(global_manager->dialog, "Johnny", dead1, 1);
        } else if (global_manager->intro_step == 31 && !global_manager->dialog->active) {
           global_manager->boss_stage = 4;
           global_manager->intro_step = 32;
           static const char* dead2[] = {"Is... Is that Jeff, my roommate?", "Welp, there's no way my boss is gonna gimme overtime...."};
           dialog_show(global_manager->dialog, "Johnny", dead2, 2);
        } else if (global_manager->intro_step == 32 && !global_manager->dialog->active) {
           global_manager->current_level = 11;
           tilemap_load_level(11);
        }
      } else if (global_manager->boss_hp <= 30 && global_manager->boss_stage == 3) {
        if (global_manager->intro_step == 20) {
           global_manager->intro_step = 21;
           static const char* stage3_p[] = {"What have we ever done to you? My kind has just been trying to live!"};
           dialog_show(global_manager->dialog, "The Rat King", stage3_p, 1);
        } else if (global_manager->intro_step == 21 && !global_manager->dialog->active) {
           global_manager->intro_step = 30;
        }
      } else if (global_manager->boss_hp < 100 && global_manager->boss_stage == 2) {
        if (global_manager->intro_step == 14) {
           global_manager->intro_step = 15;
           static const char* q2[] = {"What's the serum for?"};
           dialog_show(global_manager->dialog, "Johnny", q2, 1);
        } else if (global_manager->intro_step == 15 && !global_manager->dialog->active) {
           global_manager->intro_step = 16;
           static const char* ans2[] = {"My kind has been pushed around long enough, it's time for us to rise!"};
           dialog_show(global_manager->dialog, "The Rat King", ans2, 1);
        } else if (global_manager->intro_step == 16 && !global_manager->dialog->active) {
           global_manager->intro_step = 20;
           global_manager->boss_stage = 3;
           int rats_summoned = 0;
           for (int i = 0; i < MAX_ENEMIES && rats_summoned < 10; i++) {
             struct enemy* elem = &global_manager->enemies[i];
             if (elem->active == true) continue;
             rats_summoned++;
             elem->pos.x = (3 + rats_summoned * 1.25) * TILE_SIZE;
             elem->pos.y = 4 * TILE_SIZE;
             elem->active = true;
             elem->health = 1;
             elem->type = 2;
           }
        }
      } else if (global_manager->boss_hp < 200 && global_manager->boss_stage == 1) {
        if (global_manager->intro_step == 10) {
           global_manager->intro_step = 11;
           static const char* q1[] = {"What's the green goo?"};
           dialog_show(global_manager->dialog, "Johnny", q1, 1);
        } else if (global_manager->intro_step == 11 && !global_manager->dialog->active) {
           global_manager->intro_step = 12;
           static const char* ans1[] = {"It's not green goo, it's a specially formulated growth serum."};
           dialog_show(global_manager->dialog, "The Rat King", ans1, 1);
        } else if (global_manager->intro_step == 12 && !global_manager->dialog->active) {
           global_manager->intro_step = 14;
           global_manager->boss_stage = 2;
        }
      }
    }
  }
}

static void check_level_transition() {
  if (global_manager->player->pos.x > (MAP_WIDTH * TILE_SIZE) - 70) {
    // Check if all enemies are defeated
    bool all_enemies_dead = true;
    if (global_manager->current_level != 0 &&
        global_manager->current_level != 1 &&
        global_manager->current_level != 3 &&
        global_manager->current_level != 6 &&
        global_manager->current_level != 9 &&
        global_manager->current_level != 10) {
      for (int i = 0; i < MAX_ENEMIES; i++) {
        if (global_manager->enemies[i].active) {
          all_enemies_dead = false;
          break;
        }
      }
    } else {
      // NPC levels (3,6,9), intro (0), tutorial (1), boss (10) have no rats
      all_enemies_dead = true;
    }

    if (all_enemies_dead) {
      if (global_manager->current_level < LEVELS_AMOUNT - 1) {
        global_manager->current_level++;
        tilemap_load_level(global_manager->current_level);
        global_manager->player->pos.x = TILE_SIZE;
        global_manager->intro_step = 0;
        // spawn enemies and money for new level
        enemies_spawn(global_manager->enemies,
                      levels[global_manager->current_level].rat_spawns);

        // Random coin spawn on walkable tiles
        for (int i = 0; i < 5; i++) {
          int cx, cy;
          do {
            cx = (rand() % (levels[global_manager->current_level].width - 4)) +
                 2;
            cy = (rand() % (levels[global_manager->current_level].height - 4)) +
                 2;
          } while (!isWalkable(cx, cy));
          global_manager->money_items[i].pos =
              (Vector2){cx * TILE_SIZE, cy * TILE_SIZE};
          global_manager->money_items[i].active = true;
        }
      }
      if (global_manager->current_level == 10) {
        global_manager->boss_hp = 300;
      }
    } else {
      // Prevent player from exiting the screen if enemies are alive
      global_manager->player->pos.x = (MAP_WIDTH - 1) * TILE_SIZE - 64;
    }
  }
}

static void update_collisions_and_entities(float dt) {
  Vector2 playerPos = {global_manager->player->pos.x + 32,
                       global_manager->player->pos.y + 32};
  if (global_manager->current_level != 0) {
    enemies_update(global_manager->enemies, playerPos, dt,
                   global_manager->dialog->active);
  }

  // Walk-over coin pickup
  Vector2 playerCenter = {global_manager->player->pos.x + 32,
                          global_manager->player->pos.y + 32};
  for (int i = 0; i < MAX_MONEY; i++) {
    if (!global_manager->money_items[i].active) continue;
    if (Vector2Distance(playerCenter, global_manager->money_items[i].pos) <
        40.0f) {
      global_manager->money += 10;
      global_manager->money_items[i].active = false;
      static const char* pickup_lines[] = {"Don't mind if I do..."};
      dialog_show(global_manager->dialog, "Johnny", pickup_lines, 1);
    }
  }

  for (int i = 0; i < MAX_BULLETS; i++) {
    if (!global_manager->bullets[i].active) continue;
    bullet_update(&global_manager->bullets[i], dt);

    if (global_manager->current_level == 10 && !global_manager->dialog->active) {
      if (CheckCollisionCircles(
              global_manager->bullets[i].pos, BULLET_RADIUS,
              (struct Vector2){19 * TILE_SIZE, 10 * TILE_SIZE},
              ENEMY_RADIUS * 4)) {
        global_manager->bullets[i].active = false;
        global_manager->boss_hp -= 10;
        continue;
      }
    }

    // Check collision with enemies (bullets)
    if (!global_manager->dialog->active) {
      for (int j = 0; j < MAX_ENEMIES; j++) {
        if (!global_manager->enemies[j].active) continue;

        Vector2 enemyCenter = (Vector2){global_manager->enemies[j].pos.x + 8,
                                        global_manager->enemies[j].pos.y + 8};

        if (CheckCollisionCircles(global_manager->bullets[i].pos, BULLET_RADIUS,
                                  enemyCenter, ENEMY_RADIUS)) {
          global_manager->bullets[i].active = false;
          global_manager->enemies[j].health -= 10;
          if (global_manager->enemies[j].health <= 0) {
            global_manager->enemies[j].active = false;
          }
          break;
        }
      }
    }

    int tx = (int)(global_manager->bullets[i].pos.x / TILE_SIZE);
    int ty = (int)(global_manager->bullets[i].pos.y / TILE_SIZE);
    if (!isWalkable(tx, ty)) {
      global_manager->bullets[i].active = false;
    }
  }

  // Check collision player vs enemies (Rat Bites)
  bool touching_enemy = false;
  int enemy_damage = 0;

  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (!global_manager->enemies[i].active) continue;

    Vector2 enemyCenter = (Vector2){global_manager->enemies[i].pos.x + 8,
                                    global_manager->enemies[i].pos.y + 8};

    if (CheckCollisionCircles(playerPos, 24.0f, enemyCenter, ENEMY_RADIUS)) {
      touching_enemy = true;
      // Stage 1 (Type 1) = damage 1 (half-heart)
      // Stage 2/3 (Type 2, 3) = damage 2 (full heart)
      if (global_manager->enemies[i].type == 1) {
        enemy_damage = 1;
      } else {
        enemy_damage = 2;
      }
      break;
    }
  }

  if (touching_enemy && global_manager->player_invincibility_timer <= 0) {
    global_manager->contact_timer += dt;
    if (global_manager->contact_timer >= 0.4f) {
      global_manager->lives -= enemy_damage;
      global_manager->player_invincibility_timer = 2.0f;
      global_manager->contact_timer = 0.0f;
      if (global_manager->lives <= 0) {
        // Player death logic
        global_manager->lives = 0;
        global_manager->current_level = 12;
        tilemap_load_level(12);
      }
    }
  } else {
    global_manager->contact_timer = 0.0f;
  }
}

void manager_run_game() {
  while (global_manager->game_should_run) {
    float dt = GetFrameTime();

    // Updates
    controller_tick();
    dialog_update(global_manager->dialog, dt);
    UpdateMusicStream(global_manager->bgm);

    // Systems
    update_level_script(dt);
    check_level_transition();
    update_collisions_and_entities(dt);

    // Render
    render_game();
  }
}

struct manager* manager_get_global() { return global_manager; }
