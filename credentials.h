#pragma once
#include "includes.h"
#include "json.hpp"

const std::string prefFilePath = "C:\\Users\\Default\\output.pref";

char username[256] = "";
char password[256] = "";

bool ReadPrefFile() {
    std::ifstream inputFile(prefFilePath);

    if (inputFile.is_open()) {
        nlohmann::json jsonData;
        inputFile >> jsonData;

        if (!jsonData.empty()) {
            std::string tempUsername = jsonData[0].get<std::string>();
            std::string tempPassword = jsonData[1].get<std::string>();
            strncpy_s(globals.username, sizeof(globals.username), tempUsername.c_str(), sizeof(globals.username) - 1);
            strncpy_s(globals.password, sizeof(globals.password), tempPassword.c_str(), sizeof(globals.password) - 1);

            // Output the data
            std::cout << "Username: " << globals.username << std::endl;
            std::cout << "Password: "<< globals.password << std::endl;

            inputFile.close();
            return true;
        }
        else {
            std::cerr << "Empty .pref file." << std::endl;
        }

        inputFile.close();
    }
    else {
        std::cerr << "No .pref file found." << std::endl;
    }

    return false;
}

void UpdatePrefFile() {
    nlohmann::json data = {
        std::string(globals.username),
        std::string(globals.password),
        std::string(globals.username),
    };

    std::ofstream outputFile(prefFilePath);
    if (outputFile.is_open()) {
        outputFile << data.dump();
        outputFile.close();
        std::cout << "Data written to the .pref file." << std::endl;
    }
    else {
        std::cerr << "Failed to write to the .pref file." << std::endl;
    }
}