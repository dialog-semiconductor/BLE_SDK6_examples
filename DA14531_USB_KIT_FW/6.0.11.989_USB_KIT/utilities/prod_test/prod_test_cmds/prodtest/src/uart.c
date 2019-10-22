/**
 ****************************************************************************************
 *
 * @file uart.c
 *
 * @brief UART interface for HCI messages.
 *
 * Copyright (c) 2012-2018 Dialog Semiconductor. All rights reserved.
 *
 * This software ("Software") is owned by Dialog Semiconductor.
 *
 * By using this Software you agree that Dialog Semiconductor retains all
 * intellectual property and proprietary rights in and to this Software and any
 * use, reproduction, disclosure or distribution of the Software without express
 * written permission or a license agreement from Dialog Semiconductor is
 * strictly prohibited. This Software is solely for use on or in conjunction
 * with Dialog Semiconductor products.
 *
 * EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, THE
 * SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. EXCEPT AS OTHERWISE
 * PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, IN NO EVENT SHALL
 * DIALOG SEMICONDUCTOR BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THE SOFTWARE.
 *
 ****************************************************************************************
 */

#include <conio.h>
#include <process.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <windows.h>
#include <process.h>
#include <stddef.h>     // standard definition

#include "queue.h"
#include "uart.h"

//#define COMM_DEBUG

HANDLE hComPortHandle = NULL;
OVERLAPPED ovlRd,ovlWr;

/*
 ****************************************************************************************
 * @brief Write message to UART.
 * @param[in] payload_type 0x01 = HCI_CMD, 0x05 = FE_MSG
 * @param[in] payload_size Message size.
 * @param[in] payload      Pointer to message data.
 * @return void.
 ****************************************************************************************
*/
void UARTSend(unsigned char payload_type, unsigned short payload_size, unsigned char *payload)
{
	unsigned char bTransmit232ElementArr[500];
	unsigned short bSenderSize;
	unsigned long dwWritten;

	bTransmit232ElementArr[0] = payload_type; // message header
	memcpy(&bTransmit232ElementArr[1], payload, payload_size);

	bSenderSize = payload_size + 1;

	ovlWr.Offset     = 0;
    ovlWr.OffsetHigh = 0;
    ResetEvent(ovlWr.hEvent);

	WriteFile(hComPortHandle, bTransmit232ElementArr, bSenderSize, &dwWritten, &ovlWr);
}

/*
 ****************************************************************************************
 * @brief Send message received from UART to application's main thread.
 * @param[in] length        Message size.
 * @param[in] bInputDataPtr Pointer to message data.
 * @return void.
 ****************************************************************************************
*/
void SendToMain(unsigned char payload_type, unsigned short length, uint8_t *bInputDataPtr)
{
	QueueElement * qe; 
	unsigned char *bDataPtr; 

	// filter out FE API messages
	if (payload_type == 0x05)
	{
		return;
	}

	qe = (QueueElement *) malloc(sizeof(QueueElement));
	bDataPtr = (unsigned char *) malloc(length);
	
	memcpy(bDataPtr, bInputDataPtr, length);
	
	qe->payload_type = payload_type;
	qe->payload_size = length;
	qe->payload = bDataPtr;
	
	WaitForSingleObject(UARTRxQueueSem, INFINITE);
	EnQueue(&UARTRxQueue, qe);
	ReleaseMutex(UARTRxQueueSem);
}

