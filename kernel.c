void printString(char*);
void readString(char*);
void readSector(char* buffer, int sector);  
void handleInterrupt21(int ax, int bx, int cx, int dx); 
void readFile(char*, char*); 
void executeProgram(char* name, int segment);
void terminate();

int main () 
{
	// /* 
	// 	Testing Task1
	// */ 
	// char buffer[13312]; /*this is the maximum size of a file*/
	// makeInterrupt21();
	// interrupt(0x21, 3, "messag\0", buffer, 0); /*read the file into buffer*/
	// interrupt(0x21, 0, buffer, 0, 0); /*print out the file*/
	// while(1); /*hang up*/

	/*
		Testing Task2
	*/
	makeInterrupt21();
	interrupt(0x21, 4, "tstprg\0", 0x2000, 0);
	while(1);
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
	return;
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
		{	
			interrupt(0x10, 0xE*256+'\b', 0, 0, 0);
			interrupt(0x10, 0xE*256+' ', 0, 0, 0);
			interrupt(0x10, 0xE*256+'\b', 0, 0, 0);
			if(i > 0) i--;
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
		executeProgram(bx, cx);
	}
	else if(ax == 5){
		terminate();
	}
	else {
		printString("Error");
	}
}

/*
	Takes as input the name of a file and loads it into an array in memory
*/
void readFile(char* fileName, char* buffer) 
{
	char directory[512] ;
	int i = 0;
	readSector(directory, 2);
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
			// if the file name is less than 6 characters, break
			if(fileName[j] == '\0')
				break;
		}
		if(found) 
		{
			int k = i + 6;
			int count = 0; 
			while(directory[k]) 
			{
				readSector(buffer + (count++) * 512, directory[k++]); 
			}
			break; 
		}
	}
	
}

/*
	Takes as input the name of a program and the segment where you want it to run
*/
void executeProgram(char* name, int segment)
{
	char buffer[13312];
	int i = 0;
	readFile(name, buffer);
	for(i = 0;  i < 13312; i++)
	{
		putInMemory(segment, i, buffer[i]);
	}
	launchProgram(segment);
}

void terminate()
{
	while(1);
}

