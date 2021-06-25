#ifndef STACK_H_
#define STACK_H_

#include "Arduino.h"
#include "instruction_set.h"
#include "utils.h"

#define STACK_SIZE 32

//union PROGMEM StackItem {
//  char s_[12];
//  int i_;
//  float f_;
//};

class Stack {

 public:
  uint8_t stack_[STACK_SIZE] = { 0 };
  uint8_t pos_ = 0;
//  struct stack_t {
//    uint8_t stack[STACK_SIZE];
//    uint8_t pos = 0;
//  };

 private:
  bool peeked_ = false;

 public:
  Stack();
  ~Stack();

  void Reset();

  /**
   * @brief Push byte to stack
   * @param elem
   */
  void PushByte(uint8_t elem);

  /**
   * @brief Pop byte from stack
   * @return Popped byte
   */
  uint8_t PopByte();

  /**
   * @brief Push char to stack
   * @param elem
   */
  void PushChar(char elem);

  /**
   * @brief Pop char from task
   * @return Popped char
   */
  char PopChar();

  /**
   * @brief Push int to stack
   * @param elem
   */
  void PushInt(int elem);

  /**
   * @brief Pop int from stack
   * @return Popped int
   */
  int PopInt();

  /**
   * @brief Push float to stack
   * @param elem
   */
  void PushFloat(float elem);

  /**
   * @brief Pop float from stack
   * @return Popped float
   */
  float PopFloat();

  /**
   * @brief Push string to stack
   * @param elem
   */
  void PushString(char* elem);

  /**
   * @brief Pop string from stack
   * @return Popped string
   */
  char* PopString();
  /**
   * @brief Look for the next value
   * @return value
   */
  float Peek();

  /**
   * @brief Pop value where type does not matter
   *        Looks for type on the stack itself
   * @return Popped value
   */
  float PopValue();

};

#endif //STACK_H_
