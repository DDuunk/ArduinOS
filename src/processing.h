#ifndef PROCESSING_H_
#define PROCESSING_H_

#include "Arduino.h"
#include "EEPROM.h"
#include "fat.h"
#include "stack.h"
#include "memory.h"
#include "instruction_set.h"
#include "utils.h"

#define PROCESS_MAX_AMOUNT 10
#define PROCESS_NAME_SIZE 12

class Processing {

 public:
  enum ProcessState {
    DEAD = '\0',
    TERMINATED = 't',
    RUNNING = 'r',
    PAUSED = 'p',
  };

  typedef PROGMEM struct {
    int pid_;
    char name_[PROCESS_NAME_SIZE];
    ProcessState state_;
    uint8_t addr;
    uint8_t program_ctr_;
    uint8_t file_ptr_;
    int loop_addr_;
    Stack* stack_;
//    Stack::stack_t stack;
  } Process;

  Process process_table_[PROCESS_MAX_AMOUNT] = { 0 };
  int num_of_processes_ = 0;

 public:
  Processing();
  ~Processing();

  void Reset();

  /**
   * @brief Automatically checks stored processes
   *        and start or stop them based on their state
   */
  int Service();

  /**
   * @brief Creates a new process and automatically runs it
   * @param name
   * @return 0 (error-code: OK) || -1 (error-code: FAILED)
   */
  int CreateProcess(char* name);

  /**
   * @brief Changes process's state
   * @param id
   * @param state
   * @return 0 (error-code: OK) || -1 (error-code: FAILED)
   */
  int EditProcess(uint8_t id, ProcessState state);

  /**
   * @brief Prints a list of all stored processes
   * @return 0 (error-code: OK)
   */
  int ListProcesses();

 private:
  /**
   * @brief Executes a process
   * @param index
   * @return 0 (error-code: OK) || -1 (error-code: FAILED)
   */
  int Execute(uint8_t index);

  /**
   * @brief Checks whether a process exists
   * @param id
   * @return process pid
   */
  int FindProcess(uint8_t id);

  /**
   * @brief Removes a process
   * @param id
   * @return 0 (error-code: OK)
   */
  int KillProcess(uint8_t id);

  /**
   * @brief Sets a new state for a process
   * @param pid
   * @param new_state
   * @return 0 (error-code: OK) || -1 (error-code: FAILED)
   */
  int SetProcessState(int pid, ProcessState new_state);


};

#endif //PROCESSING_H_
