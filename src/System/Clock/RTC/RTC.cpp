/*
 *       ______            __ __                 __
 *      / __/ /_ ____ __  / //_/__ _______  ___ / /
 *     / _// / // /\ \ / / ,< / -_) __/ _ \/ -_) / 
 *    /_/ /_/\_,_//_\_\ /_/|_|\__/_/ /_//_/\__/_/  
 *    
 *   copyright (c) 2021 Gavin Kellam (aka corigan01)
 *   
 *   Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
 *   to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *   and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 *   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                              
 *  
 *   
 */

#include "RTC.hpp" 
#include "../../Port/port.hpp"

using namespace System;
using namespace System::IO;
using namespace System::Clock;

#define CURRENT_YEAR        2021                            // Change this each year!
 
int century_register = 0x00;                                // Set by ACPI table parsing code if possible
 
i16 RTC_SECOND_HOLD;
i16 RTC_MINUTE_HOLD;
i16 RTC_HOUR_HOLD;
i16 RTC_DAY_HOLD;
i16 RTC_MONTH_HOLD;
i32 RTC_YEAR_HOLD;
 

void RTC::Read() {
    i16 century;
    i16 last_second;
    i16 last_minute;
    i16 last_hour;
    i16 last_day;
    i16 last_month;
    i16 last_year;
    i16 last_century;
    i16 registerB;

    // Note: This uses the "read registers until you get the same values twice in a row" technique
    //       to avoid getting dodgy/inconsistent values due to RTC updates

    while (RTC::getUpdateFlag());                // Make sure an update isn't in progress

    RTC_SECOND_HOLD =   RTC::GetRegister(0x00);
    RTC_MINUTE_HOLD =   RTC::GetRegister(0x02);
    RTC_HOUR_HOLD   =   RTC::GetRegister(0x04);
    RTC_DAY_HOLD    =   RTC::GetRegister(0x07);
    RTC_MONTH_HOLD  =   RTC::GetRegister(0x08);
    RTC_YEAR_HOLD   =   RTC::GetRegister(0x09);

    if(century_register != 0) {
        century = RTC::GetRegister(century_register);
    }

    do {
        last_second     =   RTC_SECOND_HOLD;
        last_minute     =   RTC_MINUTE_HOLD;
        last_hour       =   RTC_HOUR_HOLD;
        last_day        =   RTC_DAY_HOLD;
        last_month      =   RTC_MONTH_HOLD;
        last_year       =   RTC_YEAR_HOLD;
        last_century    =   century;

        while (RTC::getUpdateFlag());           // Make sure an update isn't in progress

        RTC_SECOND_HOLD = RTC::GetRegister(0x00);
        RTC_MINUTE_HOLD = RTC::GetRegister(0x02);
        RTC_HOUR_HOLD   = RTC::GetRegister(0x04);
        RTC_DAY_HOLD    = RTC::GetRegister(0x07);
        RTC_MONTH_HOLD  = RTC::GetRegister(0x08);
        RTC_YEAR_HOLD   = RTC::GetRegister(0x09);

        if(century_register != 0) {
                century = RTC::GetRegister(century_register);
        }

    } while((last_second != RTC_SECOND_HOLD) ||
            (last_minute != RTC_MINUTE_HOLD) || 
            (last_hour != RTC_HOUR_HOLD)     ||
            (last_day != RTC_DAY_HOLD)       || 
            (last_month != RTC_MONTH_HOLD)   || 
            (last_year != RTC_YEAR_HOLD)     ||
            (last_century != century))       ;;

    registerB = RTC::GetRegister(0x0B);

    // Convert BCD to binary values if necessary

    if (!(registerB & 0x04)) {

        RTC_SECOND_HOLD = (RTC_SECOND_HOLD & 0x0F) + ((RTC_SECOND_HOLD / 16) * 10);
        RTC_MINUTE_HOLD = (RTC_MINUTE_HOLD & 0x0F) + ((RTC_MINUTE_HOLD / 16) * 10);
        RTC_HOUR_HOLD   = (((RTC_HOUR_HOLD & 0x0F) + (((RTC_HOUR_HOLD & 0x70) / 16) * 10) ) | (RTC_HOUR_HOLD & 0x80)) + (7 - 12); // <---- changed to our time zone
        RTC_DAY_HOLD    = (RTC_DAY_HOLD & 0x0F)    + ((RTC_DAY_HOLD / 16) * 10); 
        RTC_MONTH_HOLD  = (RTC_MONTH_HOLD & 0x0F)  + ((RTC_MONTH_HOLD / 16) * 10);
        RTC_YEAR_HOLD   = (RTC_YEAR_HOLD & 0x0F)   + ((RTC_YEAR_HOLD / 16) * 10);

        if(century_register != 0) {
                century = (century & 0x0F) + ((century / 16) * 10);
        }
    }

    // Convert 12 hour clock to 24 hour clock if necessary

    if (!(registerB & 0x02) && (RTC_HOUR_HOLD & 0x80)) {
        RTC_HOUR_HOLD = ((RTC_HOUR_HOLD & 0x7F) + 12) % 24;
    }

    // Calculate the full (4-digit) year

    if(century_register != 0) {
        RTC_YEAR_HOLD += century * 100;
    } 
    else {
        RTC_YEAR_HOLD += (CURRENT_YEAR / 100) * 100;
        if(RTC_YEAR_HOLD < CURRENT_YEAR) RTC_YEAR_HOLD += 100;
    }


    kout << "RTC Read()" << endl;

}


i8 RTC::GetRegister(int reg) {
    Port::byte_out(cmos_address, reg);
    return Port::byte_in(cmos_data);
}
int RTC::getUpdateFlag() {
    Port::byte_out(cmos_address, 0x0A);
    return (Port::byte_in(cmos_data) & 0x80);
}


i16 RTC::GetSeconds() {
    return RTC_SECOND_HOLD;
}

i16 RTC::GetMin() {
    return RTC_MINUTE_HOLD;
}

i16 RTC::GetHours() {
    return RTC_HOUR_HOLD;
}

i16 RTC::GetDays() {
    return RTC_DAY_HOLD;

}
i16 RTC::GetMonth() {
    return RTC_MONTH_HOLD;
}

i16 RTC::GetYear() {
    return RTC_YEAR_HOLD;
}

RTC::Date RTC::GetDate() {
    return {
        RTC::GetSeconds(),
        RTC::GetMin(),
        RTC::GetHours(),
        RTC::GetDays(),
        RTC::GetMonth(),
        RTC::GetYear()
    };
}