/*
 ****************************************************************************************
 * @brief UART Reception thread loop.
 * @return void.
 ****************************************************************************************
*/
void UARTProc(PVOID unused)
{
   unsigned long dwBytesRead;
   unsigned char tmp;
   unsigned short wReceive232Pos=0;
   unsigned short wDataLength;
   unsigned char bReceive232ElementArr[1000];
   unsigned char bReceiveState = 0;
   unsigned char bHdrBytesRead = 0;

   while(StopRxTask == FALSE)
   {

      ovlRd.Offset     = 0;
      ovlRd.OffsetHigh = 0;
      ResetEvent(ovlRd.hEvent);

      // use overlapped read, not because of async read, but, due to
      // multi thread read/write
      ReadFile( hComPortHandle, &tmp, 1, &dwBytesRead, &ovlRd );

      GetOverlappedResult( hComPortHandle,
                           &ovlRd,
                           &dwBytesRead,
                           TRUE );

      switch(bReceiveState)
      {
         case 0:   // Receive FE_MSG
            if(tmp == 0x05)
            {
               bReceiveState = 1;
			   wDataLength = 0;
               wReceive232Pos = 0;
			   bHdrBytesRead = 0;

			   bReceive232ElementArr[wReceive232Pos]=tmp;
			   wReceive232Pos++;

				#ifdef COMM_DEBUG
					printf("\nI: ");
					printf("%02X ", tmp);
				#endif   
            }
			else if (tmp == 0x04) // HCI event	
			{
					bReceiveState = 11; 
					wDataLength = 0;
					wReceive232Pos = 0;
					bHdrBytesRead = 0;

					bReceive232ElementArr[wReceive232Pos]=tmp;
					wReceive232Pos++; 	
			} 
            else
            {
                  #ifdef COMM_DEBUG
                     printf("%02X ", tmp);
                  #endif
            }
            break;

		 case 1:   // Receive Header size = 6
               #ifdef COMM_DEBUG
                  printf("%02X ", tmp);
               #endif
			 bHdrBytesRead++;
			 bReceive232ElementArr[wReceive232Pos] = tmp;
			 wReceive232Pos++;

			 if (bHdrBytesRead == 6)
				 bReceiveState = 2;
				
			 break;
		 case 2:   // Receive LSB of the length
			#ifdef COMM_DEBUG
				printf("%02X ", tmp);
			#endif
			wDataLength += tmp;
            if(wDataLength > MAX_PACKET_LENGTH)
            {
                 bReceiveState = 0;
            }
            else
			{
				bReceive232ElementArr[wReceive232Pos] = tmp;
				wReceive232Pos++;
                bReceiveState = 3;
			}
          break;
         case 3:   // Receive MSB of the length
               #ifdef COMM_DEBUG
                  printf("%02X ", tmp);
               #endif
            wDataLength += (unsigned short) (tmp*256);
            if(wDataLength > MAX_PACKET_LENGTH)
            {

				#ifdef COMM_DEBUG
					printf("\nSIZE: %d ", wDataLength);
				#endif
                bReceiveState = 0;
            }
			else if(wDataLength == 0)
			{
				#ifdef COMM_DEBUG
					printf("\nSIZE: %d ", wDataLength);
				#endif
				SendToMain(0x05, (unsigned short) (wReceive232Pos-1), &bReceive232ElementArr[1]); // an FE msg
                bReceiveState = 0;
			}
            else
			{
			   bReceive232ElementArr[wReceive232Pos] = tmp;
			   wReceive232Pos++;
               bReceiveState = 4;
			}
            break;
         case 4:   // Receive Data
			#ifdef COMM_DEBUG
				printf("%02X ", tmp);
            #endif
            bReceive232ElementArr[wReceive232Pos] = tmp;
            wReceive232Pos++;
			
            if(wReceive232Pos == wDataLength + 9 ) // 1 ( first byte - 0x05) + 2 (Type) + 2 (dstid) + 2 (srcid) + 2 (lengths size)
            {
               // Sendmail program
               SendToMain(0x05, (unsigned short) (wReceive232Pos-1), &bReceive232ElementArr[1]); ///FE msg
			   bReceiveState = 0;
				#ifdef COMM_DEBUG
					printf("\nSIZE: %d ", wDataLength);
				#endif
            }
           break;

			
		 case 11:   // Receive HCI event type byte
				bReceive232ElementArr[wReceive232Pos] = tmp;
				wReceive232Pos++;

				bReceiveState = 12;
				break;
	
		 case 12:   // Receive HCI event length byte
				wDataLength = tmp;
				
				if(wDataLength == 0)
				{
					bReceive232ElementArr[wReceive232Pos] = tmp;
					wReceive232Pos++;

					SendToMain(0x04, (unsigned short) (wReceive232Pos-1), &bReceive232ElementArr[1]);
					bReceiveState = 0;
				}
				else
				{
					bReceive232ElementArr[wReceive232Pos] = tmp;
					wReceive232Pos++;
					bReceiveState = 13;
				}
				break;
	
		 case 13:   // Receive HCI event data
				bReceive232ElementArr[wReceive232Pos] = tmp;
				wReceive232Pos++;

				if(wReceive232Pos == wDataLength + 3 ) // 1 ( first byte - 0x01) + 1 (event) + 1 (length)
				{
					SendToMain(0x04, (unsigned short) (wReceive232Pos-1), &bReceive232ElementArr[1]);
					bReceiveState = 0;
				}
				break;
	}
      
   }

   StopRxTask = TRUE;   // To indicate that the task has stopped

   PurgeComm(hComPortHandle, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);

   Sleep(100);

   CloseHandle(hComPortHandle);

   ExitThread(0);
}



