#include <stdio.h>
#include <stdlib.h>
#include "diag/trace.h"
#include <string.h>
#include <strings.h>
#include <time.h>
/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

#define CCM_RAM __attribute__((section(".ccmram")))
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

/* Char Size */
#define Char_Size 30
#define RAND_MAX 36767
/* *****************  TIMERS & SEMAPHORES  ******************** */
TimerHandle_t sender1Timer, sender2Timer, receiverTimer;
SemaphoreHandle_t Sender1_Signal = NULL, Sender2_Signal = NULL, Receiver_Signal = NULL;

/* ***************** Initialzing  ******************** */
//		Periods
int Tsender1[6] = { 50, 80, 110, 140, 170, 200 };
int Tsender2[6] = { 150, 200, 250, 300, 350, 400 };
int size = (sizeof Tsender1 / sizeof * Tsender1) - 1;
//		Identifying Array Selection
int ArrayIndex = 0;
// 		Final checker
int Final = 0;
//		Fixed Period for Receiver
int Treceiver = 100;

//		Counters
int Successcounter = 0, Failedcounter = 0, Receivedcounter = 0;

char QueueMesg[Char_Size];

//		Getting the Average time for every 500 mesg
int Sum1 = 0, Sum2 = 0, Avg1 = 0, Avg2 = 0, one = 0, two = 0;

//Identifying Queue
xQueueHandle Main_Queue = 0;



/* ***************** Uniform Distribution Function ******************** */
int uniform_distribution(int rangeLow, int rangeHigh)
{
	int range = rangeHigh - rangeLow + 1;

	// Largest value that when multiplied by "range"
	// is less than or equal to RAND_MAX
	long double chunkSize = (RAND_MAX + 1) / range;
	int endOfLastChunk = chunkSize * range;

	int r = rand();
	while (r >= endOfLastChunk)
	{
		r = rand();
	}
	return rangeLow + r / chunkSize;
}
/* ***************** Getting the Average Time for Sender one and Sender two ******************** */
void Avg()
{
	//	For first sender
	Avg1 = Sum1 / one;
	Avg2 = Sum2 / two;
}


/* ***************** Next Stage function ******************** */
//	This Function gonna will be called every 500 Received mesg and then changes some values in order to move to the next state
void NextStage()
{
	printf(" No of Turns =  %d\n", ArrayIndex + 1);
	ArrayIndex++;

	//Checking if there still numbers in the array
	if (ArrayIndex <= size)
	{
		//			Printing Counters
		Avg();
		printf(" Number of Messages Successfully sent     = %d\n", Successcounter);
		printf(" Number of Messages Failed to be sent     = %d\n", Failedcounter);
		printf(" Number of Messages Successfully Received = %d\n", Receivedcounter);

		//			Reseting
		Successcounter = 0;
		Failedcounter = 0;
		Receivedcounter = 0;
		xQueueReset(Main_Queue);

		xTimerChangePeriod(sender1Timer, pdMS_TO_TICKS(uniform_distribution(Tsender1[ArrayIndex], Tsender2[ArrayIndex])), 0);
		xTimerChangePeriod(sender2Timer, pdMS_TO_TICKS(uniform_distribution(Tsender1[ArrayIndex], Tsender2[ArrayIndex])), 0);

	}
	else
	{
		Avg();
		printf(" Number of Messages Successfully sent     = %d\n", Successcounter);
		printf(" Number of Messages Failed to be sent     = %d\n", Failedcounter);
		printf(" Number of Messages Successfully Received = %d\n", Receivedcounter);


		//		Letting Final = 1 So, that we don't have any more periods
		Final = 1;
	}

	if (Final == 1)
	{
		//		Deleting Timers and Stopping the Program
		xTimerDelete(sender1Timer, 0);
		xTimerDelete(sender2Timer, 0);
		xTimerDelete(receiverTimer, 0);
		printf("***************Game Over!***************\n");
		exit(0);
	}

}

/*
 * Note:
 *		Nothing the dependent on the other ( Sender1 and Sender2 just sending without waiting each other as required) So, we have 3 different Semaphores
*/

