#include "operation.h"

Stack stack;

Operation::Operation() {
}

Operation::~Operation() {

}

void Operation::StackOp(Processing::Process* process, uint8_t data_type) {
  if (data_type == STRING) {
    uint8_t length = 0;
    char next_char = 0;

    do {
      next_char = EEPROM[process->program_ctr++];
      length++;
      stack.PushByte(&(process->stack), next_char);
    } while (next_char != 0);

    stack.PushByte(&(process->stack), length);
  } else {
    for (uint8_t i = 0; i < data_type; i++) {
      stack.PushByte(&(process->stack), EEPROM[process->program_ctr++]);
    }
  }

  stack.PushByte(&(process->stack), data_type);
}

void Operation::UnaryOp(Processing::Process* process, uint8_t operation) {
  uint8_t type = stack.PopByte(&(process->stack));
  stack.PushByte(&(process->stack), type);

  // Get the Data as float from stack
  float value = stack.PopValue(&(process->stack));

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
      value = ~(int)value;
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
    default:
      ThrowError((char*) "Operation", (char*) "Unknown unary operation", false);
      Serial.println(operation, DEC);
      break;
  }

  // Push (float) Data back on the stack by type
  switch (type) {
    case CHAR:
      stack.PushChar(&(process->stack), (char) value);
      break;
    case INT:
      stack.PushInt(&(process->stack), (int) value);
      break;
    case FLOAT:
      stack.PushFloat(&(process->stack), value);
      break;
    default:
      break;
  }
}

void Operation::BinaryOp(Processing::Process* process, uint8_t operation) {
  float y_type = stack.PopByte(&(process->stack));
  stack.PushByte(&(process->stack), y_type);
  float y = stack.PopValue(&(process->stack));

  float x_type = stack.PopByte(&(process->stack));
  stack.PushByte(&(process->stack), x_type);
  float x = stack.PopValue(&(process->stack));

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
//      ThrowError((char*) "Operation", (char*) "Unknown binary operation", false);
      Serial.println(operation);
      break;
  }

  // Push output value z back on the stack
  switch ((int) z_type) {
    case CHAR:
      stack.PushChar(&(process->stack), (char) z);
      break;
    case INT:
      stack.PushInt(&(process->stack), (int) z);
      break;
    case FLOAT:
      stack.PushFloat(&(process->stack), z);
      break;
    default:
      break;
  }
}

void Operation::TimeOp(Processing::Process* process, uint8_t operation) {
  switch (operation) {
    case DELAY:
      delay((int) stack.PopValue(&(process->stack)));
      break;
    case DELAYUNTIL: {
      int x = (int) stack.PopValue(&(process->stack));

      if (x > (int) millis()) {
        stack.PushInt(&(process->stack), x);
        process->program_ctr -= 1;
      }
      break;
    }
    case MILLIS:
      stack.PushInt(&(process->stack), (int) millis());
      break;
    default:
      ThrowError((char*) "Operation", (char*) "Unknown time operation", false);
      Serial.println(operation);
      break;
  }
}

void Operation::IoOp(Processing::Process* process, uint8_t operation) {
  switch (operation) {
    case CONSTRAIN: {
      // Pop in reverse order
      int max = (int) stack.PopValue(&(process->stack)); // Maximum
      int min = (int) stack.PopValue(&(process->stack)); // Minimum
      int x = (int) stack.PopValue(&(process->stack)); // Constrain value
      stack.PushInt(&(process->stack), min(max(x, min), max)); // Push Result
      break;
    }
    case MAP: {
      // Pop in reverse order
      int out_max = (int) stack.PopValue(&(process->stack));
      int out_min = (int) stack.PopValue(&(process->stack));
      int in_max = (int) stack.PopValue(&(process->stack));
      int in_min = (int) stack.PopValue(&(process->stack));
      int x = (int) stack.PopValue(&(process->stack)); // Value to map
      stack.PushInt(&(process->stack), map(x, in_min, in_max, out_min, out_max)); // Push Result
      break;
    }
    case PINMODE:
    case DIGITALWRITE:
    case ANALOGWRITE: {
      int value = (int) stack.PopValue(&(process->stack));
      int pin = (int) stack.PopValue(&(process->stack));

      switch (operation) {
        case PINMODE:
          pinMode(pin, value);
          break;
        case ANALOGWRITE:
          analogWrite(pin, value);
          break;
        case DIGITALWRITE:
          Serial.println(value ? "IO > ON" : "IO > OFF");
          digitalWrite(pin, value);
          break;
        default:
          break;
      }
      break;
    }
    default:
      ThrowError((char*) "Operation", (char*) "Unknown IO operation", false);
      Serial.println(operation);
      break;
  }
}

