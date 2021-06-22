#ifndef OPERATION_H_
#define OPERATION_H_

#include "Arduino.h"
#include "EEPROM.h"
#include "processing.h"
#include "fat.h"
#include "stack.h"
#include "instruction_set.h"
#include "utils.h"

class Operation {

 public:
  Operation();
  ~Operation();

  /**
   * @brief Stack operations
   * @param process
   * @param data_type
   */
  static void StackOp(Processing::Process* process, uint8_t data_type);

  /**
   * @brief Unary operations
   * @param process
   * @param operation
   */
  static void UnaryOp(Processing::Process* process, uint8_t operation);

  /**
   * @brief Math operations
   * @param process
   * @param operation
   */
  static void BinaryOp(Processing::Process* process, uint8_t operation);

  /**
   * @brief Time operations
   * @param process
   * @param operation
   */
  static void TimeOp(Processing::Process* process, uint8_t operation);

  /**
   * @brief Hardware Input/Output operations
   * @param process
   * @param operation
   */
  static void IoOp(Processing::Process* process, uint8_t operation);

  /**
   * @brief Conditional operations
   * @param process
   * @param operation
   */
  static void ConditionalOp(Processing::Process* process, uint8_t operation);

  /**
   * @brief File manipulation operations
   * @param process
   * @param operation
   */
  static void FileOp(Processing::Process* process, uint8_t operation);

  /**
   * @brief Serial print operations
   * @param process
   * @param new_line
   */
  static void PrintOp(Processing::Process* process, bool new_line = false);
};

#endif //OPERATION_H_
