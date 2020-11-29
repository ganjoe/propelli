/*
 * ds18x20.c
 *
 *  Created on: Oct 10, 2020
 *      Author: daniel
 */
#include "ds18x20.h"
#include "delay.h"
void mfinit_tsensor(ds18x20 *tsensor)
    {
    modflag_init(&tsensor->mf_tsensor, HALTICK, 1);
    }
void mftick_tsensor(ds18x20 *tsensor)
    {
    modflag_upd_regular(&tsensor->mf_tsensor);
    }
void mftask_tsensor(ds18x20 *tsensor)
    {
    if (tsensor->mf_tsensor.flag && tsensor->mf_tsensor.init_done)
   	{
	tsensor->mf_tsensor.repeat = modflag_tickdiff(&tsensor->mf_tsensor);
	tsensor->mf_tsensor.duration = modflag_tickdiff(&tsensor->mf_tsensor);
	tsensor->mf_tsensor.callcount++;
	tsensor->mf_tsensor.flag = false;
	ts_read_single(tsensor);
   	}
    }

int ts_read_single(ds18x20 *tsensor)
    {
    int type_s, cfg, raw ;

    if ( !oneWire_search(&tsensor->wire, tsensor->addr, 1))
        {
        oneWire_rstSearch(&tsensor->wire);
        delay_us(&delay,250);
        }

    switch (tsensor->addr[0])
        {
	  case 0x10:type_s = 1;break;//"Chip = DS18S20");  // or old DS1820
	  case 0x28:type_s = 0;break;//Chip = DS18B20
	  case 0x22:type_s = 0;break;//Chip = DS1822
        }
    oneWire_reset(&tsensor->wire);
    oneWire_SelROM(&tsensor->wire, tsensor->addr);
    // start conversion, with parasite power on at the end
    tsensor->wire.powerflag = 1;
    oneWire_WriteByte(&tsensor->wire, 0x44);
    delay_us(&delay, 1000);     // maybe 750ms is enough, maybe not
    // we might do a ds.depower() here, but the reset will take care of it.
    tsensor->presentFlag = oneWire_reset(&tsensor->wire);
    oneWire_SelROM(&tsensor->wire, tsensor->addr);
   // Read Scratchpad
    tsensor->wire.powerflag = 0;
    oneWire_WriteByte(&tsensor->wire, 0xBE);

    for (int i = 0; i < 9; i++)
        {           // we need 9 bytes
	tsensor->data[i] = oneWire_ReadByte(&tsensor->wire);
        }
     raw = (tsensor->data[1] << 8) | tsensor->data[0];
     if (type_s)
         {
	 raw = raw << 3; // 9 bit resolution default
	 if (tsensor->data[7] == 0x10)
	     {
	 // "count remain" gives full 12 bit resolution
	     raw = (raw & 0xFFF0) + 12 - tsensor->data[6];
	     }
         }
	 else
	     {
	       cfg = (tsensor->data[4] & 0x60);
	       // at lower res, the low bits are undefined, so let's zero them
	       if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
	       else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
	       else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
	       //// default is 12 bit resolution, 750 ms conversion time
     }
     tsensor->lastTempF = (float)raw / 16.0;// fahrenheit = celsius * 1.8 + 32.0;
     return tsensor->presentFlag;
    }


ds18x20 tsensor_cold, tsensor_hot;
