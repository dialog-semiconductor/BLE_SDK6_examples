/**
 ****************************************************************************************
 *
 * @file queue.c
 *
 * @brief Software for queues and threads.
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

#include "queue.h"
//////////#include "console.h"
#include "uart.h"

// Used to stop the tasks.
BOOL StopRxTask;

HANDLE UARTRxQueueSem;     // mutex semaphore to protect TestbusFrameQueue

HANDLE Rx232Id; // Thread handles

QueueRecord UARTRxQueue; //Queues UARTRx -> Main thread /  Console -> Main thread

HANDLE QueueHasAvailableData;

void InitTasks(void)
{
   StopRxTask = FALSE;

   Rx232Id   = (HANDLE) _beginthread(UARTProc, 10000, NULL);

   // Set thread priorities
   SetThreadPriority(Rx232Id, THREAD_PRIORITY_TIME_CRITICAL);

   UARTRxQueueSem = CreateMutex( NULL, FALSE, NULL );

   QueueHasAvailableData = CreateEvent(0, TRUE, FALSE, NULL);
}

void EnQueue(QueueRecord *rec,void *vdata)
{
  struct QueueStorage *tmp;
  tmp=(struct QueueStorage *) malloc(sizeof(struct QueueStorage));
  tmp->Next=NULL;
  tmp->Data=vdata;
  if(rec->First==NULL) {
    rec->First=tmp;
    rec->Last=tmp;
  } else {
    rec->Last->Next=tmp;
    rec->Last=tmp;
  }
  SetEvent(QueueHasAvailableData);
}

void *DeQueue(QueueRecord *rec)
{
  void *tmp;
  struct QueueStorage *tmpqe;
  if(rec->First==NULL)
  {
	  ResetEvent(QueueHasAvailableData);
    return NULL;
  }
  tmpqe=rec->First;
  rec->First=tmpqe->Next;
  tmp=tmpqe->Data;
  free(tmpqe);
  if(rec->First==NULL) 
	  ResetEvent(QueueHasAvailableData);
  return tmp;
}
