#include "fat.h"

EERef Fat::num_of_files_ = EEPROM[0];

Fat::Fat() {
  num_of_files_ = 0;

  for (uint8_t i = 0; i < NUM_OF_FAT_ENTRIES; i++) {
    FatEntry empty_file = (FatEntry) {
        "", 0, 0
    };
    WriteFatEntry(i, empty_file);
  }
}

Fat::~Fat() {}

bool Fat::Write(char *name, uint16_t size, char *data) {
  Serial.print(F("[Fat]("));
  Serial.print(size);
  Serial.print(F(") Attempting to write file "));
  Serial.println(name);

  if (num_of_files_ >= NUM_OF_FAT_ENTRIES) {
    ThrowError((char*) "Fat", (char*) "Fat is full");
    return false;
  }

  if (FindFile(name) != -1) {
    ThrowError((char*) "Fat", (char*) "Filename already exists in Fat");
    return false;
  }

  int16_t data_addr = GetAvailableAddr(size);
  if (data_addr == -1) {
    ThrowError((char*) "Fat", (char*) "Not enough space available");
    return false;
  }

  FatEntry file = (FatEntry) {"", (uint16_t) data_addr, size};
  strcpy(file.name_, name);

  int8_t file_addr = GetAvailableFileAddr();
  if (file_addr == -1) {
    ThrowError((char*) "Fat", (char*) "No empty file available");
    return false;
  }

  WriteFatEntry(file_addr, file);
  WriteData(file.addr_, size, data);
  num_of_files_ += 1;
  return true;
}

bool Fat::Read(char *name) {
  uint8_t entry = FindFile(name);

  if (entry >= 0) {
    char buffer[60];
    FatEntry file = ReadFatEntry(entry);

    for (uint16_t i = 0; i < file.size_; i++) {
      uint16_t address = file.addr_ + i;
      uint8_t value = EEPROM.read(address);
      buffer[i] = value;
    }

    Serial.println(buffer);
    return true;
  } else {
    ThrowError((char*) "Fat", (char*) "File", false);
    Serial.print(name);
    Serial.println(F(" does not exists"));
    return false;
  }
}

bool Fat::Delete(char *name) {
  uint8_t entry = FindFile(name);

  if (entry >= 0) {
    FatEntry file = ReadFatEntry(entry);
    file.name_[0] = '\0';
    file.addr_ = 0;
    file.size_ = 0;

    WriteFatEntry(entry, file);
    num_of_files_ -= 1;

    Serial.print(F("[Fat] "));
    Serial.print(name);
    Serial.println(F(" successfully deleted"));
    return true;
  } else {
    ThrowError((char*) "Fat", (char*) "File does not exist");
    return false;
  }
}

bool Fat::WriteData(uint16_t addr, uint16_t size, char *data) {
  uint16_t next_addr = 0;

  for (uint16_t i = 0; i < size; i++) {
    EEPROM.write(addr + next_addr, data[i]);
    next_addr += sizeof(data[i]);
  }

  return true;
}

int Fat::WriteFatEntry(uint8_t pos, Fat::FatEntry file) {
  EEPROM.put((FAT_START_ADDR + pos * sizeof(FatEntry)), file);
  return 0;
}

Fat::FatEntry Fat::ReadFatEntry(uint8_t pos) {
  FatEntry file;
  EEPROM.get((FAT_START_ADDR + pos * sizeof(FatEntry)), file); // Write data to file
  return file;
}

int Fat::GetStartAddr(uint8_t index) {
  FatEntry file = ReadFatEntry(index);
  return file.addr_;
}

int Fat::GetAvailableFileAddr() {
  for (uint8_t i = 0; i < NUM_OF_FAT_ENTRIES; i++) {
    FatEntry file = ReadFatEntry(i);

    if (file.size_ == 0) {
      return i;
    }
  }

  return -1;
}

int Fat::GetAvailableAddr(uint16_t size) {
  uint16_t available_addr = sizeof(num_of_files_) + (sizeof(FatEntry) * NUM_OF_FAT_ENTRIES) + 1;

  if (num_of_files_ == 0) {
    return available_addr;
  } else {
    bool first_file = true;

    for (uint8_t i = 0; i < NUM_OF_FAT_ENTRIES; i++) {
      FatEntry file = ReadFatEntry(i);

      if (file.size_ > 0) {
        if (first_file && size < file.addr_ - available_addr) {
          return available_addr;
        }

        int next_file_addr = E2END + 1;

        for (uint8_t j = i + 1; j < NUM_OF_FAT_ENTRIES; j++) {
          FatEntry next_file = ReadFatEntry(j);

          if (next_file.size_ != 0) {
            next_file_addr = next_file.addr_;
          }
        }

        if (size < next_file_addr - (file.addr_ + file.size_)) {
          return file.addr_ + file.size_;
        }

        first_file = false;
      }
    }
  }

  return 1;
}

