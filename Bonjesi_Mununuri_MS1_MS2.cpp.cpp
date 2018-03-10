// The encryption program in C++ and ASM with a very simple encryption method - it simply adds 1 to the character.
// The encryption method is written in ASM. You will replace this with your allocated version for the assignment.
// In this version parameters are passed via registers (see 'encrypt' for details).
// Filename: "4473 FoCA Assignment 2 2017 - Encryption Original with ASM.cpp"
// Last revised Feb 2017 by A.Oram

char EKey = 'L';			// Replace x with your Encryption key.

#define StudentName "Mununuri Bonjesi"

//  *** PLEASE CHANGE THE NAME IN QUOTES ABOVE TO YOUR NAME  ***
//  *** KEEP ALL COMMENTS UP-TO-DATE. COMMENT USEFULLY ALL CODE YOU PRODUCE. ***

#define MAXCHARS 6		// feel free to alter this, but 6 is the minimum

using namespace std;
#include <conio.h>		// for kbhit
#include <windows.h>
#include <string>       // for strings
#include <fstream>		// file I/O
#include <iostream>		// for cin >> and cout <<
#include <iomanip>		// for fancy output
#include "TimeUtils.h"  // for GetTime, GetDate, etc.

#define dollarchar '$'  // string terminator

char OChars[MAXCHARS],
EChars[MAXCHARS],
DChars[MAXCHARS];	// Global Original, Encrypted, Decrypted character strings

//----------------------------- C++ Functions ----------------------------------------------------------

void get_char(char& a_character)
{
	cin >> a_character;
	while (((a_character < '0') | (a_character > 'z')) && (a_character != dollarchar))
	{
		cout << "Alphanumeric characters only, please try again > ";
		cin >> a_character;
	}
}
//-------------------------------------------------------------------------------------------------------------

void get_original_chars(int& length)
{
	char next_char;
	length = 0;
	get_char(next_char);

	while ((length < MAXCHARS) && (next_char != dollarchar))
	{
		OChars[length++] = next_char;
		get_char(next_char);
	}
}

//---------------------------------------------------------------------------------------------------------------
//----------------- ENCRYPTION ROUTINES -------------------------------------------------------------------------

void encrypt_chars(int length, char EKey)
{
	char temp_char;						                       // Character temporary store

	for (int i = 0; i < length; i++)	                      // Encrypt characters one at a time
	{
		temp_char = OChars[i];			                     // Get the next char from Original Chars array
		
		__asm {							                   //start of assembly code




			    push   eax						       //push the loop counter
				push   ecx					          //push the value of character to be encrypted on to the stack
                movzx  ecx, temp_char		         //move the character to be encrypted into the temporary char
				lea    eax, EKey		            //load effective address of the Ekey into the Eax register
				push eax
				push ecx
				call encrypt_12			          //call encrypt_12 function 
				add  esp,8
				mov    temp_char, al			 //move the encrypted lower 8 bit value into the temporary char


				pop    ecx					   // get the value the value of the character off the stack
				pop    eax					  // remove address of the ekey of the stack
		}
		EChars[i] = temp_char;			    // Store encrypted char in the Encrypted Chars array
	}
	return;



	// Inputs: register EAX = 32-bit address of Ekey,
	//					ECX = the character to be encrypted (in the low 8-bit field, CL).
	// Output: register EAX = the encrypted value of the source character (in the low 8-bit field, AL).

	__asm {  //start of assembly code

	encrypt_12:

		    push ebp                                                   //stores the base pointer for use later on in the program

			mov ebp, esp                                              // set up the base on pointer to the function call
			mov edx, [ebp + 12]                                      // your moving the second parameter values into the ekey 
			mov ecx, [ebp + 8]					                    //  your moving first parameters values into the ekey
		    push edi                                               //save the values of registers that function is going to change 
			push ecx                                              //save a copy of the character to the encrypted for later use
			inc[eax]                                             // increase the value in the 32 bit address of eax, the ekey is in eax
			add byte ptr[eax], 0x3e                             //adds 62 with to low 8-bit field of eax
			mov edi, [eax]                                     //copy the values of the registers to the address of eax
			and edi, 0xFF                                     //and's Ekey with FF which 255 in decimal
			pop eax                                          // puts the encrypted value into eax
			xor eax, edi 
			rol al, 1                                      // shift the encrypted value of the source character to the left by one 
			rol al, 1                                     //  shift the encrypted value of the source character to the left by one 
			rol al, 1                                    // shift the encrypted value of the source character to the left by one 
			xor eax, edi
			
			pop ecx 
			pop ebp
			ret                                     //return values of subroutine function

	}

	//--- End of Assembly code
}
// end of encrypt_chars function
//---------------------------------------------------------------------------------------------------------------




