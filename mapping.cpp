#include "mapping.h"
#include "mainwindow.h"

Mapping::Mapping(QMainWindow *parrent) {
    this->par = parrent;
    this->init();
}


void Mapping::init() {
    for (int i = 0; i < MAP_WIDTH/5; i++) {
        for (int j = 0; i < MAP_HEIGHT/5; j++) {
            map[i][j] = 0;
        }
    }
}


void Mapping::update_map() {

    double fi, d;
    double x, y;
    // ideme point_count/2 lebo v scan_array su pary [uhol vzdialenost]
    for (int i = 0; i <  10; i++) {
    }
}


