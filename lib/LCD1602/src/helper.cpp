#include <string.h>
#include <Arduino.h>
#define BUFLEN 170

char* convertUsingCustomChars(const char* str, bool uppercase) {

  int index = 0;
  static char strn[BUFLEN];
  static char newStr[BUFLEN];
  bool E = false;
  strlcpy(strn, str, BUFLEN);
  newStr[0] = '\0';
  bool next = false;

  while (*str) {
    byte charIndex = 255;
    if ((*str & 0xE0) == 0xC0 || (*str & 0xF0) == 0xE0 || (*str & 0xF8) == 0xF0) {
      // UTF-8 karakterek kezelése
      uint16_t code = 0;
      if ((*str & 0xE0) == 0xC0) { // 2 bájtos UTF-8
        code = ((*str & 0x1F) << 6) | (*(str + 1) & 0x3F);
        str += 2;
      } else if ((*str & 0xF0) == 0xE0) { // 3 bájtos UTF-8
        code = ((*str & 0x0F) << 12) | ((*(str + 1) & 0x3F) << 6) | (*(str + 2) & 0x3F);
        str += 3;
      } else if ((*str & 0xF8) == 0xF0) { // 4 bájtos UTF-8
        code = ((*str & 0x07) << 18) | ((*(str + 1) & 0x3F) << 12) | ((*(str + 2) & 0x3F) << 6) | (*(str + 3) & 0x3F);
        str += 4;
      }
      ESP_LOGI("HELPER","%d",code);
      switch (code) {
        case 0x00C1: charIndex = 1; break; // Á
        case 0x00E1: charIndex = 1; break; // á
        case 0x00C9: charIndex = 2; break; // É
        case 0x00E9: charIndex = 2; break; // é
        case 0x00CD: newStr[index++] = 'i';  continue; // Í
        case 0x00ED: newStr[index++] = 'I';  continue; // í
        case 0x00D3: charIndex = 3; break; // Ó
        case 0x00F3: charIndex = 3; break; // ó
        case 0x00D6: charIndex = 4; break; // Ö
        case 0x00F6: charIndex = 4; break; // ö
        case 0x0150: charIndex = 5; break; // Ő
        case 0x0151: charIndex = 5; break; // ő
        case 0x00DA: charIndex = 6; break; // Ú
        case 0x00FA: charIndex = 6; break; // ú
        case 0x00DC: charIndex = 7; break; // Ü
        case 0x00FC: charIndex = 7; break; // ü
        case 0x0170: charIndex = 7; break; // Ű
        case 0x0171: charIndex = 7; break; // ű
        default: {
            newStr[index++] ='?'; continue; // Ismeretlen karakter helyettesítése kérdőjellel
        }
      }
    }

    if (charIndex != 255) {
      newStr[index++] = charIndex;
    }
    else {
      newStr[index++] = *str; // Normál ASCII karakterek kezelése
      str++;
    }
  } //end while
  newStr[index] = 0;
  return newStr;
}