/* ***************** Callback Functions ******************** */
void senderoneTimerCallback(TimerHandle_t xTimer)
{
	//	Getting Random Time from Uniform Distribution function
	int NewTime = uniform_distribution(Tsender1[ArrayIndex], Tsender2[ArrayIndex]);

	/* Create a boolean xTaskWoken to check if releasing the semaphore allowed the sender task to be unblocked */
	static BaseType_t xTaskWoken = pdFALSE;

	/* Release the senderSemaphore for the sender task to continue execution */
	xSemaphoreGiveFromISR(Sender1_Signal, &xTaskWoken);
	// The printf used for checking if it was getting a correct random values
		//printf("Newtime at First = %d\n",NewTime);
	Sum1 += NewTime;
	one++;
	// Changing the Period to the new Period according to Uniform Distribution
	xTimerChangePeriod(sender1Timer, pdMS_TO_TICKS(NewTime), 0);
}

void sendertwoTimerCallback(TimerHandle_t xTimer)
{
	//	Getting Random Time from Uniform Distribution function
	int NewTime = uniform_distribution(Tsender1[ArrayIndex], Tsender2[ArrayIndex]);

	/* Create a boolean xTaskWoken to check if releasing the semaphore allowed the sender task to be unblocked */
	static BaseType_t xTaskWoken = pdFALSE;

	/* Release the senderSemaphore for the sender task to continue execution */
	xSemaphoreGiveFromISR(Sender2_Signal, &xTaskWoken);
	// The printf used for checking if it was getting a correct random values
		//printf("Newtime at Second = %d\n",NewTime);
	// Changing the Period to the new Period according to Uniform Distribution
	Sum2 += NewTime;
	two++;
	xTimerChangePeriod(sender2Timer, pdMS_TO_TICKS(NewTime), 0);
}

void receiverTimerCallback(TimerHandle_t xTimer)
{
	/* Create a boolean xTaskWoken to check if releasing the semaphore allowed the receiver task to be unblocked */
	static BaseType_t xTaskWoken = pdFALSE;
	/* Release the receiverSemaphore for the receiver task to continue execution */
	xSemaphoreGiveFromISR(Receiver_Signal, &xTaskWoken);

	//	Checking Received Counter if it got 500 mesgs or not
	if (Receivedcounter == 500)
	{
		NextStage();
	}
}


/* ***************** First Sender ******************** */
void Sender_1(void* p)
{

	while (1)
	{
		//		Waiting for its Interrupt to start actions ( Sending in Queue )
		xSemaphoreTake(Sender1_Signal, portMAX_DELAY);

		//		Storing system ticks in QueueMesg
		sprintf(QueueMesg, "Time is %d", xTaskGetTickCount());

		//			Attemping to send and checking if it's sent or not
		if (!xQueueSend(Main_Queue, QueueMesg, 0))
		{
			//printf("Sender 1 failed to access \n");
			Failedcounter++;
		}
		else
		{
			//printf("Sender 1 Succ \n");
			Successcounter++;
		}
	}
}

/* ***************** Second Sender ******************** */
void Sender_2(void* p)
{

	while (1)
	{
		xSemaphoreTake(Sender2_Signal, portMAX_DELAY);

		//		Storing system ticks in QueueMesg
		sprintf(QueueMesg, "Time is %d", xTaskGetTickCount());

		//			Attemping to send and checking if it's sent or not

		if (!xQueueSend(Main_Queue, QueueMesg, 0))
		{
			//printf("Sender 2 failed to access \n");
			Failedcounter++;
		}
		else
		{
			//printf("Sender 2 Succ \n");
			Successcounter++;
		}
	}
}

/* ***************** Receiver ******************** */
void Receiver(void* p)
{
	char ReceivedMesg[Char_Size];

	while (1)
	{
		//		Waiting to use the Semaphore till it gets unblocked
		xSemaphoreTake(Receiver_Signal, portMAX_DELAY);
		//Checking if we have received the mesg Successfully or it was empty
		if (xQueueReceive(Main_Queue, ReceivedMesg, 0)) // Queue name - add value to - waiting
		{

			//printf("Received \n");

			Receivedcounter++;
		}
		else
		{
			//printf("Empty queue \n");
		}

	}
}


