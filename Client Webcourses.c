        // Client *********************************

           /*send to server*/
	    char* file_name = "/home/jmccarthy/Documents/Apps/week9/socketExample/clientFiles/list.txt";
            
	    char file_buffer[512]; 
	    printf("[Client] Sending %s to the Server... ", file_name);
	    FILE *file_open = fopen(file_name, "r");
	    bzero(file_buffer, 512); 
	    int block_size,i=0; 
	    while((block_size = fread(file_buffer, sizeof(char), 512, file_open)) > 0) {
		printf("Data Sent %d = %d\n",i,block_size);
		if(send(SID, file_buffer, block_size, 0) < 0) {
		    exit(1);
		}
		bzero(file_buffer, 512);
		i++;
	    }
        }

