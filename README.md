# Compilation and Dependencies

This project depends on the MOMOS2D graphics engine, whose code can be downloaded from https://github.com/mvrey/MOMOS2D. The library located at https://github.com/mvrey/MOMOS2D/blob/2025/build/windows/debug/Momos2D.lib must be included.

For operating systems other than Windows x64, or for release builds, the engine code must be compiled manually.


# 1. Project Description

This demo implements a series of agents that act independently and interact with each other. The bodies of these agents are represented by different sprites from the video game _Prison Architect_.

Several types of agents with diverse objectives and behaviors, as well as the ability to interact with the environment, are implemented. These will be detailed in the following section.

A limit has been implemented on the AI's computation time per frame. If the AI's calculations become too intensive, or if there is no remaining time in a frame due to other prior calculations, the program will save the position of the last updated agent and continue updating them from there in the next frame.

For more comprehensive documentation on the implementation of the algorithms used, please consult the source code. Descriptions of functions and attributes are generally found in the headers, and specific implementation details are found within the class and method implementations themselves.


# 2. Agents

The agents implemented in this demo exhibit movement and behavior patterns based on finite state machines. They move using pathfinding algorithms (A*), as well as other deterministic movement patterns and tracking.

Each agent has a set of different states and specific conditions for transitioning between them.

Movement across the map is achieved by combining the A* algorithm with a series of waypoints for common paths.


2.1 Guard

The guard patrols the interior areas of the prison using waypoints for common paths and A* for longer, more complex, or unsearched distances.

The guard has a cone of vision that collides with the impassable areas defined on the cost map, preventing him from seeing through walls, but allowing him to detect doors and other types of agents (prisoners and soldiers).

In its normal state, the guard patrols the interior areas of the prison and is able to detect open doors and soldiers within its field of vision.

If the guard spots a soldier, it will enter suspect mode and pursue the soldier indefinitely using tracking. The fact that the field of vision does not penetrate walls allows the guard to avoid passing through walls while pursuing a potential enemy on the other side.

If the guard detects an open door, it enters alert mode, moves toward the door, and raises the alarm. Note that a door will close when the guard is closer than its detection range, provided there are no soldiers nearby.

If the alarm has been raised, all guards will immediately enter alert mode and remain in this state until the alarm is deactivated. The alarm will turn off when no guard has spotted an open door or a soldier for more than T.

While on alert, the guard will patrol the prison as usual and pursue any soldier or prisoner indefinitely, until the alarm turns off or they escape through the lower part of the map.


### FSM

List of State Transitions:

**Normal - Suspect**

The guard has seen a soldier.

**Suspect - Normal**

The guard has not seen any soldiers in T time.

**Normal - Alert**

The guard has closed a door.

**Alert - Normal**

The guard has not seen any soldiers or an open door in T time.

**Suspect - Alert**

The guard has closed a door while chasing a soldier.


2.2 Prisoner

The prisoner performs a series of cyclical movements between the work area and the rest area. These movements mostly use waypoints and very short deterministic movements.

The prisoners transport boxes from the loading area to the unloading area. These boxes are their own persistent entities in the demo.

At the start of the demo, the prisoner is assigned a rest or work shift depending on whether their assigned shift coincides with the current one in the prison.

The prisoner moves along predetermined waypoints to their assigned area, and upon arrival, their state changes.

A prisoner resting walks around the rest area, moving to randomly assigned fixed points within it (assigned as deterministic movement with a single point).

A prisoner working cycles between Loaded and Unloaded as they move from the loading to the unloading area and transport boxes in the process.

When the work shift changes, prisoners resting or going to rest will become going to work. Similarly, prisoners working or going to work will become going to rest.

If an alarm is triggered in the prison, the prisoner will transition from any of the other states to Escaping. In this state, they will move from door to door until they find an open one. If this occurs, a path will be calculated to the escape base located at the bottom of the map, and the prisoner will disappear upon arrival.

If a prisoner is escaping and the alarm is turned off, they will complete their current movement route and then return to work or rest according to their assigned work shift.


### FSM

List of State Transitions:

**Idle - GoingToRest/GoingToWork**

Set at the start of the simulation according to the prisoner's current work shift. Prisoners whose assigned shift is active will go to the work area, while the rest will go to the rest area.

**GoingToRest - Resting**

The prisoner has arrived at the rest area.

**GoingToRest/Resting - GoingToWork**

The prisoner's work shift has been activated.

**GoingToWork - WorkingUnloaded**

The prisoner has arrived at the loading area.

**WorkingUnloaded - WorkingLoaded**

The prisoner has arrived at the unloading zone.

**ANY - Escaping**

The prison alarm has been triggered. This state can be reached from any other state.

**Escaping - GoingToRest/GoingToWork**

The base alarm has been deactivated, and the prisoner has completed their current movement path (towards a door or towards escape).


2.3 Soldier

Soldiers depart from the lower left base on the map and immediately head towards the right-hand gate of the prison.

Soldiers will open any door within their range, and that door will remain locked for a short time (no guard can close it).

Under normal circumstances, the soldier moves alternately between prison gates, opening them upon arrival.

If an alarm is triggered at the base, the soldier will complete their current movement path and then flee back to the lower base on the map.

### FSM

State Transition List:

**Standard - Running**

An alarm has been triggered at the base.

**Running - Standard**

The alarm has been deactivated, and the soldier has completed their current movement path.



## 2.4 Pathfinder

The Pathfinder agent maintains a queue of calculation requests A*, which the other agents fill. Pathfinder calculates these requests in strict order of arrival, with a maximum of one request per frame.

This agent must be updated first, before the other agents. Since it performs costly operations, this allows for a better estimation of the time available in the frame and the number of remaining agents that can be updated within it.



## 2.5 Messager

The messaging system between agents has not been implemented in this demo. A message manager agent is included, to which the other agents would send messages for redistribution.



# 3. Pathfinding

This demo uses various pathfinding systems, with A* being the most powerful and accurate, but also by far the most computationally expensive.

Several classes are involved in reducing the cost of pathfinding algorithms or specifying traversable sections and the costs associated with agent movement.

Many of these improvements are specific to the map on which the demo is developed and are manually specified in the code.


## 3.1 A*

The A* algorithm calculates a path given a starting and ending point on a cost map.


## 3.2 Cost Map

The cost map is loaded from an image of varying resolutions and specifies passable and impassable sections. It can also be used to specify the movement cost of different areas.


## 3.3 Path

The path is a simple vector of points that indicates the deterministic movement to follow to get from point A to point B.


## 3.4 PrisonMap

This class contains movement optimizations specific to the current map. Its values ​​should be adapted for use with a different prison design.


### Rooms

Several rooms with their respective corners have been saved. This is useful when you want to calculate paths between rooms regardless of the specific point of the destination room.

It is also used to determine if a point is located inside a room.


### Doors

List of doors present in the prison.


### Waypoints

Intermediate points to facilitate common movements. Several pre-calculated routes are established between pairs of rooms.



# 4. Other Classes

Some other classes to consider that do not fall under the previous categories.


## 4.1 Doors

Doors act as independent entities located in the scenario that modify the cost map at their corresponding position.

A door can be opened by a soldier, allowing the calculation of paths through its position. When a soldier opens a door, it cannot be closed in less than T.

Similarly, a door can be closed by a guard, marking its position as impassable on the cost map.


## 4.2 GameStatus

This singleton class contains global variables that must be accessed from various points in the program, as well as other variables related to the program's general execution that do not fall under any specific category (e.g., elapsed simulation time).