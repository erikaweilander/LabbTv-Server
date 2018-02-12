/********************************************************************\
* server.c                                                           *
*                                                                    *
* Desc: example of the server-side of an application                 *
* Revised: Dag Nystrom & Jukka Maki-Turja                     *
*                                                                    *
* Based on generic.c from Microsoft.                                 *
*                                                                    *
*  Functions:                                                        *
*     WinMain      - Application entry point                         *
*     MainWndProc  - main window procedure                           *
*                                                                    *
* NOTE: this program uses some graphic primitives provided by Win32, *
* therefore there are probably a lot of things that are unfamiliar   *
* to you. There are comments in this file that indicates where it is *
* appropriate to place your code.                                    *
* *******************************************************************/

#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "wrapper.h"

HANDLE myMutex;
planet_type *HeadPlanet;
void addPlanet(planet_type *data);
void removePlanet(planet_type *remove);
planet_type* updatePlanet(planet_type *planet);

							/* the server uses a timer to periodically update the presentation window */
							/* here is the timer id and timer period defined                          */

#define UPDATE_FREQ     10	/* update frequency (in ms) for the timer */

							/* (the server uses a mailslot for incoming client requests) */



/*********************  Prototypes  ***************************/
/* NOTE: Windows has defined its own set of types. When the   */
/*       types are of importance to you we will write comments*/ 
/*       to indicate that. (Ignore them for now.)             */
/**************************************************************/

LRESULT WINAPI MainWndProc( HWND, UINT, WPARAM, LPARAM );
DWORD WINAPI mailThread(LPVOID);


HDC hDC;		/* Handle to Device Context, gets set 1st time in MainWndProc */
				/* we need it to access the window for printing and drawin */

/********************************************************************\
*  Function: int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)    *
*                                                                    *
*   Purpose: Initializes Application                                 *
*                                                                    *
*  Comments: Register window class, create and display the main      *
*            window, and enter message loop.                         *
*                                                                    *
*                                                                    *
\********************************************************************/

							/* NOTE: This function is not too important to you, it only */
							/*       initializes a bunch of things.                     */
							/* NOTE: In windows WinMain is the start function, not main */

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow ) {

	HWND hWnd;
	DWORD threadID;
	MSG msg;

	

							/* Create the window, 3 last parameters important */
							/* The tile of the window, the callback function */
							/* and the backgrond color */

	hWnd = windowCreate (hPrevInstance, hInstance, nCmdShow, "Himmel", MainWndProc, COLOR_WINDOW+1);

							/* start the timer for the periodic update of the window    */
							/* (this is a one-shot timer, which means that it has to be */
							/* re-set after each time-out) */
							/* NOTE: When this timer expires a message will be sent to  */
							/*       our callback function (MainWndProc).               */
  
	windowRefreshTimer (hWnd, UPDATE_FREQ);
  

							/* create a thread that can handle incoming client requests */
							/* (the thread starts executing in the function mailThread) */
							/* NOTE: See online help for details, you need to know how  */ 
							/*       this function does and what its parameters mean.   */
							/* We have no parameters to pass, hence NULL				*/
  

	threadID = threadCreate (mailThread, NULL); 
  

							/* (the message processing loop that all windows applications must have) */
							/* NOTE: just leave it as it is. */
	while( GetMessage( &msg, NULL, 0, 0 ) ) {
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}


	return msg.wParam;
}


