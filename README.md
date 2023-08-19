# RTOS_Project
Developed an RTOS system using FreeRTOS for sender/receiver tasks with synchronized communication and timers. Analyzed performance trends, showcasing expertise in RTOS, synchronization, and data analysis.
Certainly, here's a summary of the RTOS project and the key points you can include on your CV:

**Project: Real-Time Operating System (RTOS) Implementation for Communication Tasks**

**Description:**
- Implemented an RTOS-based system on an embedded emulation board using FreeRTOS within Eclipse CDT environment.
- Designed a system with three tasks – two sender tasks and one receiver task – communicating through a fixed-size queue.
- Used timers to control task sleep/wake behavior and semaphore-based synchronization.

**Sender Tasks:**
- Two sender tasks operate concurrently.
- Each sender task sleeps for a random period, Tsender, drawn from a uniform distribution.
- Upon waking, sender tasks send a message containing the current system time to the queue.
- If the queue is full, the sending operation fails, and a blocked message counter is incremented.
- On successful sending, a transmitted message counter is incremented, and the sender task sleeps again.

**Receiver Task:**
- One receiver task sleeps for a fixed period, Treceiver.
- Upon waking, it checks the queue for received messages.
- If a message is present, it reads it, increments the received message counter, and sleeps again.
- The receiver reads one message at a time, even if there are more messages in the queue.

**Timer Callback Functions:**
- Sender Timer Callback Function releases a semaphore to unblock the sender task, allowing it to send to the queue.
- Receiver Timer Callback Function releases a semaphore to unblock the receiver task, which reads from the queue.
- When the receiver receives 500 messages, the callback function triggers a "Reset" procedure.

**Reset Function:**
- Resets counters for sent, blocked, and received messages.
- Clears the queue.
- Updates sender timer period, Tsender, based on predefined lower and upper bound arrays.
- If all values are used, stops execution and prints "Game Over."

**Data Analysis:**
- Plotted the number of total sent messages as a function of the average sender timer period.
- Analyzed the gap between the sent and received message counts during the running period.
- Plotted the number of blocked messages against the average value of Tsender.

**Queue Size Variation:**
- Conducted experiments with a queue of size 2 and size 20.
- Explored the impact of queue size on system behavior and performance.

**Efficient Random Number Generation:**
- Efficiently generated random numbers using a uniform distribution for determining sleep periods.

**Skills Demonstrated:**
- Real-Time Operating System (RTOS) design and implementation using FreeRTOS.
- Task synchronization and communication using queues and semaphores.
- Timer-based task scheduling and control.
- Embedded system development within Eclipse CDT.
- Data analysis and visualization using plotting.

**Conclusion:**
- Successfully implemented an RTOS-based communication system with sender and receiver tasks using FreeRTOS.
- Demonstrated proficiency in embedded system development, task synchronization, and performance analysis.
  
  **-Flow Chart**
  
   ![image](https://github.com/MoustafaAhmed01/RTOS_Project/assets/104157153/3533ba46-3299-41ad-8669-e8204dd6e6a7)

