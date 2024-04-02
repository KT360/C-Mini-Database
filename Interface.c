#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>



FILE *f;
FILE *newf;

char instruction[] = "Welcome, Input one of the following commands.";

char EditInstruction[] = "\nFor editing -> EditR Id Field EditText";

char AddInstruction[] = "\nFor adding -> AddR Name Email Number";

char RemoveInstruction[] = "\nTo remove a record -> Remove Id\n\n";

char ExitInstruction[] = "To exit -> type 'Exit Data'";

char GetInstruction[] = "To get record info -> GetR Id";

char command[100];

char commandParams[4][10];

int myIndex = 0;



char temp[100];

/*
    Tokenize the string from the command prompt stored into "command"
*/
void getParams()
{
    char *token = strtok(command, " ");

    myIndex = 0;

    while(token)
    {
        strcpy(commandParams[myIndex], token);
        token = strtok(NULL, " ");
        myIndex++;

        //Remove newline from last command parameter
        if(myIndex == 4)
        {
            char *lastParam = commandParams[3];
            lastParam[strcspn(lastParam,"\n")] = '\0';

            break;
        }

    }

}

//Copy over records while looking for matching ID
//Return the matching record/line
//Make sure to open TempDatabase.Txt before this function for Everything but "Get" command
char* findMatch(char* ID)
{
    char *RecordID;

    char lineVal[100];

    char holder[100];

    printf("Searching for ID:(%s)\n",ID);

    while(fgets(temp,100,f) != NULL)
    {

        strcpy(lineVal,temp);

        //Copy line into new storage after while loop is done 'temp' still contains original, unmodified line for next processing step
        strcpy(holder, temp);

        RecordID = strtok(holder,",");

        if(strcmp(ID, RecordID) == 0)
        {
            break;
        }else if(strcmp(commandParams[0],"GetR") != 0) //No need to create new database instance for "Get"
        {
            fprintf(newf,"%s",lineVal);
        }
    }

    //If we've reached  the end of the file with the wrong ID still, else return 'temp' which contains the matched record line
    if(strcmp(ID, RecordID) != 0)
    {
        return NULL;
    }else
    {
        return temp;
    }
}



