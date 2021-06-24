#include "operation.h"

Operation::Operation() {}

Operation::~Operation() {}

int Operation::StackOp(Processing::Process* process, uint8_t data_type) {
  if (data_type == STRING) {
    uint8_t length = 0;
    char next_char;

    do {
      next_char = EEPROM[process->program_ctr_++];
      length++;
      process->stack_->PushByte(next_char);
    } while (next_char != 0);

    process->stack_->PushByte(length);
  } else {
    for (uint8_t i = 0; i < data_type; i++) {
      process->stack_->PushByte(EEPROM[process->program_ctr_++]);
    }
  }

  process->stack_->PushByte(data_type);
  return 0;
}

int Operation::UnaryOp(Processing::Process *process, uint8_t operation) {
  uint8_t type = process->stack_->PopByte();
  process->stack_->PushByte(type);

  // Get the Data as float from stack
//  float value = process->stack_->PopValue();
  uint8_t value = process->stack_->PopValue();

  switch (operation) {
    case INCREMENT:
      value += 1.0F;
      break;
    case DECREMENT:
      value -= 1.0F;
      break;
    case UNARYMINUS:
      value *= -1.0F;
      break;
    case ABS:
      value = (value >= 0 ? value : value* - 1.0F);
      break;
    case SQ:
      value = value * value;
      break;
    case SQRT:
      value = sqrt(value);
      break;
    case ANALOGREAD:
      value = analogRead(value);
      type = INT;
      break;
    case DIGITALREAD:
      value = digitalRead(value);
      type = CHAR;
      break;
    case LOGICALNOT:
      value = (value == 0.0F ? 1 : 0);
      type = CHAR;
      break;
    case BITWISENOT:
      value = ~(uint8_t)value;
      type = CHAR;
      break;
    case TOCHAR:
      value = round(value);
      type = CHAR;
      break;
    case TOINT:
    case ROUND:
      value = round(value) + 0;
      type = INT;
      break;
    case TOFLOAT:
      type = FLOAT;
      break;
    case FLOOR:
      value = floor(value);
      type = INT;
      break;
    case CEIL:
      value = ceil(value);
      type = INT;
      break;
    default:
      ThrowError((char*) "Operation", (char*) "Unknown unary operation", false);
      Serial.println(operation, DEC);
      return -1;
  }

  // Push (float) Data back on the stack by type
  switch (type) {
    case CHAR:
      process->stack_->PushChar((char) value);
      break;
    case INT:
      process->stack_->PushInt((int) value);
      break;
    case FLOAT:
      process->stack_->PushFloat(value);
      break;
    default:
      break;
  }
  return 0;
}

