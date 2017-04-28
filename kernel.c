void printString(char*);
void readString(char*);
void readSector(char* buffer, int sector); 
void writeSector(char* buffer, int sector);  
void handleInterrupt21(int ax, int bx, int cx, int dx); 
int readFile(char*, char*); 
void writeFile(char* name, char* buffer, int secNum);
int executeProgram(char* name, int segment);
void terminate();
void writeFile(char* name, char* buffer, int secNum);
void handleTimerInterrupt(int segment, int sp); 
void deleteFile(char* name);

//process table 
int stackP[8]; 
int active[8]; 

int currentProcess; 
int quantum;

int main () 
{	int i; 
	for(i = 0; i < 8; i++) {
		stackP[i] = 0xff00; 
		active[i] = 0; 
	}
	currentProcess = 0;
	quantum = 0;  
	makeTimerInterrupt();
	makeInterrupt21();  
	interrupt(0x21, 4, "hello1\0", 0, 0);
	interrupt(0x21, 4, "hello2\0", 0, 0);
	while(1){}
}

 void handleTimerInterrupt(int segment, int sp) {
	int i;
	int counter;
	setKernelDataSegment(); 
	quantum++; 
	if(quantum == 100) 
	{
		currentProcess = (segment / 4096) - 2; 
		printString("quantum"); 
		quantum = 0; 
		stackP[currentProcess] = sp; 
		i = currentProcess + 1;
		counter = 0;
		while(counter < 8) 
		{
			if(active[i] == 1)
			{	
				printString("quantum2..."); 
				break;
			}	 
			i++;
			counter++; 
			if(i == 8)
				i = 0; 
		}
		currentProcess = i;
		segment = (currentProcess + 2) * 4096;
		sp = stackP[currentProcess]; 
		restoreDataSegment(); 
		returnFromTimer(segment, sp); 
	}
	else
	{
		restoreDataSegment(); 
		returnFromTimer(segment, sp); 
	}	 
 }

void printString(char* s)
{
	int i = 0;
	// loop on the characters until the end of the String encountered
	while(s[i] != '\0')
	{	
	// if the current character is '\n', print it and then print '\r' to return to the start of the next line
		if(s[i] == '\n')
		{
			interrupt(0x10, 0xE*256+s[i++], 0, 0, 0);
			interrupt(0x10, 0xE*256+'\r', 0, 0, 0);
		}
		else	
			interrupt(0x10, 0xE*256+s[i++], 0, 0, 0);
	}
}

void readString(char* s)
{
	int i = 0;
	while(1)
	{
		char in = interrupt(0x16,0x0, 0x0, 0x0, 0x0);
		// if Enter is pressed, print '/n' and '/r' to move the cursor to the beginning of next line 
		// then put 0xA, and 0x0 at the end of the String, then return
		if(in == 0xd)
		{
			interrupt(0x10, 0xE*256+'\n', 0, 0, 0);
			interrupt(0x10, 0xE*256+'\r', 0, 0, 0);
			s[i] = 0xA;
			s[++i] = 0x0;
			return;
		}
		// if backspace is pressed, print '\b' which moves the cursor one step back
		// then write a space to remove the last character written on the screen
		// again write '\b' to move the cursor back one character
		// and update the current index of the String
		if(in == 0x8)
		{	if(i > 0) 
			{
				interrupt(0x10, 0xE*256+'\b', 0, 0, 0);
				interrupt(0x10, 0xE*256+' ', 0, 0, 0);
				interrupt(0x10, 0xE*256+'\b', 0, 0, 0);
				i--;
			}
		}
		else
		{
			s[i] = in;
			interrupt(0x10, 0xE*256+s[i++], 0, 0, 0);
		}
	}
}

void readSector(char* buffer, int sector) {
	int AH = 2; 
	int AL = 1; 
	int CH = sector / 36 ;                //cylinder
	int CL = mod(sector, 18) + 1;         //sector 
	int DH = mod(sector / 18, 2);         //head 
	int DL = 0;                           //device number, 0 for floppy disk 
	interrupt(0x13,AH*256 + AL, buffer, CH*256 + CL, DH*256 + DL); 
}

int mod(int a, int b) {
	return a - a/b * b; 
}

void handleInterrupt21(int ax, int bx, int cx, int dx) {
	// task 4
	// printString("kotomoto ya 7elwa ya batta!"); 
	// task 5
	if(ax==0){
		printString(bx);
	}
	else if(ax==1){
		readString(bx);

	}
	else if(ax==2){
		readSector(bx,cx);
	}
	else if(ax == 3){
		readFile(bx, cx);
	}
	else if(ax == 4){
		executeProgram(bx);
	}
	else if(ax == 5){
		terminate();
	}
	else if(ax==6){
		writeSector(bx,cx);
	}
	else if(ax==7){
		deleteFile(bx);
	}
	else if(ax==8){
		writeFile(bx,cx,dx);
	}
	else {
		printString("Error");
	}
}