int main(int argc, char *argv[]){

/*
    DWORD bufferLength = MAX_PATH;  // MAX_PATH is defined in windows.h
    char currentDirectory[MAX_PATH];

    DWORD lengthReturned = GetCurrentDirectory(bufferLength, currentDirectory);
    
    printf(currentDirectory);
*/
    

    printf("%s\n%s\n%s\n%s\n%s\n", instruction, EditInstruction, AddInstruction, RemoveInstruction,GetInstruction ,ExitInstruction);

    printf("Please enter you command\n\n");

    fgets(command, sizeof(command), stdin);

    while(1)
    {

        getParams();

        if(strcmp(commandParams[0], "AddR") == 0) //When adding, check header for next useable ID, Copy over data, append record, append new header with incremented ID
        {
            f = fopen("Database.txt","r+");

            if(f == NULL)
            {
                printf("Error opening file!\n");
                exit(1);
            }else
            {
                if(myIndex >= 4)
                {
                    //Get FirstLine/Header for next available ID
                    char DocLine[100];
                    fgets(DocLine,100,f);

                    char HeaderLine[100];

                    strcpy(HeaderLine,DocLine); //Copy Header Line to use docline buffer later

                    //Get pointer to digit token
                    char *recordIndex = strtok(HeaderLine,"=");
                    char *delim;

                    //Convert to long int, store "=" into "delim" (quirk of the function)
                    long nextAvailable = strtol(recordIndex,&delim,10);
                    
                    nextAvailable++;

                    newf = fopen("TempDatabase.txt","w");

                    if(newf == NULL)
                    {
                        printf("Could not open Temporary Database for ID increment...");
                    }else
                    {
                        //Print new Header with nex available index
                        fprintf(newf,"%d=NextID\n",nextAvailable);
                        //Copy other elements 
                        while(fgets(DocLine,100,f) != NULL)
                        {
                            fprintf(newf,DocLine);
                        }
                        //Add new record
                        fprintf(newf,"%s,%s,%s,%s\n", recordIndex,commandParams[1],commandParams[2],commandParams[3]);

                        fclose(f);
                        fclose(newf);

                        remove("Database.txt");
                        //Rename copied data file
                        rename("TempDatabase.txt","Database.txt");

                        printf("Record Added!\n\n");
                    }

                }else
                {
                    printf("Not enough Parameters for add....");
                }
                fclose(f);

            }

        }else if(strcmp(commandParams[0], "EditR") == 0) //After matching line is found, Tokenize matching Line, Build new DocLine by appending data, skip over Field to be edited and replace it with EditVal from commands
        {

            f = fopen("Database.txt", "r+");

            if(f == NULL)
            {

                printf("Cannot open file!");
                exit(0);

            }else
            {
                if(myIndex >= 4)
                {
                    char *ID = commandParams[1];
                    char *Field = commandParams[2];
                    char *EditVal = commandParams[3];

                    newf = fopen("TempDatabase.txt","w");

                    if(newf == NULL)
                    {
                        printf("Could not open temporary file for modification...");
                    }else
                    {
                        //calling strtok on temp modifies
                        //Make sure to finish tokenization of one string before moving to another
                        //To make sure you've continued where you left off in tokenization, pass in a NULL pointer
                        //Remember, string pointers actually only point to the first character of that string as a value

                        char *RecordLine = findMatch(ID);

                        if(RecordLine != NULL)
                        {
                            printf("Match Found.\n");
                            //Variable to hold record with edits
                            char editedRecord[100] = "";
                            char delim[] = ",";

                            //Get the ID and append it to the edit, 
                            char *fieldToken = strtok(RecordLine,",");

                            strcat(editedRecord, fieldToken);

                            if(strcmp(Field, "NAME") == 0)
                            {
                                //Add delimiter and append new name
                                strcat(editedRecord, delim);

                                strcat(editedRecord, EditVal);

                                //Call this once to skip over the existing Name in the record
                                fieldToken = strtok(NULL,",");

                                //Get the other fields
                                for(int i =0; i<2; i++)
                                {
                                    fieldToken = strtok(NULL, ",");
                                    strcat(editedRecord,delim);
                                    strcat(editedRecord, fieldToken);
                                }

                                printf("Final Record: %s\n", editedRecord);

                                fprintf(newf,"%s",editedRecord);

                            }else if(strcmp(Field, "EMAIL") == 0)
                            {
                                strcat(editedRecord, delim);

                                fieldToken = strtok(NULL,",");

                                strcat(editedRecord, fieldToken);

                                //skip over email
                                fieldToken = strtok(NULL,",");

                                //Append email
                                strcat(editedRecord, delim);
                                strcat(editedRecord, EditVal);

                                //Get the phone
                                fieldToken = strtok(NULL,",");

                                strcat(editedRecord,delim);
                                strcat(editedRecord,fieldToken);

                                printf("Final Record: %s\n", editedRecord);

                                fprintf(newf,"%s",editedRecord);

                            }else if(strcmp(Field, "PHONE") == 0)
                            {

                                //Append name and email
                                for(int i = 0; i<2; i++)
                                {
                                    strcat(editedRecord,delim);
                                    fieldToken = strtok(NULL,",");
                                    strcat(editedRecord,fieldToken);
                                }

                                //append new phone number
                                strcat(editedRecord, delim);
                                strcat(editedRecord, EditVal);
                                strcat(editedRecord,"\n");

                                printf("Final Record: %s\n", editedRecord);

                                fprintf(newf,"%s",editedRecord);

                            }else
                            {
                                printf("Could not recognize Edit field please enter: [NAME, EMAIL or PHONE] as edit fields");
                            }


                            printf("Porting database...");

                            char copy[100];

                            //Copy over the rest of the information to the new file
                            while(fgets(copy,100,f) != NULL)
                            {
                                fprintf(newf,"%s",copy);
                            }

                            //Close files
                            fclose(f);
                            fclose(newf);

                            remove("Database.txt");
                            //Rename copied data file
                            rename("TempDatabase.txt","Database.txt");

                            printf("Record Edited!\n\n");
                        }else
                        {
                            printf("Record ID not found, Please try again.");

                            remove("TempDatabase.txt");
                        }

                    }
                }else
                {
                    printf("Not enough Parameters for edit..");
                }
                
            }
            
            

        }else if(strcmp(commandParams[0], "RemoveR") == 0)//After matching line is found, skip over it and simply copy over rest of database
        {
            f = fopen("Database.txt","r+");

            if(f == NULL)
            {
                printf("Error opening database file...");
                exit(0);
            }else
            {

                newf = fopen("TempDatabase.txt","w");

                if(newf == NULL)
                {
                    printf("Could not open Temporary file");
                    exit(0);
                }else
                {
                    if(myIndex == 2)
                    {
                        char *ID = commandParams[1];

                        ID[strcspn(ID,"\n")] = '\0'; // Remove newline from input

                        char *RecordLine = findMatch(ID);

                        if(RecordLine != NULL)
                        {
                            //Skip over the returned record and just copy over the rest of the file

                            printf("Porting database...");

                            char copy[100];

                            while(fgets(copy,100,f) != NULL)
                            {
                                fprintf(newf,"%s",copy);
                            }

                            fclose(f);
                            fclose(newf);

                            remove("Database.txt");
                            //Rename copied data file
                            rename("TempDatabase.txt","Database.txt");

                            printf("Record Deleted!\n\n");
                        
                        }else
                        {
                            printf("Could not find specified ID, please try again...\n");
                        }
                    }else if(myIndex < 2)
                    {
                        printf("Not enough Parameters for delete operation\n\n");
                    }else
                    {
                        printf("Too many Parameters for delete operation only: [ID] should be supplied\n\n");
                    }
                }
            }

        }else if(strcmp(commandParams[0], "Exit") == 0)
        {
            break;
        }else if(strcmp(commandParams[0], "GetR") == 0)//Return matching line
        {
            f = fopen("Database.txt","r");

            char DocLine[100];

            if(f == NULL)
            {
                printf("Could not Open file...\n");
            }else
            {
                if(myIndex == 2)
                {
                    char *ID = commandParams[1];

                    ID[strcspn(ID,"\n")] = '\0'; // Remove newline from input

                    char *response = findMatch(ID);

                    printf("Matching Record: %s\n\n",response);

                }else if(myIndex >= 2)
                {
                    printf("Too Many parameters for Get command\n\n");
                }else
                {
                    printf("Not enough parameters for Get command\n\n");
                }
            }

        }else
        {
            printf("Command not recognized. Availabel commands are: [AddR, EditR, RemoveR, GetR,Exit]\n");
        }

        fgets(command, sizeof(command), stdin);

    }

    return 0;
}
