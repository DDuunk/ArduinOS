#ifndef MEMORY_H_
#define MEMORY_H_

#include "Arduino.h"
#include "stack.h"
#include "instruction_set.h"
#include "utils.h"

#define TABLE_SIZE 25
#define MEMORY_SIZE 256

class Memory {

 public:
  struct MemoryEntry {
    int pid;
    uint8_t name;
    uint8_t type;
    uint16_t addr;
    uint16_t size;
  };

  MemoryEntry memory_table_[TABLE_SIZE] = {};
  uint8_t memory_[MEMORY_SIZE] = {0 };

  int num_of_vars_;

 public:
  Memory();
  ~Memory();

  bool StoreEntry(uint8_t name, int pid, Stack::stack_t* stack);

  int GetEntry(uint8_t name, int pid, Stack::stack_t* stack);

  /**
   * @brief Find a entry in the memory table
   * @return entry index
   */
  int FindEntry(uint8_t name, int pid);

  /**
   * @brief Delete a entry from memory table
   */
  void DeleteEntry(int index);

  /**
   * @brief Clear all entries in memory table
   */
  void ClearEntries(int pid);

  /**
   * @brief Return available start position
   */
  int GetStartAddr(int size);

  static int GetTypeSize(char type, Stack::stack_t* stack);

  /**
   * @brief Print memory table
   */
  void PrintMemoryTable();

};

#endif //MEMORY_H_
