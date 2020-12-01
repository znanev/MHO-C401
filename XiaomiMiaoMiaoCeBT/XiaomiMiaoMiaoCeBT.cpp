#include "XiaomiMiaoMiaoCeBT.h"

uint8_t T_DTM_init[18] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t T_DTM2_init[18] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

//----------------------------------
// LUTV, LUT_KK and LUT_KW values taken from the actual device with a
// logic analyzer
//----------------------------------
uint8_t T_LUTV_init[15] = {0x47, 0x47, 0x01, 0x87, 0x87, 0x01, 0x47, 0x47, 0x01, 0x87, 0x87, 0x01, 0x81, 0x81, 0x01};
uint8_t T_LUT_KK_init[15] = {0x87, 0x87, 0x01, 0x87, 0x87, 0x01, 0x47, 0x47, 0x01, 0x47, 0x47, 0x01, 0x81, 0x81, 0x01};
uint8_t T_LUT_KW_init[15] = {0x47, 0x47, 0x01, 0x47, 0x47, 0x01, 0x87, 0x87, 0x01, 0x87, 0x87, 0x01, 0x81, 0x81, 0x01};
uint8_t T_LUT_KK_update[15] = {0x87, 0x87, 0x01, 0x87, 0x87, 0x01, 0x87, 0x87, 0x01, 0x87, 0x87, 0x01, 0x81, 0x81, 0x01};
uint8_t T_LUT_KW_update[15] = {0x47, 0x47, 0x01, 0x47, 0x47, 0x01, 0x47, 0x47, 0x01, 0x47, 0x47, 0x01, 0x81, 0x81, 0x01};

//----------------------------------
// define segments
// the data in the arrays consists of {byte, bit} pairs of each segment
//----------------------------------
uint8_t top_left_1[2] = {12, 3};
uint8_t top_left[22] = {16, 7, 15, 4, 14, 1, 14, 7, 12, 5, 12, 4, 13, 3, 15, 7, 15, 6, 15, 5, 14, 0};
uint8_t top_middle[22] = {15, 0, 15, 1, 14, 6, 13, 0, 13, 5, 13, 4, 14, 5, 14, 4, 15, 3, 15, 2, 14, 3};
uint8_t top_right[22] = {13, 1, 13, 7, 12, 1, 12, 7, 11, 5, 11, 2, 12, 6, 12, 0, 13, 6, 13, 2, 12, 2};
uint8_t bottom_left[22] = {9, 1, 9, 7, 8, 5, 1, 1, 0, 3, 1, 4, 9, 4, 10, 0, 10, 6, 10, 3, 8, 2};
uint8_t bottom_right[22] = {7, 7, 6, 5, 2, 0, 2, 3, 0, 2, 1, 7, 2, 6, 7, 4, 7, 1, 8, 6, 6, 2};
uint8_t battery_low[2] = {16, 4};
uint8_t dashes[4] = {16, 6, 14, 2};
uint8_t face[14] = {3, 5, 5, 3, 5, 6, 4, 1, 4, 4, 4, 7, 3, 2};
uint8_t face_smile[8] = {4, 1, 5, 6, 3, 2, 4, 7};
uint8_t face_frown[6] = {5, 6, 4, 4, 4, 1};
uint8_t face_neutral[6] = {5, 6, 4, 1, 4, 7};
uint8_t sun[2] = {5, 0};
uint8_t fixed[2] = {16, 5};
uint8_t fixed_deg_C[2] = {14, 2};
uint8_t fixed_deg_F[2] = {16, 6};

// These values closely match times captured with logic analyser
uint8_t delay_SPI_clock_pulse = 8;
uint8_t delay_SPI_end_cycle = 12;

/*

Now define how each digit maps to the segments:

          1
 10 :-----------
    |           |
  9 |           | 2
    |     11    |
  8 :-----------: 3
    |           |
  7 |           | 4
    |     5     |
  6 :----------- 

*/

int digits[16][11] = {
    {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 0},  // 0
    {2, 3, 4, 0, 0, 0, 0, 0, 0, 0, 0},   // 1
    {1, 2, 3, 5, 6, 7, 8, 10, 11, 0, 0}, // 2
    {1, 2, 3, 4, 5, 6, 10, 11, 0, 0, 0}, // 3
    {2, 3, 4, 8, 9, 10, 11, 0, 0, 0, 0}, // 4
    {1, 3, 4, 5, 6, 8, 9, 10, 11, 0, 0}, // 5
    {1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0}, // 6
    {1, 2, 3, 4, 10, 0, 0, 0, 0, 0, 0},  // 7
    {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}, // 8
    {1, 2, 3, 4, 5, 6, 8, 9, 10, 11, 0}, // 9
    {1, 2, 3, 4, 6, 7, 8, 9, 10, 11, 0}, // A
    {3, 4, 5, 6, 7, 8, 9, 10, 11, 0, 0}, // b
    {5, 6, 7, 8, 11, 0, 0, 0, 0, 0, 0},  // c
    {2, 3, 4, 5, 6, 7, 8, 11, 0, 0, 0},  // d
    {1, 5, 6, 7, 8, 9, 10, 11, 0, 0, 0}, // E
    {1, 6, 7, 8, 9, 10, 11, 0, 0, 0, 0}  // F
};