//---------------------------------------------------------------------------------------------------------------
//----------------- DECRYPTION ROUTINES -------------------------------------------------------------------------
//
void decrypt_chars(int length, char EKey)
{
	char temp_char;											      //Character temporary store

	for (int i = 0; i < length; i++)						   //Encrypt characters one at a time
	{
		temp_char = EChars[i];								  //Get the next char from Original Chars array

		__asm {												//start of assembly code 
			push   eax
				push   ecx								   //push the value of the character on to the stack
				movzx  ecx, temp_char					  //move the character to be encrypted into the temporary char
				lea    eax, EKey                         //load effective address of the Ekey into the Eax register
				push   eax                              //push the value of the Ekey on to the stack 
				push   ecx                             //push the value of the Ekey on to the stack
				call   decrypt_12				      //the decrypt 12 function
				add    esp, 8					     //cleaning the values of the stack
				mov    temp_char, al			    //move the encrypted lower 8 bit value into the temporary char



				pop    ecx					     // get the value the value of the character off the stack
				pop    eax                      //remove the value of the Ekey off the stack

		}

		DChars[i] = temp_char;			      // Store encrypted char in the Encrypted Chars array     
	}

	return;

	__asm {

	decrypt_12:

		push ebp                                                   //stores the base pointer for use later on in the program

			mov ebp, esp                                              // set up the base on pointer to the function call
			mov edx, [ebp + 12]                                      // your moving the second parameter values into the ekey 
			mov ecx, [ebp + 8]					                    //  your moving first parameters values into the ekey 
			push edi                                               //push the values of the registers the registers are going to change

			push ecx                                              //save the values of registers that function is going to change
			//save a copy of the character to the encrypted for later use
			inc[eax]                                            // increase the value in the 32 bit address of eax, the ekey is in eax
			add byte ptr[eax], 0x3e							   //adds 62 with to low 8-bit field of eax which is key
			mov edi, [eax]								      //copy the values of the registers to the address of eax
			mov eax, [ebp + 8]
			and edi, 0xFF									 //multiply the ekey by 255

			pop eax											// puts the encrypted value into eax
			xor eax, edi //
			ror al, 1									  //shift the encrypted value of the source character to the left by one 
			ror al, 1								     //shift the encrypted value of the source character to the left by one 
			ror al, 1									//shift the encrypted value of the source character to the left by one 
			xor eax, edi//





			mov    temp_char, al			        //move the encrypted lower 8 bit value into the temporary char


			pop    ecx					          // get the value the value of the character off the stack
			//pop    eax			             // remove address of the ekey of the stack
			pop    ebp                          // sets base pointer back to the place function call where it was before
			ret

	}

}



// end of decrypt_chars function
//---------------------------------------------------------------------------------------------------------------


int main(void)
{
	int char_count(0);  // The number of actual characters entered (upto MAXCHARS limit).

	cout << "\nPlease enter upto " << MAXCHARS << " alphanumeric characters:  ";
	get_original_chars(char_count);

	ofstream EDump;
	EDump.open("EncryptDump.txt", ios::app);
	EDump << "\n\nFoCA Encryption program results (" << StudentName << ") Encryption key = '" << EKey << "'";
	EDump << "\nDate: " << GetDate() << "  Time: " << GetTime();

	// Display and save initial string
	cout << "\n\nOriginal string =  " << OChars << "\tHex = ";
	EDump << "\n\nOriginal string =  " << OChars << "\tHex = ";
	for (int i = 0; i < char_count; i++)
	{
		cout << hex << setw(2) << setfill('0') << ((int(OChars[i])) & 0xFF) << "  ";
		EDump << hex << setw(2) << setfill('0') << ((int(OChars[i])) & 0xFF) << "  ";
	};

	//*****************************************************
	// Encrypt the string and display/save the result
	encrypt_chars(char_count, EKey);

	cout << "\n\nEncrypted string = " << EChars << "\tHex = ";
	EDump << "\n\nEncrypted string = " << EChars << "\tHex = ";
	for (int i = 0; i < char_count; i++)
	{
		cout << ((int(EChars[i])) & 0xFF) << "  ";
		EDump << ((int(EChars[i])) & 0xFF) << "  ";
	}

	//*****************************************************
	// Decrypt the encrypted string and display/save the result

	decrypt_chars(char_count, EKey);

	cout << "\n\nDecrypted string = " << DChars << "\tHex = ";
	EDump << "\n\nDecrypted string = " << DChars << "\tHex = ";
	for (int i = 0; i < char_count; i++)
	{
		cout << ((int(DChars[i])) & 0xFF) << "  ";
		EDump << ((int(DChars[i])) & 0xFF) << "  ";
	}
	//*****************************************************

	cout << "\n\n\n";
	EDump << "\n\n-------------------------------------------------------------";
	EDump.close();
	system("PAUSE");
	return (0);


} // end of whole encryption/decryption program --------------------------------------------------------------------