void Operation::ConditionalOp(Processing::Process* process, uint8_t operation) {
  switch (operation) {
    case LOOP:
      process->loop_addr = process->program_ctr;
      break;
    case ENDLOOP:
      process->program_ctr = process->loop_addr;
      break;
    case IF:
      if (stack.Peek(&(process->stack)) == 0) {
        process->program_ctr += EEPROM[process->program_ctr] + 1;
      }
      break;
    case ELSE:
      if (stack.Peek(&(process->stack)) != 0) {
        process->program_ctr += EEPROM[process->program_ctr] + 1;
      }
      break;
    case (ENDIF):
      stack.PopValue(&(process->stack));
      break;
    case (WHILE):
      if (stack.Peek(&(process->stack)) == 0) {
        process->program_ctr += EEPROM[process->program_ctr + 1] + 2;
      } else {
        stack.PushByte(&(process->stack), 4 + (EEPROM[process->program_ctr]) + EEPROM[process->program_ctr + 1]);
        process->program_ctr += 2;
      }
      break;
    case (ENDWHILE):
      process->program_ctr -= stack.PopByte(&(process->stack));
      break;
    default:
      ThrowError((char*) "Operation", (char*) "Unknown conditional operation", false);
      Serial.println(operation);
      break;
  }
}

void Operation::FileOp(Processing::Process *process, uint8_t operation) {
  switch (operation) {
    case OPEN: {
      int size = stack.PopInt(&(process->stack));
      char* file_name = stack.PopString(&(process->stack));
      char* data = new char[size];

      Fat::Write(file_name, size, data);

      // Set file pointer to start address of file
      process->file_ptr = Fat::GetStartAddr(Fat::FindFile(file_name));
      break;
    }
    case CLOSE:
      // Does nothing
      break;
    case WRITE: {
      uint8_t type = stack.PopByte(&(process->stack));
      stack.PushByte(&(process->stack), type);

      char* data = new char[FAT_DATA_SIZE];

      switch (type) {
        case CHAR: {
//          sprintf(data, "%c", stack.PopChar(&(process->stack)));
          break;
        }
        case STRING: {
          sprintf(data, "%s", stack.PopString(&(process->stack)));
          break;
        }
        case INT: {
//          sprintf(data, "%d", stack.PopInt(&(process->stack)));
          break;
        }
        case FLOAT: {
//          sprintf(data, "%f", stack.PopFloat(&(process->stack)));
          break;
        }
        default:
          break;
      }

      Serial.println(data);
      Fat::WriteData(process->file_ptr, sizeof(data), data);
      process->file_ptr++;
      Fat::Read((char*) "new_file");
      break;
    }
    case READINT:
//      process->file_ptr++;
      break;
    case READCHAR:
//      process->file_ptr++;
      break;
    case READFLOAT:
//      process->file_ptr++;
      break;
    case READSTRING:
//      process->file_ptr++;
      break;
    default:
      ThrowError((char*) "Operation", (char*) "Unknown file operation", false);
      Serial.println(operation);
      break;
  }
}

void Operation::PrintOp(Processing::Process* process, bool new_line) {
  uint8_t type = stack.PopByte(&(process->stack));
  stack.PushByte(&(process->stack), type);

  switch (type) {
    case CHAR:
//      Serial.print(stack.PopChar(&(process->stack)));
      break;
    case STRING:
//      Serial.print(stack.PopString(&(process->stack)));
      break;
    case INT:
//      Serial.print(stack.PopInt(&(process->stack)));
      break;
    case FLOAT:
//      Serial.print(stack.PopFloat(&(process->stack)));
      break;
    default:
      break;
  }

  if (new_line) {
//    Serial.println();
  }
}