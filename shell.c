void split(char*);
int cmprstr(char*, char*);

int main()
{
	char* line = "\0";
	interrupt(0x21, 0, "SHELL> ", 0, 0);
	while(*line == '\0')
	{
		interrupt(0x21, 1, line, 0, 0);
	}
	split(line); 
	interrupt(0x21, 5, 0, 0);	
}

void split(char* line)
{
	int i = 0, j = 0, k = 0;
	char* token[5]; 
	char* tmp;
	while(*(line + i) != '\0')
	{
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

	if(cmprstr(token[0],"view\0"))
	{	
		char* buffer;	
		interrupt(0x21, 3, token[1], buffer, 0); /*read the file into buffer*/
		interrupt(0x21, 0, buffer, 0, 0); /*print out the file*/				
	}
	else if(cmprstr(token[0],"execute"))
	{
		interrupt(0x21, 4, token[1], 0x2000, 0);
	}
	else
	{
		interrupt(0x21, 0, "BAD COMMAND!\n", 0, 0);			
	}
}

int cmprstr(char* str1,char* str2){
    int i=0,flag=0;
    while(str1[i] != '\0' && str2[i] != '\0')
    {
         if(str1[i]!=str2[i])
         {
             flag=1;
             break;
         }
         i++;
    }

    if (flag==0 && str1[i]=='\0' && str2[i]=='\0')
         return 1;
    else
         return 0;

}