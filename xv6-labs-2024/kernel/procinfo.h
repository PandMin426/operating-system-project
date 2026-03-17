struct procinfo {
  int pid;        // Process ID
  int ppid;       // Parent process ID
  int state;      // Process state
  uint64 sz;      // Size of process memory (bytes)
  char name[16];  // Process name
};
