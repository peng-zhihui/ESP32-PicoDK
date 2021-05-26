#include <Arduino.h>
#include <ESP32CAN.h>
#include <CAN_config.h>

CAN_device_t CAN_cfg;               // CAN Config
unsigned long previousMillis = 0;   // will store last time a CAN Message was send
const int interval = 1;          // interval at which send CAN Messages (milliseconds)
const int rx_queue_size = 10;       // Receive Queue size

void setup()
{
    Serial.begin(115200);
    Serial.println("Basic Demo - ESP32-Arduino-CAN");
    CAN_cfg.speed = CAN_SPEED_1000KBPS;
    CAN_cfg.tx_pin_id = GPIO_NUM_5;
    CAN_cfg.rx_pin_id = GPIO_NUM_4;
    CAN_cfg.rx_queue = xQueueCreate(rx_queue_size, sizeof(CAN_frame_t));
    // Init CAN Module
    ESP32Can.CANInit();
}

float val = 20000;
unsigned char *b;
int t;

void loop()
{

    CAN_frame_t rx_frame;


    // Receive next CAN frame from queue
    if (xQueueReceive(CAN_cfg.rx_queue, &rx_frame, 0) == pdTRUE)
    {

//        if (rx_frame.FIR.B.FF == CAN_frame_std)
//        {
//            printf("New standard frame");
//        } else
//        {
//            printf("New extended frame");
//        }
//
//        if (rx_frame.FIR.B.RTR == CAN_RTR)
//        {
//            printf(" RTR from 0x%08X, DLC %d\r\n", rx_frame.MsgID, rx_frame.FIR.B.DLC);
//        } else
//        {
//            printf(" from 0x%08X, DLC %d, Data ", rx_frame.MsgID, rx_frame.FIR.B.DLC);
//
//            printf("%.2f\n", *(float *) (rx_frame.data.u8 + 4));
//        }
    }


    CAN_frame_t tx_frame;
    tx_frame.FIR.B.FF = CAN_frame_std;
    tx_frame.MsgID = 0x104;
    tx_frame.FIR.B.DLC = 8;
    tx_frame.data.u8[0] = 0x01;
    tx_frame.data.u8[1] = 0x02;
    tx_frame.data.u8[2] = 0x00;
    tx_frame.data.u8[3] = 0x00;

    val = 150000 * sin(t * 0.003);
    t++;
    b = (unsigned char *) &val;
    for (int i = 4; i < 8; i++)
        tx_frame.data.u8[i] = *(b + i - 4);
    tx_frame.data.u8[2] = 0x80;
    tx_frame.data.u8[3] = 0x23;

    Serial.printf("%ld ", millis());
    ESP32Can.CANWriteFrame(&tx_frame);
    Serial.printf("%ld\n", millis());


    tx_frame.MsgID = 0x204;
    tx_frame.FIR.B.DLC = 2;
    tx_frame.data.u8[0] = 0x01;
    tx_frame.data.u8[1] = 0x02;
    ESP32Can.CANWriteFrame(&tx_frame);

    delay(1);
}