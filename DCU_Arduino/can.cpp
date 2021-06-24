#include "can.h"

extern MCP_CAN CAN(SPI_CS_PIN);  

// Initialize CAN module, run in the setup function
void CAN_initialize() {
  while (CAN_OK != CAN.begin(CAN_500KBPS)) {            // init can bus : baudrate = 500k
    SERIAL.println("CAN BUS Shield init fail");
    SERIAL.println(" Init CAN BUS Shield again");
    delay(100);
  }
    
  SERIAL.println("CAN BUS Shield init ok!");    
}

// Returns a can message struct with the id and data buffer
can_message CAN__receive_packet() {
    can_message out;
    unsigned char len = 0;
    unsigned char buf[8];

    if (CAN_MSGAVAIL == CAN.checkReceive()) {         // check if data coming
        CAN.readMsgBuf(&len, buf);                    // read data,  len: data length, buf: data buf
        unsigned long canId = CAN.getCanId();
        out.id = canId;
        for(int i = 0; i < len; i++) {
          out.data[i] = buf[i];
        }        
    } else {
      out.id = 0x00;
      for(int i = 0; i < 8; i++) {
        out.data[i] = 0x01;
      }
    }    
    return out;
}

// Input is a can message struct
// Returns the RPM as a signed integer
// The CAN packet is little endian
// RPM is byte 2 and 3 (buf[5] and buf[4]) of address 0x0A5
signed int CAN__convert_RPM(can_message packet) {
  return (packet.data[5] | (packet.data[4] << 8));
}

// Input is a can message struct
// Returns the battery state of charge as a signed float 
// The value returned is a percentage with one decimal of precision (Ex: 51.2%)
// The CAN packet is big endian
// SOC percentage is located in bytes 0, 1, 2, and 3 (buf[0 - 3]) of address 0x627
float CAN__convert_SOC(can_message packet) {
  return ((float)(packet.data[0] | (packet.data[1] << 8) | (packet.data[2] << 16) | (packet.data[3] << 32)) * 0.1f);
}

// Input is a can message struct
// Returns the average battery temperature as a signed float 
// The value returned is in Celsius with one decimal of precision
// The CAN packet is big endian
// SOC percentage is located in bytes 0, 1, 2, and 3 (buf[0 - 3]) of address 0x628
float CAN__convert_TEMP(can_message packet) {
  return ((float)(packet.data[0] | (packet.data[1] << 8) | (packet.data[2] << 16) | (packet.data[3] << 32)) * 0.1f) - 100;
}

// Input is a can message struct
// Prints the packet info over serial
void CAN__print_packet(can_message packet) {
  // Print the received packet for testing
  Serial.println("-------------------------------");
  Serial.print("Messages at ID: 0x");
  Serial.println(packet.id, HEX);
  for (int i = 0; i < 8; i++) { // print the data
        Serial.print(packet.data[i], HEX);
        Serial.print("\t");
  }
  Serial.println();
}

// Input are the RPM, TEMP, and SOC values
// Prints the RPM, SOC, and TEMP values over serial
void CAN__print_recieved_values(signed int RPM, float SOC, float TEMP) {
  // Print the RPM, SOC, and TEMP values for testing
  Serial.println("******************************************************************************");
  Serial.print("RPM:\t");
  Serial.println(RPM);
  Serial.print("SOC:\t");
  Serial.println(SOC);
  Serial.print("TEMP:\t");
  Serial.println(TEMP);
  Serial.println("******************************************************************************");
  
}
