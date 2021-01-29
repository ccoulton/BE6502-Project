#include <Arduino.h>
#include "WiFi.h"
#include "..\inc\6502-monitor.h"
#include "..\inc\IrcBot.h"

/*#define JTAG_TDO 15 //p4
#define JTAG_TDI 12 //p9
#define JTAG_TCK 13 //p10
#define JTAG_TMS 14 //p23
rx/tx0 = 3/1, rx/tx1 = 15/4, rx/tx2 = 16/17
sda/scl0 = 12/13, sda/scl1 = 21,22?
ws2812/bootloader pin 0, safeboot, 21
sd dat0 2/p8, sclk 14/p23, cmd 15/p4
*/

#undef ANT_SELECT
#define ANT_SELECT 16

void setup() {
    Serial.begin(115200); //USB moniter
    Serial.print("mode reset\n");
    WiFi.mode(WIFI_STA);
    Serial.print("Disconnect\n");
    WiFi.disconnect();
    delay(100);
    connectToKnownNetworks();
    //arduino defaults to core 1 
    xTaskCreatePinnedToCore(DebuggerTask, "6502 Debugger", 1000, NULL, 1, NULL, 1);
    //put wifi on core 0 
    xTaskCreatePinnedToCore(IrcBotTask,   "IrcBot", 3000, NULL, 1, NULL, 0);
}

void loop(){}