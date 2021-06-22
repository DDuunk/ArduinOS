#include "cli.h"
#include "../test/program_test.h"

Fat* fat;
//Processing* processing;

Cli::Cli() {
  Reset();
  fat = new Fat();
//  processing = new Processing();

//  fat->Write("hello", sizeof(test_blink), (char*)test_blink);
  Serial.println(F("*****************************"));
  Serial.println(F("***   ArduinOS 1.0 Ready  ***"));
  Serial.println(F("*****************************"));

//  processing->StartProcess("hello");
}

Cli::~Cli() {}

void Cli::Service() {
  if (!ProcessSerial()) {
    return;
  }
}

void Cli::ServiceProcess() {
//  processing->Service();
}

bool Cli::ProcessSerial() {
  char c;

  while (Serial.available() > 0) {
    c = Serial.read();

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
    if (strcmp(argv[0], i.name) == 0) {
      if (i.args_length != argc - 1) {
        ThrowError((char*) "Fat", (char*) "Error: args length not matching");
        Help(nullptr);
        return -1;
      }
      return (this->*(i.func))(argv + 1 /* Offset for the command itself */);
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
  int freespace = Fat::Freespace();

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
  Serial.println("run");
//  processing->StartProcess(args[0]);
//  processing->ListProcesses();
  return 38;
}

int Cli::List(char **args) {
  Serial.println("list");
//  processing->ListProcesses();
  return 38;
}

int Cli::Suspend(char **args) {
  Serial.println("suspend");
//  int pid = atoi(args[0]);
//  processing->EditProcess(pid, processing->PAUSED);
  return 38;
}

int Cli::Resume(char **args) {
  Serial.println("resume");
//  int pid = atoi(args[0]);
//  processing->EditProcess(pid, processing->RUNNING);
  return 38;
}

int Cli::Kill(char **args) {
  Serial.println("kill");
//  int pid = atoi(args[0]);
//  processing->EditProcess(pid, processing->TERMINATED);
  return 38;
}

int Cli::Eeprom(char **args) {
  Serial.println("eeprom");
//  Fat::PrintEeprom();
  return 38;
}

int Cli::Clear(char **args) {
  Serial.println("clear");
//  Fat::ClearEeprom();
  return 38;
}