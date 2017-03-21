void split(char*);

int main()
{
	while(1)
	{
		char* line;
		makeInterrupt21();
		interrupt(0x21, 0, "SHELL> ", 0, 0);

		while(line == "\0") {
			makeInterrupt21();
			interrupt(0x21, 1, line, 0, 0);
		}

		split(line); 

	}
}

void split(char*line)
{
	int i = 0;
	int j = 0;  
	char* nxt; 
	while(*(line + i) != "\0"){
		while(line != ' ') {
			*(nxt + j) = *(line + i); 
			j++;
		}

		if(nxt != "view" && nxt != "execute") {
			makeInterrupt21();
			interrupt(0x21, 0, "BAD COMMAND!", 0, 0);			
		}
		j = 0; 
		i++; 
	}
}