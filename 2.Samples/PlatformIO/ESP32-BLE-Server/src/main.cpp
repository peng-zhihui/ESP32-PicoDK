#include <Arduino.h>
#include <NimBLEDevice.h>


class ServerCallbacks : public NimBLEServerCallbacks
{
    void onConnect(NimBLEServer* pServer)
    {
        Serial.println("Client connected");
        Serial.println("Multi-connect support: start advertising");
        NimBLEDevice::startAdvertising();
    };
    /** Alternative onConnect() method to extract details of the connection.
     *  See: src/ble_gap.h for the details of the ble_gap_conn_desc struct.
     */
    void onConnect(NimBLEServer* pServer, ble_gap_conn_desc* desc)
    {
        Serial.print("Client address: ");
        Serial.println(NimBLEAddress(desc->peer_ota_addr).toString().c_str());
        /** We can use the connection handle here to ask for different connection parameters.
         *  Args: connection handle, min connection interval, max connection interval
         *  latency, supervision timeout.
         *  Units; Min/Max Intervals: 1.25 millisecond increments.
         *  Latency: number of intervals allowed to skip.
         *  Timeout: 10 millisecond increments, try for 5x interval time for best results.
         */
        pServer->updateConnParams(desc->conn_handle, 24, 48, 0, 60);
    };
    void onDisconnect(NimBLEServer* pServer)
    {
        Serial.println("Client disconnected - start advertising");
        NimBLEDevice::startAdvertising();
    };
    void onMTUChange(uint16_t MTU, ble_gap_conn_desc* desc)
    {
        Serial.printf("MTU updated: %u for connection ID: %u\n", MTU, desc->conn_handle);
    };

/********************* Security handled here **********************
****** Note: these are the same return values as defaults ********/
    uint32_t onPassKeyRequest()
    {
        Serial.println("Server Passkey Request");
        /** This should return a random 6 digit number for security
         *  or make your own static passkey as done here.
         */
        return 123456;
    };

    bool onConfirmPIN(uint32_t pass_key)
    {
        Serial.print("The passkey YES/NO number: ");
        Serial.println(pass_key);
        /** Return false if passkeys don't match. */
        return true;
    };

    void onAuthenticationComplete(ble_gap_conn_desc* desc)
    {
        /** Check that encryption was successful, if not we disconnect the client */
        if (!desc->sec_state.encrypted)
        {
            NimBLEDevice::getServer()->disconnect(desc->conn_handle);
            Serial.println("Encrypt connection failed - disconnecting client");
            return;
        }
        Serial.println("Starting BLE work!");
    };
};

class CharacteristicCallbacks : public NimBLECharacteristicCallbacks
{
    void onRead(NimBLECharacteristic* pCharacteristic)
    {
        Serial.print(pCharacteristic->getUUID().toString().c_str());
        Serial.print(": onRead(), value: ");
        Serial.println(*(float*) (pCharacteristic->getValue().c_str()));
    };

    void onWrite(NimBLECharacteristic* pCharacteristic)
    {
        Serial.print(pCharacteristic->getUUID().toString().c_str());
        Serial.print(": onWrite(), value: ");
        Serial.println(pCharacteristic->getValue().c_str());
    };
    /** Called before notification or indication is sent,
     *  the value can be changed here before sending if desired.
     */
    void onNotify(NimBLECharacteristic* pCharacteristic)
    {
        pCharacteristic->setValue((float) esp_random());
        Serial.println("Sending notification to clients");
    };


    /** The status returned in status is defined in NimBLECharacteristic.h.
     *  The value returned in code is the NimBLE host return code.
     */
    void onStatus(NimBLECharacteristic* pCharacteristic, Status status, int code)
    {
        String str = ("Notification/Indication status code: ");
        str += status;
        str += ", return code: ";
        str += code;
        str += ", ";
        str += NimBLEUtils::returnCodeToString(code);
        Serial.println(str);
    };

    void onSubscribe(NimBLECharacteristic* pCharacteristic, ble_gap_conn_desc* desc, uint16_t subValue)
    {
        String str = "Client ID: ";
        str += desc->conn_handle;
        str += " Address: ";
        str += std::string(NimBLEAddress(desc->peer_ota_addr)).c_str();
        if (subValue == 0)
        {
            str += " Unsubscribed to ";
        } else if (subValue == 1)
        {
            str += " Subscribed to notfications for ";
        } else if (subValue == 2)
        {
            str += " Subscribed to indications for ";
        } else if (subValue == 3)
        {
            str += " Subscribed to notifications and indications for ";
        }
        str += std::string(pCharacteristic->getUUID()).c_str();

        Serial.println(str);
    };
};

class DescriptorCallbacks : public NimBLEDescriptorCallbacks
{
    void onWrite(NimBLEDescriptor* pDescriptor)
    {
        std::string dscVal((char*) pDescriptor->getValue(), pDescriptor->getLength());
        Serial.print("Descriptor witten value:");
        Serial.println(dscVal.c_str());
    };

    void onRead(NimBLEDescriptor* pDescriptor)
    {
        Serial.print(pDescriptor->getUUID().toString().c_str());
        Serial.println(" Descriptor read");
    };
};

struct JointStatus_t
{
    NimBLEService* pService;
    NimBLECharacteristic* pChr_J1;
    NimBLECharacteristic* pChr_J2;
    NimBLECharacteristic* pChr_J3;
    NimBLECharacteristic* pChr_J4;
    NimBLECharacteristic* pChr_J5;
    NimBLECharacteristic* pChr_J6;
};