void XiaomiMiaoMiaoCeBT::init(uint8_t redraw)
{
    // set the pin modes (note: no hardware SPI is used)
    pinMode(SPI_ENABLE, OUTPUT);
    pinMode(SPI_MOSI, OUTPUT);
    pinMode(SPI_CLOCK, OUTPUT);
    pinMode(IO_RST_N, OUTPUT);
    pinMode(EPD_TO_PC4, OUTPUT);
    pinMode(IO_BUSY_N, INPUT);

    // set all outputs to 1
    digitalWrite(SPI_ENABLE, HIGH); 
    digitalWrite(SPI_MOSI, HIGH);
    digitalWrite(IO_RST_N, HIGH); 
    digitalWrite(SPI_CLOCK, HIGH);
    digitalWrite(EPD_TO_PC4, HIGH); 

    digitalWrite(SPI_MOSI, LOW);
    digitalWrite(SPI_CLOCK, LOW);

    if(redraw)
    {
        // pulse RST_N low for 110 microseconds
        digitalWrite(IO_RST_N, LOW);
        delayMicroseconds(110);
        digitalWrite(IO_RST_N, HIGH);
              
        // start an initialisation sequence (black - all 0xFF)
        send_sequence(T_LUTV_init, T_LUT_KK_init, T_LUT_KW_init, T_DTM_init, 1);
        // Original firmware pauses here for about 1500 ms
        // in addition to display refresh busy signal
        // Might be necessary in order to fully energise the black particles,
        // but even without this delay the display seems to be working fine
        delay(2000);

        // pulse RST_N low for 110 microseconds
        digitalWrite(IO_RST_N, LOW);
        delayMicroseconds(110);
        digitalWrite(IO_RST_N, HIGH);

        // start an initialisation sequence (white - all 0x00)
        send_sequence(T_LUTV_init, T_LUT_KW_update, T_LUT_KK_update, T_DTM2_init, 1);
        // Original firmware pauses here for about 100 ms
        // in addition to display refresh busy signal.
        // Might be dedicated to sensor data aquisition
        // delay(100);
              
        // pulse RST_N low for 110 microseconds
        digitalWrite(IO_RST_N, LOW);
        delayMicroseconds(110);
        digitalWrite(IO_RST_N, HIGH);
    }
    else
    {
        // Remove all black segments
        //send_sequence(T_LUTV_init, T_LUT_KW_update, T_LUT_KK_update, T_DTM2_init, 1);
    }
    
}

