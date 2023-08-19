**Project Summary: Real-Time Operating System (RTOS) Implementation for Communication Tasks**

**Description:**
- Developed an RTOS-based system using FreeRTOS on an embedded emulation board within Eclipse CDT.
- Designed sender and receiver tasks with synchronized communication using fixed-size queues.
- Utilized timers and semaphores for efficient task coordination.

**Sender Tasks:**
- Implemented concurrent sender tasks with randomized sleep periods (Tsender).
- Sent messages to the queue upon waking, managing full queue scenarios.
- Tracked successful transmissions and blocked messages.

**Receiver Task:**
- Configured receiver task with fixed sleep period (Treceiver).
- Retrieved messages from the queue and managed counters.
- Supported single message retrieval from the queue.

**Timer Callback Functions:**
- Employed sender and receiver timer callbacks for task synchronization.
- Utilized semaphores to unblock tasks at appropriate intervals.
- Triggered "Reset" process upon receiving 500 messages.

**Reset Function:**
- Created reset function to reconfigure system parameters.
- Reset counters, cleared queue, and updated sender timer period (Tsender).
- Halted execution and displayed "Game Over" when all predefined timer values were used.

**Data Analysis:**
- Conducted performance analysis with data visualization.
- Plotted total sent messages vs. average sender timer period.
- Analyzed message count disparities and blocked messages vs. average Tsender values.

**Queue Size Variation:**
- Explored system behavior with different queue sizes (2 and 20).
- Investigated impact of queue size on system performance.

**Efficient Random Number Generation:**
- Utilized efficient random number generation for sleep period determination.

**Skills Demonstrated:**
- Real-Time Operating System (RTOS) design and implementation using FreeRTOS.
- Task synchronization and communication using queues and semaphores.
- Timer-based task scheduling and control mechanisms.
- Embedded system development with Eclipse CDT.
- Data analysis and visualization using plotting techniques.

**Conclusion:**
- Successfully executed an RTOS-based communication system using FreeRTOS.
- Demonstrated expertise in embedded systems, synchronization, and performance analysis.

**Flow Chart**
  
   ![image](https://github.com/MoustafaAhmed01/RTOS_Project/assets/104157153/3533ba46-3299-41ad-8669-e8204dd6e6a7)

