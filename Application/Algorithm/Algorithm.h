#pragma once

#include "AStar/AStar.h"
#include "QuadTree/QuadTree.h"

struct Algorithm
{
	// 핵심 알고리즘: 쿼드 트리
	static AStar aStar;

	// 핵심 알고리즘: A*
	static QuadTree quadTree;
};