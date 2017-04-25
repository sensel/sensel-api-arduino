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

#ifndef SENSEL_H
#define SENSEL_H

//Serial of the Sensel device
#define SenselSerial Serial1

//Serial for debug information
#define SenselDebugSerial Serial

//Max size for Sensel RX buffer
#define SENSEL_RX_BUFFER_SIZE 512

//Struct for Sensel contact information
typedef struct __attribute__((__packed__))
{
    byte id;
    byte type;
    float x_pos;
    float y_pos;
    float total_force;
    float area;
    float orientation;
    float major_axis;
    float minor_axis;
} SenselContact;

//Struct for Sensel frame, only contains contacts
typedef struct __attribute__((__packed__))
{
  byte n_contacts;
  SenselContact contacts[16];
}SenselFrame;

//Flag for enabling contact scanning
const byte SENSEL_REG_CONTACTS_FLAG = 0x04;

//Ack for read register
const byte SENSEL_PT_READ_ACK = 1;

//Ack for read variable size register
const byte SENSEL_PT_RVS_ACK = 3;

//Ack for write register
const byte SENSEL_PT_WRITE_ACK = 5;

const byte SENSEL_CONTACT_TYPE_INVALID = 0;
const byte SENSEL_CONTACT_TYPE_START = 1;
const byte SENSEL_CONTACT_TYPE_MOVE = 2;
const byte SENSEL_CONTACT_TYPE_END = 3;

#endif
