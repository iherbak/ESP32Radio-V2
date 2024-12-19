//***************************************************************************************************
//*  LCD1602.h -- Driver for LCD 1602 display with I2C backpack.                                    *
//***************************************************************************************************
// The backpack communicates with the I2C bus and converts the serial data to parallel for the      *
// 1602 board.                                                                                      *
// Do not forget the PULL-UP resistors (4.7k on both SDA and CLK).                                  *
// In the serial data, the 8 bits are assigned as follows:                                          *
// Bit   Destination  Description                                                                   *
// ---   -----------  ------------------------------------                                          *
//  0    RS           H=data, L=command                                                             *
//  1    RW           H=read, L=write.  Only write is used.                                         *
//  2    E            Enable                                                                        *
//  3    BL           Backlight, H=on, L=off.  Always on.                                           *
//  4    D4           Data bit 4                                                                    *
//  5    D5           Data bit 5                                                                    *
//  6    D6           Data bit 6                                                                    *
//  7    D7           Data bit 7                                                                    *
//***************************************************************************************************
//
// Note that the display function are limited due to the minimal available space.

#include <Wire.h>
#include "LCD1602.h"
#include <vector>

scrseg_struct LCD1602_tftdata[TFTSECS] = // Screen divided in 4 segments
    {
        {true, 0, WHITE, 0, 0, RADIO_TITLE_MAX_LENGTH, "", 0, 0},                                // 1 title
        {true, 0, WHITE, 0, 1, RDS_MAX_LENGTH, "", 0, 0},                                        // 2 rds info
        {false, 0, WHITE, dsp_getwidth() - CLOCK_MAX_LENGTH, 0, CLOCK_MAX_LENGTH, "", 0, 0},     // 3 clock
        {false, 0, WHITE, dsp_getwidth() - BITRATE_MAX_LENGTH, 1, BITRATE_MAX_LENGTH, "", 0, 0}, // 3 bitrate

};

LCD1602 *LCD1602_tft = NULL;

const char *LTAG = "LCD1602"; // For debugging

int pos = 0;

//***********************************************************************************************
//                                L C D 1 6 0 2  write functions                                *
//***********************************************************************************************
// Write functins for command, data and general.                                                *
//***********************************************************************************************
void LCD1602::swrite(uint8_t val, uint8_t rs) // General write, 8 bits data
{
  strobe((val & 0xf0) | rs); // Send 4 LSB bits
  strobe((val << 4) | rs);   // Send 4 MSB bits
}

void LCD1602::write_data(uint8_t val)
{
  swrite(val, FLAG_RS_DATA); // Send data (RS = HIGH)
}

void LCD1602::write_cmd(uint8_t val)
{
  swrite(val, FLAG_RS_COMMAND); // Send command (RS = LOW)
}

//***********************************************************************************************
//                                L C D 1 6 0 2 :: S T R O B E                                  *
//***********************************************************************************************
// Send data followed by strobe to clock data to LCD.                                           *
//***********************************************************************************************
void LCD1602::strobe(uint8_t cmd)
{
  scommand(cmd | FLAG_ENABLE);                  // Send command with E high
  scommand(cmd);                                // Same command with E low
  delayMicroseconds(DELAY_ENABLE_PULSE_SETTLE); // Wait a short time
}

//***********************************************************************************************
//                                L C D 1 6 0 2 :: S C O M M A N D                              *
//***********************************************************************************************
// Send a command to the LCD.                                                                   *
// Actual I/O.  Open a channel to the I2C interface and write one byte.                         *
//***********************************************************************************************
void LCD1602::scommand(uint8_t cmd)
{
  Wire.beginTransmission(LCD_I2C_ADDRESS);
  Wire.write(cmd | bl); // Add command including BL state
  Wire.endTransmission();
}

//***********************************************************************************************
//                                L C D 1 6 0 2 :: P R I N T                                    *
//***********************************************************************************************
// Put a character in the buffer.                                                               *
//***********************************************************************************************
void LCD1602::print(char c)
{
  write_data(c);
}

