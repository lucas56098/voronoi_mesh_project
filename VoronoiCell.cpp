#include "VoronoiCell.h"
#include "Point.h"
#include "Halfplane.h"
//using namespace std;
#include <iostream>
#include <cmath>


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
        } else if (!(hp1.boundary || hp2.boundary)) {
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
// generate all halfplanes + boundary halfplanes
void VoronoiCell::generate_halfplane_vector() {
    
    // generate boundary halfplanes
    halfplanes.push_back(Halfplane(Point(0.5,0.5), Point(0.5,1.5), N, true));
    halfplanes.push_back(Halfplane(Point(0.5,0.5), Point(1.5, 0.5), N, true));
    halfplanes.push_back(Halfplane(Point(0.5,0.5), Point(0.5,-0.5), N, true));
    halfplanes.push_back(Halfplane(Point(0.5,0.5), Point(-0.5,0.5), N, true));

    // generate usual halfplanes
    for (int i = 0; i<N-1; i++) {
        halfplanes.push_back(Halfplane(seed, other_points[i], N));
    }
}

void VoronoiCell::search_hp_closest_to_seed(Halfplane &first_hp) {
    // search for closest point
    float dist_min = 42;

    for (int i = 0; i<N+3; i++) {
        float dist = sqrt((seed.x - halfplanes[i].seed2.x)*(seed.x - halfplanes[i].seed2.x)+
                            (seed.y - halfplanes[i].seed2.y) * (seed.y - halfplanes[i].seed2.y));
        if (dist_min > dist) {
            dist_min = dist;
            first_hp = halfplanes[i];
        }
    }
}

float VoronoiCell::get_signed_angle(Point u, Point v) {
    float dotp = u.x * v.x + u.y * v.y;
    float crossp = v.x*u.y - v.y*u.x;
    float u_abs = sqrt(u.x*u.x + u.y*u.y);
    float v_abs = sqrt(v.x*v.x + v.y*v.y);
    float angle;

    if (dotp >= 0) {
        angle = asin(crossp/(u_abs * v_abs));
    } else {
        angle = (M_PI/2 * crossp/fabs(crossp)) - asin(crossp/(u_abs * v_abs));
    }

    return angle;
}

void VoronoiCell::construct_cell() {

    // generate and intersect all halfplanes
    generate_halfplane_vector();
    intersect_all_halfplanes();

    // set first and current_hp to hp closest to seed
    Halfplane current_hp;
    search_hp_closest_to_seed(current_hp);
    Halfplane first_hp = current_hp;
    Point last_vertex_seed_2 = Point(42,42);

    // intitalize last_vertex  for the first time
    Point last_vertex = current_hp.get_midpoint();

    // step by step generate voronoi cell
    do {

    // determine signed distance between last vertex and current hp_midpoint
    float last_vertex_dist_to_midpoint = (last_vertex.x - current_hp.get_midpoint().x)*current_hp.get_half_plane_vec().x
                                        + (last_vertex.y - current_hp.get_midpoint().y)*current_hp.get_half_plane_vec().y;

    float smallest_pos_dist = 42;
    Halfplane next_hp;
    Point vertex;
    bool need_to_check_for_degeneracy = false;

    // find intersection with smallest positive signed distance to last_vertex
    for (int i = 0; i<current_hp.intersections.size(); i++) {

        // calculate signed relative distance
        float rel_dist =  current_hp.intersections[i].dist_to_midpoint - last_vertex_dist_to_midpoint;

        bool same_vertex = (((*current_hp.intersections[i].intersecting_with).seed2.x == last_vertex_seed_2.x) && 
                                (*current_hp.intersections[i].intersecting_with).seed2.y == last_vertex_seed_2.y);

        // if distance <= smallest_pos_distance and positive replace intersection with closer one
        if (rel_dist <= smallest_pos_dist && rel_dist > 0 && !same_vertex) {
            // check if there could be a degenerate case
            if (rel_dist == smallest_pos_dist) {
                need_to_check_for_degeneracy = true;
            }
            // update nearest intersection candidate
            smallest_pos_dist = rel_dist;
            next_hp = *current_hp.intersections[i].intersecting_with;
            vertex = current_hp.intersections[i].intersect_pt;
        }
    }
    // if degeneracy possible check for it and choose the correct halfplane
    if (need_to_check_for_degeneracy) {
        vector<Halfplane> deg_hp_list;
        
        for (int i = 0; i<current_hp.intersections.size(); i++) {
            
            // calculate signed relative distance
            float rel_dist =  current_hp.intersections[i].dist_to_midpoint - last_vertex_dist_to_midpoint;
            
            // put degenerate cases into vector
            if (rel_dist == smallest_pos_dist) {
                deg_hp_list.push_back(*current_hp.intersections[i].intersecting_with);
            }

        }

        // calculate the signed angle between current_hp vec and deg_hp_list vec
        float max_angle = 0;

        for (int i = 0; i<deg_hp_list.size(); i++) {
            float angle = get_signed_angle(current_hp.get_half_plane_vec(), deg_hp_list[i].get_half_plane_vec());
            
            // choose the hp with highest signed angle as the next hp
            if (angle > max_angle) {
                max_angle = angle;
                next_hp = deg_hp_list[i];
            }
        }
    }
    
    // when done save current halfplane as edge and next closest intersection as vertex
    edges.push_back(current_hp);
    verticies.push_back(vertex);

    last_vertex_seed_2 = Point(current_hp.seed2.x, current_hp.seed2.y);

    // update last vertex and current hp
    last_vertex = vertex;
    current_hp = next_hp;

    // continue with the steps above until the half plane is the same as the first one
    } while (!(first_hp.seed2.x == current_hp.seed2.x && first_hp.seed2.y == current_hp.seed2.y));

}

void VoronoiCell::print_cell() {
    // print out edges and vertices of cell
    cout << "Cell parameters:" << endl;
    cout << "Seed1: " << seed.x << ":" << seed.y << endl;

    for (int i = 0; i < edges.size(); i++) {
        cout << "Edge Seed2: " << edges[i].seed2.x << ":" << edges[i].seed2.y << endl;
        cout << "Vertex: " << verticies[i].x << ":" << verticies[i].y << endl;
    }
}