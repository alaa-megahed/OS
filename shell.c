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
	int i = 0, j = 0, k = 0;
	char* token[5]; 
	char* tmp;
	while(*(line + i) != '\0')
	{
		// Split the command into tokens, save them in the token array
		while(*(line + i) != ' ') 
		{
			*(tmp + k) = *(line + i);
			if(*(line + i) == '\0')
			{
				i--;
				break;
			} 
			k++;
			i++;	
		}
		*(tmp + k) = '\0';
		(token[j]) = tmp;
		tmp = "";
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
	else if(cmprstr(token[0],"execute"))
	{
		interrupt(0x21, 4, token[1], 0x2000, 0);	
	} else if(cmprstr(token[0],"delete")) {
		interrupt(0x21, 7, token[1], 0, 0); 
	}
	
	
	// If the command written wasn't vlaid, print out BAD COMMAND
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