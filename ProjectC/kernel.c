void printChar(char);
void printString(char *);
void readString(char*);
void readSector(char *, int);
int interrupt(int, int, int, int, int);
void handleInterrupt21(int, int, int, int);
void makeInterrupt21(void);
void readFile(char*, char*, int*);
void executeProgram(char* name);

void main(){


/*OS B:
    //Part 1:

    //printString("Hello World");
    //while (1);

    //Part 2;


    //char line[80];
    
    //printString("Enter a line: ");
    //readString(line);
    //printString(line);
    //while (1);


    //Part3:
    //char buffer[512];
    //readSector(buffer, 30);
    //buffer[511] = 0;
    
    //printString(buffer);
    //while (1);


    //Part4:

    //makeInterrupt21();
    //interrupt(0x21,0,0,0,0);
    //while(1);


    //Part5:
    //char line[80];
    //makeInterrupt21();
    //interrupt(0x21, 0, (int)"System calls are working!\r\n", 0, 0);
    //interrupt(0x21, 1, line, 0, 0);
    //interrupt(0x21, 0, line, 0, 0);
    //while(1);


}
*/

//OS C:
    //Part1:
    //char buffer[13312]; /*this is the maximum size of a file*/
    //int sectorsRead;
    //makeInterrupt21();
    //interrupt(0x21, 3, "messag", buffer, &sectorsRead); /*read the file into buffer*/
    //if (sectorsRead>0)
    //interrupt(0x21, 0, buffer, 0, 0); /*print out the file*/
    //else
    //interrupt(0x21, 0, "messag not found\r\n", 0, 0); /*no sectors read? then print an error*/
    //while(1); /*hang up*/

    //Part2:
    makeInterrupt21();
    interrupt(0x21, 4, "tstpr1", 0, 0);
    while(1);

}
void printChar(char c){

interrupt(0x10, 0x0E00 + c, 0, 0, 0);

}
void printString(char *chars)
{
    int i;

    i = 0;

    while (chars[i])
       {
            printChar(chars[i]);
            i++;
        }


}
void readString(char *buffer)
{
    int i;
    int key;
    i=0;
    while (1)
    {
        key = interrupt(0x16, 0, 0, 0, 0);
        /*Enter key */
        if (key == 0x0D)
        {
            printChar(0x0D);
            printChar(0x0A);
            break;
        }
        /*Backspace*/
        if (key == 0x08)
        {
            if (i>0)
            {
                i--;
                printChar(0x08);
                printChar(' ');
                printChar(0x08);
            }
            continue;
        }
        else if (key >= 0x20 && i < 78)
        {
            buffer[i] = key;
            i++;
            printChar(key);
        
        }  
    }
    buffer[i] = 0x0A;
    buffer[i + 1] =0;
    
}
void readSector(char *buffer, int sector)
{
    int ax;
    int cx;
    int dx;

    ax = 0x0201;
    cx = sector +1;
    dx = 0x0080;

    interrupt(0x13, ax, (int)buffer, cx, dx);

}

void handleInterrupt21(int ax, int bx, int cx, int dx) {
    if (ax == 0) {
        printString((char*)bx);
    } else if (ax == 1) {
        readString((char *)bx);
    } else if (ax == 2) {
        readSector((char *)bx, cx);
    } else if (ax == 3) {
        readFile((char*)bx, (char*)cx, (int*)dx);
    } else if (ax == 4)  {
        executeProgram((char*)bx);    
    } 
}



//second attempt. original function works, but only for 6 character filenames. anything less and the logic breaks. tested with mes.txt
void readFile(char* filename, char* buffer, int* sectorsRead) 
{
    char dir[512];
    int fileFound = 0;
    int fileentry = 0;
    int filenameIndex= 0;
    int sectorListIndex = 0;
    int currentSector = 0;
    
    *sectorsRead = 0; 

    readSector(dir, 2);

    for (fileentry = 0; fileentry< 512;  fileentry+= 32) 
    {
        fileFound = 1;
        for (filenameIndex = 0; filenameIndex < 6; filenameIndex++) 
        {
  
            if (filename[filenameIndex] == 0) 
            {
                if (dir[fileentry + filenameIndex] != 0 && dir[fileentry+ filenameIndex] != ' ') 
                {
            fileFound = 0;
                }
                break; 
                }

                if (dir[fileentry + filenameIndex] != filename[filenameIndex]) 
                {
                fileFound = 0;
                break;
                }
            }


            if (fileFound == 1) 
            {
                for (sectorListIndex = 6; sectorListIndex < 32; sectorListIndex++) {
                   currentSector = (int)dir[fileentry + sectorListIndex];
                    if (currentSector == 0) 
                    {
                        break;
                    }
                readSector(buffer, currentSector);
                buffer += 512;
                
                
                (*sectorsRead)++; 
            }
            return; 
        }
    }
    
   
    buffer[0] = 0x00;
    *sectorsRead = 0;
}

void executeProgram(char* name)
{
    
    char buffer[13312]; 
    int sectorsRead;
    int i;
    
    readFile(name, buffer, &sectorsRead);
    if (sectorsRead > 0) {
       
        for (i = 0; i < sectorsRead * 512; i++) {
           
            putInMemory(0x2000, i, buffer[i]);
        }
        
      
    }
    launchProgram(0x2000);
}
//deprecated!!
/*
void readFile(char* filename, char* buffer, int* sectorsRead) 
{
    char dir[512];
    int fileFound = 0;
    int fileentry = 0;
    int filenameIndex= 0;
    int sectorListIndex = 0;
    int currentSector = 0;
    
    *sectorsRead = 0; 

    readSector(dir, 2);

    for (fileentry = 0; fileentry< 512;  fileentry+= 32) 
    {
        fileFound = 0;
        for (filenameIndex = 0; filenameIndex < 6; filenameIndex++) 
        {
            if (filename[filenameIndex] ==0)
            {
                if (dir[fileentry + filenameIndex] != 0 && dir[fileentry + filenameIndex] != ' ')
                {
                    break;
                }
                
                fileFound=1;
                break;
            } 
            if(dir[fileentry + filenameIndex] != filename[filenameIndex])
            {
                break;
            }  
            //iterates 6 times 0, 1, 2, 3, 4, 5. If it gets to 5 that means all 6 characters matched. in this case 'messag'. g is filenameindex 5
            if(filenameIndex ==5)
            {
                fileFound =1;
            }
        } 
        if (fileFound == 1)
        {
            for (sectorListIndex = 6; sectorListIndex < 32; sectorListIndex++)
            {
                currentSector = (int)dir[fileentry + sectorListIndex];
                if (currentSector ==0)
                {
                    break;                
                }

                readSector(buffer, currentSector);
                buffer += 512;
                (*sectorsRead)++;
            }
            return;

        }
    }
   
    buffer[0] = 0x00;
    *sectorsRead = 0;
}
*/