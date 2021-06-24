#include "memory.h"

Memory::Memory() {
  Reset();
}

Memory::~Memory() {}

void Memory::Reset() {
  memset(memory_table_, '\0', sizeof(memory_table_));
  memset(memory_, '\0', sizeof(memory_));
  num_of_vars_ = 0;
}

int Memory::StoreEntry(uint8_t name, uint8_t pid, Stack* stack) {
  if (num_of_vars_ >= MEMORY_COUNT) {
    ThrowError((char*) "Memory", (char*) "Memory is full");
    return -1;
  }

  int exists = FindEntry(name, pid);
  if (exists != -1) {
    DeleteEntry(exists);
  }

  uint8_t type = stack->PopByte();
  int size = GetTypeSize(type, stack);
  int addr = GetStartAddr(size);

  MemoryEntry memory_entry = (MemoryEntry) {
    pid, name, type, (uint16_t) addr, (uint8_t) size
  };

  memory_table_[num_of_vars_] = memory_entry;

  PrintMemoryTable();

  int i = memory_table_[num_of_vars_].size_ - 1;
  do {
    memory_[memory_table_[num_of_vars_].addr_ + i] = stack->PopByte();
    i--;
  } while (i >= 0);


//  Serial.println(memory_table_[num_of_vars_].size - 1);
//  for (int i = (memory_table_[num_of_vars_].size - 1); i >= 0; i--) {
//    memory_[memory_table_[num_of_vars_].addr + i] = stack->PopByte(); // Set data
//  }

  Serial.println("Peek");
  ++num_of_vars_;
  return 0;
}

int Memory::GetEntry(uint8_t name, uint8_t pid, Stack* stack) {
  int index = FindEntry(name, pid);
  if (index < 0 || index > num_of_vars_) {
    return -1;
  }

  for (uint8_t i = 0; i < memory_table_[index].size_; i++) {
    stack->PushByte(memory_[memory_table_[index].addr_ + i]);
  }

  if (memory_table_[index].type_ == STRING) {
    stack->PushByte(memory_table_[index].size_);
  }

  stack->PushByte(memory_table_[index].type_);

  return 0;
}

int Memory::FindEntry(uint8_t name, uint8_t pid) {
  if (num_of_vars_ <= 0) {
    return -1;
  }

  for (uint8_t i = 0; i < num_of_vars_; i++) {
    if (name == memory_table_[i].name_ && pid == memory_table_[i].pid_) {
      return i;
    }
  }

  return -1;
}

int Memory::DeleteEntry(uint8_t index) {
  while (memory_table_[index + 1].name_ != '\0') {
    memory_table_[index] = memory_table_[index + 1];
    ++index;
  }

  memory_table_[index] = MemoryEntry {};
  num_of_vars_--;
  return 0;
}

int Memory::ClearEntries(uint8_t pid) {
  for (uint8_t i = 0; i < num_of_vars_; i--) {
    if (memory_table_[i].pid_ == pid) {
      DeleteEntry(i);
      i--;
    }
  }

  return 0;
}

int Memory::GetStartAddr(uint8_t size) {
  if (num_of_vars_ == 0 && size <= MEMORY_SIZE) {
    return 0; // First address
  }

  for (uint8_t i = 0; i < num_of_vars_ - 1; i++) {
    // Check for space between entries
    if (memory_table_[i + 1].addr_ - (memory_table_[i].addr_ + memory_table_[i].size_) >= size) {
      return memory_table_[i].addr_ + memory_table_[i].size_;
    }
  }

  // Check for space after the last entry
  if (MEMORY_SIZE - (memory_table_[num_of_vars_ - 1].addr_ + memory_table_[num_of_vars_ - 1].size_) >= size) {
    return memory_table_[num_of_vars_ - 1].addr_ + memory_table_[num_of_vars_ - 1].size_;
  }

  return -1;
}

int Memory::GetTypeSize(char type, Stack* stack) {
  switch (type) {
    case INT:
      return 2;
    case CHAR:
      return 1;
    case STRING:
      return stack->PopByte();
    case FLOAT:
      return 4;
    default:
      return -1;
  }
}

int Memory::PrintMemoryTable() {;
//  for (auto & i : memory_table_) {
//    if (i.name == '\0') {
//      break;
//    }
//
//    Serial.print(F("Process: "));
//    Serial.print(i.pid);
//    Serial.print(F("\t Name: "));
//    Serial.print(i.name);
//    Serial.print(F("\t Type: "));
//    Serial.print(i.type);
//    Serial.print(F("\t Addr: "));
//    Serial.print(i.addr);
//    Serial.print(F("\t Size: "));
//    Serial.print(i.size);
//    Serial.print(F("\t Data: "));
//
//    switch (i.type) {
//      case INT:
//        Serial.print(word(memory_[i.addr], memory_[i.addr + 1]));
//        break;
//      case CHAR:
//        Serial.print(memory_[i.addr]);
//        break;
//      case STRING: {
//        char string[i.size + 1];
//
//        for (uint8_t j = 0; j < i.size; j++) {
//          string[j] = memory_[i.addr + j];
//        }
//
//        Serial.print(string);
//        break;
//      }
//      case FLOAT: {
//        float f = 0.0;
//        auto *b = &f;
//
//        for (int j = 0; j < 4; j++) {
//          Serial.println(memory_[i.addr + j]);
//          b[j] = memory_[i.addr + j];
//        }
//        Serial.print(f);
//        break;
//      }
//      default:
//        break;
//    }
//
//    Serial.println();
//  }

  return 0;
}

