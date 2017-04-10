/******************************************************************************************
* Copyright 2013-2017 Sensel, Inc
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this
* software and associated documentation files (the "Software"), to deal in the Software
* without restriction, including without limitation the rights to use, copy, modify, merge,
* publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons
* to whom the Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or
* substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
* PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
* FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
* OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
******************************************************************************************/

#include "sensel.h"

#define SENSEL_SERIAL Serial1
#define DEBUG_SERIAL Serial

bool sensel_ready = false;

byte rx_buf[256];
unsigned int counter = 0;
SenselContact c[16];

void senselInit()
{
  DEBUG_SERIAL.begin(115200);
  SENSEL_SERIAL.begin(115200);
  delay(3000);
  senselSetFrameContentControl(SENSEL_REG_CONTACTS_FLAG);
  senselStartScanning();
  DEBUG_SERIAL.println("Sensel Setup Complete!");
  sensel_ready = true;
}

void senselSetFrameContentControl(byte content)
{
  senselWriteReg(SENSEL_REG_FRAME_CONTENT_CONTROL, 1, content);
}

void senselStartScanning()
{
  senselWriteReg(SENSEL_REG_SCAN_ENABLED, 1, 1);
}

void senselStopScanning()
{
  senselWriteReg(SENSEL_REG_SCAN_ENABLED, 1, 0);
}

void senselReadAvailable() {
  int len = SENSEL_SERIAL.available();
  if (len > 0) {
    SENSEL_SERIAL.readBytes(&rx_buf[counter%256], len);
    counter = (counter + len);
  }
}

void senselWriteReg(byte addr, byte sizeVar, byte data)
{
  SENSEL_SERIAL.write(0x01);
  SENSEL_SERIAL.write(addr);
  SENSEL_SERIAL.write(sizeVar);
  SENSEL_SERIAL.write(data);
  SENSEL_SERIAL.write(data);
  SENSEL_SERIAL.readBytes(rx_buf, 2);
  if (rx_buf[0] != SENSEL_PT_WRITE_ACK) {
    DEBUG_SERIAL.print("FAILED TO RECEIVE ACK ON WRITE");
  }
}

unsigned long _convertBytesTo32(byte b0, byte b1, byte b2, byte b3)
{
  return ((((unsigned long)b3) & 0xff) << 24) | ((((unsigned long)b2) & 0xff) << 16) | ((((unsigned long)b1) & 0xff) << 8) | (((unsigned long)b0) & 0xff);
}

unsigned int _convertBytesTo16(byte b0, byte b1)
{
  return ((((unsigned int)b1) & 0xff) << 8) | (((unsigned int)b0) & 0xff);
}

void _senselFlush()
{
  while(SENSEL_SERIAL.available() > 0) {
    SENSEL_SERIAL.read();
  delay(1);
  }
  SENSEL_SERIAL.flush();
}

void senselContactPrint(int index){
  DEBUG_SERIAL.print("Contact ");
  DEBUG_SERIAL.print(c[index].id);
  DEBUG_SERIAL.print(": x_pos ");
  DEBUG_SERIAL.print(c[index].x_pos);
  DEBUG_SERIAL.print(" y_pos ");
  DEBUG_SERIAL.print(c[index].y_pos);
  DEBUG_SERIAL.print(" total_force ");
  DEBUG_SERIAL.println(c[index].total_force);
}

//Return the number of contacts
byte senselReadContacts()
{
  counter = 0;
  if(!sensel_ready)
    return 0;
  SENSEL_SERIAL.write(0x81);
  SENSEL_SERIAL.write(SENSEL_REG_FRAME);
  SENSEL_SERIAL.write((byte)0x00);
  byte num_contacts = 0;
  int i;
  int contact_size = 16;
  int timeout = 20;
  while(counter < 5 && timeout > 0){
    senselReadAvailable();
    delay(1);
    timeout--;
  }
  if(timeout == 0 || rx_buf[0] != SENSEL_PT_RVS_ACK){
    DEBUG_SERIAL.print(rx_buf[0]);
    DEBUG_SERIAL.print(" 1 ");
    DEBUG_SERIAL.println(counter);
    _senselFlush();
    return 0;
  }
  unsigned int resp_size = _convertBytesTo16(rx_buf[3], rx_buf[4]);
  timeout = 50;
  while(counter < resp_size+6 && timeout > 0){
    senselReadAvailable();
    delay(1);
    timeout--;
  }
  if(timeout == 0){
    //DEBUG_SERIAL.println("FLUSH1");
    DEBUG_SERIAL.print(counter);
    DEBUG_SERIAL.print(" 2 ");
    DEBUG_SERIAL.println(resp_size);
    _senselFlush();
    return 0;
  }
  num_contacts = rx_buf[12];
  if(rx_buf[5] == SENSEL_REG_CONTACTS_FLAG && (unsigned int)(num_contacts*contact_size) == resp_size-8){
    DEBUG_SERIAL.print("Num Contacts: ");
    DEBUG_SERIAL.println(num_contacts);
    for(i = 0; i < num_contacts; i++){
      int offset = 13+i*contact_size;
      c[i].id = rx_buf[offset+0];
      c[i].type = rx_buf[offset+1];
      c[i].x_pos = _convertBytesTo16(rx_buf[offset+2],rx_buf[offset+3])/256.0f;
      c[i].y_pos = _convertBytesTo16(rx_buf[offset+4],rx_buf[offset+5])/256.0f;
      c[i].total_force = _convertBytesTo16(rx_buf[offset+6],rx_buf[offset+7])/256.0f;
      c[i].area = _convertBytesTo16(rx_buf[offset+8],rx_buf[offset+9])/256.0f;
      c[i].orientation = _convertBytesTo16(rx_buf[offset+10],rx_buf[offset+11])/16.0f;
      c[i].major_axis = _convertBytesTo16(rx_buf[offset+12],rx_buf[offset+13])/256.0f;
      c[i].minor_axis = _convertBytesTo16(rx_buf[offset+14],rx_buf[offset+15])/256.0f;
      senselContactPrint(i);
    }
  }
  else{
    _senselFlush();
    return 0;
  }
  return num_contacts;
}