void LCD1602::printstring(String &text)
{
  int newline = text.indexOf("\n");
  ESP_LOGI(LTAG, "String is %s newline at %d", text.c_str(), newline);
  std::vector<String> lines;
  lines.reserve(2);
  if (newline > 0)
  {
    lines.push_back(text.substring(0, newline));
    lines.push_back(text.substring(newline + 1, text.length() - 1));
  }
  else
  {
    lines.push_back(text);
  }
  for (int i = 0; i < lines.size(); i++)
  {
    ESP_LOGI(LTAG, "Line%d: %s", i, lines[i].c_str());
    LCD1602_tft->scursor(0, i);
    const char *p;
    p = lines[i].c_str();
    int l = strlen(p);
    // ESP_LOGI(LTAG,"String is %s length is (%d)",p, l);
    for (int i = 0; i < l; i++)
    {
      write_data(*p++);
    }
  }
}

//***********************************************************************************************
//                                L C D 1 6 0 2 :: S C U R S O R                                *
//***********************************************************************************************
// Place the cursor at the requested position.                                                  *
//***********************************************************************************************
void LCD1602::scursor(uint8_t col, uint8_t row)
{
  const int row_offsets[] = {0x00, 0x40, 0x14, 0x54};

  write_cmd(COMMAND_SET_DDRAM_ADDR |
            (col + row_offsets[row]));
}

//***********************************************************************************************
//                                L C D 1 6 0 2 :: S C L E A R                                  *
//***********************************************************************************************
// Clear the LCD.                                                                               *
//***********************************************************************************************
void LCD1602::sclear()
{
  write_cmd(COMMAND_CLEAR_DISPLAY);
}

//***********************************************************************************************
//                                L C D 1 6 0 2 :: S C R O L L                                  *
//***********************************************************************************************
// Set scrolling on/off.                                                                        *
//***********************************************************************************************
void LCD1602::scroll(bool son)
{
  uint8_t ecmd = COMMAND_ENTRY_MODE_SET | // Assume no scroll
                 FLAG_ENTRY_MODE_SET_ENTRY_INCREMENT;

  if (son) // Scroll on?
  {
    ecmd |= FLAG_ENTRY_MODE_SET_ENTRY_SHIFT_ON; // Yes, change function
  }
  write_cmd(ecmd); // Perform command
}

//***********************************************************************************************
//                                L C D 1 6 0 2 :: S H O M E                                    *
//***********************************************************************************************
// Go to home position.                                                                         *
//***********************************************************************************************
void LCD1602::shome()
{
  write_cmd(COMMAND_RETURN_HOME);
}

//***********************************************************************************************
//                                L C D 1 6 0 2 :: R E S E T                                    *
//***********************************************************************************************
// Reset the LCD.                                                                               *
//***********************************************************************************************
void LCD1602::reset()
{
  scommand(0); // Put expander to known state
  delayMicroseconds(1000);
  for (int i = 0; i < 3; i++) // Repeat 3 times
  {
    strobe(0x03 << 4); // Select 4-bit mode
    delayMicroseconds(4500);
  }
  strobe(0x02 << 4); // 4-bit
  delayMicroseconds(4500);
  write_cmd(COMMAND_FUNCTION_SET |
            FLAG_FUNCTION_SET_MODE_4BIT |
            FLAG_FUNCTION_SET_LINES_2 |
            FLAG_FUNCTION_SET_DOTS_5X8);
  write_cmd(COMMAND_DISPLAY_CONTROL |
            FLAG_DISPLAY_CONTROL_DISPLAY_ON);
  sclear();
  write_cmd(COMMAND_ENTRY_MODE_SET |
            FLAG_ENTRY_MODE_SET_ENTRY_INCREMENT);
  shome();
  for (char a = 'a'; a < 'q'; a++)
  {
    print(a);
  }
}

//**************************************************************************************************
//                                          I 2 C S C A N                                          *
//**************************************************************************************************
// Utility to scan the I2C bus.                                                                    *
//**************************************************************************************************
// void i2cscan()
// {
//   byte error, address ;

//   ESP_LOGI ( LTAG, "Scanning I2C bus..." ) ;

