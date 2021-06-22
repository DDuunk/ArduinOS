#ifndef FAT_H_
#define FAT_H_

#include "Arduino.h"
#include "EEPROM.h"
#include "utils.h"

#define NUM_OF_FAT_ENTRIES 10
#define FAT_START_ADDR 1
#define FAT_DATA_SIZE 60

class Fat {

 public:

 private:
  struct FatEntry {
    char name[12]; // Name of file
    int addr; // Data address
    int size; // Size of data
  };

  static EERef num_of_files_;

 public:
  Fat();
  ~Fat();

  /**
   * @brief Create a new file
   * @param name
   * @param size
   * @param data
   * @return true if succesful, false if not
   */
  static bool Write(char* name, int size, char* data);

  /**
   * @brief Write file's data to Fat
   * @param addr
   * @param size
   * @param data
   * @return true if successful, false if not
   */
  static bool WriteData(int addr, int size, char* data);

  /**
   * @brief Read a file
   * @param name
   * @return true if successful, false if not
   */
  static bool Read(char* name);

  /**
   * @brief Delete file from Fat
   * @param name
   * @return true if successful, false if not
   */
  static bool Delete(char* name);

  /**
   * @brief Return amount of free space left in EEPROM
   * @return Space available
   */
  static int Freespace();

  /**
   * @brief Find file
   * @param file_name
   * @return File number
   */
  static int FindFile(char* file_name);

  /**
   * @brief Get start address from a file with index
   * @param index
   * @return start address
   */
  static int GetStartAddr(int index);

  /**
   * @brief Get available address for data storage
   * @param size
   * @return EEPROM address
   */
  static int GetAvailableAddr(int size);

  /**
   * @brief List all files in Fat
   */
  static int ListFiles();

  /**
   * @brief Print EEPROM content
   */
  static int PrintEeprom();

  /**
   * @brief Erase EEPROM memory
   */
  static int ClearEeprom();

 private:
  /**
   * @brief Write Fat entry in EEPROM
   * @param pos
   * @param file
   */
  static int WriteFatEntry(int pos, FatEntry file);

  /**
   * @brief Read Fat entry from EEPROM
   * @param pos
   * @return Fat entry
   */
  static FatEntry ReadFatEntry(int pos);

  /**
   * @brief Get available address for entry storage
   * @return EEPROM address
   */
  static int GetAvailableFileAddr();

};

#endif //FAT_H_
