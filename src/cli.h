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

  typedef int (Cli::*Function)(char** args);

  struct CommandType {
    char name[BUFFER_SIZE];
    Cli::Function func;
    int args_length;
  };

  // Initialize all available commands
  CommandType command_[13] = {
      { "help",      &Cli::Help,       0 }, // Show command usage
      { "store",     &Cli::Store,      2 }, // Store a file in filesystem
      { "retrieve",  &Cli::Retrieve,   1 }, // Get a file from filesystem
      { "erase",     &Cli::Erase,      1 }, // Delete a file
      { "files",     &Cli::Files,      0 }, // Get all Files from filesystem
      { "freespace", &Cli::Freespace,  0 }, // Get available storage capacity
      { "run",       &Cli::Run,        1 }, // Start a program
      { "list",      &Cli::List,       0 }, // Get a List of all running processes
      { "suspend",   &Cli::Suspend,    1 }, // Pause a process
      { "resume",    &Cli::Resume,     1 }, // Start a process
      { "kill",      &Cli::Kill,       1 }, // Stop a process
      { "eeprom",    &Cli::Eeprom,     0 }, // TESTING - Print EEPROM
      { "clear",     &Cli::Clear,      0 } // TESTING - Clear EEPROM
  };

 public:
  Cli();
  ~Cli();

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
   */
  int HandleCmd(int argc, char *argv[]);

  /**
   * @brief Print help information
   */
  int Help(char **args);

  /**
   * @brief Store a file on the filesystem
   * @param args - file name, size & Data
   */
  int Store(char **args);

  /**
   * @brief Retrieve a file from the filesystem
   * @param args - file name
   */
  int Retrieve(char **args);

  /**
   * @brief Erase a file from the filesystem
   * @param args - file name
   */
  int Erase(char **args);

  /**
   * @brief Printing a list of files
   * @param args - none
   */
  int Files(char **args);

  /**
   * @brief Printing available space on the filesystem
   * @param args - none
   */
  int Freespace(char **args);

  /**
   * @brief Start a program
   * @param args - file name
   */
  int Run(char **args);

  /**
   * @brief Print a list of all processes
   * @param args - none
   */
  int List(char **args);

  /**
   * @brief Suspend a running process
   * @param args - process id
   */
  int Suspend(char **args);

  /**
   * @brief Resume a process
   * @param args - process id
   */
  int Resume(char **args);

  /**
   * @brief Kill a process
   * @param args - process id
   */
  int Kill(char **args);

  /**
   * @brief TESTING - Print EEPROM
   * @param args - none
   */
  int Eeprom(char **args);

  /**
   * @brief TESTING - Clear EEPROM
   * @param args - none
   */
  int Clear(char **args);

};

#endif //CLI_H_
