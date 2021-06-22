#ifndef PROGRAM_TEST_H_
#define PROGRAM_TEST_H_

#include "instruction_set.h"

uint8_t test_hello[] = {
    STRING,'h','e','l','l','o',' ','w','o','r','l','d','\n',0,
    PRINT,
    STOP
};

// program test_vars
uint8_t test_vars[] = {
    STRING,'t','e','s','t',0,SET,'s',
    CHAR,'a',SET,'c',
    INT,1,7,SET,'i', // 1 * 256 + 7 = 263
    FLOAT,66,246,230,102,SET,'f', //123.45
    STRING,'p','a','s','s','e','d',0,SET,'s',
    GET,'c',INCREMENT,SET,'c',
    GET,'i',DECREMENT,SET,'i',
    GET,'f',INCREMENT,SET,'f',
    GET,'s',PRINTLN,
    GET,'c',PRINTLN,
    GET,'i',PRINTLN,
    GET,'f',PRINTLN,
    STOP
};

uint8_t test_loop[] = {
    INT,0,0,SET,'i',
    LOOP,
    GET,'i',INCREMENT,SET,'i',
    GET,'i',PRINTLN,
    MILLIS,INT,3,232,PLUS, // 1000
    DELAYUNTIL,
    ENDLOOP
};

uint8_t test_if[] = {
    INT,0,3,SET,'a',
    CHAR,5,SET,'b',
    GET,'a',GET,'b',EQUALS,
    IF,7,
    STRING,'T','r','u','e',0,
    PRINTLN,
    ELSE,8,
    STRING,'F','a','l','s','e',0,
    PRINTLN,
    ENDIF,
    CHAR,3,SET,'b',
    GET,'a',GET,'b',EQUALS,
    IF,7,
    STRING,'T','r','u','e',0,
    PRINTLN,
    ELSE,8,
    STRING,'F','a','l','s','e',0,
    PRINTLN,
    ENDIF,
    STOP
};

uint8_t test_while[] = {
    INT,0,0,SET,'i',
    GET,'i',CHAR,5,LESSTHAN,
    WHILE,5,14,
    GET,'i',INCREMENT,SET,'i',
    GET,'i',PRINTLN,
    MILLIS,INT,3,232,PLUS, // 1000
    DELAYUNTIL,
    ENDWHILE,
    STOP
};

uint8_t test_blink[] = {
    INT,0,LED_BUILTIN,SET,'p',
    GET,'p',INT,0,OUTPUT,PINMODE,
    LOOP,
    GET,'p',INT,0,HIGH,DIGITALWRITE,
    MILLIS,INT,1,244,PLUS, // 500
    DELAYUNTIL,
    GET,'p',INT,0,LOW,DIGITALWRITE,
    MILLIS,INT,1,244,PLUS, // 500
    DELAYUNTIL,
    ENDLOOP
};

uint8_t test_read[] = {
    CHAR,20,SET,'i',
    STRING,'t','e','s','t','_','v','a','r','s',0,
    INT,0,0,
    OPEN,
    GET,'i',
    WHILE,2,8,
    READCHAR,TOINT,PRINTLN,
    GET,'i',DECREMENT,SET,'i',
    ENDWHILE,
    STOP
};

uint8_t test_write[] = {
    STRING,'n','e','w','_','f','i','l','e',0,SET,'s',
    GET,'s',
    INT,0,20,
    OPEN,
    STRING,'t','e','x','t','\n',0,WRITE,
    CHAR,'a',WRITE,
    INT,98,99,WRITE, // 25187
    FLOAT,61,204,204,205,WRITE, // 0.1
    GET,'s',
    INT,0,20,
    OPEN,
    READSTRING,PRINTLN,
    READCHAR,PRINTLN,
    READINT,PRINTLN,
    READFLOAT,PRINTLN,
    STOP
};

uint8_t test_fork[] = {
    INT,0,0,SET,'i',
    LOOP,
    GET,'i',INCREMENT,SET,'i',
    GET,'i',PRINTLN,
    GET,'i',CHAR,5,EQUALS,
    IF,14,
    STRING,'t','e','s','t','_','w','h','i','l','e',0,
    FORK,WAITUNTILDONE,
    ENDIF,
    MILLIS,INT,3,232,PLUS, // 1000
    DELAYUNTIL,
    ENDLOOP
};

#endif //PROGRAM_TEST_H_