int main(int argc, char* argv[])
{
	BaseType_t STATUS;

	/* ***************** Creating Binary Semaphore ******************** */
	vSemaphoreCreateBinary(Sender1_Signal);
	vSemaphoreCreateBinary(Sender2_Signal);
	vSemaphoreCreateBinary(Receiver_Signal);

	//	Initializing Queue + Checking
	Main_Queue = xQueueCreate(2, sizeof(char[Char_Size]));
	if (Main_Queue == NULL)
	{
		printf("Queue could not be created!");
		exit(0);
	}

	/* ***************** Tasks ******************** */
//	Creating Task1 + Checking

	STATUS = xTaskCreate(Sender_1, (signed char*)"S1", 2048, NULL, 1, NULL);
	if (STATUS != pdPASS)
	{
		printf("Sender1 task could not be created!");
		exit(0);
	}
	//	Creating Task1 + Checking

	STATUS = xTaskCreate(Sender_2, (signed char*)"S2", 2048, NULL, 1, NULL);
	if (STATUS != pdPASS)
	{
		printf("Sender2 task could not be created!");
		exit(0);
	}

	//	Create Receiver + Checking

	STATUS = xTaskCreate(Receiver, (signed char*)"R", 2048, NULL, 1, NULL);
	if (STATUS != pdPASS)
	{
		printf("Receiver task could not be created!");
		exit(0);
	}

	/* ***************** CallBackFunctions ******************** */

	sender1Timer = xTimerCreate("First Sender", pdMS_TO_TICKS(uniform_distribution(Tsender1[ArrayIndex], Tsender2[ArrayIndex])), pdTRUE, 0, senderoneTimerCallback);
	if (sender1Timer == NULL)
	{
		printf("Sender one timer could not be created!");
		exit(0);
	}

	sender2Timer = xTimerCreate("Second Sender", pdMS_TO_TICKS(uniform_distribution(Tsender1[ArrayIndex], Tsender2[ArrayIndex])), pdTRUE, 0, sendertwoTimerCallback);
	if (sender2Timer == NULL)
	{
		printf("Sender two timer could not be created!");
		exit(0);
	}

	receiverTimer = xTimerCreate("Receiver", pdMS_TO_TICKS(Treceiver), pdTRUE, 0, receiverTimerCallback);
	if (receiverTimer == NULL)
	{
		printf("Receiver timer could not be created!");
		exit(0);
	}

	STATUS = xTimerStart(sender1Timer, 0);
	if (STATUS != pdPASS)
	{
		printf("Receiver timer could not be started!");
		exit(0);
	}
	STATUS = xTimerStart(sender2Timer, 0);
	if (STATUS != pdPASS)
	{
		printf("Receiver timer could not be started!");
		exit(0);
	}
	STATUS = xTimerStart(receiverTimer, 0);
	if (STATUS != pdPASS)
	{
		printf("Receiver timer could not be started!");
		exit(0);
	}



	/* ***************** Scheduler ******************** */
	vTaskStartScheduler();
	return 0;

}




#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------

void vApplicationMallocFailedHook(void)
{
	/* Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
	for (;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook(TaskHandle_t pxTask, char* pcTaskName)
{
	(void)pcTaskName;
	(void)pxTask;

	/* Run time stack overflow checking is performed if
	configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	for (;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook(void)
{
	volatile size_t xFreeStackSpace;

	/* This function is called on each cycle of the idle task.  In this case it
	does nothing useful, other than report the amout of FreeRTOS heap that
	remains unallocated. */
	xFreeStackSpace = xPortGetFreeHeapSize();

	if (xFreeStackSpace > 100)
	{
		/* By now, the kernel has allocated everything it is going to, so
		if there is a lot of heap remaining unallocated then
		the value of configTOTAL_HEAP_SIZE in FreeRTOSConfig.h can be
		reduced accordingly. */
	}
}

void vApplicationTickHook(void) {
}

StaticTask_t xIdleTaskTCB CCM_RAM;
StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE] CCM_RAM;

void vApplicationGetIdleTaskMemory(StaticTask_t** ppxIdleTaskTCBBuffer, StackType_t** ppxIdleTaskStackBuffer, uint32_t* pulIdleTaskStackSize) {
	/* Pass out a pointer to the StaticTask_t structure in which the Idle task's
	state will be stored. */
	*ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

	/* Pass out the array that will be used as the Idle task's stack. */
	*ppxIdleTaskStackBuffer = uxIdleTaskStack;

	/* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
	Note that, as the array is necessarily of type StackType_t,
	configMINIMAL_STACK_SIZE is specified in words, not bytes. */
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

static StaticTask_t xTimerTaskTCB CCM_RAM;
static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH] CCM_RAM;

/* configUSE_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
application must provide an implementation of vApplicationGetTimerTaskMemory()
to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory(StaticTask_t** ppxTimerTaskTCBBuffer, StackType_t** ppxTimerTaskStackBuffer, uint32_t* pulTimerTaskStackSize) {
	*ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
	*ppxTimerTaskStackBuffer = uxTimerTaskStack;
	*pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

