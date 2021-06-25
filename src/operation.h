#ifndef OPERATION_H_
#define OPERATION_H_

#include "Arduino.h"
#include "EEPROM.h"
#include "processing.h"
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
   * @return 0 (error-code: OK) || -1 (error-code: FAILED)
   */
  static int StackOp(Processing::Process* process, uint8_t data_type);

  /**
   * @brief Unary operations
   * @param process
   * @param operation
   * @return 0 (error-code: OK) || -1 (error-code: FAILED)
   */
  static int UnaryOp(Processing::Process* process, uint8_t operation);

  /**
   * @brief Math operations
   * @param process
   * @param operation
   * @return 0 (error-code: OK) || -1 (error-code: FAILED)
   */
  static int BinaryOp(Processing::Process* process, uint8_t operation);

  /**
   * @brief Time operations
   * @param process
   * @param operation
   * @return 0 (error-code: OK) || -1 (error-code: FAILED)
   */
  static int TimeOp(Processing::Process* process, uint8_t operation);

  /**
   * @brief Hardware Input/Output operations
   * @param process
   * @param operation
   * @return 0 (error-code: OK) || -1 (error-code: FAILED)
   */
  static int IoOp(Processing::Process* process, uint8_t operation);

  /**
   * @brief Conditional operations
   * @param process
   * @param operation
   * @return 0 (error-code: OK) || -1 (error-code: FAILED)
   */
  static int ConditionalOp(Processing::Process* process, uint8_t operation);

  /**
   * @brief File manipulation operations
   * @param process
   * @param operation
   * @return 0 (error-code: OK) || -1 (error-code: FAILED)
   */
  static int FileOp(Processing::Process* process, uint8_t operation);

  /**
   * @brief Serial print operations
   * @param process
   * @param new_line
   * @return 0 (error-code: OK) || -1 (error-code: FAILED)
   */
  static int PrintOp(Processing::Process* process, bool new_line = false);

};

#endif //OPERATION_H_