NimBLEServer* pServer;
DescriptorCallbacks dscCallbacks;
CharacteristicCallbacks chrCallbacks;

JointStatus_t jointStatus;

void setup()
{
    Serial.begin(115200);
    Serial.println("Starting Dummy-BLE Server");

    /** sets device name */
    NimBLEDevice::init("Dummy-BLE");

    /** Optional: set the transmit power, default is 3db */
    NimBLEDevice::setPower(ESP_PWR_LVL_P9); /** +9db */
    NimBLEDevice::setSecurityAuth(false, false, true);

    pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());

    jointStatus.pService = pServer->createService("0001");

    jointStatus.pChr_J1 = jointStatus.pService->createCharacteristic(
        "1001",
        NIMBLE_PROPERTY::READ |
        NIMBLE_PROPERTY::NOTIFY
    );
    jointStatus.pChr_J1->setValue(10.0f);
    jointStatus.pChr_J1->setCallbacks(&chrCallbacks);
    auto* pJointStatusDsc_J1 = (NimBLE2904*) jointStatus.pChr_J1->createDescriptor("2904");
    pJointStatusDsc_J1->setFormat(NimBLE2904::FORMAT_FLOAT32);
    pJointStatusDsc_J1->setCallbacks(&dscCallbacks);

    jointStatus.pChr_J2 = jointStatus.pService->createCharacteristic(
        "1002",
        NIMBLE_PROPERTY::READ |
        NIMBLE_PROPERTY::NOTIFY
    );
    jointStatus.pChr_J2->setValue(20.0f);
    jointStatus.pChr_J2->setCallbacks(&chrCallbacks);
    auto* pJointStatusDsc_J2 = (NimBLE2904*) jointStatus.pChr_J2->createDescriptor("2904");
    pJointStatusDsc_J2->setFormat(NimBLE2904::FORMAT_FLOAT32);
    pJointStatusDsc_J2->setCallbacks(&dscCallbacks);

    jointStatus.pChr_J3 = jointStatus.pService->createCharacteristic(
        "1003",
        NIMBLE_PROPERTY::READ |
        NIMBLE_PROPERTY::NOTIFY
    );
    jointStatus.pChr_J3->setValue(30.0f);
    jointStatus.pChr_J3->setCallbacks(&chrCallbacks);
    auto* pJointStatusDsc_J3 = (NimBLE2904*) jointStatus.pChr_J3->createDescriptor("2904");
    pJointStatusDsc_J3->setFormat(NimBLE2904::FORMAT_FLOAT32);
    pJointStatusDsc_J3->setCallbacks(&dscCallbacks);

    jointStatus.pChr_J4 = jointStatus.pService->createCharacteristic(
        "1004",
        NIMBLE_PROPERTY::READ |
        NIMBLE_PROPERTY::NOTIFY
    );
    jointStatus.pChr_J4->setValue(40.0f);
    jointStatus.pChr_J4->setCallbacks(&chrCallbacks);
    auto* pJointStatusDsc_J4 = (NimBLE2904*) jointStatus.pChr_J4->createDescriptor("2904");
    pJointStatusDsc_J4->setFormat(NimBLE2904::FORMAT_FLOAT32);
    pJointStatusDsc_J4->setCallbacks(&dscCallbacks);

    jointStatus.pChr_J5 = jointStatus.pService->createCharacteristic(
        "1005",
        NIMBLE_PROPERTY::READ |
        NIMBLE_PROPERTY::NOTIFY
    );
    jointStatus.pChr_J5->setValue(50.0f);
    jointStatus.pChr_J5->setCallbacks(&chrCallbacks);
    auto* pJointStatusDsc_J5 = (NimBLE2904*) jointStatus.pChr_J5->createDescriptor("2904");
    pJointStatusDsc_J5->setFormat(NimBLE2904::FORMAT_FLOAT32);
    pJointStatusDsc_J5->setCallbacks(&dscCallbacks);

    jointStatus.pChr_J6 = jointStatus.pService->createCharacteristic(
        "1006",
        NIMBLE_PROPERTY::READ |
        NIMBLE_PROPERTY::NOTIFY
    );
    jointStatus.pChr_J6->setValue(60.0f);
    jointStatus.pChr_J6->setCallbacks(&chrCallbacks);
    auto* pJointStatusDsc_J6 = (NimBLE2904*) jointStatus.pChr_J6->createDescriptor("2904");
    pJointStatusDsc_J6->setFormat(NimBLE2904::FORMAT_FLOAT32);
    pJointStatusDsc_J6->setCallbacks(&dscCallbacks);

    /** Start the services when finished creating all Characteristics and Descriptors */
    jointStatus.pService->start();

    NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
    /** Add the services to the advertisment data **/
    pAdvertising->addServiceUUID(jointStatus.pService->getUUID());
    /** If your device is battery powered you may consider setting scan response
     *  to false as it will extend battery life at the expense of less data sent.
     */
    pAdvertising->setScanResponse(true);
    pAdvertising->start();

    Serial.println("Advertising Started");
}


void loop()
{
    if (pServer->getConnectedCount())
    {
        NimBLEService* pSvc = pServer->getServiceByUUID("0001");
        if (pSvc)
        {
            NimBLECharacteristic* pChr = pSvc->getCharacteristic("1003");
            if (pChr)
            {
                pChr->notify();
            }
        }
    }

    delay(2000);
}