void XiaomiMiaoMiaoCeBT::send_sequence(uint8_t *dataV, uint8_t *dataKK,
                                     uint8_t *dataKW, uint8_t *data,
                                     uint8_t is_init)
{
/*
    if(is_init || transition)
    {
        // pulse RST_N low for 110 microseconds
        digitalWrite(IO_RST_N, LOW);
        delayMicroseconds(110);
        digitalWrite(IO_RST_N, HIGH);
    }
*/
    // send Charge Pump ON command
    transmit(0, POWER_ON);

    // wait for the display to become ready to receive new
    // commands/data: when ready, the display sets IO_BUSY_N to 1
    while (digitalRead(IO_BUSY_N) == 0)
        delay(1);

    // Original firmware pauses here for about 100ms - this time is not required by the display,
    // but is probably dedicated to sensor data aquisition (temperature, humidity and battery).
    //delay(100);

    transmit(0, PANEL_SETTING);
    transmit(1, 0x0B);
    transmit(0, POWER_SETTING);
    transmit(1, 0x46);
    transmit(1, 0x46);
    transmit(0, POWER_OFF_SEQUENCE_SETTING);
    if (is_init)
    {
        transmit(1, 0x00);
    }
    else
    {
        transmit(1, 0x06);
    }
    
    transmit(0, PLL_CONTROL); // Frame Rate Control
    if (is_init)
    {
        transmit(1, 0x02);
    }
    else
    {
        transmit(1, 0x03);
    }

    // NOTE: Original firmware makes partial refresh on update, but not when initialising the screen.
    // Switching the background segment on/off requires full refresh, hence do not send partial refresh
    // command when switching between inverted / non-inverted screen mode.
    if ( !is_init && !transition )
    {
        transmit(0, PARTIAL_DISPLAY_REFRESH);
        transmit(1, 0x00);
        transmit(1, 0x87);
        transmit(1, 0x01);
    }

    // send the e-paper voltage settings (waves)
    transmit(0, LUT_FOR_VCOM);
    for (int i = 0; i < 15; i++)
        transmit(1, dataV[i]);

    if(is_init)
    {
        transmit(0, LUT_CMD_0x23);
        for (int i = 0; i < 15; i++)
            transmit(1, dataKK[i]);

        transmit(0, LUT_CMD_0x26);
        for (int i = 0; i < 15; i++)
            transmit(1, dataKW[i]);
    }
    else
    {
        transmit(0, LUT_CMD_0x23);
        for (int i = 0; i < 15; i++)
            transmit(1, dataV[i]);

        transmit(0, LUT_CMD_0x24);
        for (int i = 0; i < 15; i++)
            transmit(1, dataKK[i]);

        transmit(0, LUT_CMD_0x25);
        for (int i = 0; i < 15; i++)
            transmit(1, dataKW[i]);

        transmit(0, LUT_CMD_0x26);
        for (int i = 0; i < 15; i++)
            transmit(1, dataV[i]);
    }
 
    // send the actual data
    transmit(0, DATA_START_TRANSMISSION_1);
    for (int i = 0; i < 18; i++)
        transmit(1, data[i]);

    while (digitalRead(IO_BUSY_N) == 0)
        delay(1);

    // Original firmware sends DATA_START_TRANSMISSION_2 command only
    // when performing full refresh
    if (is_init && !transition)
    {
        transmit(0, DATA_START_TRANSMISSION_2);
        for(int i = 0; i < 18; i++)
            transmit(1, data[i]);

        while (digitalRead(IO_BUSY_N) == 0)
            delay(1);
    }
    
    if (transition)
    {
        // NOTE: Original firmware doesn't perform this, but I found through experiments
        // that this is the way to clear the black background segment, without a full re-initialisation
        // (also partial refresh should not be sent in this case).
        {
            transmit(0, DATA_START_TRANSMISSION_2);
            for(int i = 0; i < 18; i++)
                transmit(1, ~data[i]);
        }
    }

    transmit(0, DISPLAY_REFRESH);

    // wait for the display to become ready to receive new
    // commands/data: when ready, the display sets IO_BUSY_N to 1
    while (digitalRead(IO_BUSY_N) == 0)
        delay(1);

    // send Charge Pump OFF command
    transmit(0, POWER_OFF);
    transmit(1, 0x03);

    // wait for the display to become ready to receive new
    // commands/data: when ready, the display sets IO_BUSY_N to 1
    while (digitalRead(IO_BUSY_N) == 0)
        delay(1);
}

void XiaomiMiaoMiaoCeBT::transmit(uint8_t cd, uint8_t data_to_send)
{
#if DEBUG_SERIAL
    if (cd == 0)
    {
        Serial.printf("[%lu] Sending COMMAND: 0x%02x\r\n", millis(), data_to_send);
    }
    else
    {
        Serial.printf("[%lu] Sending DATA:    0x%02x\r\n", millis(), data_to_send);
    }
#endif

    // enable SPI
    digitalWrite(SPI_ENABLE, LOW);
    delayMicroseconds(delay_SPI_clock_pulse);

    // send the first bit, this indicates if the following is a command or data
    digitalWrite(SPI_CLOCK, LOW);
    if (cd != 0)
        digitalWrite(SPI_MOSI, HIGH);
    else
        digitalWrite(SPI_MOSI, LOW);
    delayMicroseconds(delay_SPI_clock_pulse * 2 + 1);
    digitalWrite(SPI_CLOCK, HIGH);
    delayMicroseconds(delay_SPI_clock_pulse);

    // send 8 bytes
    for (int i = 0; i < 8; i++)
    {
        // start the clock cycle
        digitalWrite(SPI_CLOCK, LOW);
        // set the MOSI according to the data
        if (data_to_send & 0x80)
            digitalWrite(SPI_MOSI, HIGH);
        else
            digitalWrite(SPI_MOSI, LOW);
        // prepare for the next bit
        data_to_send = (data_to_send << 1);
        delayMicroseconds(delay_SPI_clock_pulse * 2 + 1);
        // the data is read at rising clock (halfway the time MOSI is set)
        digitalWrite(SPI_CLOCK, HIGH);
        delayMicroseconds(delay_SPI_clock_pulse);
    }

    // finish by ending the clock cycle and disabling SPI
    digitalWrite(SPI_CLOCK, LOW);
    delayMicroseconds(delay_SPI_end_cycle);
    digitalWrite(SPI_ENABLE, HIGH);
    delayMicroseconds(delay_SPI_end_cycle);
}