//bool Memory::StoreEntry(uint8_t name, int pid, Stack::stack_t* stack) {
//  if (num_of_vars_ >= TABLE_SIZE) {
//    ThrowError((char*) "Memory", (char*) "Memory is full");
//    return false;
//  }
//
//  int exists = FindEntry(name, pid);
//  if (exists != -1) {
//    DeleteEntry(exists);
//  }
//
//  uint8_t type = stack_c->PopByte(stack);
//  int size = GetTypeSize(type, stack);
//  int addr = GetStartAddr(size);
//
//  MemoryEntry memory_entry;
//  memory_entry.pid = pid;
//  memory_entry.name = name;
//  memory_entry.type = type;
//  memory_entry.addr = addr;
//  memory_entry.size = size;
//
//  memory_table_[num_of_vars_] = memory_entry;
//
//  for (int i = (memory_table_[num_of_vars_].size - 1); i >= 0; i--) {
//    memory_[memory_table_[num_of_vars_].addr + i] = stack_c->PopByte(stack); // Set data
//  }
//
//  ++num_of_vars_;
//  return true;
//}
//
//int Memory::GetEntry(uint8_t name, int pid, Stack::stack_t* stack) {
//  int index = FindEntry(name, pid);
//  if (index < 0 || index > num_of_vars_) {
//    return -1;
//  }
//
//  for (int i = 0; i < memory_table_[index].size; i++) {
//    stack_c->PushByte(stack, memory_[memory_table_[index].addr + i]);
//  }
//
//  if (memory_table_[index].type == STRING) {
//    stack_c->PushByte(stack, memory_table_[index].size);
//  }
//
//  stack_c->PushByte(stack, memory_table_[index].type);
//
//  return 1;
//}
//
//int Memory::FindEntry(uint8_t name, int pid) {
//  if (num_of_vars_ <= 0) {
//    return -1;
//  }
//
//  for (int8_t i = 0; i < num_of_vars_; i++) {
//    if (name == memory_table_[i].name && pid == memory_table_[i].pid) {
//      return i;
//    }
//  }
//
//  return -1;
//}
//
//void Memory::DeleteEntry(int index) {
//  while (memory_table_[index + 1].name != '\0') {
//    memory_table_[index] = memory_table_[index + 1];
//    ++index;
//  }
//  memory_table_[index] = MemoryEntry {};
//  num_of_vars_--;
//}
//
//void Memory::ClearEntries(int pid) {
//  for (int i = 0; i < num_of_vars_; i--) {
//    if (memory_table_[i].pid == pid) {
//      DeleteEntry(i);
//      i--;
//    }
//  }
//}
//
//int Memory::GetStartAddr(int size) {
//  if (num_of_vars_ == 0 && size <= MEMORY_SIZE) {
//    return 0; // First address
//  }
//
//  for (int i = 0; i < num_of_vars_ - 1; i++) {
//    // Check for space between entries
//    if (memory_table_[i + 1].addr - (memory_table_[i].addr + memory_table_[i].size) >= size) {
//      return memory_table_[i].addr + memory_table_[i].size;
//    }
//  }
//
//  // Check for space after the last entry
//  if (MEMORY_SIZE - (memory_table_[num_of_vars_ - 1].addr + memory_table_[num_of_vars_ - 1].size) >= size) {
//    return memory_table_[num_of_vars_ - 1].addr + memory_table_[num_of_vars_ - 1].size;
//  }
//
//  return -1;
//}
//
//int Memory::GetTypeSize(char type, Stack::stack_t* stack) {
//  switch (type) {
//    case INT:
//      return 2;
//    case CHAR:
//      return 1;
//    case STRING:
//      return stack_c->PopByte(stack);
//    case FLOAT:
//      return 4;
//    default:
//      return -1;
//  }
//}
//
//void Memory::PrintMemoryTable() {;
//  for (auto & i : memory_table_) {
//    if (i.name == '\0') {
//      break;
//    }
//
//    Serial.print(F("Process: "));
//    Serial.print(i.pid);
//    Serial.print(F("\t Name: "));
//    Serial.print(i.name);
//    Serial.print(F("\t Type: "));
//    Serial.print(i.type);
//    Serial.print(F("\t Addr: "));
//    Serial.print(i.addr);
//    Serial.print(F("\t Size: "));
//    Serial.print(i.size);
//    Serial.print(F("\t Data: "));
//
//    switch (i.type) {
//      case INT:
//        Serial.print(word(memory_[i.addr], memory_[i.addr + 1]));
//        break;
//      case CHAR:
//        Serial.print(memory_[i.addr]);
//        break;
//      case STRING: {
//        char string[i.size + 1];
//
//        for (uint8_t j = 0; j < i.size; j++) {
//          string[j] = memory_[i.addr + j];
//        }
//
//        Serial.print(string);
//        break;
//      }
//      case FLOAT: {
//        float f = 0.0;
//        auto *b = &f;
//
//        for (int j = 0; j < 4; j++) {
//          Serial.println(memory_[i.addr + j]);
//          b[j] = memory_[i.addr + j];
//        }
//        Serial.print(f);
//        break;
//      }
//      default:
//        break;
//    }
//
//    Serial.println();
//  }
//}
