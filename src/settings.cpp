#include "settings.h"

Settings Settings::instance;

const std::string Settings::FILE_NAME = "smk_settings.cfg";
const std::string Settings::MUSIC_VOLUME = "music_volume";
const std::string Settings::SFX_VOLUME = "sfx_volume";
const std::string Settings::RESOLUTION = "resolution";

const std::array<float, 8> Settings::ALLOWED_MULTIPLIERS = {
    2.0f, 2.5f, 3.0f, 3.5f, 4.0f, 5.0f, 6.0f, 8.0f};

bool Settings::applySetting(const std::string &key, const std::string &value) {
    try {
        if (key.compare(MUSIC_VOLUME) == 0) {
            int vol = std::stoi(value);
            if (vol % 5 != 0 || vol < 0 || vol > 100) {
                return false;
            }
            Audio::setVolume(0.01f * vol, Audio::getSfxVolume());
        } else if (key.compare(SFX_VOLUME) == 0) {
            int vol = std::stoi(value);
            if (vol % 5 != 0 || vol < 0 || vol > 100) {
                return false;
            }
            Audio::setVolume(Audio::getMusicVolume(), 0.01f * vol);
        } else if (key.compare(RESOLUTION) == 0) {
            float resolution = std::stof(value);
            auto iter = std::find(ALLOWED_MULTIPLIERS.begin(),
                                  ALLOWED_MULTIPLIERS.end(), resolution);
            if (iter == ALLOWED_MULTIPLIERS.end()) {
                return false;
            }
            instance.resolutionIndex = iter - ALLOWED_MULTIPLIERS.begin();
            instance.resolutionMultiplier = resolution;
        } else {
            // Controls
            int keyId = std::stoi(key);
            if (keyId < 0 || keyId >= (int)Key::__COUNT) {
                return false;
            }
            unsigned int valueId = std::stoi(value);
            Input::set(Key(keyId), sf::Keyboard::Key(valueId));
        }
        return true;
    } catch (...) {
        std::cerr << "Error when reading setting " << key << "." << std::endl;
        return false;
    }
}

void Settings::writeDefaultSettings() {
    instance.resolutionIndex = DEFAULT_RESOLUTION_INDEX;
    instance.resolutionMultiplier =
        ALLOWED_MULTIPLIERS[instance.resolutionIndex];
    Settings::saveSettings();
}

void Settings::tryLoadSettings() {
    std::ifstream file(FILE_NAME);
    if (!file.is_open()) {
        // settings don't exists, create new
        Settings::writeDefaultSettings();
        return;
    }

    std::string line;
    std::getline(file, line);  // ignore first line
    while (std::getline(file, line) && !file.eof()) {
        size_t equals = line.find('=');
        std::string key = line.substr(0, equals);
        std::string value = line.substr(equals + 1);
        if (!applySetting(key, value)) {
            std::cerr << "Error while loading settings" << std::endl;
            file.close();
            Settings::writeDefaultSettings();
            return;
        }
    }

    file.close();
}

void Settings::saveSettings() {
    std::ofstream file(FILE_NAME);
    if (!file.is_open()) {
        std::cerr << "Error: cannot create " << FILE_NAME
                  << " configuration file" << std::endl;
        return;
    }

    int musicVol = (Audio::getMusicVolume() + 0.005f) * 100;
    int sfxVol = (Audio::getSfxVolume() + 0.005f) * 100;

    // write config and close
    file << "SUPER MARIO KART SETTINGS. DO NOT EDIT THIS FILE." << std::endl;
    file << MUSIC_VOLUME << "=" << musicVol << std::endl;
    file << SFX_VOLUME << "=" << sfxVol << std::endl;
    file << RESOLUTION << "=" << instance.resolutionMultiplier << std::endl;
    for (int i = 0; i < (int)Key::__COUNT; i++) {
        file << (int)i << "=" << (int)Input::get((Key)i) << std::endl;
    }
    file.close();
}