int Fat::FindFile(char *file_name) {
  for (uint8_t i = 0; i < NUM_OF_FAT_ENTRIES; i++) {
    FatEntry file = ReadFatEntry(i);

    if (strcmp(file.name_, file_name) == 0) {
      return i;
    }
  }

  return -1;
}

int Fat::Freespace() {
  if (num_of_files_ == NUM_OF_FAT_ENTRIES) {
    return -1;
  }

  if (num_of_files_ == 0) {
    return (FAT_DATA_SIZE * NUM_OF_FAT_ENTRIES);
  }

  uint16_t first_writable_pos = FAT_DATA_SIZE + sizeof(num_of_files_) + (sizeof(FatEntry) * NUM_OF_FAT_ENTRIES);
  uint16_t unused_space = EEPROM.length() - (first_writable_pos + (FAT_DATA_SIZE * NUM_OF_FAT_ENTRIES));

  uint16_t start_pos_space = first_writable_pos;
  uint16_t largest_free_space = 0;
  uint16_t end_pos_space = 0;
  uint16_t space = 0;

  for (int i = 0; i < NUM_OF_FAT_ENTRIES; i++) {
    FatEntry file = ReadFatEntry(i);

    if (i != (NUM_OF_FAT_ENTRIES - 1) && file.size_ <= 0) {
      continue;
    }

    // If no last file, space ends at end of usable FAT
    if (i == (NUM_OF_FAT_ENTRIES - 1)) {
      end_pos_space = EEPROM.length() - unused_space;
    } else if (i == 0) {
      end_pos_space = start_pos_space;
    } else {
      end_pos_space = file.addr_ - 1;
    }

    space = end_pos_space - start_pos_space;
    if (space > largest_free_space) {
      largest_free_space = space;
    }

    start_pos_space = file.addr_ + file.size_;
  }

  return largest_free_space;
}

int Fat::ListFiles() {
  bool has_files = false;

  for (uint8_t i = 0; i < NUM_OF_FAT_ENTRIES; i++) {
    FatEntry file = ReadFatEntry(i);

    if (file.size_ > 0) {
      has_files = true;

      Serial.print(F("["));
      Serial.print(i);
      Serial.print(F("] "));
      Serial.print(file.name_);
      Serial.print(F(" "));
      Serial.print(file.addr_);
      Serial.print(F("-"));
      Serial.print(file.addr_ + file.size_ - 1);
      Serial.print(F(" ("));
      Serial.print(file.size_);
      Serial.println(F(" bytes)"));
    }
  }

  if (!has_files) {
    ThrowError((char*) "Fat", (char*) "No files found in Fat");
    return -1;
  }

  return 0;
}

int Fat::PrintEeprom() {
//  char buffer[16];
//  char value_print[4];
//  int value;
//  unsigned int address;
//  int trailing_space = 2;
//
//  Serial.print(F("Dumping "));
//  Serial.print(E2END + 1);
//  Serial.println(F(" bytes from EEPROM."));
//  Serial.print(F("baseAddr "));
//
//  for (uint8_t x = 0; x < 2; x++) {
//    Serial.print(F(" "));
//
//    for (uint8_t y = 0; y < 25; y++) {
//      Serial.print(F("="));
//    }
//  }
//
//  // E2END is a macro defined as the last EEPROM address
//  // (1023 for ATMEGA328P)
//  for (address = 0; address <= E2END; address++) {
//    // read a byte from the current address of the EEPROM
//    value = EEPROM.read(address);
//
//    // add space between two sets of 8 bytes
//    if (address % 8 == 0) {
//      Serial.print(F("  "));
//    }
//
//    // newline and address for every 16 bytes
//    if (address % 16 == 0) {
//      // print the buffer
//      if (address > 0 && address % 16 == 0) {
//        PrintAscii(buffer);
//      }
//
//      sprintf_P(buffer, "\n 0x%05X: ", address);
//      Serial.print(buffer);
//      Serial.print(F(" "));
//      Serial.print(address);
//      Serial.print(F("\t"));
//
//      //Clear the buffer for the next Data block
//      memset(buffer, 32, 16);
//    }
//
//    // save the value in temporary storage
//    buffer[address % 16] = value;
//
//    // print the formatted value
//    sprintf_P(value_print, " %02X", value);
//    Serial.print(value_print);
//  }
//
//  if (address % 16 > 0) {
//    if (address % 16 < 9) {
//      trailing_space += 2;
//    }
//
//    trailing_space += (16 - address % 16) * 3;
//  }
//
//  for (uint8_t i = trailing_space; i > 0; i--) {
//    Serial.print(F(" "));
//  }
//
//  // last line of Data and a new line
//  PrintAscii(buffer);
//  Serial.println(F(""));
  return 0;
}

int Fat::ClearEeprom() {
  Serial.println(F("Attempting to clear"));
  for (uint8_t i = 0; i < EEPROM.length(); i++) {
    EEPROM.write(i, 0);
  }
  Serial.println(F("[Fat] EEPROM memory is erased"));
  return 0;
}