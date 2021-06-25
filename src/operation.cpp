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
      Serial.println(operation);
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
  float y_type = process->stack_->PopByte();
  process->stack_->PushByte(y_type);
  float y = process->stack_->PopValue();

  float x_type = process->stack_->PopByte();
  process->stack_->PushByte(x_type);
  float x = process->stack_->PopValue();

  // Use Z as output value, type determined by the largest type of X or Y
  float z_type = (x_type > y_type ? x_type : y_type); // Get largest type
  float z;

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
      char *file_name = process->stack_->PopString();
      char *data = new char[size];

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

      switch (type) {
        case CHAR: {
          char char_val[1] = { process->stack_->PopChar() };
          Fat::WriteData(process->file_ptr_, sizeof(char_val), char_val);
          process->file_ptr_ += sizeof(char_val);
          break;
        }
        case STRING: {
          char *str_val = process->stack_->PopString();
          Fat::WriteData(process->file_ptr_, strlen(str_val), str_val);
          process->file_ptr_ += strlen(str_val);
          break;
        }
        case INT: {
          char int_arr[2];
          int int_val = process->stack_->PopInt();
          int_arr[0] = lowByte(int_val);
          int_arr[1] = highByte(int_val);

          Fat::WriteData(process->file_ptr_, sizeof(int_arr), int_arr);
          process->file_ptr_ += sizeof(int_arr);
          break;
        }
        case FLOAT: {
          // TODO: Due to this stupid assignment being on a retro chip, being atmega328 and it stupidly short memory this exceeds it.
//          float float_val = process->stack_->PopFloat();
//          char *float_ptr = (char *) &float_val;
//          char float_arr[4];
//
//          for (uint8_t i = 3; i >= 0; i--) {
//            float_arr[i] = float_ptr[i];
//          }
//
//          Serial.println(float_ptr);
//
//          Fat::WriteData(process->file_ptr_, sizeof(float_arr), float_arr);
//          process->file_ptr_ += sizeof(float_arr);
          break;
        }
        default:
          ThrowError((char *) "Operation", (char *) "Unknown file data type", false);
          Serial.println(type);
          break;
      }
      break;
    }
    case READINT: {
      process->stack_->PushInt(word(EEPROM[process->file_ptr_++], EEPROM[process->file_ptr_++]));
      break;
    }
    case READCHAR: {
      process->stack_->PushChar(EEPROM[process->file_ptr_++]);
      break;
    }
    case READFLOAT: {
//      float f = 0.0;
//      char *b = (char *) &f;
//
//      for (uint8_t i = 3; i >= 0; i--) {
//        b[i] = EEPROM[process->file_ptr_++];
//      }
//
//      Serial.println(f);
      break;
    }
    case READSTRING: {
      char *str; // It's not ideal but it works
      char next_char;
      uint8_t idx = 0;

      do {
        next_char = EEPROM[process->file_ptr_++];
        str[idx] = next_char;
        idx++;
      } while (next_char != '\n');

      process->stack_->PushString(str);
      break;
    }
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
    case FLOAT: {
      Serial.print(process->stack_->PopFloat());
      break;
    }
    default:
      break;
  }

  if (new_line) {
    Serial.println();
  }

  return 0;
}