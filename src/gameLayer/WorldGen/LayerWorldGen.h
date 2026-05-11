//
// Created by sunvy on 11/05/2026.
//

#ifndef TERRARIA_LAYERWORLDGEN_H
#define TERRARIA_LAYERWORLDGEN_H


struct GameMap;

struct LayerWorldGen
{
    virtual ~LayerWorldGen() = default;

    virtual void operator() (GameMap& gameMap) {};
};


#endif //TERRARIA_LAYERWORLDGEN_H