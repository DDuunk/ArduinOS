#include "processing.h"
#include "operation.h"

static Memory memory;

Processing::Processing() {
  Reset();
}

Processing::~Processing() {}

void Processing::Reset() {
  memset(process_table_, '\0', sizeof(process_table_));
  num_of_processes_ = 0;
}

int Processing::Service() {
  int num_of_running_processes = 0;

  for (uint8_t i = 0; i < PROCESS_MAX_AMOUNT; i++) {
    if (process_table_[i].state_ == RUNNING) {
      num_of_running_processes++;
    }

    if (process_table_[i].state_ == TERMINATED) {
      KillProcess(i);
      break;
    }
  }

  if (num_of_running_processes == 0) {
    return -1;
  }

  uint32_t start_time = 0;
  uint32_t run_time = ceil(50 / num_of_running_processes);

  for (uint8_t i = 0; i < PROCESS_MAX_AMOUNT; i++) {
    start_time = millis();

    while (process_table_[i].state_ == RUNNING && (millis() - start_time < run_time)) {
      if (Execute(i) == 0) {
        i--;
        break;
      }
    }
  }

  return 0;
}

int Processing::CreateProcess(char* name) {
  if (num_of_processes_ >= PROCESS_MAX_AMOUNT) {
    // @todo panic
    return -1;
  }

  int file_index = Fat::FindFile(name);
  if (file_index == -1) {
    // @todo panic
    return -1;
  }

  int pid = 0;
  for (int i = 0; i < PROCESS_MAX_AMOUNT; i++) {
    if (process_table_[i].state_ == DEAD) {
      pid = i;
      break;
    }
  }

  Process process;
  process.pid_ = pid;
  process.state_ = RUNNING;
  process.stack_ = new Stack();
  process.file_ptr_ = 0;
  process.program_ctr_ = Fat::GetStartAddr(file_index);
  process.loop_addr_ = 0;
  strcpy(process.name_, name);

  process_table_[pid] = process;
  num_of_processes_++;

  Serial.print(F("[Processing] Started: "));
  Serial.println(process.name_);

//  ListProcesses();
  return 0;
}

int Processing::EditProcess(uint8_t id, ProcessState state) {
  int pid = FindProcess(id);
  if (pid == -1) {
    ThrowError((char*) "Processing", (char*) "Process does not exist");
    return -1;
  }

  if (!SetProcessState(pid, state)) {
//    ThrowError((char*) "Processing", (char*) "Process ", false);
//    Serial.print(pid);
//    Serial.print(F(" already has state "));
//    Serial.println(state);
    return -1;
  }

  return 0;
}

int Processing::KillProcess(uint8_t id) {
  // Debatable to just remove the entire process instead of killing it
  Process process = process_table_[id];

  memset(process.name_, '\0', sizeof(process.name_));
  process.pid_ = 0;
  process.state_ = DEAD;
  process.program_ctr_ = '\0';
  process.file_ptr_ = '\0';
  process.stack_ = new Stack();
//  process.stack_ = {};
  process.addr = '\0';

  process_table_[id] = process;
  num_of_processes_--;

  Serial.print(F("[Processing] Process "));
  Serial.print(id);
  Serial.println(F(" successfully terminated"));
  return 0;
}

