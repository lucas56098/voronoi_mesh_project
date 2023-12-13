#include <iostream>
#include <vector>
#include <algorithm>
#include "Point.h"
using namespace std;

int main() {

    vector<int> indices;
    vector<int> objects;
    vector<Point> pts;

    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(3);
    indices.push_back(0);

    objects.push_back(0);
    objects.push_back(1);
    objects.push_back(2);
    objects.push_back(3);

    pts.push_back(Point(3,3));
    pts.push_back(Point(2,2));
    pts.push_back(Point(4,4));
    pts.push_back(Point(1,1));

    vector<pair<int, int> > combined;

    for (int i = 0; i < indices.size(); ++i) {
        combined.push_back(make_pair(indices[i], objects[i]));
    }

    cout << combined[0].first << "   " << combined[0].second << endl;
    cout << combined[1].first << "   " << combined[1].second << endl;
    cout << combined[2].first << "   " << combined[2].second << endl;
    cout << combined[3].first << "   " << combined[3].second << endl;

    sort(combined.begin(), combined.end());

    cout << combined[0].first << "   " << combined[0].second << endl;
    cout << combined[1].first << "   " << combined[1].second << endl;
    cout << combined[2].first << "   " << combined[2].second << endl;
    cout << combined[3].first << "   " << combined[3].second << endl;

    vector<Point> sorted_pts;

    for (int i = 0; i < combined.size(); i++) {
        sorted_pts.push_back(pts[combined[i].second]);
    }

    for (int i = 0; i < pts.size(); i++) {
        cout << pts[i].x << ", " << pts[i].y << endl;
    }

    for (int i = 0; i < sorted_pts.size(); i++) {
        cout << sorted_pts[i].x << ", " << sorted_pts[i].y << endl;
    }

    return 0;
}
