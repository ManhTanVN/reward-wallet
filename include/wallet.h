#pragma once
#include "data_manager.h"
#include <string>

bool transferPoints(DataManager& manager,
                    const std::string& fromWallet,
                    const std::string& toWallet,
                    int amount,
                    const std::string& note = "");
