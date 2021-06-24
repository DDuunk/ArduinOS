#ifndef MEMORY_H_
#define MEMORY_H_

#include "Arduino.h"
#include "stack.h"
#include "instruction_set.h"
#include "utils.h"

#define MEMORY_COUNT 25
#define MEMORY_SIZE 256

class Memory {

 public:
  typedef PROGMEM struct {
    uint8_t pid_;
    uint8_t name_;
    uint8_t type_;
    uint16_t addr_;
    uint8_t size_;
  } MemoryEntry;

  MemoryEntry memory_table_[MEMORY_COUNT];
  uint8_t memory_[MEMORY_SIZE];

  uint8_t num_of_vars_ = 0;

 public:
  Memory();
  ~Memory();

  /**
   * @brief Store an entry in memory
   * @param name
   * @param pid
   * @param stack
   * @return 0 (error-code: OK) || -1 (error-code: FAILED)
   */
  int StoreEntry(uint8_t name, uint8_t pid, Stack* stack);

  /**
   * @brief Get an entry from memory, and push to stack
   * @param name
   * @param pid
   * @param stack
   * @return 0 (error-code: OK) || -1 (error-code: FAILED)
   */
  int GetEntry(uint8_t name, uint8_t pid, Stack* stack);

  /**
   * @brief Find an entry in the memory table
   * @param name
   * @param pid
   * @return entry index
   */
  int FindEntry(uint8_t name, uint8_t pid);

  /**
   * @brief Delete an entry from the memory table
   * @param index
   * @return 0 (error-code: OK) || -1 (error-code: FAILED)
   */
  int DeleteEntry(uint8_t index);

  /**
   * @brief Clear all entries in the memory table
   * @param pid
   * @return 0 (error-code: OK) || -1 (error-code: FAILED)
   */
  int ClearEntries(uint8_t pid);

  /**
   * @brief Return available start position where size fits
   * @param size
   * @return Start position
   */
  int GetStartAddr(uint8_t size);

  /**
   * @brief Get the size of a type (CHAR, INT, FLOAT, STRING)
   * @param type
   * @param stack
   * @return type size
   */
  int GetTypeSize(char type, Stack* stack);

  /**
   * @brief Print memory table
   * @return 0 (error-code: OK)
   */
  int PrintMemoryTable();

//  bool StoreEntry(uint8_t name, int pid, Stack::stack_t* stack);
//  int GetEntry(uint8_t name, int pid, Stack::stack_t* stack);
//  int FindEntry(uint8_t name, int pid);
//  void DeleteEntry(int index);
//  void ClearEntries(int pid);
//  int GetStartAddr(int size);
//  static int GetTypeSize(char type, Stack::stack_t* stack);
//  void PrintMemoryTable();

 private:
  void Reset();

};

#endif //MEMORY_H_