/********************************************************************\
* Function: mailThread                                               *
* Purpose: Handle incoming requests from clients                     *
* NOTE: This function is important to you.                           *
/********************************************************************/
DWORD WINAPI mailThread(LPVOID arg) {

	char buffer[1024];
	DWORD bytesRead;
	static int posY = 0;
	HANDLE mailbox;
	planet_type *data = (planet_type*) malloc(sizeof(planet_type));
	int apa = sizeof(planet_type);
	char text[50];
	LPTSTR Slot = TEXT("\\\\.\\mailslot\\mailslot");


							/* create a mailslot that clients can use to pass requests through   */
							/* (the clients use the name below to get contact with the mailslot) */
							/* NOTE: The name of a mailslot must start with "\\\\.\\mailslot\\"  */

	
	mailbox = mailslotCreate(Slot);


	for(;;) {				
							/* (ordinary file manipulating functions are used to read from mailslots) */
							/* in this example the server receives strings from the client side and   */
							/* displays them in the presentation window                               */
							/* NOTE: binary data can also be sent and received, e.g. planet structures*/
 
	bytesRead = mailslotRead(mailbox, data, sizeof(planet_type));

	if(bytesRead!= 0) {
							/* NOTE: It is appropriate to replace this code with something */
							/*       that match your needs here.                           */
		posY++;  


		
		addPlanet(data);


							/* (hDC is used reference the previously created window) */							
		//TextOut(hDC, data->sx, 50+(int)(data->sy)%200, data, sizeof(data->name));
		SetPixel(hDC, data->sx, 50 + (int)(data->sy) % 200, data, (COLORREF)3);
	}
	else {
							/* failed reading from mailslot                              */
							/* (in this example we ignore this, and happily continue...) */
    }
  }

	

  return 0;
}


/********************************************************************\
* Function: LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM) *
*                                                                    *
* Purpose: Processes Application Messages (received by the window)   *
* Comments: The following messages are processed                     *
*                                                                    *
*           WM_PAINT                                                 *
*           WM_COMMAND                                               *
*           WM_DESTROY                                               *
*           WM_TIMER                                                 *
*                                                                    *
\********************************************************************/
/* NOTE: This function is called by Windows when something happens to our window */

LRESULT CALLBACK MainWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) {
  
	PAINTSTRUCT ps;
	static int posX = 10;
	int posY;
	HANDLE context;
	static DWORD color = 0;
  
	switch( msg ) {
							/**************************************************************/
							/*    WM_CREATE:        (received on window creation)
							/**************************************************************/
		case WM_CREATE:       
			hDC = GetDC(hWnd);  
			break;   
							/**************************************************************/
							/*    WM_TIMER:         (received when our timer expires)
							/**************************************************************/
		case WM_TIMER:

							/* NOTE: replace code below for periodic update of the window */
							/*       e.g. draw a planet system)                           */
							/* NOTE: this is referred to as the 'graphics' thread in the lab spec. */

							/* here we draw a simple sinus curve in the window    */
							/* just to show how pixels are drawn                  */
			posX += 4;
			posY = (int) (10 * sin(posX / (double) 30) + 20);
			SetPixel(hDC, posX % 547, posY, (COLORREF) color);
			color += 12;
			windowRefreshTimer (hWnd, UPDATE_FREQ);
			break;
							/****************************************************************\
							*     WM_PAINT: (received when the window needs to be repainted, *
							*               e.g. when maximizing the window)                 *
							\****************************************************************/

		case WM_PAINT:
							/* NOTE: The code for this message can be removed. It's just */
							/*       for showing something in the window.                */
			context = BeginPaint( hWnd, &ps ); /* (you can safely remove the following line of code) */
			TextOut( context, 10, 10, "Welcome to Erika and Linus Universe!", 36 ); /* 13 is the string length */
			
			EndPaint( hWnd, &ps );
			break;
							/**************************************************************\
							*     WM_DESTROY: PostQuitMessage() is called                  *
							*     (received when the user presses the "quit" button in the *
							*      window)                                                 *
							\**************************************************************/
		case WM_DESTROY:
			PostQuitMessage( 0 );
							/* NOTE: Windows will automatically release most resources this */
     						/*       process is using, e.g. memory and mailslots.           */
     						/*       (So even though we don't free the memory which has been*/     
     						/*       allocated by us, there will not be memory leaks.)      */

			ReleaseDC(hWnd, hDC); /* Some housekeeping */
			break;

							/**************************************************************\
							*     Let the default window proc handle all other messages    *
							\**************************************************************/
		default:
			return( DefWindowProc( hWnd, msg, wParam, lParam )); 
   }
   return 0; 
}

