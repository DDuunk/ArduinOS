#include "cli.h"
#include "../test/program_test.h"

static Fat fat;
static Processing processing;

Cli::CommandType Cli::command_[] = {
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

Cli::Cli() {
  Reset();
}

Cli::~Cli() {}

void Cli::Start(bool test_mode) {
  Serial.println(F("*****************************"));
  Serial.println(F("***   ArduinOS 1.0 Ready  ***"));
  Serial.println(F("*****************************"));

  if (test_mode) {
//    fat.Write((char *) "hello", sizeof(test_hello), (char *) test_hello);
//    fat.Write((char *) "vars", sizeof(test_vars), (char *) test_vars);
    fat.Write((char *) "loop1", sizeof(test_loop), (char *) test_loop);
    fat.Write((char *) "loop2", sizeof(test_loop), (char *) test_loop);
//    fat.Write((char *) "if", sizeof(test_if), (char *) test_if);
//    fat.Write((char *) "while", sizeof(test_while), (char *) test_while);
//    fat.Write((char *) "blink", sizeof(test_blink), (char *) test_blink);
//    fat.Write((char *) "read", sizeof(test_read), (char *) test_read);
//    fat.Write((char *) "write", sizeof(test_write), (char *) test_write);
//    fat.Write((char *) "fork", sizeof(test_fork), (char *) test_fork);
    processing.CreateProcess((char *) "loop1");
    processing.CreateProcess((char *) "loop2");
  }
}

void Cli::Service() {
  if (!ProcessSerial()) {
    return;
  }
}

void Cli::ServiceProcess() {
  processing.Service();
}

bool Cli::ProcessSerial() {
  while (Serial.available() > 0) {
    char c = Serial.read();

    if (c != '\n') {
      // Account for backspaces
      if (c == '\b') {
        if (strlen(buffer_)) {
          buffer_[strlen(buffer_) - 1] = '\0';
        }
        return false;
      }

      // Check whether it fits inside the buffer
      if (strlen(buffer_) >= COMMAND_MAX_SIZE - 1) {
        // @todo panic
        ThrowError((char*) "Fat", (char*) "Buffer is full");
        return false;
      }
      buffer_[strlen(buffer_)] = c;
      return false;
    }

    Serial.print(F("$ "));
    Serial.println(buffer_);

    // Tokenize
    int argc = 0;
    char *argv[COMMAND_MAX_ARGS + 1] = {nullptr};
    argv[argc++] = strtok(buffer_, COMMAND_SPLIT);
    do {
      argv[argc] = strtok(NULL, COMMAND_SPLIT);
    } while (argv[argc] && ++argc < COMMAND_MAX_ARGS);

    // Handle command
    int ret = HandleCmd(argc, argv);
    if (ret) {
      ThrowError((char* )"Fat", (char*) "Error: code ", false);
      Serial.println(ret);
    }

    // Reset
    Reset();

    return true;
  }

  return false;
}

void Cli::Reset() {
  memset(buffer_, '\0', COMMAND_MAX_SIZE);
}

int Cli::HandleCmd(int argc, char *argv[]) {
  for (auto &i : command_) {
    if (strcmp(argv[0], i.name_) == 0) {
      if (i.args_length_ != argc - 1) {
        ThrowError((char*) "Fat", (char*) "Error: args length not matching");
        Help(nullptr);
        return -1;
      }
      return (*(i.func))(argv + 1 /* Offset for the command itself */);
    }
  }

  return Help(nullptr);
}

int Cli::Help(char **args) {
  Serial.println(F("Usage: [command] [arguments]"));
  Serial.println(F(""));
  Serial.println(F("Arguments:"));
  Serial.println(F("  [command]     The command to Execute"));
  Serial.println(F("  [arguments]   Arguments to pass to the command"));
  Serial.println(F(""));
  Serial.println(F("Available Commands:"));
  Serial.println(F("  Help          Shows this message"));
  Serial.println(F("  Store         Store a file in the filesystem"));
  Serial.println(F("  Retrieve      Get a file from the filesystem"));
  Serial.println(F("  Erase         Remove a file from the filesystem"));
  Serial.println(F("  files         Get all Files"));
  Serial.println(F("  Freespace     Get available storage capacity"));
  Serial.println(F("  Run           Start a process"));
  Serial.println(F("  list          Get a List of all running processes"));
  Serial.println(F("  Suspend       Pause a process"));
  Serial.println(F("  Resume        Start a process"));
  Serial.println(F("  Kill          Stop a process"));
  return 0;
}

int Cli::Store(char **args) {
  if (Fat::Write(args[0], strlen(args[1]) + 1, args[1])) {
    Serial.print(F("[CLI] File `"));
    Serial.print(args[0]);
    Serial.print(F("` with `"));
    Serial.print(args[1]);
    Serial.println(F("` stored"));
  } else {
    ThrowError((char*) "Cli", (char*) "Error: failed to store file");
    return -1;
  }
  return 0;
}

int Cli::Retrieve(char **args) {
  Fat::Read(args[0]);
  return 0;
}

int Cli::Erase(char **args) {
  Fat::Delete(args[0]);
  return 0;
}

int Cli::Files(char **args) {
  Fat::ListFiles();
  return 0;
}

int Cli::Freespace(char **args) {
  int8_t freespace = Fat::Freespace();

  if (freespace == 0 || freespace == -1) {
    ThrowError((char*) "Cli", (char*) "Error: No space available on filesystem");
    return -1;
  }

  Serial.print(F("[CLI] "));
  Serial.print(freespace);
  Serial.println(F(" bytes available on filesystem"));
  return 0;
}

int Cli::Run(char **args) {
  processing.CreateProcess(args[0]);
  return 0;
}

int Cli::List(char **args) {
  processing.ListProcesses();
  return 0;
}

int Cli::Suspend(char **args) {
  processing.EditProcess(atoi(args[0]), processing.PAUSED);
  return 0;
}

int Cli::Resume(char **args) {
  processing.EditProcess(atoi(args[0]), processing.RUNNING);
  return 0;
}

int Cli::Kill(char **args) {
  processing.EditProcess(atoi(args[0]), processing.TERMINATED);
  return 0;
}

int Cli::Eeprom(char **args) {
  Fat::PrintEeprom();
  return 0;
}

int Cli::Clear(char **args) {
  Serial.println(F("clear"));
//  Fat::ClearEeprom();
  return 0;
}