           //Server *****************************
           /*Receive File from Client */
           char file_buffer[512]; // Receiver buffer
           char* file_name = "/home/jmccarthy/Documents/Apps/week9/socketExample/serverFiles/list.txt";
           
           FILE *file_open = fopen(file_name, "w");
           if(file_open == NULL)
              printf("File %s Cannot be opened file on server.\n", file_name);
           else {
              bzero(file_buffer, 512); 
              int block_size = 0;
              int i=0;
              while((block_size = recv(cs, file_buffer, 512, 0)) > 0) {
                 printf("Data Received %d = %d\n",i,block_size);
                 int write_sz = fwrite(file_buffer, sizeof(char), block_size, file_open);
                 bzero(file_buffer, 512);
                 i++;
              }
             
           }
           printf("Transfer Complete!\n");
           fclose(file_open); 
           //*****************************
             