void addPlanet(planet_type *data)
{
	planet_type *currentPlanet = HeadPlanet;

	DWORD waitResult = WaitForSingleObject(myMutex, INFINITE);


	if (waitResult == WAIT_OBJECT_0) {
		if (HeadPlanet == NULL) {
			printf("YOU HAVE NO PLANET, YOU PEASANT");
			HeadPlanet = data;
			printf("BUT YOU SURE HELL DESERVE ONE! kind regards, Steve Jobs\n");
		}

		else {

			while (!(currentPlanet->next == NULL))
			{
				currentPlanet = currentPlanet->next;
			}

			currentPlanet->next = data;

		}

		threadCreate(updatePlanet, data);
		// en thread create med update planet som input
	}

	ReleaseMutex(myMutex);
}
void removePlanet( planet_type *remove)
{
	planet_type *currentPlanet = HeadPlanet;
	planet_type *planetToRemove = remove;
	planet_type *previous = HeadPlanet;
	
	WaitForSingleObject(myMutex, INFINITE);

	if (HeadPlanet == remove)
	{
		free(HeadPlanet);
	
		return;
	}
	
	while (currentPlanet->next != NULL)
	{
		previous = currentPlanet;
		currentPlanet = currentPlanet->next;

		if (currentPlanet->next == remove)
		{
			if (currentPlanet->next != NULL) 
			{
				planetToRemove = currentPlanet->next;
				previous = currentPlanet;
				free(planetToRemove);
				planetToRemove = NULL;
				
			}
			else
			{
				free(currentPlanet);
				previous->next = NULL;
			}
		}
	}
	
	ReleaseMutex(myMutex);
	return ;
}
planet_type* updatePlanet(planet_type *planet)
{
	myMutex = CreateMutex(NULL, FALSE, NULL);
	DWORD waitResult;
	double gravity = 6.67259e-11;
	int dt = 10;
	planet_type *currentPlanet = HeadPlanet;
	char deadmsg[100] = "has sadly died, all because of darth vader.";
	char deadmsgtosend[100] = { 0 };
	char mailslotPid[100] = { 0 };
	HANDLE hWrite;



	while (planet->life > 0) {

		double atot_x = 0;
		double atot_y = 0;
		
		waitResult = WaitForSingleObject(myMutex, INFINITE);

		if (waitResult == WAIT_OBJECT_0)
		{
			while (currentPlanet->next != NULL)
			{
				//r som anv�nds i a1 formeln(hur mycket andra planeter bidrar i acceleration)
				double r = sqrt(pow((currentPlanet->sx) - (planet->sx), 2) + pow((currentPlanet->sy) - (planet->sy), 2));
				double a1 = gravity * (currentPlanet->mass) / (r*r);

				//acceleration i x och y led
				atot_x += a1 * (currentPlanet->sx - planet->sx) / r;
				atot_y += a1 * (currentPlanet->sy - planet->sy) / r;
			}
			currentPlanet = currentPlanet->next;
		}
		ReleaseMutex(myMutex);
			//planetens nya position och acceleration
		planet->vx = (planet->vx + atot_x * dt);
		planet->sx = (planet->vx + atot_x * dt);

		planet->vy = (planet->vy + atot_y * dt);
		planet->sy = (planet->vy + atot_y * dt);
		
		planet->life--;

		if (planet->sx >= 800 || planet->sy >= 600 || planet->life <=0) //Om planeten g�r out of bounds eller om den d�r
		{  
			if (planet->sx >= 800 || planet->sy >= 600) {
				planet->life = 0;
			}

			removePlanet(planet);
	
			*deadmsgtosend = strcat_s(planet->name, sizeof(planet->name), deadmsg);
			*mailslotPid = strcat_s("\\\\.\\mailslot\\mailslot", sizeof("\\\\.\\mailslot\\mailslot"), planet->pid);
		
			hWrite = mailslotConnect(*mailslotPid);

			if (hWrite == INVALID_HANDLE_VALUE) {
				printf("Failed to get a handle to the mailslot!!\nHave you started the Client?\n");
				return;
			}
			else {
				mailslotWrite(*mailslotPid, *deadmsgtosend, sizeof(*deadmsgtosend));
				printf("Sucessfully written to mailslot\n");
			}

			Sleep(UPDATE_FREQ);

			return 0;

		}

	}

	return 0;
	  //else removePlanet(data);
	//mailslotWrite(mailbox, data, sizeof(data));

}



