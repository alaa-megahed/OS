void split(char*);
int cmprstr(char*, char*);
int strlen(char*);
int mod(int, int);


int main()
{
	char* line = "\0";
	while(1)
	{
		// Print "SHELL> " on the screen
		interrupt(0x21, 0, "SHELL> ", 0, 0);
		// Wait for input
		while(*line == '\0')
		{
			interrupt(0x21, 1, line, 0, 0);
		}
		// Split the input command, and execute it
		split(line);
		line[0] = '\0';
		// Terminate
		// interrupt(0x21, 5, 0, 0);
	}	
}

/*
	Takes as input the command inserted
*/
void split(char* line)
{
	int i = 0, j = 0, k = 0, m = 0, n = 0; 
	char token[5][9];
	for(m = 0; m < 5; m++) {
		for(n = 0; n < 9; n++) {
			token[m][n] = 0x00; 
		}
	} 
	
	while(*(line + i) != 0xA)
	{
		// Split the command into tokens, save them in the token array
		while(*(line + i) != ' ') 
		{
			if(*(line + i) == 0xA)
			{
				i--;
				break;
			} 
			token[j][k] = *(line + i);
			k++;
			i++;	
		}
		token[j][k] = '\0';
		k = 0;
		i++;
		j++;
	}

	// If command is view, load the desired file and print out its content
	if(cmprstr(token[0],"view\0"))
	{	
		char* buffer;	
		buffer = "";
		interrupt(0x21, 3, token[1], buffer, 0);
		interrupt(0x21, 0, buffer, 0, 0);         /*print out the file*/				
	}
	// If command is execute, execute the desired program
	else if(cmprstr(token[0],"execute\0"))
	{
		interrupt(0x21, 4, token[1], 0, 0);	
	}
	else if(cmprstr(token[0],"delete\0")) 
	{
		interrupt(0x21, 7, token[1], 0, 0); 	
	}
	else if(cmprstr(token[0], "copy\0")) 
	{
		char* buffer;
		buffer = ""; 
		interrupt(0x21, 3, token[1], buffer, 0); //read file
		interrupt(0x21, 8, token[2], buffer, 3); //write file 	
	}
	else if(cmprstr(token[0], "dir")) 
	{								
		char dir [512] ; 

		char fileName[7]; 
		int i;
		interrupt(0x21, 2, dir, 2, 0); 
		for(i = 0; i < 512; i+=32) 
		{
			int j; 
			if(dir[j] != 0x00)
			{
				for(j = 0; j < 6; j++) 
				{
					if(dir[i + j] == '\0')
					{
						fileName[j] = '\0';
						break; 
					}
					else 
						fileName[j] = dir[i + j]; 
				}
				for (; j < 7; ++j)
				{
					fileName[j] = '\0';
				}
			// if(fileName[0] != '\0')
				interrupt(0x21, 0, fileName, 0, 0);
				interrupt(0x21, 0, " \0", 0, 0); 
			}
		}
		interrupt(0x21, 0, "\n", 0, 0);

	} 
	else if (cmprstr(token[0], "create\0")) 
	{			
		char* line = "\0";
		char buffer[13312];  
		int len, idx = 0; 
		int numSec = 0;
		int m = 0;

		while(1) 
		{
			int i; 
			interrupt(0x21, 0, "Enter line:\n\0", 0, 0); 			
			interrupt(0x21, 1, line, 0, 0); 
			 
			len = strlen(line);
			if(len <= 1)
				break;
			for(i = 0; i < len; i++) 
			{
				buffer[idx + i] = line[i]; 
			}
			buffer[idx + i] = "\n";
			idx += len;
			line = "\0";	
		}
		buffer[idx] = '\0';		
		// interrupt(0x21, 0, buffer, 0, 0); 
		m = mod(idx + 1, 512);
		numSec = idx / 512;
		if(m != 0)
			numSec++;
		interrupt(0x21, 8, token[1], buffer, numSec); 			
	}
	else if(cmprstr(token[0],"kill\0"))
	{
		if(token[1][0] == '0')
			interrupt(0x21, 9, 0, 0, 0);
		else if(token[1][0] == '1')
			interrupt(0x21, 9, 1, 0, 0);
		else if(token[1][0] == '2')
			interrupt(0x21, 9, 2, 0, 0);	
		else if(token[1][0] == '3')
			interrupt(0x21, 9, 3, 0, 0);	
		else if(token[1][0] == '4')
			interrupt(0x21, 9, 4, 0, 0);	
		else if(token[1][0] == '5')
			interrupt(0x21, 9, 5, 0, 0);	
		else if(token[1][0] == '6')
			interrupt(0x21, 9, 6, 0, 0);	
		else if(token[1][0] == '7')
			interrupt(0x21, 9, 7, 0, 0);		
	}
	else
	{
		interrupt(0x21, 0, "BAD COMMAND!\n", 0, 0);			
	}
}

/*
	Takes two Strings and compare them, character by character
*/
int cmprstr(char* str1,char* str2)
{
    int i = 0, noteq = 0;
    while(str1[i] != '\0' && str2[i] != '\0')
    {
         if(str1[i]!=str2[i])
         {
             noteq=1;
             break;
         }
         i++;
    }	
    // Return 1 if they equal, 0 otherwise
    if (noteq == 0 && str1[i] == '\0' && str2[i] == '\0')
         return 1;
    return 0;
}
int strlen(char* str) {
	int len; 
	len = 0; 
	while(str[len] != '\0') {
		len++; 
	}
	return len; 
}

int mod(int a, int b) {
	return a - a/b * b; 
}