/*
	Takes as input the name of a file and loads it into an array in memory
*/
int readFile(char* fileName, char* buffer) 
{
	// Load the directory sector into a 512 byte character array using readSector
	char directory[512] ;
	int i = 0;
	char fail[16];
	readSector(directory, 2);

	// Go through the directory trying to match the file name
	for(i = 0; i < 512; i += 32)
	{
		int found = 1; 
		int j = 0;
		for(j = 0; j < 6; j++) 
		{
			if(fileName[j] != directory[i + j]) 
			{
				found = 0; 
				break; 
			}
			// If the file name is less than 6 characters, break
			if(fileName[j] == '\0')
				break;
		}

		// Using the sector numbers in the directory, load the file, sector by sector, into the buffer array
		if(found) 
		{
			int k = i + 6;
			int count = 0; 
			while(directory[k]) 
			{
				readSector(buffer + (count++) * 512, directory[k++]); 
			}
			return 1;
		}
	}
	// Print failure message if file not found
	fail[0]='F';
	fail[1]='I';
	fail[2]='L';
	fail[3]='E';
	fail[4]=' ';
	fail[5]='N';
	fail[6]='O';
	fail[7]='T';
	fail[8]=' ';
	fail[9]='F';
	fail[10]='O';
	fail[11]='U';
	fail[12]='N';
	fail[13]='D';
	fail[14]='!';
	fail[15]='\n';
	fail[16]='\0';
	interrupt(0x21, 0, fail, 0, 0);
	return 2;
}

/*
	Takes as input the name of a program and the segment where you want it to run
*/
int executeProgram(char* name)
{	
	char buffer[13312];
	int i = 0;
	int segment = 0; 
	
	// Loading the program into a buffer
	int found = readFile(name, buffer);
 
	setKernelDataSegment(); 
	for(i = 0; i < 8; i++) { //checking for empty space for program 
		if(active[i] == 0) {
		
			segment = (i + 2) * 4096;
			break; 
		}
	}
	active[i] = 1; 
	restoreDataSegment(); 

	if(found == 2)
		return 2;
	// Transferring the program into the bottom of the segment where you want it to run
	for(i = 0;  i < 13312; i++)
	{
		putInMemory(segment, i, buffer[i]);
	}
	// Jump to the program after setting the registers and stack pointer to the appropriate values
	// using the assemply function launchProgram
	 
	initializeProgram(segment);
	



	return 1;
}

/*
	Launch a new shell
*/
void terminate()
{	setKernelDataSegment(); 
	active[currentProcess] = 0; 
	restoreDataSegment(); 
	while(1);
	// char shell[6];
	// shell[0]='s';
	// shell[1]='h';
	// shell[2]='e';
	// shell[3]='l';
	// shell[4]='l';
	// shell[5]='\0';
	// interrupt(0x21, 4, shell, 0x2000, 0);
}

void writeSector(char* buffer, int sector) 
{
	int AH = 3; 
	int AL = 1; 
	int CH = sector / 36 ;                //cylinder
	int CL = mod(sector, 18) + 1;         //sector 
	int DH = mod(sector / 18, 2);         //head 
	int DL = 0;                           //device number, 0 for floppy disk 
	interrupt(0x13,AH*256 + AL, buffer, CH*256 + CL, DH*256 + DL); 
}

void deleteFile(char* name)
{
	char directory [512];
	char map [512];
	int i = 0; 
	//loading the map and directory
	readSector(map,1);
	readSector(directory,2);

 	// Go through the directory trying to match the file name

	for(; i < 512; i += 32)
	{
		int found = 1; 
		int j = 0;
		for(j = 0; j < 6; j++) 
		{
			if(name[j] != directory[i + j]) 
			{
				found = 0; 
				break; 
			}
			// If the file name is less than 6 characters, break
			if(name[j] == '\0')
				break;
		}

		// Using the sector numbers in the directory, puting zero in map sectors corresponding to them
		if(found) 
			{	int k; 
				directory[i] = 0x00; 
				k = i + 6;
				while(directory[k]) 
				{
					map[directory[k]+1] = 0x00;
					k++;				 
				}
				writeSector(directory,2);
				writeSector(map,1);
				break;
			}
		}

	}


void writeFile(char* name, char* buffer, int secNum)
{
	char directory [512];
	char map [512];
	int i =0;
	int found =0;
	//loading the map and directory
	readSector(map,1);
	readSector(directory,2);

    // find a free entry to write the file in the directory
	for(i = 0; i < 512; i += 32)
	{
		if (directory[i]==0x00)
		{
			int j = 0;
			int k = 0;
			int l = i + 6;
			int count = 0;
			int done = 0;
			int f = i + 6 + secNum;
    		//writing the name of the file in the directory entry
			for(j = 0; j < 6; j++) 
			{		
				directory[i+j] = name[j];
				if(name[j] == '\0')
					break;
			}
			for(; j < 6; j++)
			{
				directory[i+j] = 0x00;
			}

			for(j = 0; j < secNum; j++)
			{
				for (k = 0; k < 512 ; k++)
				{
					if(map[k]==0x00)
					{
						map[k]=0xFF;
						directory[l++] = k - 1;
						done++;
						writeSector(buffer + (count++) * 512, k - 1);
						break;
					}
				}
				if(done < j + 1)
				{
					printString("There is no space in map");
					return;
				}
			}
			for(j = f; j < 32; j++)
			{
				directory[j] = 0x00;
			}

			writeSector(map,1);
			writeSector(directory,2);
			found =1;
			break;

		}

	}
	if(!found)
	{
		printString("There is no space in directory");
		return;
	}
}