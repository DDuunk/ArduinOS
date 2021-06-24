#ifndef STACK_H_
#define STACK_H_

#include "Arduino.h"
#include "instruction_set.h"
#include "utils.h"

#define STACK_SIZE 32

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

//  /**
//   * @brief PushByte byte to stack
//   * @param stack
//   * @param elem
//   */
//  void PushByte(stack_t* stack, uint8_t elem);
//
//  /**
//   * @brief PopByte byte from stack
//   * @param stack
//   * @return Popped byte
//   */
//  uint8_t PopByte(stack_t* stack);
//
//  /**
//   * @brief PushByte char to stack
//   * @param stack
//   * @param elem
//   */
//  void PushChar(stack_t* stack, char elem);
//
//  /**
//   * @brief PopByte char from stack
//   * @param stack
//   * @return Popped char
//   */
//  char PopChar(stack_t* stack);
//
//  /**
//   * @brief PushByte int to stack
//   * @param stack
//   * @param elem
//   */
//  void PushInt(stack_t* stack, int elem);
//
//  /**
//   * @brief PopByte int from stack
//   * @param stack
//   * @return Popped int
//   */
//  int PopInt(stack_t* stack);
//
//  /**
//   * @brief PushByte float to stack
//   * @param stack
//   * @param elem
//   */
//  void PushFloat(stack_t* stack, float elem);
//
//  /**
//   * @brief PopByte float from stack
//   * @param stack
//   * @return Popped float
//   */
//  float PopFloat(stack_t* stack);
//
//  /**
//   * @brief PushByte string to stack
//   * @param stack
//   * @param elem
//   */
//  void PushString(stack_t* stack, char* elem);
//
//  /**
//   * @brief PopByte string from stack
//   * @param stack
//   * @return Popped stack
//   */
//  char* PopString(stack_t* stack);
//
//  /**
//   * @brief Look for next value
//   * @param stack
//   * @return value
//   */
//  float Peek(stack_t* stack);
//
//  /**
//   * @brief PopByte value where type does not matter
//   *        Looks for type on the stack itself
//   * @param stack
//   * @return Popped value
//   */
//  float PopValue(stack_t* stack);
//
//  /**
//   * @brief Print stack
//   * @param stack
//   */
//  void PrintStack(stack_t* stack);

};

#endif //STACK_H_
