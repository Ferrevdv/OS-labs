#ifndef XV6_KERNEL_STAT_H
#define XV6_KERNEL_STAT_H

#include "types.h"

#define T_DIR     1   // Directory
#define T_FILE    2   // File
#define T_DEVICE  3   // Device

#define M_READ    (1 << 0)
#define M_WRITE   (1 << 1)
#define M_EXECUTE (1 << 2)
#define M_ALL     (M_READ | M_WRITE | M_EXECUTE)

struct stat {
  int dev;     // File system's disk device
  uint ino;    // Inode number
  short type;  // Type of file
  short nlink; // Number of links to file
  uint64 size; // Size of file in bytes
  
  uint mode;   // File permissions
};

#endif