int Operation::BinaryOp(Processing::Process* process, uint8_t operation) {
  uint8_t y_type = process->stack_->PopByte();
  process->stack_->PushByte(y_type);
  uint8_t y = process->stack_->PopValue();


  uint8_t x_type = process->stack_->PopByte();
  process->stack_->PushByte(x_type);
  uint8_t x = process->stack_->PopValue();


  uint8_t z_type = (x_type > y_type ? x_type : y_type);
  uint8_t z;
//  float y_type = process->stack_->PopByte();
//  process->stack_->PushByte(y_type);
//  float y = process->stack_->PopValue();
//
//  float x_type = process->stack_->PopByte();
//  process->stack_->PushByte(x_type);
//  float x = process->stack_->PopValue();
//
//  // Use Z as output value, type determined by the largest type of X or Y
//  float z_type = (x_type > y_type ? x_type : y_type); // Get largest type
//  float z;

  switch (operation) {
    case PLUS:
      z = x + y;
      break;
    case MINUS:
      z = x - y;
      break;
    case TIMES:
      z = x * y;
      break;
    case DIVIDEDBY:
      z = x / y;
      break;
    case MODULUS:
      z = (int) x % (int) y;
      break;
    case EQUALS:
      z = (x == y ? 1 : 0);
      z_type = CHAR;
      break;
    case NOTEQUALS:
      z = (x != y ? 1 : 0);
      z_type = CHAR;
      break;
    case LESSTHAN:
      z = (x < y ? 1 : 0);
      z_type = CHAR;
      break;
    case LESSTHANOREQUALS:
      z = (x <= y ? 1 : 0);
      z_type = CHAR;
      break;
    case GREATERTHAN:
      z = (x > y ? 1 : 0);
      z_type = CHAR;
      break;
    case GREATERTHANOREQUALS:
      z = (x >= y ? 1 : 0);
      z_type = CHAR;
      break;
    case MIN:
      z = (x < y ? x : y);
      break;
    case MAX:
      z = (x > y ? x : y);
      break;
    case POW:
      for (int i = 0; i < y; i++) {
        z *= x;
      }
      break;
    case LOGICALAND:
      z = (x && y ? 1 : 0);
      z_type = CHAR;
      break;
    case LOGICALOR:
      z = (x || y ? 1 : 0);
      z_type = CHAR;
      break;
    case LOGICALXOR:
      z = (!x != !y ? 1 : 0);
      z_type = CHAR;
      break;
    case BITWISEAND:
      z = (int) x & (int) y;
      break;
    case BITWISEOR:
      z = (int) x | (int) y;
      break;
    case BITWISEXOR:
      z = (int) x ^ (int) y;
      break;
    default:
      ThrowError((char*) "Operation", (char*) "Unknown binary operation", false);
      Serial.println(operation);
      return -1;
  }

  // Push output value z back on the stack
  switch ((int) z_type) {
    case CHAR:
      process->stack_->PushChar((char) z);
      break;
    case INT:
      process->stack_->PushInt((int) z);
      break;
    case FLOAT:
      process->stack_->PushFloat(z);
      break;
    default:
      break;
  }

  return 0;
}

int Operation::TimeOp(Processing::Process* process, uint8_t operation) {
  switch (operation) {
    case DELAY:
      delay((int) process->stack_->PopValue());
      break;
    case DELAYUNTIL: {
      int x = (int) process->stack_->PopValue();

      if (x > (int) millis()) {
        process->stack_->PushInt(x);
        process->program_ctr_ -= 1;
      }
      break;
    }
    case MILLIS:
      process->stack_->PushInt((int) millis());
      break;
    default:
      ThrowError((char*) "Operation", (char*) "Unknown time operation", false);
      Serial.println(operation);
      return -1;
  }

  return 0;
}

int Operation::IoOp(Processing::Process* process, uint8_t operation) {
  switch (operation) {
    case CONSTRAIN: {
      // Pop in reverse order
      int max = (int) process->stack_->PopValue(); // Maximum
      int min = (int) process->stack_->PopValue(); // Minimum
      int x = (int) process->stack_->PopValue(); // Constrain value
      process->stack_->PushInt(min(max(x, min), max)); // Push Result
      break;
    }
    case MAP: {
      // Pop in reverse order
      int out_max = (int) process->stack_->PopValue();
      int out_min = (int) process->stack_->PopValue();
      int in_max = (int) process->stack_->PopValue();
      int in_min = (int) process->stack_->PopValue();
      int x = (int) process->stack_->PopValue(); // Value to map
      process->stack_->PushInt(map(x, in_min, in_max, out_min, out_max)); // Push Result
      break;
    }
    case PINMODE:
    case DIGITALWRITE:
    case ANALOGWRITE: {
      int value = (int) process->stack_->PopValue();
      int pin = (int) process->stack_->PopValue();

      switch (operation) {
        case PINMODE:
          pinMode(pin, value);
          break;
        case ANALOGWRITE:
          analogWrite(pin, value);
          break;
        case DIGITALWRITE:
          Serial.println(value ? F("IO >> ON") : F("IO >> OFF"));
          digitalWrite(pin, value);
          break;
      }
      break;
    }
    default:
      ThrowError((char*) "Operation", (char*) "Unknown IO operation", false);
      Serial.println(operation);
      return -1;
  }

  return 0;
}

