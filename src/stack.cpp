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

char* Stack::PopString()  {
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