int Processing::Execute(uint8_t index) {
  uint8_t next_instruction = EEPROM[process_table_[index].program_ctr_++];

//  Serial.println(next_instruction);

  switch (next_instruction) {
    case CHAR:
    case INT:
    case STRING:
    case FLOAT:
      Operation::StackOp(process_table_ + index, next_instruction);
      break;
    case SET:
      memory.StoreEntry(
          EEPROM[process_table_[index].program_ctr_++],
          process_table_[index].pid_,
          process_table_[index].stack_
      );
      break;
    case GET:
      memory.GetEntry(
          EEPROM[process_table_[index].program_ctr_++],
          process_table_[index].pid_,
          process_table_[index].stack_
      );
      break;
    case INCREMENT:
    case DECREMENT:
    case UNARYMINUS:
    case ABS:
    case SQ:
    case SQRT:
    case ANALOGREAD:
    case DIGITALREAD:
    case LOGICALNOT:
    case BITWISENOT:
    case TOCHAR:
    case TOINT:
    case TOFLOAT:
    case ROUND:
    case FLOOR:
    case CEIL:
      Operation::UnaryOp(process_table_ + index, next_instruction);
      break;
    case PLUS:
    case MINUS:
    case TIMES:
    case DIVIDEDBY:
    case MODULUS:
    case EQUALS:
    case NOTEQUALS:
    case LESSTHAN:
    case LESSTHANOREQUALS:
    case GREATERTHAN:
    case GREATERTHANOREQUALS:
    case MIN:
    case MAX:
    case POW:
    case LOGICALAND:
    case LOGICALOR:
    case LOGICALXOR:
    case BITWISEAND:
    case BITWISEOR:
    case BITWISEXOR:
      Operation::BinaryOp(process_table_ + index, next_instruction);
      break;
    case IF:
    case ELSE:
    case ENDIF:
    case LOOP:
    case ENDLOOP:
    case WHILE:
    case ENDWHILE:
      Operation::ConditionalOp(process_table_ + index, next_instruction);
      break;
    case DELAY:
    case DELAYUNTIL:
    case MILLIS:
      Operation::TimeOp(process_table_ + index, next_instruction);
      break;
    case CONSTRAIN:
    case MAP:
    case PINMODE:
    case DIGITALWRITE:
    case ANALOGWRITE:
      Operation::IoOp(process_table_ + index, next_instruction);
      break;
    case PRINT:
      Operation::PrintOp(process_table_ + index);
      break;
    case PRINTLN:
      Operation::PrintOp(process_table_ + index, true);
      break;
    case OPEN:
    case CLOSE:
    case WRITE:
    case READINT:
    case READCHAR:
    case READFLOAT:
    case READSTRING:
      Operation::FileOp(process_table_ + index, next_instruction);
      break;
    case FORK:
      // TODO: Pop type STRING, push type INT, start new process; push process-id
      break;
    case WAITUNTILDONE:
      // TODO: Pop type getal, push type getal, PC-- en push x als process x niet TERMINATED is, anders niets
      break;
    case STOP:
      SetProcessState(process_table_[index].pid_, TERMINATED);
      return 0; // Terminate process
    default:
      ThrowError((char*) "Processing", (char*) "unknown instruction: ", false);
      Serial.println(next_instruction);
      SetProcessState(process_table_[index].pid_, TERMINATED);
      return 0; // Terminate process
  }

  return -1;
}

int Processing::FindProcess(uint8_t id) {
  for (int i = 0; i < PROCESS_MAX_AMOUNT; i++) {
    Process process = process_table_[i];

    if (process.pid_ == id && process.state_ != DEAD) {
      return i;
    }
  }

  return -1;
}

int Processing::SetProcessState(int pid, ProcessState new_state) {
  Process process = process_table_[pid];

  if (process.state_ == new_state) {
    return -1;
  }

  process.state_ = new_state;
  process_table_[pid] = process;

  Serial.print(F("[Processing] PID "));
  Serial.print(pid);
  Serial.print(F(" state changed to: "));

  // Can't get key from value for enums so this garbage is needed...
  switch (process.state_) {
    case DEAD:
      Serial.println(F("DEAD"));
      break;
    case TERMINATED:
      Serial.println(F("TERMINATED"));
      break;
    case RUNNING:
      Serial.println(F("RUNNING"));
      break;
    case PAUSED:
      Serial.println(F("PAUSED"));
      break;
    default:
      Serial.println(F("unknown"));
      break;
  }

  return 0;
}

int Processing::ListProcesses() {
  Serial.println(F("PID\tSTATE\tADDR\tPC\tFP\tLA\tNAME"));

  for (const auto &process : process_table_) {
    Serial.print(process.pid_);
    Serial.print(F("\t"));
    Serial.print(process.state_);
    Serial.print(F("\t"));
    Serial.print(process.addr);
    Serial.print(F("\t"));
    Serial.print(process.program_ctr_);
    Serial.print(F("\t"));
    Serial.print(process.file_ptr_);
    Serial.print(F("\t"));
    Serial.print(process.loop_addr_);
    Serial.print(F("\t"));
    Serial.println(process.name_);
  }
  return 0;
}