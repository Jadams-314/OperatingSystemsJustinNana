/* kernel.c - Nana and Justin - Project C */

void printString(char*);
void readString(char*);
void readSector(char*, int);
void readFile(char*, char*, int*);
void executeProgram(char*);
void terminate();
void handleInterrupt21(int, int, int, int);

int main()
{
  makeInterrupt21();
  interrupt(0x21, 4, "shell", 0, 0);

  while (1) {}
  return 0;
}

void printString(char* chars)
{
  int i = 0;
  while (chars[i] != '\0')
  {
    interrupt(0x10, 0xe * 256 + chars[i], 0, 0, 0);
    i = i + 1;
  }
}

void readString(char* line)
{
  int i = 0;
  int c;

  while (1)
  {
    c = interrupt(0x16, 0, 0, 0, 0);

    if (c == 0xd)
    {
      line[i] = 0xa;
      line[i + 1] = 0x0;
      interrupt(0x10, 0xe * 256 + 0xd, 0, 0, 0);
      interrupt(0x10, 0xe * 256 + 0xa, 0, 0, 0);
      return;
    }
    else if (c == 0x8 || c == 0x7f)
    {
      if (i > 0)
      {
        i = i - 1;
        interrupt(0x10, 0xe * 256 + 0x8, 0, 0, 0);
        interrupt(0x10, 0xe * 256 + ' ', 0, 0, 0);
        interrupt(0x10, 0xe * 256 + 0x8, 0, 0, 0);
      }
    }
    else
    {
      line[i] = c;
      i = i + 1;
      interrupt(0x10, 0xe * 256 + c, 0, 0, 0);
    }
  }
}

void readSector(char* buffer, int sector)
{
  int relsector = sector + 1;
  int track = 0;
  int head = 0;

  interrupt(0x13, 2 * 256 + 1, buffer, track * 256 + relsector, head * 256 + 0x80);
}

void readFile(char* filename, char* buffer, int* sectorsRead)
{
  char dir[512];
  int fileentry;
  int i;
  int match;
  int s;

  *sectorsRead = 0;
  readSector(dir, 2);

  for (fileentry = 0; fileentry < 512; fileentry = fileentry + 32)
  {
    match = 1;
    for (i = 0; i < 6; i = i + 1)
    {
      if (filename[i] != dir[fileentry + i])
      {
        match = 0;
        break;
      }
      if (filename[i] == '\0')
        break;
    }

    if (match == 1)
    {
      s = 6;
      while (s < 32 && dir[fileentry + s] != 0)
      {
        readSector(buffer, dir[fileentry + s]);
        buffer = buffer + 512;
        *sectorsRead = *sectorsRead + 1;
        s = s + 1;
      }
      return;
    }
  }
}

void executeProgram(char* name)
{
  char buffer[13312];
  int sectorsRead;
  int i;

  readFile(name, buffer, &sectorsRead);

  if (sectorsRead == 0)
  {
    printString("Program not found\r\n");
    return;
  }

  for (i = 0; i < sectorsRead * 512; i = i + 1)
  {
    putInMemory(0x2000, i, buffer[i]);
  }

  launchProgram(0x2000);
}

void terminate()
{
  char shellname[6];

  shellname[0] = 's';
  shellname[1] = 'h';
  shellname[2] = 'e';
  shellname[3] = 'l';
  shellname[4] = 'l';
  shellname[5] = '\0';

  executeProgram(shellname);
}

void handleInterrupt21(int ax, int bx, int cx, int dx)
{
  if (ax == 0)
  {
    printString((char*) bx);
  }
  else if (ax == 1)
  {
    readString((char*) bx);
  }
  else if (ax == 2)
  {
    readSector((char*) bx, cx);
  }
  else if (ax == 3)
  {
    readFile((char*) bx, (char*) cx, (int*) dx);
  }
  else if (ax == 4)
  {
    executeProgram((char*) bx);
  }
  else if (ax == 5)
  {
    terminate();
  }
  else
  {
    printString("Error: invalid interrupt 21 service number");
  }
}
