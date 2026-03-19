#pragma once

#include <Arduino.h>
#include <functional>
#include "../MQTT/MqttManager.h"
#include "../Config.h"

class Chaudiere {
public:
    struct ETAT_CHAUDIERE {
        ETAT_CHAUDIERE() : fonctionnement(false), arretChauffage(false) {}
        ETAT_CHAUDIERE(byte etat) {
            set(etat);
        }
        void set(byte etat) {
            fonctionnement = (etat & 0b00001000) != 0;
            arretChauffage = (etat & 0b00000100) != 0;
        }
        byte toByte() {
            byte etat = 0;
            if(fonctionnement) etat |= 0b00001000;
            if(arretChauffage) etat |= 0b00000100;
            return etat;
        }

        bool fonctionnement = false;
        bool arretChauffage = false;

        String getLibelle() {
            if(arretChauffage) {
                return "Arrêt chauffage";
            } else if(!fonctionnement) {
                return "Veille";
            } else {
                return "Fonctionnement";
            }
        }
    };

    enum MODE_ECS : uint8_t {
        INCONNU = 0XFF,
        STOP = 0x29,
        MAX = 0x01,
        ECO = 0x09,
        ECO_HORAIRES = 0x11,
        ECOPLUS = 0x19,
        ECOPLUS_HORAIRES = 0x21
    };

    Chaudiere(MqttManager& mqtt, Config& cfg) : _mqtt(mqtt), _cfg(cfg) {}

    void begin(std::function<void(const String&)> modeEcsCommandCb = {});
    void publishMqtt();
    void publishModeEcs();

    void setTemperatureExterieure(float temperature);
    void setTemperatureECS(float temperature);
    void setTemperatureCDC(float temperature);
    void setPuissanceInstantaneeECS(float puissance) { _puissanceInstantaneeECS = puissance; }
    void setPuissanceInstantaneeChauffage(float puissance) { _puissanceInstantaneeChauffage = puissance; }

    float getTemperatureExterieure() const { return _temperatureExterieure; }
    float getTemperatureECS() const { return _temperatureECS; }
    float getTemperatureCDC() const { return _temperatureCDC; }
    float getPuissanceInstantaneeECS() const { return _puissanceInstantaneeECS; }
    float getPuissanceInstantaneeChauffage() const { return _puissanceInstantaneeChauffage; }

    void setConsommationECS(int16_t consommation);
    void setConsommationChauffage(int16_t consommation);
    int16_t getConsommationChauffage() const { return _consommationGazChauffage; }
    int16_t getConsommationECS() const { return _consommationGazECS; }

    bool setModeECS(MODE_ECS modeECS);
    bool setModeECS(const String& modeECS);
    MODE_ECS getModeECS() const { return _modeECS; }
    String getNomModeECS() const;

    void setPression(float pression);
    float getPression() const { return _pression; }

    void setEtatChaudiere(byte etatChaudiere) { _etatChaudiere.set(etatChaudiere); }
    ETAT_CHAUDIERE getEtatChaudiere() const { return _etatChaudiere; }

private:
    MqttManager& _mqtt;
    Config& _cfg;

    float _temperatureECS = NAN;
    float _temperatureCDC = NAN;
    float _temperatureExterieure = NAN;
    float _puissanceInstantaneeECS = NAN;
    float _puissanceInstantaneeChauffage = NAN;
    float _pression = NAN;

    int16_t _consommationGazECS = -1;
    int16_t _consommationGazChauffage = -1;

    MODE_ECS _modeECS = MODE_ECS::INCONNU;

    int16_t _lastPubConsommationECS = -1;
    int16_t _lastPubConsommationChauffage = -1;
    ETAT_CHAUDIERE _etatChaudiere;

    struct {
        MqttEntity etatChaudiere;
        MqttEntity modeECS;
        MqttEntity tempECS;
        MqttEntity tempCDC;
        MqttEntity tempExterieure;
        MqttEntity puissanceInstantaneeECS;
        MqttEntity puissanceInstantaneeChauffage;
        MqttEntity consommationChauffage;
        MqttEntity consommationECS;
        MqttEntity pression;
    } _mqttEntities;
};