int Operation::ConditionalOp(Processing::Process* process, uint8_t operation) {
  switch (operation) {
    case LOOP:
      process->loop_addr_ = process->program_ctr_;
      break;
    case ENDLOOP:
      process->program_ctr_ = process->loop_addr_;
      break;
    case IF:
      if (process->stack_->Peek() == 0) {
        process->program_ctr_ += EEPROM[process->program_ctr_] + 1;
      }
      break;
    case ELSE:
      if (process->stack_->Peek() != 0) {
        process->program_ctr_ += EEPROM[process->program_ctr_] + 1;
      }
      break;
    case (ENDIF):
      process->stack_->PopValue();
      break;
    case (WHILE):
      if (process->stack_->Peek() == 0) {
        process->program_ctr_ += EEPROM[process->program_ctr_ + 1] + 2;
      } else {
        process->stack_->PushByte(4 + (EEPROM[process->program_ctr_]) + EEPROM[process->program_ctr_ + 1]);
        process->program_ctr_ += 2;
      }
      break;
    case (ENDWHILE):
      process->program_ctr_ -= process->stack_->PopByte();
      break;
    default:
      ThrowError((char*) "Operation", (char*) "Unknown conditional operation", false);
      Serial.println(operation);
      return -1;
  }

  return 0;
}

int Operation::FileOp(Processing::Process *process, uint8_t operation) {
  switch (operation) {
    case OPEN: {
      int size = process->stack_->PopInt();
      char* file_name = process->stack_->PopString();
      char* data = new char[size];

      Fat::Write(file_name, size, data);

      // Set file pointer to start address of file
      process->file_ptr_ = Fat::GetStartAddr(Fat::FindFile(file_name));
      break;
    }
    case CLOSE:
      // Does nothing
      break;
    case WRITE: {
      uint8_t type = process->stack_->PopByte();
      process->stack_->PushByte(type);

      char* data = new char[FAT_DATA_SIZE];

      switch (type) {
        case CHAR: {
//          sprintf(data, "%c", process->stack_->PopChar());
          break;
        }
        case STRING: {
          sprintf(data, "%s", process->stack_->PopString());
          break;
        }
        case INT: {
//          sprintf(data, "%d", process->stack_->PopInt());
          break;
        }
        case FLOAT: {
//          sprintf(data, "%f", process->stack_->PopFloat());
          break;
        }
        default:
          break;
      }

      Serial.println(data);
      Fat::WriteData(process->file_ptr_, sizeof(data), data);
      process->file_ptr_++;
      Fat::Read((char*) "new_file");
      break;
    }
    case READINT:
//      process->file_ptr_++;
      break;
    case READCHAR:
//      process->file_ptr_++;
      break;
    case READFLOAT:
//      process->file_ptr_++;
      break;
    case READSTRING:
//      process->file_ptr_++;
      break;
    default:
      ThrowError((char*) "Operation", (char*) "Unknown file operation", false);
      Serial.println(operation);
      return -1;
  }

  return 0;
}

int Operation::PrintOp(Processing::Process* process, bool new_line) {
  uint8_t type = process->stack_->PopByte();
  process->stack_->PushByte(type);

  switch (type) {
    case CHAR:
      Serial.print(process->stack_->PopChar());
      break;
    case STRING:
      Serial.print(process->stack_->PopString());
      break;
    case INT:
      Serial.print(process->stack_->PopInt());
      break;
    case FLOAT:
      Serial.print(process->stack_->PopFloat());
      break;
    default:
      break;
  }

  if (new_line) {
    Serial.println();
  }

  return 0;
}

//void Operation::StackOp(Processing::Process* process, uint8_t data_type) {
//  if (data_type == STRING) {
//    uint8_t length = 0;
//    char next_char = 0;
//
//    do {
//      next_char = EEPROM[process->program_ctr_++];
//      length++;
//      stack.PushByte(&(process->stack_), next_char);
//    } while (next_char != 0);
//
//    stack.PushByte(&(process->stack_), length);
//  } else {
//    for (uint8_t i = 0; i < data_type; i++) {
//      stack.PushByte(&(process->stack_), EEPROM[process->program_ctr_++]);
//    }
//  }
//
//  stack.PushByte(&(process->stack_), data_type);
//}

