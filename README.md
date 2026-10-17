# ⭐EE186-Lab2⭐

###### For future reference: Reference manual (https://www.st.com/resource/en/reference_manual/dm00310109-stm32l4-series-advanced-armbased-32bit-mcus-stmicroelectronics.pdf) 

## Part 2: Push Button

###### How do external interrupts work? GPIO peripheral detects the external event and a interrupt request is generated once the pending request is accepted based on its priority. The Interrupt Service Routine runs and after the handler completes the interrupts execution, the CPU continues with running the rest of the program. 

## Part 3: Timer

###### How do timer interrupts work? A timer keeps track of clock ticks and after a certain amount of time, an interrupt request is generated for the CPU to then execute ISR. The clock source generates a pulse usually at some sort of MHz frequency. No software function counts the time, instead a hardware counter does it. This is also true of the function that triggers the ISR. The main difference between a timer interrupt and external interrupt is that a timer interrupt's event is internal and does not require an external signal from a GPIO pin. 

###### Timer Frequency Calculation

###### Time = (PSC + 1)(ARR + 1) / ftimer  60 MHz
###### 1 = (PSC + 1)(ARR + 1) / 60,000,000 --> 60,000,000 = (PSC + 1)(ARR + 1) 
###### If PSC = 5,999, then PSC + 1 = 6,000
###### 60,000,000 = 6,000(ARR + 1) --> ARR + 1 = 60,000,000/6,000 = 10,000, ARR = 9,999


