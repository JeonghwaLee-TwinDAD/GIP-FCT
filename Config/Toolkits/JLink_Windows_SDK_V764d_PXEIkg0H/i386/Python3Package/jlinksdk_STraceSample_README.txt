README for the jlinksdk_STraceSample:

Requirements:
  1) The STRACE sample script expects the Start_STM32F407_Trace_Example.hex file in the same directory as itself.
  2) A Probe with trace capabilities is required (J-Trace Pro V2 or higher) for the sample to work.
  3) The script is designed to be used with SEGGER's Cortex-M Trace Reference Board V1.2 (STM32F407VE).

Instructions:
  1) Make sure that all requirements above are fulfilled.
  2) Connect the J-Trace Pro to your PC.
  3) Power the Cortex-M Trace Reference Board V1.2 via the Mini-USB port.
  4) Connect the J-Trace with the Cortex-M Trace Reference Board V1.2, using the trace ribbon cable shipped with the J-Trace PRO.
  5) Start the sample script.
  
Additional information:
  1) https://wiki.segger.com/Tracing_on_ST_STM32F407_(SEGGER_Cortex-M_Trace_Reference_Board)
  2) For additional information, please refer to the Sample.