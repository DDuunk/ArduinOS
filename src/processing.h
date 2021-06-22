#ifndef PROCESSING_H_
#define PROCESSING_H_

#include "Arduino.h"
#include "EEPROM.h"
#include "fat.h"
#include "stack.h"
#include "memory.h"
#include "instruction_set.h"
#include "utils.h"

#define MAX_PROCESSES 10
#define MAX_NAME_SIZE 12

class Processing {

 public:
  enum ProcessState {
    DEAD = '\0',
    TERMINATED = 't',
    RUNNING = 'r',
    PAUSED = 'p',
  };

  struct Process {
    int pid;
    char name[MAX_NAME_SIZE];
    ProcessState state;
    uint8_t addr;
    uint8_t program_ctr;
    uint8_t file_ptr;
    int loop_addr;
//    Stack* stack;
    Stack::stack_t stack;
  };

  Process process_table_[MAX_PROCESSES]{};

 public:
  Processing();
  ~Processing();

  /**
   * @brief Automatically checks stored processes
   *        and start or stop them based on their state
   */
  void Service();

  /**
   * @brief Creates a new process and automatically runs it
   * @param name
   * @return
   */
  bool StartProcess(char* name);

  /**
   * @brief Changes process's state
   * @param id
   * @param state
   * @return true if successful, false if not
   */
  bool EditProcess(int id, ProcessState state);

  /**
   * @brief Prints a list of all stored processes
   */
  void ListProcesses();

 private:
  int num_of_processes_ = 0;

  /**
   * @brief Executes a process
   * @param index
   * @return True if successfully executed, false if not
   */
  bool Execute(int index);

  /**
   * @brief Checks whether a process exists
   * @param id
   * @return process pid
   */
  int FindProcess(int id);

  /**
   * @brief Removes a process
   * @param id
   */
  void KillProcess(int id);

  /**
   * @brief Sets a new state for a process
   * @param pid
   * @param new_state
   * @return true if successful, false if not
   */
  bool SetProcessState(int pid, ProcessState new_state);

};

#endif //PROCESSING_H_