void XiaomiMiaoMiaoCeBT::write_display()
{
    // Send update waveforms
    send_sequence(T_LUTV_init, T_LUT_KK_update, T_LUT_KW_update, display_data, 0);
}

void XiaomiMiaoMiaoCeBT::write_display(uint8_t *data)
{
    for (int i = 0; i < 18; i++)
    {
        display_data[i] = data[i];    
    }

    write_display();
}

void XiaomiMiaoMiaoCeBT::set_digit(uint8_t digit, uint8_t where)
{
    // check if the input is valid
    if ((digit >= 0) && (digit < 16) && (where >= TOP_LEFT) && (where <= BOTTOM_RIGHT))
    {
        // set which segments are to be used
        uint8_t *segments;
        switch(where)
        {
            case TOP_LEFT: segments = top_left; break;
            case TOP_MIDDLE: segments = top_middle; break;
            case TOP_RIGHT: segments = top_right; break;
            case BOTTOM_LEFT: segments = bottom_left; break;
            case BOTTOM_RIGHT: segments = bottom_right; break;
            default:
                break;
        }

        // set the segments, there are up to 11 segments in a digit
        int segment_byte;
        int segment_bit;
        for (int i = 0; i < 11; i++)
        {
            // get the segment needed to display the digit 'digit',
            // this is stored in the array 'digits'
            int segment = digits[digit][i] - 1;
            // segment = -1 indicates that there are no more segments to display
            if (segment >= 0)
            {
                segment_byte = segments[2 * segment];
                segment_bit = segments[1 + 2 * segment];
                set_segment(segment_byte, segment_bit, 1);
            }
            else
                // there are no more segments to be displayed
                break;
        }
    }
}

void XiaomiMiaoMiaoCeBT::set_shape(uint8_t where)
{
    int num_of_segments = 0;
    uint8_t *segments = NULL;

    // set the number of segments and which segments has to be displayed
    switch (where)
    {
        case TOP_LEFT_1:
            num_of_segments = sizeof(top_left_1) / 2;
            segments = top_left_1;
            break;
        case BATTERY_LOW:
            num_of_segments = sizeof(battery_low) / 2;
            segments = battery_low;
            break;
        case DASHES:
            num_of_segments = sizeof(dashes) / 2;
            segments = dashes;
            break;
        case FACE:
            num_of_segments = sizeof(face) / 2;
            segments = face;
            break;
        case FACE_SMILE:
            num_of_segments = sizeof(face_smile) / 2;
            segments = face_smile;
            break;
        case FACE_FROWN:
            num_of_segments = sizeof(face_frown) / 2;
            segments = face_frown;
            break;
        case FACE_NEUTRAL:
            num_of_segments = sizeof(face_neutral) / 2;
            segments = face_neutral;
            break;
        case SUN:
            num_of_segments = sizeof(sun) / 2;
            segments = sun;
            break;
        case FIXED:
            num_of_segments = sizeof(fixed) / 2;
            segments = fixed;
            break;
        case FIXED_DEG_C:
            num_of_segments = sizeof(fixed_deg_C) / 2;
            segments = fixed_deg_C;
            break;
        case FIXED_DEG_F:
            num_of_segments = sizeof(fixed_deg_F) / 2;
            segments = fixed_deg_F;
            break;
        default:
            return;
    }

    // set the segments
    for (uint8_t segment = 0; segment < num_of_segments; segment++)
    {
        uint8_t segment_byte = segments[2 * segment];
        uint8_t segment_bit = segments[1 + 2 * segment];
        set_segment(segment_byte, segment_bit, 1);
    }
}

void XiaomiMiaoMiaoCeBT::set_segment(uint8_t segment_byte, uint8_t segment_bit,
                                   uint8_t value)
{
    // depending on whether the display is inverted and the desired value
    // the bit needs to be set or cleared
    if (((inverted == 0) && (value == 1)) ||
        ((inverted == 1) && (value == 0)))
        // set the bit
        display_data[segment_byte] |= (1 << segment_bit);
    else
        // remove the bit
        display_data[segment_byte] &= ~(1 << segment_bit);
}

void XiaomiMiaoMiaoCeBT::start_new_screen(uint8_t _inverted)
{
    // Set transition flag, indicating if the background
    // segment needs to be set or cleared
    if (_inverted != inverted)
        transition = 1;
    else
        transition = 0;

     if (_inverted == 1)
        inverted = 1;
    else
        inverted = 0;

    // prepare the data to be displayed, assume all segments are either on or off
    if (_inverted)
    {
        for (int i = 0; i < 18; i++)
            display_data[i] = 0xFF;

        // set the bit to switch the background to black,
        // use value=0 because of inversion
        set_segment(17, 7, 0);
    }
    else
    {
        for (int i = 0; i < 18; i++)
           display_data[i] = 0x00;
    }
}