//void Operation::UnaryOp(Processing::Process* process, uint8_t operation) {
//  uint8_t type = stack.PopByte(&(process->stack_));
//  stack.PushByte(&(process->stack_), type);
//
//  // Get the Data as float from stack
//  float value = stack.PopValue(&(process->stack_));
//
//  switch (operation) {
//    case INCREMENT:
//      value += 1.0F;
//      break;
//    case DECREMENT:
//      value -= 1.0F;
//      break;
//    case UNARYMINUS:
//      value *= -1.0F;
//      break;
//    case ABS:
//      value = (value >= 0 ? value : value* - 1.0F);
//      break;
//    case SQ:
//      value = value * value;
//      break;
//    case SQRT:
//      value = sqrt(value);
//      break;
//    case ANALOGREAD:
//      value = analogRead(value);
//      type = INT;
//      break;
//    case DIGITALREAD:
//      value = digitalRead(value);
//      type = CHAR;
//      break;
//    case LOGICALNOT:
//      value = (value == 0.0F ? 1 : 0);
//      type = CHAR;
//      break;
//    case BITWISENOT:
//      value = ~(int)value;
//      type = CHAR;
//      break;
//    case TOCHAR:
//      value = round(value);
//      type = CHAR;
//      break;
//    case TOINT:
//    case ROUND:
//      value = round(value) + 0;
//      type = INT;
//      break;
//    case TOFLOAT:
//      type = FLOAT;
//      break;
//    case FLOOR:
//      value = floor(value);
//      type = INT;
//      break;
//    case CEIL:
//      value = ceil(value);
//      type = INT;
//    default:
//      ThrowError((char*) "Operation", (char*) "Unknown unary operation", false);
//      Serial.println(operation, DEC);
//      break;
//  }
//
//  // Push (float) Data back on the stack by type
//  switch (type) {
//    case CHAR:
//      stack.PushChar(&(process->stack_), (char) value);
//      break;
//    case INT:
//      stack.PushInt(&(process->stack_), (int) value);
//      break;
//    case FLOAT:
//      stack.PushFloat(&(process->stack_), value);
//      break;
//    default:
//      break;
//  }
//}
//
//void Operation::BinaryOp(Processing::Process* process, uint8_t operation) {
//  float y_type = stack.PopByte(&(process->stack_));
//  stack.PushByte(&(process->stack_), y_type);
//  float y = stack.PopValue(&(process->stack_));
//
//  float x_type = stack.PopByte(&(process->stack_));
//  stack.PushByte(&(process->stack_), x_type);
//  float x = stack.PopValue(&(process->stack_));
//
//  // Use Z as output value, type determined by the largest type of X or Y
//  float z_type = (x_type > y_type ? x_type : y_type); // Get largest type
//  float z;
//
//  switch (operation) {
//    case PLUS:
//      z = x + y;
//      break;
//    case MINUS:
//      z = x - y;
//      break;
//    case TIMES:
//      z = x * y;
//      break;
//    case DIVIDEDBY:
//      z = x / y;
//      break;
//    case MODULUS:
//      z = (int) x % (int) y;
//      break;
//    case EQUALS:
//      z = (x == y ? 1 : 0);
//      z_type = CHAR;
//      break;
//    case NOTEQUALS:
//      z = (x != y ? 1 : 0);
//      z_type = CHAR;
//      break;
//    case LESSTHAN:
//      z = (x < y ? 1 : 0);
//      z_type = CHAR;
//      break;
//    case LESSTHANOREQUALS:
//      z = (x <= y ? 1 : 0);
//      z_type = CHAR;
//      break;
//    case GREATERTHAN:
//      z = (x > y ? 1 : 0);
//      z_type = CHAR;
//      break;
//    case GREATERTHANOREQUALS:
//      z = (x >= y ? 1 : 0);
//      z_type = CHAR;
//      break;
//    case MIN:
//      z = (x < y ? x : y);
//      break;
//    case MAX:
//      z = (x > y ? x : y);
//      break;
//    case POW:
//      for (int i = 0; i < y; i++) {
//        z *= x;
//      }
//      break;
//    case LOGICALAND:
//      z = (x && y ? 1 : 0);
//      z_type = CHAR;
//      break;
//    case LOGICALOR:
//      z = (x || y ? 1 : 0);
//      z_type = CHAR;
//      break;
//    case LOGICALXOR:
//      z = (!x != !y ? 1 : 0);
//      z_type = CHAR;
//      break;
//    case BITWISEAND:
//      z = (int) x & (int) y;
//      break;
//    case BITWISEOR:
//      z = (int) x | (int) y;
//      break;
//    case BITWISEXOR:
//      z = (int) x ^ (int) y;
//      break;
//    default:
////      ThrowError((char*) "Operation", (char*) "Unknown binary operation", false);
//      Serial.println(operation);
//      break;
//  }
//
//  // Push output value z back on the stack
//  switch ((int) z_type) {
//    case CHAR:
//      stack.PushChar(&(process->stack_), (char) z);
//      break;
//    case INT:
//      stack.PushInt(&(process->stack_), (int) z);
//      break;
//    case FLOAT:
//      stack.PushFloat(&(process->stack_), z);
//      break;
//    default:
//      break;
//  }
//}
//
//void Operation::TimeOp(Processing::Process* process, uint8_t operation) {
//  switch (operation) {
//    case DELAY:
//      delay((int) stack.PopValue(&(process->stack_)));
//      break;
//    case DELAYUNTIL: {
//      int x = (int) stack.PopValue(&(process->stack_));
//
//      if (x > (int) millis()) {
//        stack.PushInt(&(process->stack_), x);
//        process->program_ctr_ -= 1;
//      }
//      break;
//    }
//    case MILLIS:
//      stack.PushInt(&(process->stack_), (int) millis());
//      break;
//    default:
//      ThrowError((char*) "Operation", (char*) "Unknown time operation", false);
//      Serial.println(operation);
//      break;
//  }
//}
//
//void Operation::IoOp(Processing::Process* process, uint8_t operation) {
//  switch (operation) {
//    case CONSTRAIN: {
//      // Pop in reverse order
//      int max = (int) stack.PopValue(&(process->stack_)); // Maximum
//      int min = (int) stack.PopValue(&(process->stack_)); // Minimum
//      int x = (int) stack.PopValue(&(process->stack_)); // Constrain value
//      stack.PushInt(&(process->stack_), min(max(x, min), max)); // Push Result
//      break;
//    }
//    case MAP: {
//      // Pop in reverse order
//      int out_max = (int) stack.PopValue(&(process->stack_));
//      int out_min = (int) stack.PopValue(&(process->stack_));
//      int in_max = (int) stack.PopValue(&(process->stack_));
//      int in_min = (int) stack.PopValue(&(process->stack_));
//      int x = (int) stack.PopValue(&(process->stack_)); // Value to map
//      stack.PushInt(&(process->stack_), map(x, in_min, in_max, out_min, out_max)); // Push Result
//      break;
//    }
//    case PINMODE:
//    case DIGITALWRITE:
//    case ANALOGWRITE: {
//      int value = (int) stack.PopValue(&(process->stack_));
//      int pin = (int) stack.PopValue(&(process->stack_));
//
//      switch (operation) {
//        case PINMODE:
//          pinMode(pin, value);
//          break;
//        case ANALOGWRITE:
//          analogWrite(pin, value);
//          break;
//        case DIGITALWRITE:
//          Serial.println(value ? "IO > ON" : "IO > OFF");
//          digitalWrite(pin, value);
//          break;
//        default:
//          break;
//      }
//      break;
//    }
//    default:
//      ThrowError((char*) "Operation", (char*) "Unknown IO operation", false);
//      Serial.println(operation);
//      break;
//  }
//}
//
//void Operation::ConditionalOp(Processing::Process* process, uint8_t operation) {
//  switch (operation) {
//    case LOOP:
//      process->loop_addr = process->program_ctr_;
//      break;
//    case ENDLOOP:
//      process->program_ctr_ = process->loop_addr;
//      break;
//    case IF:
//      if (stack.Peek(&(process->stack_)) == 0) {
//        process->program_ctr_ += EEPROM[process->program_ctr_] + 1;
//      }
//      break;
//    case ELSE:
//      if (stack.Peek(&(process->stack_)) != 0) {
//        process->program_ctr_ += EEPROM[process->program_ctr_] + 1;
//      }
//      break;
//    case (ENDIF):
//      stack.PopValue(&(process->stack_));
//      break;
//    case (WHILE):
//      if (stack.Peek(&(process->stack_)) == 0) {
//        process->program_ctr_ += EEPROM[process->program_ctr_ + 1] + 2;
//      } else {
//        stack.PushByte(&(process->stack_), 4 + (EEPROM[process->program_ctr_]) + EEPROM[process->program_ctr_ + 1]);
//        process->program_ctr_ += 2;
//      }
//      break;
//    case (ENDWHILE):
//      process->program_ctr_ -= stack.PopByte(&(process->stack_));
//      break;
//    default:
//      ThrowError((char*) "Operation", (char*) "Unknown conditional operation", false);
//      Serial.println(operation);
//      break;
//  }
//}
//
//void Operation::FileOp(Processing::Process *process, uint8_t operation) {
//  switch (operation) {
//    case OPEN: {
//      int size = stack.PopInt(&(process->stack_));
//      char* file_name = stack.PopString(&(process->stack_));
//      char* data = new char[size];
//
//      Fat::Write(file_name, size, data);
//
//      // Set file pointer to start address of file
//      process->file_ptr_ = Fat::GetStartAddr(Fat::FindFile(file_name));
//      break;
//    }
//    case CLOSE:
//      // Does nothing
//      break;
//    case WRITE: {
//      uint8_t type = stack.PopByte(&(process->stack_));
//      stack.PushByte(&(process->stack_), type);
//
//      char* data = new char[FAT_DATA_SIZE];
//
//      switch (type) {
//        case CHAR: {
////          sprintf(data, "%c", stack.PopChar(&(process->stack_)));
//          break;
//        }
//        case STRING: {
//          sprintf(data, "%s", stack.PopString(&(process->stack_)));
//          break;
//        }
//        case INT: {
////          sprintf(data, "%d", stack.PopInt(&(process->stack_)));
//          break;
//        }
//        case FLOAT: {
////          sprintf(data, "%f", stack.PopFloat(&(process->stack_)));
//          break;
//        }
//        default:
//          break;
//      }
//
//      Serial.println(data);
//      Fat::WriteData(process->file_ptr_, sizeof(data), data);
//      process->file_ptr_++;
//      Fat::Read((char*) "new_file");
//      break;
//    }
//    case READINT:
////      process->file_ptr_++;
//      break;
//    case READCHAR:
////      process->file_ptr_++;
//      break;
//    case READFLOAT:
////      process->file_ptr_++;
//      break;
//    case READSTRING:
////      process->file_ptr_++;
//      break;
//    default:
//      ThrowError((char*) "Operation", (char*) "Unknown file operation", false);
//      Serial.println(operation);
//      break;
//  }
//}
//
//void Operation::PrintOp(Processing::Process* process, bool new_line) {
//  uint8_t type = stack.PopByte(&(process->stack_));
//  stack.PushByte(&(process->stack_), type);
//
//  switch (type) {
//    case CHAR:
////      Serial.print(stack.PopChar(&(process->stack_)));
//      break;
//    case STRING:
////      Serial.print(stack.PopString(&(process->stack_)));
//      break;
//    case INT:
////      Serial.print(stack.PopInt(&(process->stack_)));
//      break;
//    case FLOAT:
////      Serial.print(stack.PopFloat(&(process->stack_)));
//      break;
//    default:
//      break;
//  }
//
//  if (new_line) {
////    Serial.println();
//  }
//}