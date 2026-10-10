#ifndef BLEHANDLER_H
#define BLEHANDLER_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// ==== UUIDs do serviço e característica BLE ====
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// ==== Ponteiros globais BLE ====
static BLEServer* pServer = nullptr;
static BLECharacteristic* pCharacteristic = nullptr;
static BLEAdvertising* pAdvertising = nullptr;

// ==== Inicialização BLE ====
inline void BLE_setup() {
    Serial.println("🔵 Inicializando BLE...");

    // Inicializa BLE e define nome do dispositivo
    BLEDevice::init("ESP32_MAX30102");

    // Cria servidor BLE
    pServer = BLEDevice::createServer();

    // Cria serviço BLE
    BLEService *pService = pServer->createService(SERVICE_UUID);

    // Cria característica BLE com propriedade de notificação
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_NOTIFY
    );

    // Adiciona descritor para permitir que clientes ativem notificações
    pCharacteristic->addDescriptor(new BLE2902());

    // Inicia serviço
    pService->start();

    // Configura publicidade
    pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);  // recomendação para iOS
    pAdvertising->setMinPreferred(0x12);

    // Inicia publicidade
    BLEDevice::startAdvertising();

    Serial.println("✅ BLE iniciado e anunciando serviço!");
}

// ==== Envio de dados via BLE ====
// O app espera dados no formato "bpm,spo2,glucose"
inline void BLE_send(int bpm, float spo2, float glucose, bool finger_detected) {
    if (!pCharacteristic) return;

    if (!finger_detected) {
        Serial.println("⚠️ Dedo não detectado — enviando zeros");
        bpm = 0;
        spo2 = 0;
        glucose = 0;
    }

    // Monta payload simples (CSV)
    String payload = String(bpm) + "," + String(spo2, 1) + "," + String(glucose, 1);

    // Converte e envia
    std::string strPayload = payload.c_str();
    pCharacteristic->setValue((uint8_t*)strPayload.c_str(), strPayload.length());
    pCharacteristic->notify();

    Serial.print("📤 Enviado via BLE: ");
    Serial.println(payload);
}

#endif // BLEHANDLER_H
