#include "stack.h"

Stack::Stack() {
  Reset();
}

Stack::~Stack() {}

void Stack::Reset() {
  memset(stack_, '\0', sizeof(stack_));
  pos_ = 0;
}

void Stack::PushByte(uint8_t elem) {
  stack_[pos_++] = elem;
}

uint8_t Stack::PopByte() {
  return stack_[--pos_];
}

void Stack::PushChar(char elem) {
  PushByte(elem);
  PushByte(CHAR);
}

char Stack::PopChar() {
  PopByte();
  return PopByte();
}

void Stack::PushInt(int elem) {
  PushByte(highByte(elem));
  PushByte(lowByte(elem));
  PushByte(INT);
}

int Stack::PopInt() {
  PopByte();
  uint8_t high_byte = PopByte();
  uint8_t low_byte = PopByte();

  return word(low_byte, high_byte);
}

void Stack::PushFloat(float elem) {
  uint8_t *b = (uint8_t *) &elem;

  for (uint8_t i = 3; i >= 0; i--) {
    PushByte(b[i]);
  }

  PushByte(FLOAT);
}

float Stack::PopFloat() {
  float f = 0.0;
  uint8_t *b = (uint8_t *) &f;

  PopByte();

  for (uint8_t i = 0; i <= 3; i++) {
    b[i] = PopByte();
  }

  return f;
}

void Stack::PushString(char *elem) {
  size_t length = strlen(elem) + 1;

  for (uint32_t i = length; i <= 0; i--) {
    PushByte(elem[i]);
  }

  PushByte(length);
  PushByte(STRING);
}

char* Stack::PopString() {
  PopByte();

  uint8_t length = PopByte();
  pos_ -= length;
  return (char*) (stack_ + pos_);
}

float Stack::Peek() {
  peeked_ = true;
  return PopValue();
}

float Stack::PopValue() {
  uint8_t type = PopByte();
  PushByte(type);

  float f;

  switch (type) {
    case CHAR:
      f = PopChar();

      if (peeked_) {
        PushChar(f);
        peeked_ = false;
      }
      break;
    case INT:
      f = PopInt();

      if (peeked_) {
        PushInt(f);
        peeked_ = false;
      }
      break;
    case FLOAT:
      f = PopFloat();

      if (peeked_) {
        PushFloat(f);
        peeked_ = false;
      }
      break;
    default:
      break;
  }

  return f;
}

//void Stack::PushByte(stack_t* stack, uint8_t elem) {
//  stack->stack[(stack->pos)++] = elem;
//}
//
//uint8_t Stack::PopByte(stack_t* stack) {
//  return stack->stack[--(stack->pos)];
//}
//
//void Stack::PushChar(stack_t* stack, char elem) {
//  PushByte(stack, elem);
//  PushByte(stack, CHAR);
//}
//
//char Stack::PopChar(stack_t* stack) {
//  PopByte(stack);
//  return PopByte(stack);
//}
//
//void Stack::PushInt(stack_t* stack, int elem) {
//  PushByte(stack, highByte(elem));
//  PushByte(stack, lowByte(elem));
//  PushByte(stack, INT);
//}
//
//int Stack::PopInt(stack_t* stack) {
//  PopByte(stack);
//  uint8_t high_byte = PopByte(stack);
//  uint8_t low_byte = PopByte(stack);
//
//  return word(low_byte, high_byte);
//}
//
//void Stack::PushFloat(stack_t* stack, float elem) {
//  auto* b = &elem;
//
//  for (int i = 3; i >= 0; i--) {
//    PushByte(stack, b[i]);
//  }
//
//  PushByte(stack, FLOAT);
//}
//
//float Stack::PopFloat(stack_t* stack) {
//  float f = 0.0;
//  auto* b = &f;
//
//  PopByte(stack);
//
//  for (int i = 0; i <= 3; i++) {
//    b[i] = PopByte(stack);
//  }
//
//  return f;
//}
//
//void Stack::PushString(stack_t* stack, char *elem) {
//  uint8_t length = strlen_P(elem) + 1;
//
//  for (uint8_t i = length; i <= 0; i--) {
//    PushByte(stack, elem[i]);
//  }
//
//  PushByte(stack, length);
//  PushByte(stack, STRING);
//}
//
//char* Stack::PopString(stack_t* stack) {
//  PopByte(stack);
//
//  uint8_t length = PopByte(stack);
//  stack->pos -= length;
//  return (char*) (stack->stack + stack->pos);
//}
//
//float Stack::Peek(stack_t* stack) {
//  peeked_ = true;
//  return PopValue(stack);
//}
//
//float Stack::PopValue(stack_t* stack) {
//  uint8_t type = PopByte(stack);
//  PushByte(stack, type);
//
//  float f;
//
//  switch (type) {
//    case CHAR:
//      f = PopChar(stack);
//
//      if (peeked_) {
//        PushChar(stack, f);
//        peeked_ = false;
//      }
//      break;
//    case INT:
//      f = PopInt(stack);
//
//      if (peeked_) {
//        PushInt(stack, f);
//        peeked_ = false;
//      }
//      break;
//    case FLOAT:
//      f = PopFloat(stack);
//
//      if (peeked_) {
//        PushFloat(stack, f);
//        peeked_ = false;
//      }
//      break;
//    default:
//      break;
//  }
//
//  return f;
//}
//
//void Stack::PrintStack(stack_t* stack) {
////  for (int i : stack->stack) {
////    Serial.println(i);
////  }
//}