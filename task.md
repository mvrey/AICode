# Task

Implement a modular, extensible system for pawns to satisfy their needs through world interactions.

## Goal

Pawns should monitor their own needs, decide which need to satisfy, locate suitable world objects that restore that need, walk to them, interact for a defined duration, and have their need restored. System must be extendable for future needs and new world interaction objects.

## Context

You currently have pawns with needs and an FSM. Needs decay over time. Some world objects (e.g., plants, beds) restore specific needs. Pawns must choose the most urgent need, find a valid object, walk there, interact for several seconds, restore the need value, and cause the object to perform its effect (e.g., plant disappears).

The implementation should be clean, decoupled and open for extension.

## Architectural Plan

### 1. Needs System

* Each need is represented by an INeed interface/class.
* Need fields: id, value, decayRate, threshold.
* Need methods: tick(deltaTime), restore(amount).
* NeedsController manages all pawn needs and returns the most urgent one.

### 2. Need Providers

* World objects implement INeedProvider.
* Provider fields: needId, restoreAmount, useDuration.
* Methods: isAvailable(), onUsed(), getPosition().
* Providers register into ProviderRegistry.

### 3. Pawn Decision Layer

* PawnAI queries NeedsController for the most urgent need.
* Queries ProviderRegistry for providers matching that need.
* Selects nearest valid provider.
* Asks FSM to satisfy need using this provider.

### 4. Pawn FSM

* States: Idle, MoveToProvider, Working, Sleeping.
* MoveToProvider handles pathfinding.
* Working handles useDuration timer and calls provider.onUsed() and need.restore().
* Sleeping restores sleep over time.

### 5. Flow Example

* Hunger reaches threshold.
* AI picks closest plant.
* FSM moves pawn to plant.
* FSM enters Working for useDuration.
* Plant disappears, hunger restored.

## Requirements

* Code must remain modular and open for extension.
* Adding a new need requires minimal to no changes outside the Needs folder.
* Adding a new need provider must not require pawn code modifications.
* Pawns must never directly reference concrete provider types; all access should occur via interfaces.
* FSM should not contain decision-making logic.
* AI should not contain animation, timing, or movement logic.

## Output

Cursor should:

1. Create or update files to implement the described architecture.
2. Refactor existing needs and provider logic to match this pattern.
3. Implement provider registry.
4. Integrate the decision layer with the FSM.
5. Ensure the system compiles and functions.