//   for ( address = 1 ; address < 127 ; address++ )
//   {
//     Wire.beginTransmission ( address ) ;
//     error = Wire.endTransmission() ;
//     if ( error == 0 )
//     {
//       ESP_LOGI ( LTAG, "I2C device 0x%02X found", address ) ;
//     }
//     else if ( error == 4 )
//     {
//       ESP_LOGE ( LTAG, "Error 4 at address 0x%02X", address ) ;
//     }
//   }
// }

//***********************************************************************************************
//                                L C D 1 6 0 2                                                 *
//***********************************************************************************************
// Constructor for the display.                                                                 *
//***********************************************************************************************
LCD1602::LCD1602(int8_t sda, int8_t scl)
{
  uint8_t error;

  if (!Wire.begin(sda, scl)) // Init I2c
  {
    ESP_LOGE(LTAG, "I2C driver install error!");
  }
  else
  {
    // i2cscan() ;                                               // Scan I2C bus
    Wire.beginTransmission(LCD_I2C_ADDRESS);
    error = Wire.endTransmission();
    if (error)
    {
      ESP_LOGE(LTAG, "Display not found on I2C 0x%02X",
               LCD_I2C_ADDRESS);
    }
  }
  reset();
}

bool LCD1602_dsp_begin(int8_t sda, int8_t scl)
{
  ESP_LOGI(LTAG, "Init LCD1602, I2C pins %d,%d", sda, scl);
  if ((sda >= 0) && (scl >= 0))
  {
    LCD1602_tft = new LCD1602(sda, scl); // Create an instance for TFT
  }
  else
  {
    ESP_LOGE(LTAG, "Init LCD1602 failed!");
  }
  return (LCD1602_tft != NULL);
}

//***********************************************************************************************
//                                D S P _U P D A T E _ L I N E                                  *
//***********************************************************************************************
// Show a selected line                                                                         *
//***********************************************************************************************
void LCD1602_dsp_update_line(uint8_t lnr)
{
  // ESP_LOGI(LTAG,"Refreshing %d widget",lnr);
  const char *p;

  // ESP_LOGI(LTAG,"%s",toshow.c_str());
  uint segmentlen = LCD1602_tftdata[lnr].len;
  uint fulllength = LCD1602_tftdata[lnr].str.length();

  LCD1602_tft->scursor(LCD1602_tftdata[lnr].col, LCD1602_tftdata[lnr].row);

  if (fulllength > 0)
  {
    int sliceend = std::min(LCD1602_tftdata[lnr].pos + segmentlen, fulllength);
    // ESP_LOGI(LTAG,"%s",LCD1602_tftdata[lnr].str.c_str());
    //  ESP_LOGI(LTAG, "%s, Full: %d, SegStart:%d, SegEnd:%d", LCD1602_tftdata[lnr].str.substring(LCD1602_tftdata[lnr].pos, sliceend - 1).c_str(), fulllength, LCD1602_tftdata[lnr].pos, sliceend);
    p = LCD1602_tftdata[lnr].str.c_str();
    p += LCD1602_tftdata[lnr].pos;
    for (int i = LCD1602_tftdata[lnr].pos; i < sliceend; i++)
    {
      if ((*p >= ' ') && (*p <= '~')) // Printable?
      {
        LCD1602_tft->print(*p); // Yes
      }
      else
      {
        LCD1602_tft->print(' '); // Yes, print space
      }
      p++;
    }

    if (sliceend != fulllength)
    {
      LCD1602_tftdata[lnr].pos++;
    }
    else
    {
      // restart scroll from 0
      LCD1602_tftdata[lnr].pos = 0;
    }
    if (fulllength <= segmentlen && tftdata[lnr].scrollable)
    {
      ESP_LOGI(LTAG, "Section %d switching to non scroll", lnr);
    }
    if (fulllength > segmentlen && !tftdata[lnr].scrollable){
      ESP_LOGI(LTAG, "Section %d switching to scroll", lnr);
    }
    tftdata[lnr].scrollable = fulllength <= segmentlen ? false : true;
  }
  else
  {
    // clear space
    for (int i = 0; i < LCD1602_tftdata[lnr].len; i++)
    {
      LCD1602_tft->print(' ');
    }
  }
}

