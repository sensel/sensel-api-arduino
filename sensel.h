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

#define SenselSerial Serial1
#define SenselDebugSerial Serial
#define SENSEL_RX_BUFFER_SIZE 512

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

typedef struct __attribute__((__packed__))
{
  byte n_contacts;
  SenselContact contacts[16];
}SenselFrame;

const byte SENSEL_REG_CONTACTS_FLAG = 0x04;

const byte SENSEL_PT_READ_ACK = 1;
const byte SENSEL_PT_RVS_ACK = 3;
const byte SENSEL_PT_WRITE_ACK = 5;

const byte SENSEL_EVENT_CONTACT_INVALID = 0;
const byte SENSEL_EVENT_CONTACT_START = 1;
const byte SENSEL_EVENT_CONTACT_MOVE = 2;
const byte SENSEL_EVENT_CONTACT_END = 3;


#endif
