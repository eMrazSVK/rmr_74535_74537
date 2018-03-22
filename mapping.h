#ifndef MAPOVANIE_H
#define MAPOVANIE_H
#pragma once
#define MAP_WIDTH 1000
#define MAP_HEIGHT 800

#include <QMainWindow>
class Mapping {
    public:
    Mapping(QMainWindow *parrent);
    QMainWindow *par;
    private:
        int map[MAP_WIDTH/5][MAP_HEIGHT/5];
        void init();
        void update_map();


};

























#endif // MAPOVANIE_H