//***********************************************************************************************
//                                D S P _U P D A T E                                            *
//***********************************************************************************************
// Show a selection of the 4 sections                                                           *
//***********************************************************************************************

void replacespecchars(String &s)
{
  s.replace("''", "");
  s.replace("á", "a");
  s.replace("Á", "A");
  s.replace("é", "e");
  s.replace("É", "E");
  s.replace("ó", "o");
  s.replace("Ó", "O");
  s.replace("ö", "o");
  s.replace("Ö", "O");
  s.replace("ú", "u");
  s.replace("Ú", "U");
  s.replace("ü", "u");
  s.replace("Ű", "u");
  for (int i = 0; i < s.length(); i++)
  {
    if ((s[i] < ' ') || (s[i] > '~'))
    {
      s[i] = ' ';
    }
  }
}

void filltexttomax(String &s, int len)
{
  s.trim(); // Remove non printing
  replacespecchars(s);
  if (s.length() < len)
  {
    uint8_t fill = len - s.length();
    // ESP_LOGI(LTAG, "Str: %s, fill: %d front: %s", s, fill, fillfront ? "true" : "false");
    for (int i = 0; i < fill; i++)
    {
      s += " ";
    }
  }
}

void LCD1602_dsp_update(bool isvolume, int section)
{
  if (LCD1602_tftdata[section].scrollable)
  {
    LCD1602_tftdata[section].refreshCount++;
    if (LCD1602_tftdata[section].refreshCount < 7) // Action every 8 calls
    {
      return;
    }
    LCD1602_tftdata[section].refreshCount = 0;
  }

  if (!isvolume) // Encoder menu mode?
  {
    // dline[0].str = LCD1602_tftdata[3].str.substring(0, 16)); // Yes, different lines
    // dline[1].str = LCD1602_tftdata[3].str.substring(16);
  }
  else
  {
    // for (int i = 0; i < TFTSECS; i++)
    //{
    //}
  }
  // for (int i = 0; i < TFTSECS; i++)
  //{
  //  if (LCD1602_tftdata[i].update_req)
  // {
  // LCD1602_dsp_update_line(i);
  // LCD1602_tftdata[i].update_req = false;
  //}
  //}
  LCD1602_tftdata[section].str.trim();
  filltexttomax(LCD1602_tftdata[section].str, LCD1602_tftdata[section].len);
  LCD1602_dsp_update_line(section);
}

//**************************************************************************************************
//                                      D I S P L A Y B A T T E R Y                                *
//**************************************************************************************************
// Dummy routine for this type of display.                                                         *
//**************************************************************************************************
void LCD1602_displaybattery(uint16_t bat0, uint16_t bat100, uint16_t adcval)
{
}

//**************************************************************************************************
//                                      D I S P L A Y V O L U M E                                  *
//**************************************************************************************************
// Dummy routine for this type of display.                                                         *
//**************************************************************************************************
void LCD1602_displayvolume(uint8_t vol)
{
}

//**************************************************************************************************
//                                      D I S P L A Y T I M E                                      *
//**************************************************************************************************
// Dummy routine for this type of display.                                                         *
//**************************************************************************************************
void LCD1602_displaytime(const char *str, uint16_t color)
{
  // String s = String(str);
  // LCD1602_tftdata[2].str = s;
  // LCD1602_tftdata[2].idle = false;
  // ESP_LOGI(LTAG,"Time is: %s",str);
  // LCD1602_dsp_update_line(2);
}
//**************************************************************************************************
//                                      D I S P L A Y T I M E                                      *
//**************************************************************************************************
// Dummy routine for this type of display.                                                         *
//**************************************************************************************************

void LCD1602_displaybitrate(const char *str, uint16_t color)
{
  // String s = String(str);
  // s.trim();
  // LCD1602_tftdata[3].str = s;
  // filltexttomax(LCD1602_tftdata[3].str, LCD1602_tftdata[3].len);
  // LCD1602_tftdata[3].idle = false;
  // ESP_LOGI(LTAG, "Bitrate is: %s len: %d", s, s.length());
}