/*
 ****************************************************************************************
 * @brief Init UART iface.
 * @param[in] Port     COM prot number.
 * @param[in] BaudRate Baud rate.
 * @return -1 on failure / 0 on success.
 ****************************************************************************************
*/
uint8_t InitUART(int Port, int BaudRate)
{
   DCB dcb;
   BOOL fSuccess;
   COMSTAT stat;
   DWORD error;
   COMMTIMEOUTS commtimeouts;
   char CPName[500];
#ifdef RSX
   DWORD dwErrorCode;
#endif

   sprintf(CPName, "\\\\.\\COM%d", Port);

#ifdef DEVELOPMENT_MESSAGES
   fprintf(stderr, "[info] Connecting to %s\n", &CPName[4]);
#endif //DEVELOPMENT_MESSAGES

   ovlRd.hEvent = CreateEvent( NULL,FALSE,FALSE,NULL );
   ovlWr.hEvent = CreateEvent( NULL,FALSE,FALSE,NULL );

   hComPortHandle = CreateFile(CPName,
                               GENERIC_WRITE | GENERIC_READ,
                               0, //FILE_SHARE_WRITE | FILE_SHARE_READ,
                               NULL,
                               OPEN_EXISTING,
                               FILE_FLAG_OVERLAPPED,
                               NULL );

   if(hComPortHandle == INVALID_HANDLE_VALUE)
   {
      #ifdef RSX
         dwErrorCode = GetLastError();
         PrintfInt("Failed to open %s! %lu\n", PortName[Port], dwErrorCode);
      #endif
      return -1;
   }

   ClearCommError( hComPortHandle, &error, &stat );
   
   memset(&dcb, 0x0, sizeof(DCB) );
   fSuccess = GetCommState(hComPortHandle, &dcb);
   if(!fSuccess)
   {
      #ifdef RSX
         PrintfInt("Failed to get DCB!\n");
      #endif
      return -1;
   }

   // Fill in the DCB
   dcb.BaudRate = BaudRate;
   dcb.ByteSize = 8;
   dcb.Parity = NOPARITY;
   dcb.StopBits = ONESTOPBIT;
   dcb.fBinary = 1;
   // disable all kind of flow control and error handling
   dcb.fOutxCtsFlow = 0;
   dcb.fOutxDsrFlow = 0;
   dcb.fRtsControl  = RTS_CONTROL_DISABLE;
   dcb.fDtrControl  = DTR_CONTROL_DISABLE;
   dcb.fInX         = 0;
   dcb.fOutX        = 0;
   dcb.fErrorChar   = 0;
   dcb.fNull        = 0;
   dcb.fAbortOnError = 0;

   fSuccess = SetCommState(hComPortHandle, &dcb);
   if(!fSuccess)
   {
#ifdef DEVELOPMENT_MESSAGES
	   fprintf(stderr, "Failed to set DCB!\n");
#endif //DEVELOPMENT_MESSAGES
	   return -1;
   }
  commtimeouts.ReadIntervalTimeout = 1000; 
  commtimeouts.ReadTotalTimeoutMultiplier = 0; 
  commtimeouts.ReadTotalTimeoutConstant = 0; 
  commtimeouts.WriteTotalTimeoutMultiplier = 0; 
  commtimeouts.WriteTotalTimeoutConstant = 0;

  fSuccess = SetCommTimeouts( hComPortHandle,
                              &commtimeouts );
 
#ifdef DEVELOPMENT_MESSAGES
  fprintf(stderr, "[info] %s successfully opened, baud rate %d\n", &CPName[4], BaudRate);
#endif //DEVELOPMENT_MESSAGES

   return 0;
}
