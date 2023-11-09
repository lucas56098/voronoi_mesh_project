#include "VoronoiCell.h"
#include "Point.h"
#include "Halfplane.h"
using namespace std;
#include <iostream>

VoronoiCell::VoronoiCell() {}

VoronoiCell::VoronoiCell(Point inseed, Point* in_other_points, int N_pts) {
    seed = inseed;
    other_points = in_other_points;
    N = N_pts;
}

VoronoiCell::~VoronoiCell() {}

// intersect two halfplanes, add that intersection to the first halfplane
void VoronoiCell::intersect_two_halfplanes(Halfplane &hp1, Halfplane &hp2) {
    
    // for the two halfplanes get vector representation
    Point midpoint1 = hp1.get_midpoint();
    Point midpoint2 = hp2.get_midpoint();

    Point hp_vec_1 = hp1.get_half_plane_vec();
    Point hp_vec_2 = hp2.get_half_plane_vec();

    // solve linear system of equations for the two lines
    //calculate Determinant D
    float D = hp_vec_1.x * hp_vec_2.y - hp_vec_1.y * hp_vec_2.x;
    //calculate Dx
    float Dx = (midpoint2.x - midpoint1.x) * hp_vec_2.y - 
                (midpoint2.y -  midpoint1.y) * hp_vec_2.x;
    //calculate Dy
    float Dy = hp_vec_1.x * (midpoint2.y -  midpoint1.y) -
                hp_vec_1.y * (midpoint2.x - midpoint1.x);

    float x;
    float y;

    // if D !=0 -> exact solution x = Dx/D, y = Dy/D
    if (D != 0) {
        x = Dx/D;
        y = Dy/D;
        
        // calculate intersection
        float intersect_pt_x = midpoint1.x + x*hp_vec_1.x;
        float intersect_pt_y = midpoint1.y + x*hp_vec_1.y;

        // add intersection to hp1
        intersection intersect;
        intersect.intersect_pt = Point(intersect_pt_x, intersect_pt_y);
        intersect.dist_to_midpoint = x;
        intersect.intersecting_with = &hp2;

        hp1.intersections.push_back(intersect);
    }
    // if D = 0 -> check if Dx and Dy are 0 -> if: infinite sol, not: no sol
    else if (D == 0)
    {
        if (Dx == 0 && Dy == 0) {
            cout << "infinite solutions: that shouldnt happen" << endl;
        } else if (!(hp1.boundary && hp2.boundary)) {
            cout << "no solution while trying to intersect two halfplanes" << endl;
        }
    }
    
    
}

// intersect all halfplanes in each possible way (n^2 options)
void VoronoiCell::intersect_all_halfplanes() {
    // intersect each halfplane with each other
    // !! all halfplanes need to be generated first in construct cell
    for (int i = 0; i<N+3; i++) {
        for (int j = 0; j<N+3; j++) {
            if (i != j) {
                intersect_two_halfplanes(halfplanes[i], halfplanes[j]);
            }
        }
    }
}

void VoronoiCell::construct_cell() {
    // generate all halfplanes + boundary halfplanes
    halfplanes.push_back(Halfplane(Point(0.5,0.5), Point(0.5,1.5), N, true));
    halfplanes.push_back(Halfplane(Point(0.5,0.5), Point(1.5, 0.5), N, true));
    halfplanes.push_back(Halfplane(Point(0.5,0.5), Point(0.5,-0.5), N, true));
    halfplanes.push_back(Halfplane(Point(0.5,0.5), Point(-0.5,0.5), N, true));

    for (int i = 0; i<N-1; i++) {
        halfplanes.push_back(Halfplane(seed, other_points[i], N));
    }

    // do intersect all halfplanes
    intersect_all_halfplanes();

    for (int i = 0; i<N+3; i++) {
        cout << halfplanes[i].boundary << endl;
        for (int j = 0; j< halfplanes[i].intersections.size(); j++) {
            cout << halfplanes[i].intersections[j].intersect_pt.x << ":";
            cout << halfplanes[i].intersections[j].intersect_pt.y << "  ->";
            cout << (*halfplanes[i].intersections[j].intersecting_with).seed2.x;
            cout << ":";
            cout << (*halfplanes[i].intersections[j].intersecting_with).seed2.y;
            cout << endl;
        }
    }
    
    // search for closest point
    // start in positive direction to first intersection + save edge + verticies
    // change to next halfplane + save edge
    // until next intersection == first intersection

}