#include <string.h>
#include <Arduino.h>
#include "helper.h"

char *convertUsingCustomChars(const char *str, bool uppercase)
{

  int index = 0;
  static char strn[BUFLEN];
  static char newStr[BUFLEN];
  bool E = false;
  strlcpy(strn, str, BUFLEN);
  bool next = false;

  while (*str)
  {
    byte charIndex = 255;
    if ((*str & 0xE0) == 0xC0 || (*str & 0xF0) == 0xE0 || (*str & 0xF8) == 0xF0)
    {
      // UTF-8 karakterek kezelése
      uint16_t code = 0;
      if ((*str & 0xE0) == 0xC0)
      { // 2 bájtos UTF-8
        code = ((*str & 0x1F) << 6) | (*(str + 1) & 0x3F);
        str += 2;
      }
      else if ((*str & 0xF0) == 0xE0)
      { // 3 bájtos UTF-8
        code = ((*str & 0x0F) << 12) | ((*(str + 1) & 0x3F) << 6) | (*(str + 2) & 0x3F);
        str += 3;
      }
      else if ((*str & 0xF8) == 0xF0)
      { // 4 bájtos UTF-8
        code = ((*str & 0x07) << 18) | ((*(str + 1) & 0x3F) << 12) | ((*(str + 2) & 0x3F) << 6) | (*(str + 3) & 0x3F);
        str += 4;
      }
      //ESP_LOGI("HELPER", "%d", code);
      switch (code)
      {
      case 0x00C1:
      case 0x00E1:
        charIndex = SMALL_A;
        break;
      case 0x00C9:
      case 0x00E9:
        charIndex = SMALL_E;
        break;
      case 0x00CD:
      case 0x00ED:
        charIndex = SMALL_I;
        break;
      case 0x00D3:
      case 0x00F3:
        charIndex = SMALL_O;
        break;
      case 0x00D6:
      case 0x00F6:
      case 0x0150:
      case 0x0151:
        charIndex = SMALL_OO;
        break;
      case 0x00DA:
      case 0x00FA:
        charIndex = SMALL_U;
        break;
      case 0x0170:
      case 0x0171:
      case 0x00DC:
      case 0x00FC:
        charIndex = SMALL_UU;
        break;
      default:
      {
        charIndex = 63; //?
      }
      }
    }

    if (charIndex != 255)
    {
      newStr[index] = charIndex;
    }
    else
    {
      newStr[index] = tolower(*str); // Normál ASCII karakterek kezelése
      str++;
    }
    index++;

  } // end while
  newStr[index] = '\0';
  return newStr;
}