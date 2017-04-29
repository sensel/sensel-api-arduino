/******************************************************************************************
* MIT License
*
* Copyright (c) 2013-2017 Sensel, Inc.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
******************************************************************************************/

#include "sensel.h"

//Frame struct for containing the contact array and number of contacts
SenselFrame frame;

//Whether the Sensel device has been initialized
bool sensel_ready = false;

void setup() {
  //Open serial for SenselSerial declared in sensel.h
  senselOpen();
  
  //Set frame content to scan. No pressure or label support.
  senselSetFrameContent(SENSEL_REG_CONTACTS_FLAG);

  //Start scanning the Sensel device
  senselStartScanning();

  //Mark the Sensel device as ready
  sensel_ready = true;
}

void loop() {
  //When ready, start reading frames
  if(sensel_ready)
  {
    //Read the frame of contacts from the Sensel device
    senselGetFrame(&frame);

    //Print the frame of contact data to SenselDebugSerial if defined
    senselPrintFrame(&frame);
  }
}
