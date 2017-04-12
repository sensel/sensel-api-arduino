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
#include "sensel_register_map.h"

byte rx_buf[SENSEL_RX_BUFFER_SIZE];
unsigned int counter = 0;

void senselInit()
{
  SenselSerial.begin(115200);
  #ifdef SenselDebugSerial
    SenselDebugSerial.begin(115200);
  #endif
  delay(3000);
  senselSetFrameContentControl(SENSEL_REG_CONTACTS_FLAG);
  senselStartScanning();
  #ifdef SenselDebugSerial
    SenselDebugSerial.println("Sensel Setup Complete!");
  #endif
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
  int len = SenselSerial.available();
  if (len > 0) {
    SenselSerial.readBytes(&rx_buf[counter%256], len);
    counter = (counter + len);
  }
}

void senselWriteReg(byte addr, byte sizeVar, byte data)
{
  SenselSerial.write(0x01);
  SenselSerial.write(addr);
  SenselSerial.write(sizeVar);
  SenselSerial.write(data);
  SenselSerial.write(data);
  SenselSerial.readBytes(rx_buf, 2);
  if (rx_buf[0] != SENSEL_PT_WRITE_ACK) {
    #ifdef SenselDebugSerial
      SenselDebugSerial.println("FAILED TO RECEIVE ACK ON WRITE");
    #endif
  }
}

void senselReadReg(byte addr, byte sizeVar, byte* buf)
{
  byte checksum = 0;
  SenselSerial.write(0x81);
  SenselSerial.write(addr);
  SenselSerial.write(sizeVar);
  SenselSerial.readBytes(rx_buf, 4);
  if (rx_buf[0] != SENSEL_PT_READ_ACK) {
    #ifdef SenselDebugSerial
      SenselDebugSerial.println("FAILED TO RECEIVE ACK ON READ");
    #endif
    _senselFlush();
    return;
  }
  unsigned int resp_size = _convertBytesToU16(rx_buf[2], rx_buf[3]);
  SenselSerial.readBytes(buf, resp_size);
  SenselSerial.readBytes(&checksum, 1);
}

unsigned long _convertBytesToU32(byte b0, byte b1, byte b2, byte b3)
{
  return ((((unsigned long)b3) & 0xff) << 24) | ((((unsigned long)b2) & 0xff) << 16) | ((((unsigned long)b1) & 0xff) << 8) | (((unsigned long)b0) & 0xff);
}

unsigned int _convertBytesToU16(byte b0, byte b1)
{
  return ((((unsigned int)b1) & 0xff) << 8) | (((unsigned int)b0) & 0xff);
}

int _convertBytesToS16(byte b0, byte b1)
{
  return ((((int)b1)) << 8) | (((int)b0) & 0xff);
}

void _senselFlush()
{
  while(SenselSerial.available() > 0) {
    SenselSerial.read();
  delay(1);
  }
  SenselSerial.flush();
}

//Return the number of contacts
void senselReadContacts(SenselFrame *frame)
{
  counter = 0;
  SenselSerial.write(0x81);
  SenselSerial.write(SENSEL_REG_SCAN_READ_FRAME);
  SenselSerial.write((byte)0x00);
  frame->n_contacts = 0;
  int i;
  int contact_size = 16;
  int timeout = 20;
  while(counter < 5 && timeout > 0){
    senselReadAvailable();
    delay(1);
    timeout--;
  }
  if(timeout == 0 || rx_buf[0] != SENSEL_PT_RVS_ACK){
    _senselFlush();
    return;
  }
  unsigned int resp_size = _convertBytesToU16(rx_buf[3], rx_buf[4]);
  timeout = 50;
  while(counter < resp_size+6 && timeout > 0){
    senselReadAvailable();
    delay(1);
    timeout--;
  }
  if(timeout == 0){
    _senselFlush();
    return;
  }
  frame->n_contacts = rx_buf[12];
  if(rx_buf[5] == SENSEL_REG_CONTACTS_FLAG && (unsigned int)(frame->n_contacts*contact_size) == resp_size-8){
    for(i = 0; i < frame->n_contacts; i++){
      int offset = 13+i*contact_size;
      frame->contacts[i].id = rx_buf[offset+0];
      frame->contacts[i].type = rx_buf[offset+1];
      frame->contacts[i].x_pos = _convertBytesToU16(rx_buf[offset+2],rx_buf[offset+3])/256.0f;
      frame->contacts[i].y_pos = _convertBytesToU16(rx_buf[offset+4],rx_buf[offset+5])/256.0f;
      frame->contacts[i].total_force = _convertBytesToU16(rx_buf[offset+6],rx_buf[offset+7])/8.0f;
      frame->contacts[i].area = _convertBytesToU16(rx_buf[offset+8],rx_buf[offset+9])/1.0f;
      frame->contacts[i].orientation = _convertBytesToS16(rx_buf[offset+10],rx_buf[offset+11])/16.0f;
      frame->contacts[i].major_axis = _convertBytesToU16(rx_buf[offset+12],rx_buf[offset+13])/256.0f;
      frame->contacts[i].minor_axis = _convertBytesToU16(rx_buf[offset+14],rx_buf[offset+15])/256.0f;
    }
  }
  else{
    _senselFlush();
  }
}

void senselPrintContacts(SenselFrame *frame){
  #ifdef SenselDebugSerial
    SenselDebugSerial.print("Num Contacts: ");
    SenselDebugSerial.println(frame->n_contacts);
    for(int i = 0; i < frame->n_contacts; i++){
      SenselDebugSerial.print("Contact ");
      SenselDebugSerial.print(frame->contacts[i].id);
      SenselDebugSerial.print(": x_pos ");
      SenselDebugSerial.print(frame->contacts[i].x_pos);
      SenselDebugSerial.print(" y_pos ");
      SenselDebugSerial.print(frame->contacts[i].y_pos);
      SenselDebugSerial.print(" total_force ");
      SenselDebugSerial.println(frame->contacts[i].total_force);
    }
  #endif
}

