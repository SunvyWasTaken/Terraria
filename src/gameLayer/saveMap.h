//
// Created by sunvy on 10/05/2026.
//

#ifndef TERRARIA_SAVEMAP_H
#define TERRARIA_SAVEMAP_H
#include <string>
#include <vector>

#include "blocks.h"

bool saveBlockDatatoFile(std::vector<Block> &blocks, int w, int h, const std::string& fileName);

bool loadBlockDatatoFile(std::vector<Block> &blocks, int& w, int& h, const std::string& fileName);

bool writeEntireFile(const std::string& fileName, const void* data, size_t size);

bool readEntireFile(const std::string& fileName, std::string& data, size_t& size);

#endif //TERRARIA_SAVEMAP_H
