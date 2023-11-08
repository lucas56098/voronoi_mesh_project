#include "Halfplane.h"
#include <vector>
#include <cmath>
using namespace std;

Halfplane::Halfplane() {}

Halfplane::Halfplane(Point inseed1, Point inseed2, int N_pts) {
    seed1 = inseed1;
    seed2 = inseed2;
    N = N_pts;
    boundary = false;
}

Halfplane::Halfplane(Point inseed1, Point inseed2, int N_pts, bool is_boundary) {
    seed1 = inseed1;
    seed2 = inseed2;
    N = N_pts;
    boundary = is_boundary;
}

Halfplane::~Halfplane() {}

// returns normalized vector orthogonal to vector seed2-seed1
Point Halfplane::get_half_plane_vec() {

    float hp_vec_x = seed2.y - seed1.y;
    float hp_vec_y = - (seed2.x - seed1.x);

    float norm = sqrt(hp_vec_x * hp_vec_x + hp_vec_y * hp_vec_y);

    return Point(hp_vec_x/norm, hp_vec_y/norm);
}

// reutrns midpoint between the two seeds
Point Halfplane::get_midpoint() {
    
    float x_mid = 0.5 * (seed1.x + seed2.x);
    float y_mid = 0.5 * (seed1.y + seed2.y);

    return Point(x_mid, y_mid);
}