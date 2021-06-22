#include "processing.h"
#include "operation.h"

Memory* memory;

Processing::Processing() {
  memory = new Memory();
  num_of_processes_ = 0;
}

Processing::~Processing() {
//  num_of_processes_ = 0;
}

void Processing::Service() {
  int num_of_running_processes = 0;

  for (int i = 0; i < MAX_PROCESSES; i++) {
    if (process_table_[i].state ==  RUNNING) {
      num_of_running_processes++;
    }

    if (process_table_[i].state == TERMINATED) {
      KillProcess(i);
      break;
    }
  }

  if (num_of_running_processes == 0) {
    return;
  }

  uint32_t start_time = 0;
  uint32_t run_time = ceil(50 / num_of_running_processes);

  for (int i = 0; i < MAX_PROCESSES; i++) {
    start_time = millis();

    while (process_table_[i].state == RUNNING && (millis() - start_time < run_time)) {
      if (Execute(i)) {
        i--;
        break;
      }
    }
  }
}

bool Processing::Execute(int index) {
  uint8_t next_instruction = EEPROM[process_table_[index].program_ctr++];

  Serial.print("NEXT ");
  Serial.println(next_instruction);

  switch (next_instruction) {
    case CHAR:
    case INT:
    case STRING:
    case FLOAT:
      Operation::StackOp(process_table_ + index, next_instruction);
      break;
    case SET:
      memory->StoreEntry(
          EEPROM[process_table_[index].program_ctr++],
          process_table_[index].pid,
          &(process_table_[index].stack)
      );
      break;
    case GET:
      memory->GetEntry(
          EEPROM[process_table_[index].program_ctr++],
          process_table_[index].pid,
          &(process_table_[index].stack)
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
//      Operation::FileOp(process_table_ + index, next_instruction);
      break;
    case FORK:
      // TODO: Pop type STRING, push type INT, start new process; push process-id
      break;
    case WAITUNTILDONE:
      // TODO: Pop type getal, push type getal, PC-- en push x als process x niet TERMINATED is, anders niets
      break;
    case STOP:
      SetProcessState(process_table_[index].pid, TERMINATED);
      return true; // Terminate process
    default:
      ThrowError((char*) "Processing", (char*) "unknown instruction: ", false);
      Serial.println(next_instruction);
      SetProcessState(process_table_[index].pid, TERMINATED);
      return true; // Terminate process
  }

  return false;
}

bool Processing::StartProcess(char* name) {
  if (num_of_processes_ >= MAX_PROCESSES) {
    ThrowError((char*) "Processing", (char*) "Processing table is full");
    return false;
  }

  int file_index = Fat::FindFile(name);
  if (file_index == -1) {
    ThrowError((char*) "Processing", (char*) "File does not exist");
    return false;
  }

  int pid;
  for (int i = 0; i < MAX_PROCESSES; i++) {
    if (process_table_[i].state == '\0') {
      pid = i;
      break;
    }
  }

  Process process;
  process.pid = pid;
  process.state = RUNNING;
//  process.stack = new Stack();
  process.stack = {};
  process.file_ptr = 0;
  process.program_ctr = Fat::GetStartAddr(file_index);
  strcpy(process.name, name);

  process_table_[pid] = process;
  num_of_processes_++;

  Serial.print(F("[Processing] Started: "));
  Serial.println(process.name);
//  Serial.println(name);
  ListProcesses();
  return true;
}

bool Processing::EditProcess(int id, ProcessState state) {
  int pid = FindProcess(id);
  if (pid == -1) {
    ThrowError((char*) "Processing", (char*) "Process does not exist");
    return false;
  }

  if (!SetProcessState(pid, state)) {
    ThrowError((char*) "Processing", (char*) "Process", false);
    Serial.print(pid);
    Serial.print(F(" already has state "));
    Serial.println(state);
    return false;
  }

  return true;
}

int Processing::FindProcess(int id) {
  for (uint8_t i = 0; i < MAX_PROCESSES; i++) {
    Process process = process_table_[i];

    if (process.pid == id && process.state != '\0') {
      return i;
    }
  }

  return -1;
}

void Processing::KillProcess(int id) {
  Process process = process_table_[id];

  strcpy(process.name, nullptr);
  process.pid = 0;
  process.state = DEAD;
  process.program_ctr = '\0';
  process.file_ptr = '\0';
//  process.stack = new Stack();
  process.stack = {};
  process.addr = '\0';

  process_table_[id] = process;
  num_of_processes_--;

  Serial.print(F("[Processing] Process "));
  Serial.print(id);
  Serial.println(F(" successfully terminated"));
}

bool Processing::SetProcessState(int pid, ProcessState new_state) {
  Process process = process_table_[pid];

  if (process.state == new_state) {
    return false;
  }

  process.state = new_state;
  process_table_[pid] = process;

  Serial.print(F("[Processing] PID "));
  Serial.print(pid);
  Serial.print(F(" state changed to: "));

  // Can't get key from value for enums so this garbage is needed...
  switch (process.state) {
    case '\0':
      Serial.println(F("DEAD"));
      break;
    case 't':
      Serial.println(F("TERMINATED"));
      break;
    case 'r':
      Serial.println(F("RUNNING"));
      break;
    case 'p':
      Serial.println(F("PAUSED"));
      break;
    default:
      Serial.println(F("unknown"));
      break;
  }

  return true;
}

void Processing::ListProcesses() {
  Serial.println(F("PID\tSTATE\tADDR\tPC\tFP\tLA\tNAME"));

  for (const auto &process : process_table_) {
    Serial.print(process.pid);
    Serial.print(F("\t"));
    Serial.print(process.state);
    Serial.print(F("\t"));
    Serial.print(process.addr);
    Serial.print(F("\t"));
    Serial.print(process.program_ctr);
    Serial.print(F("\t"));
    Serial.print(process.file_ptr);
    Serial.print(F("\t"));
    Serial.print(process.loop_addr);
    Serial.print(F("\t"));
    Serial.println(process.name);
  }
}