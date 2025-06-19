#pragma once

#include "data_manager.h"

void showMenu();
void handleUserInput(DataManager& manager);
void registerUser(DataManager& manager);
void loginUser(DataManager& manager);
void listUsers(DataManager& manager);
void deleteUser(DataManager& manager);
