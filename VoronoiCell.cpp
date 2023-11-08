#include "VoronoiCell.h"
#include "Point.h"
#include "Halfplane.h"
using namespace std;
#include <iostream>

VoronoiCell::VoronoiCell() {}

VoronoiCell::VoronoiCell(Point inseed, Point* in_other_points) {
    seed = inseed;
    other_points = in_other_points;
}

VoronoiCell::~VoronoiCell() {}

// intersect two halfplanes, add that intersection to the first halfplane
void VoronoiCell::intersect_two_halfplanes(Halfplane &hp1, Halfplane &hp2) {
    
    // for the two halfplanes get vector representation
    Point midpoint1 = hp1.get_midpoint();
    Point midpoint2 = hp2.get_midpoint();

    Point hp_vec_1 = hp1.get_half_plane_vec();
    Point hp_vec_2 = hp2.get_half_plane_vec();

    //calculate Determinant D
    float D = hp_vec_1.x * hp_vec_2.y - hp_vec_1.y * hp_vec_2.x;
    cout << "D = " << D << endl;
    //calculate Dx
    float Dx = (midpoint2.x - midpoint1.x) * hp_vec_2.y - 
                (midpoint2.y -  midpoint1.y) * hp_vec_2.x;
    cout << "Dx = " <<  Dx << endl;
    //calculate Dy
    float Dy = hp_vec_1.x * (midpoint2.y -  midpoint1.y) -
                hp_vec_1.y * (midpoint2.x - midpoint1.x);
    cout << "Dy = " << Dy << endl;

    float x;
    float y;

    // if D !=0 -> exact solution x = Dx/D, y = Dy/D
    if (D != 0) {
        x = Dx/D;
        y = Dy/D;
    }
    // if D = 0 -> check if Dx and Dy are 0 -> if: infinite sol, not: no sol
    else if (D == 0)
    {
        if (Dx == 0 && Dy == 0) {
            cout << "infinite solutions" << endl;
        } else {
            cout << "no solution" << endl;
        }
    }
    
    cout << x << endl;
    cout << y << endl;

    float intersect_pt_x = midpoint1.x + x*hp_vec_1.x;
    float intersect_pt_y = midpoint1.y + x*hp_vec_1.y;

    cout << intersect_pt_x << ":" << intersect_pt_y << endl;


    // add intersection to hp1
    intersection intersect;
    intersect.intersect_pt = Point(intersect_pt_x, intersect_pt_y);
    intersect.dist_to_midpoint = x;
    intersect.intersecting_with = &hp2;

    hp1.intersections.push_back(intersect);
}

// intersect all halfplanes in each possible way (n^2 options)
void VoronoiCell::intersect_all_halfplanes() {}

void VoronoiCell::construct_cell() {}