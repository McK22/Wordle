#pragma once

#include "resource.h"
#include <string>

bool isWordValid(std::string s);
std::string getRandomWord();
void saveDifficulty();
void loadDifficulty();
long long getTime();