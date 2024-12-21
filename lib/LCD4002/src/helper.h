#ifndef HELPER_H
#define HELPER_H

#define BUFLEN 170

//#define BIG_A 1      // Á
#define SMALL_A 1    // á
//#define BIG_E 3      // É
#define SMALL_E 2    // é
//#define BIG_I 5      // Í
#define SMALL_I 3    // í
//#define BIG_O 7      // Ó
#define SMALL_O 4    // ó
//#define BIG_OO 9     // Ö
#define SMALL_OO 5  // ö
//#define BIG_OOO 11   // Ő
//#define SMALL_OOO 5 // ő
//#define BIG_U 13     // Ú
#define SMALL_U 6  // ú
//#define BIG_UU 15    // Ü
#define SMALL_UU 7  // ü
//#define BIG_UUU 17   // Ű
//#define SMALL_UUU 8 // ű

char *convertUsingCustomChars(const char *str, bool uppercase);

#endif
