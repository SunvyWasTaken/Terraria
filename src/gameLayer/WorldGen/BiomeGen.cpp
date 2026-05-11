//
// Created by sunvy on 11/05/2026.
//

#include "BiomeGen.h"

#include <limits>
#include <memory>

#include "FastNoiseSIMD.h"
#include "gameMap.h"

namespace
{
    Biome FindBiome(float temp, float humidity)
    {
        float bestDistance = std::numeric_limits<float>::max();
        Biome bestBiome = Biome::Plains;

        for (const auto& biome : Biomes)
        {
            float dx = temp - biome.temperature;
            float dy = humidity - biome.humidity;

            float dist = dx * dx + dy * dy;
            if (dist < bestDistance)
            {
                bestDistance = dist;
                bestBiome = biome.biome;
            }
        }
        return bestBiome;
    }
}

BiomeGen::BiomeGen(int w, int h, int& s)
    : width(w), height(h), seed(s)
{
    std::unique_ptr<FastNoiseSIMD> HumidityNoise = std::unique_ptr<FastNoiseSIMD>(FastNoiseSIMD::NewFastNoiseSIMD(++seed));
    HumidityNoise->SetNoiseType(FastNoiseSIMD::Simplex);
    HumidityNoise->SetFractalOctaves(1);
    HumidityNoise->SetFrequency(0.01f);
    //HumidityNoise->SetPerturbAmp(5.f);

    std::unique_ptr<FastNoiseSIMD> TemperatureNoise = std::unique_ptr<FastNoiseSIMD>(FastNoiseSIMD::NewFastNoiseSIMD(++seed));
    TemperatureNoise->SetNoiseType(FastNoiseSIMD::Simplex);
    TemperatureNoise->SetFractalOctaves(1);
    TemperatureNoise->SetFrequency(0.01f);
    //TemperatureNoise->SetPerturbAmp(5.f);

    Humidity = FastNoiseSIMD::GetEmptySet(width * height);
    HumidityNoise->FillNoiseSet(Humidity, 0, 0, 0, height, width, 1);
    for (int i = 0; i < w * h; ++i)
        Humidity[i] = (Humidity[i] + 1) / 2;

    // Put the value btw -1 & 1
    Temperature = FastNoiseSIMD::GetEmptySet(height * width);
    TemperatureNoise->FillNoiseSet(Temperature, 0, 0, 0, height, width, 1);
    for (int i = 0; i < w * h; ++i)
        Temperature[i] = (Temperature[i] + 1) / 2;
}

void BiomeGen::operator()(GameMap &gameMap)
{
    for (int x = 0; x < width; ++x)
    {
        for (int y = 0; y < height; ++y)
        {
            const int index = y * width + x;
            gameMap.biomeData[index] = FindBiome(Temperature[index], Humidity[index]);
        }
    }
}

BiomeGen::~BiomeGen()
{
    FastNoiseSIMD::FreeNoiseSet(Humidity);
    FastNoiseSIMD::FreeNoiseSet(Temperature);
}
