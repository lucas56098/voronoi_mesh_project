#include "Halfplane.h"

Halfplane::Halfplane() {}

Halfplane::Halfplane(Point inseed1, Point inseed2, int N_pts) {
    seed1 = inseed1;
    seed2 = inseed2;
    N = N_pts;
    intersections = new intersection[N];
}

Halfplane::~Halfplane() {}