void split(char*);
int cmprstr(char*, char*);

int main()
{
	char* line = "\0";
	// Print "SHELL> " on the screen
	interrupt(0x21, 0, "SHELL> ", 0, 0);
	// Wait for input
	while(*line == '\0')
	{
		interrupt(0x21, 1, line, 0, 0);
	}
	// Split the input command, and execute it
	split(line); 
	// Terminate
	interrupt(0x21, 5, 0, 0);	
}

/*
	Takes as input the command inserted
*/
void split(char* line)
{
	int i = 0, j = 0, k = 0, m = 0, n = 0; 
	char token[5][9];
	char tmp[9];
	for(m = 0; m < 5; m++) {
		for(n = 0; n < 9; n++) {
			token[m][n] = 0x00;
			tmp[n] = 0x00;  
		}
	} 
	
	while(*(line + i) != '\0')
	{
		// Split the command into tokens, save them in the token array
		while(*(line + i) != ' ') 
		{
			if(*(line + i) == '\0')
			{
				i--;
				break;
			} 
			tmp[k] = *(line + i);
			k++;
			i++;	
		}
				
		tmp[k] = '\0';
 
		for(m = 0; m < 9; m++) {
			if(tmp[m] == '\0')
				break; 
			token[j][m] = tmp[m]; 
			tmp[m] = 0x00; 
		}
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
		interrupt(0x21, 4, token[1], 0x2000, 0);	
	}
	else if(cmprstr(token[0],"delete\0")) 
	{
		interrupt(0x21, 7, token[1], 0, 0); 	
	}
	else if(cmprstr(token[0], "copy\0")) 
	{
		char* buffer;
		buffer = ""; 
		// interrupt(0x21, 0, token[0], 0, 0);
		// interrupt(0x21, 0, token[1], 0, 0);
		// interrupt(0x21, 0, token[2], 0, 0);
		interrupt(0x21, 3, token[1], buffer, 0); //read file
		// interrupt(0x21, 0, "buffer", 0, 0);
		// interrupt(0x21, 0, buffer, 0, 0);
		// interrupt(0x21, 0, "read done", 0, 0);
		interrupt(0x21, 8, token[2], buffer, 3); //write file 	
	}
	else if(cmprstr(token[0], "dir")) 
	{								
		char dir [512] ; 
		char fileName[6]; 
		int i; 	
		interrupt(0x21, 0, token[0], 0, 0);
		interrupt(0x21, 2, 2, dir, 0); 
		for(i = 0; i < 512; i+=32) {
			int j; 
			fileName[0] = '\0'; 
		if(dir[j] != 0x0)
			for(j = 0; j < 6; j++) {
				if(dir[i + j] == '\0')
					break; 
				else 
					fileName[j] = dir[i + j]; 
			}
			if(fileName[0] != '\0')
			interrupt(0x21, 0, fileName, 0, 0); 
		}
	} else if (cmprstr(token[0], "create\0")) {
		char * line;
		char buffer[13312];  
		int len, idx = 0; 
		while(1) {
			int i; 
			interrupt(0x21, 1, "Enter line:\n\0", 0, 0); 			
			interrupt(0x21, 1, line, 0, 0); 
			if(line == '\0' || line == 0xd)
				break; 
			len = strlen(line); 
			for(i = 0; i < len; i++) {
				buffer[idx + i] = line[i]; 
			}			
		}
			interrupt(0x21, 1, buffer, 0, 0); 			
		
		 
	}
	else
	{
		interrupt(0x21, 0, token[0], 0, 0);					
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
	len++; 
	return len; 
}