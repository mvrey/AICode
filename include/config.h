/* 
 * File:   config.h
 * Author: Marcos Vazquez
 *
 * Created on November 30, 2016, 1:54 PM
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <vector>
#include <MOMOS/window.h>
#include <MOMOS/draw.h>
#include <MOMOS/input.h>
#include <MOMOS/time.h>
#include <MOMOS/math.h>
#include <MOMOS/sprite.h>

class Manager;
class Agent;

typedef const struct {
  static const int width = 1600;
  static const int height = 900;
  static const int padding = 20;
} Screen;

typedef enum {
  kAlive,
  kDead
} AliveStatus;

typedef enum {
  kNil,
  kYes,
  kNo
} Response;

//Holds syntax for inter-agent messages and objectives/desires
typedef struct {
  Agent* sender;
  Agent* receiver;
  ::MOMOS::Vec2 pos;
  Response res;
} Message;

typedef enum {
	kGoUp,
	kGoDown,
	kGoLeft,
	kGoRight,
	kStop
} PatternStep;

typedef enum {
	kSlept,
	kDeterministic,
	kRandom,
	kTracking,
	kPattern
} MovementType;

typedef enum {
	kIdle,
	kGoingToWork,
	kWorkingLoaded,
	kWorkingUnloaded,
	kGoingToRest,
	kResting,
	kEscaping
} PrisonerStatus;


typedef enum {
	kNormal,
	kSuspect,
	kAlert
} GuardStatus;


typedef enum {
	kStandard,
	kRunning
} SoldierStatus;


typedef enum {
	kNONE,
	kRiver,
	kBridge,
	kGround,
	kBuilding,
	kCourtyard,
	kYard,
	kBase,
	kDoor,
	kWall
} PrisonAreaType;

#endif /* CONFIG_H */