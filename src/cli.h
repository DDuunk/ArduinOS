#ifndef CLI_H_
#define CLI_H_

#include "Arduino.h"
#include "fat.h"
#include "processing.h"
#include "utils.h"

#define BUFFER_SIZE 12
#define COMMAND_MAX_SIZE 64
#define COMMAND_MAX_ARGS 3 // 1 command + 2 args
#define COMMAND_SPLIT " ,.-\t"

class Cli {

 private:
  char buffer_[COMMAND_MAX_SIZE] = { 0 };

  typedef PROGMEM struct {
    char name_[BUFFER_SIZE];
    int (*func)(char** args);
    uint16_t args_length_;
  } CommandType;

  // Initialize all available commands
  static CommandType command_[];

 public:
  Cli();
  ~Cli();

  /**
   * @brief Initializes Cli non-construction properties
   * @param test_mode - Whether or not to initialize testing mode
   */
  void Start(bool test_mode = false);

  /**
   * @brief Contains the Cli loop,
   *        is called once in the main function
   */
  void Service();

  /**
   * @brief Start process handling
   */
  void ServiceProcess();

  /**
   * @brief Process Serial input
   */
  bool ProcessSerial();

  /**
   * @brief Reset certain variables
   */
  void Reset();

  /**
   * @brief Parse commando input
   * @param args - args buffer
   * @return 0 (error-code: OK) || -1 (error-code: FAILED)
   */
  int HandleCmd(int argc, char *argv[]);

  /**
   * @brief Print help information
   * @param args - none
   * @return 0 (error-code: OK) || -1 (error-code: FAILED)
   */
  static int Help(char **args);

  /**
   * @brief Store a file on the filesystem
   * @param args - file name, size & Data
   * @return 0 (error-code: OK) || -1 (error-code: FAILED)
   */
  static int Store(char **args);

  /**
   * @brief Retrieve a file from the filesystem
   * @param args - file name
   * @return 0 (error-code: OK) || -1 (error-code: FAILED)
   */
  static int Retrieve(char **args);

  /**
   * @brief Erase a file from the filesystem
   * @param args - file name
   * @return 0 (error-code: OK) || -1 (error-code: FAILED)
   */
  static int Erase(char **args);

  /**
   * @brief Printing a list of files
   * @param args - none
   * @return 0 (error-code: OK) || -1 (error-code: FAILED)
   */
  static int Files(char **args);

  /**
   * @brief Printing available space on the filesystem
   * @param args - none
   * @return 0 (error-code: OK) || -1 (error-code: FAILED)
   */
  static int Freespace(char **args);

  /**
   * @brief Start a program
   * @param args - file name
   * @return 0 (error-code: OK) || -1 (error-code: FAILED)
   */
  static int Run(char **args);

  /**
   * @brief Print a list of all processes
   * @param args - none
   * @return 0 (error-code: OK) || -1 (error-code: FAILED)
   */
  static int List(char **args);

  /**
   * @brief Suspend a running process
   * @param args - process id
   * @return 0 (error-code: OK) || -1 (error-code: FAILED)
   */
  static int Suspend(char **args);

  /**
   * @brief Resume a process
   * @param args - process id
   * @return 0 (error-code: OK) || -1 (error-code: FAILED)
   */
  static int Resume(char **args);

  /**
   * @brief Kill a process
   * @param args - process id
   * @return 0 (error-code: OK) || -1 (error-code: FAILED)
   */
  static int Kill(char **args);

  /**
   * @brief TESTING - Print EEPROM
   * @param args - none
   * @return 0 (error-code: OK) || -1 (error-code: FAILED)
   */
  static int Eeprom(char **args);

  /**
   * @brief TESTING - Clear EEPROM
   * @param args - none
   * @return 0 (error-code: OK) || -1 (error-code: FAILED)
   */
  static int Clear(char **args);

};

#endif //CLI_H_
