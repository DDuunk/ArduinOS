# ArduinOS
A command line interface written for Arduino

## Feature list
- [x] Command line interface
    - [x] Command input
    - [x] Command processing
- [x] File system
    - [x] File Allocation table
    - [x] File writing
    - [x] File reading
    - [x] File erasing
    - [x] Ejecting a list of stored files
    - [x] Calculating free space
- [ ] Memory management
    - [ ] Memory table (**Implemented, but with 128 bits, related to saving memory**)
    - [x] Saving variable
    - [x] Reading variable
    - [x] Erasing all variables
- [x] Stack
    - For a future version, improvements can be made by making a union stack.
    - [x] Byte popping & pushing
    - [x] Char popping & pushing
    - [x] Int popping & pushing
    - [x] Float popping & pushing
    - [x] String popping & pushing
- [ ] Process management
    - [x] Process execution
    - [x] Console output
    - [x] Variables
    - [x] Stack operations
    - [x] Unaire operations
    - [x] Binary operations
    - [x] STOP
    - [x] IF, ELSE \& ENDIF
    - [x] LOOP \& ENDLOOP
    - [x] WHILE \& ENDWHILE
    - [x] Timing operations
    - [ ] File I/O operations
      - [x] OPEN
      - [x] CLOSE
      - [ ] WRITE
        - CHAR, INT, STRING are working fine.
          Although it would work better is STRING also add length of the string to file.
        - FLOAT works but exceeds memory
      - [x] READINT
      - [x] READCHAR
      - [ ] READFLOAT
        - Works but exceeds memory
      - [ ] READSTRING
        - All character are valid, but can't figure out a good method to get the length of the string.
          Dynamically sizing the array causes in faulthy data.
    - [ ] Forking