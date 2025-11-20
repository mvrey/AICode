/*
* File:   Crate.h
* Author: Marcos Vazquez
*
* Created on May 8, 2017, 19:58 PM
*
*/

#ifndef CRATE_H
#define CRATE_H

#include "config.h"


class Crate {
public:
	Crate::Crate() { img_ = MOMOS::SpriteFromFile("data/crate.png"); };
	Crate::~Crate() { MOMOS::SpriteRelease(img_); };

	MOMOS::SpriteHandle img_;
	//In screen coordinates
	MOMOS::Vec2 pos_;
};

#endif /* CRATE_H */