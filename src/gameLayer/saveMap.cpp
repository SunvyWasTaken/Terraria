//
// Created by sunvy on 10/05/2026.
//

#include "saveMap.h"

#include <assert.h>
#include <fstream>

bool saveBlockDatatoFile(std::vector<Block> &blocks, int w, int h, const std::string &fileName)
{
    std::ofstream f(fileName, std::ios::binary);

    if (blocks.size() <= 0)
        return false;

    f.write((char*)&w, sizeof(w));
    f.write((char*)&h, sizeof(h));

    f.write((const char*)blocks.data(), sizeof(Block) * blocks.size());

    f.close();
    return true;
}

bool loadBlockDatatoFile(std::vector<Block> &blocks, int& w, int& h, const std::string &fileName)
{
    blocks.clear();
    w = 0;
    h = 0;

    std::ifstream f(fileName, std::ios::binary);

    if (!f.is_open())
        return false;

    f.read((char*)&w, sizeof(w));
    f.read((char*)&h, sizeof(h));

    if (!f || w <= 0 || h <= 0)
    {
        f.close();
        return false;
    }

    if (w > 10000)
    {
        f.close();
        return false;
    }
    if (h > 10000)
    {
        f.close();
        return false;
    }

    size_t blockSize = w * h;
    blocks.resize(blockSize);

    f.read((char*)blocks.data(), sizeof(Block) * blockSize);

    if (!f)
    {
        blocks.clear();
        w = 0;
        h = 0;
        f.close();
        return false;
    }

    for (auto& block : blocks)
        block.sanitize();

    f.close();
    return true;
}

bool writeEntireFile(const std::string &fileName, const void *data, size_t size)
{
    std::ofstream f(fileName, std::ios::binary);

    if (size <= 0)
    {
        f.close();
        return false;
    }

    f.write((char*)&size, sizeof(size));
    f.write((char*)&size, size);

    f.close();
    return true;
}

bool readEntireFile(const std::string &fileName, std::string& data, size_t& size)
{
    data.clear();
    size = 0;

    std::ifstream f(fileName, std::ios::binary);

    if (!f.is_open())
    {
        return false;
    }

    f.read((char*)&size, sizeof(size));

    f.read((char*)&data, size);

    f.close();
    return true;
}
