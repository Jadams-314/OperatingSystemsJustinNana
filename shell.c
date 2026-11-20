/* shell.c - Nana and Justin - Project C */

void getName(char*, char*);

int main()
{
  char line[80];
  char name[7];
  char buffer[13312];
  int sectorsRead;

  while (1)
  {
    syscall(0, "SHELL> ", 0, 0);
    syscall(1, line, 0, 0);

    if (line[0] == 't' && line[1] == 'y' && line[2] == 'p' && line[3] == 'e' && line[4] == ' ')
    {
      getName(line, name);
      syscall(3, name, buffer, &sectorsRead);
      if (sectorsRead > 0)
      {
        syscall(0, buffer, 0, 0);
        syscall(0, "\r\n", 0, 0);
      }
      else
      {
        syscall(0, "File not found\r\n", 0, 0);
      }
    }
    else if (line[0] == 'e' && line[1] == 'x' && line[2] == 'e' && line[3] == 'c' && line[4] == ' ')
    {
      getName(line, name);
      syscall(3, name, buffer, &sectorsRead);
      if (sectorsRead > 0)
      {
        syscall(4, name, 0, 0);
      }
      else
      {
        syscall(0, "Program not found\r\n", 0, 0);
      }
    }
    else
    {
      syscall(0, "Bad Command!\r\n", 0, 0);
    }
  }
  return 0;
}

/* copies the word after the 5-letter command into name (max 6 letters) */
void getName(char* line, char* name)
{
  int i;

  for (i = 0; i < 6; i = i + 1)
  {
    if (line[5 + i] <= ' ')
    {
      break;
    }
    name[i] = line[5 + i];
  }
  name[i] = '\0';
}
