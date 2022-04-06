#define WINVER 0x0500
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <windows.h>

using namespace std;

bool getconchar( KEY_EVENT_RECORD& krec ) 
{
   DWORD cc;
   INPUT_RECORD irec;
   HANDLE h = GetStdHandle( STD_INPUT_HANDLE );

   if (h == NULL) return 0; // console not found

   for( ; ; ) 
	{
       ReadConsoleInput( h, &irec, 1, &cc );
       if ( irec.EventType == KEY_EVENT
           &&  ((KEY_EVENT_RECORD&)irec.Event).bKeyDown
           )//&& ! ((KEY_EVENT_RECORD&)irec.Event).wRepeatCount )
       {
           krec = (KEY_EVENT_RECORD&)irec.Event;
           return 1;
       }
   }
   return 0; 
}

int main() 
{	
	// Navigation menu
	const char *menu =
	#include "menu.txt"
	;
	
	// help
	const char *help =
	#include "help.txt"
	;
	
	// credits
	const char *credits =
	#include "credits.txt"
	;
	
	// license
	const char *license =
	#include "license.txt"
	;
	
	char response[256];
	
	printf(menu);
	while(1)
	{
		printf(">>> ");
		scanf("%s", &response);
		
		if (!strcmp(response, "help")) printf(help);
		else if (!strcmp(response, "credits")) printf(credits);
		else if (!strcmp(response, "license")) printf(license);
		else if (!strcmp(response, "cls")) 
		{
			system("powershell cls");
			printf(menu);
		}
		else if (!strcmp(response, "go")) break;
	}

	// Variable definition
	// Max input array dimension
	int max = 32767;
	
	// Create an array big enough to hold user's inputs
	int *key_array = (int *) malloc(sizeof(int) * max);
	int *delay_array = (int *) malloc(sizeof(int) * max);
	
	// Memory allocation checking
	if ((key_array == NULL) || (delay_array == NULL))
	{
		printf("ERROR: cannot allocate enough memory!");
		exit(1);
	}
	
	// Integer that holds the dimension of the array of character sequences
	int idx = 0;
	
	// Delay between one character and the next one
	int delay = 0;
	
	// Recorded key
	KEY_EVENT_RECORD key;
	
	// Character insertion section
	while (1) 
	{
		printf("\nType an alphanumerical/control sequence, press ESC to stop inserting characters and start the program: ");
		getconchar(key);
		
		// Exit condition
		if (key.wVirtualKeyCode == 27) break;
		
		// Print the typed key
		std::cout << "\nInput: " << key.uChar.AsciiChar
            << " -- Ascii code:  " << key.wVirtualKeyCode << std::endl;
		
		key_array[idx] = key.wVirtualKeyCode;
		
		printf("Type a number that represents the delay (in milliseconds): ");

		if (scanf("%d", &delay) != 1)
		{
			printf("Wrong input, try again.\n");
			
			// Ripulisce il buffer di input
			fflush(stdin);
			continue;
		}
		
		delay_array[idx++] = delay;	// in millisecondi
	}
	
	printf("\nInput array length: %d\n", idx);
	Sleep(3000);
	
	// This structure will be used to create the keyboard input event.
	INPUT ip;
	
	// SEZIONE DI SIMULAZIONE DIGITAZIONE
	while(1)
	{
		for (int j = 0; j < idx; j++)
		{	
			//printf("%d %d", key_array[j], delay_array[j]);
	
			// Set up a generic keyboard event.
			ip.type = INPUT_KEYBOARD;
			ip.ki.wScan = 0; // hardware scan code for key
			ip.ki.time = 0;
			ip.ki.dwExtraInfo = 0;
			
			// Press the KEYPRESS key
			ip.ki.wVk = key_array[j]; // virtual-key code for the KEYPRESS key
			ip.ki.dwFlags = 0; // 0 for key press
			SendInput(1, &ip, sizeof(INPUT));
			
			// Release the KEYPRESS key
			ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
			SendInput(1, &ip, sizeof(INPUT));
			
			// Pause for x seconds.
			Sleep(delay_array[j]);
		}
	}
		
	// Exit normally
	free(key_array);
	free(delay_array);

	return 0;
}