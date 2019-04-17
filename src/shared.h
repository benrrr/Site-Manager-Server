#ifndef SHARED_H_
#define SHARED_H_

#define USERNAME_MAX 32
#define ID_MAX 5 //65535 val, 5 len
#define BUFSIZE 1024
#define GROUP_MAX 32
#define ENDOFFILE "\r\n END FILE STREAM \r\n"

// standard messages
#define OK "Success"
#define ERROR_UNKOWN "Something went wrong."
#define ERROR_FILE_LOCK "Could not acquire lock on file"
#define ERROR_FILE_OPEN "Could not open file"
#define ERROR_PERMISSION "User does not have permission to do that"
#define ERROR_FILE_WRITE "Error while writing piece of file, possible corruption"

//requried ids for directories
#define SALESTEAM "1001"
#define OFFERSTEAM "1002"
#define MARKETINGTEAM "1003"
#define PROMOTIONSTEAM "1004"
#define EMPLOYEE "1009"

#endif