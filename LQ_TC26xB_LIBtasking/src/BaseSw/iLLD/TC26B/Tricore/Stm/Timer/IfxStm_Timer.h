/**
 * \file IfxStm_Timer.h
 * \brief STM TIMER details
 * \ingroup IfxLld_Stm
 *
 * \version iLLD_1_0_1_11_0
 * \copyright Copyright (c) 2018 Infineon Technologies AG. All rights reserved.
 *
 *
 *                                 IMPORTANT NOTICE
 *
 *
 * Use of this file is subject to the terms of use agreed between (i) you or 
 * the company in which ordinary course of business you are acting and (ii) 
 * Infineon Technologies AG or its licensees. If and as long as no such 
 * terms of use are agreed, use of this file is subject to following:


 * Boost Software License - Version 1.0 - August 17th, 2003

 * Permission is hereby granted, free of charge, to any person or 
 * organization obtaining a copy of the software and accompanying 
 * documentation covered by this license (the "Software") to use, reproduce,
 * display, distribute, execute, and transmit the Software, and to prepare
 * derivative works of the Software, and to permit third-parties to whom the 
 * Software is furnished to do so, all subject to the following:

 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer, must
 * be included in all copies of the Software, in whole or in part, and all
 * derivative works of the Software, unless such copies or derivative works are
 * solely in the form of machine-executable object code generated by a source
 * language processor.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE 
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.

 *
 * The timer driver enabled the generation of periodical interrupt based on the STM module.
 *
 *    The driver is initialized with \ref IfxStm_Timer_init(). After initialization, the
 *    timer is started with \ref IfxStm_Timer_run() and stopped with \ref IfxStm_Timer_stop().
 *    Single shot can be set with \ref IfxStm_Timer_setSingleMode(), in case the timer is
 *    already running it will stop after the next event, else the timer event will occur only once.
 *
 *    The timer interrupt must call the \ref IfxStm_Timer_acknowledgeTimerIrq() function which
 *    clears the interrupt flag and set the next compare value.
 *
 *    Changing the period with \ref IfxStm_Timer_setPeriod() or \ref IfxStm_Timer_setFrequency() will
 *    take effect only after the next timer event.
 *
 *
 *    This driver implements a subset of the functionalities defined by \ref library_srvsw_stdif_timer.
 *    If does supports the timer with interrupt but not the trigger signal.
 *
 *    The user is free to use either the driver specific APIs below or to used the \ref library_srvsw_stdif_timer "standard interface APIs".
 *
 *  \section Specific Specific implementation
 *   For a detailed configuration of the microcontroller, see \ref IfxStm_Timer_init().
 *
 *  \section Example Usage example
 *    Initialization and interrupt:
 *  \code
 *  #define ISR_PRIORITY_TIMER_1MS (2)
 *  #define ISR_PROVIDER_TIMER_1MS IfxSrc_Tos_cpu0
 *  #define INTERRUPT_TIMER_1MS    ISR_ASSIGN(ISR_PRIORITY_TIMER_1MS, ISR_PROVIDER_TIMER_1MS)
 *
 *  IfxStm_Timer myTimer;
 *
 *  boolean AppInit_1ms(void)
 *  {
 *      boolean                  result = TRUE;
 *      IfxStm_Timer_Config timerConfig;
 *      IfxStm_Timer_initConfig(&timerConfig, &MODULE_STM0);
 *      timerConfig.base.frequency       = 1000;
 *      timerConfig.base.isrPriority     = ISR_PRIORITY(INTERRUPT_TIMER_1MS);
 *      timerConfig.base.isrProvider     = ISR_PROVIDER(INTERRUPT_TIMER_1MS);
 *      timerConfig.base.minResolution   = (1.0 / timerConfig.base.frequency) / 1000;
 *      timerConfig.comparator           = IfxStm_Comparator_0;
 *      result                           = IfxStm_Timer_init(&myTimer, &timerConfig);
 *
 *      return result;
 *  }
 *
 *  IFX_INTERRUPT(ISR_TIMER_1ms, 0, ISR_PRIORITY_TIMER_1MS)
 *  {
 *      __enable();
 *      IfxStm_Timer_acknowledgeTimerIrq(&myTimer);
 *  }
 *  \endcode
 *
 *    During run-time, the timer can be started / stopped as follow:
 *  \code
 *      IfxStm_Timer_run(&myTimer);
 *      IfxStm_Timer_stop(&myTimer);
 *  \endcode
 *
 * \defgroup IfxLld_Stm_Timer STM Timer Interface
 * \ingroup IfxLld_Stm
 * \defgroup IfxLld_Stm_Timer_STMTimerDataStructures STM Timer Data Structures
 * \ingroup IfxLld_Stm_Timer
 * \defgroup IfxLld_Stm_Timer_globalfunction global function
 * \ingroup IfxLld_Stm_Timer
 */

#ifndef IFXSTM_TIMER_H
#define IFXSTM_TIMER_H 1

/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/

#include "Stm/Std/IfxStm.h"
#include "StdIf/IfxStdIf_Timer.h"

/******************************************************************************/
/*-----------------------------Data Structures--------------------------------*/
/******************************************************************************/

/** \addtogroup IfxLld_Stm_Timer_STMTimerDataStructures
 * \{ */
/** \brief Structure for the timer base
 */
typedef struct
{
    Ifx_TimerValue          period;               /**< \brief Timer period in ticks (cached value) */
    boolean                 triggerEnabled;       /**< \brief If TRUE, the trigger functionality is Initialized */
    float32                 clockFreq;            /**< \brief Timer input clock frequency (cached value) */
    IfxStdIf_Timer_CountDir countDir;             /**< \brief Timer counting mode */
    boolean                 singleShot;           /**< \brief If TRUE, the timer will stop after 1st event */
} IfxStm_Timer_Base;

/** \} */

/** \addtogroup IfxLld_Stm_Timer_STMTimerDataStructures
 * \{ */
/** \brief Timer interface
 */
typedef struct
{
    IfxStm_Timer_Base base;                  /**< \brief Timer interface */
    Ifx_STM          *stm;                   /**< \brief STM module used for the timer functionality */
    IfxStm_Comparator comparator;            /**< \brief Comparator used for the timer functionality */
    uint32            comparatorValue;       /**< \brief Value of the comparator for the next event */
    uint8             comparatorShift;       /**< \brief Comparator shift */
} IfxStm_Timer;

/** \brief configuration structure for Timer
 */
typedef struct
{
    IfxStdIf_Timer_Config base;             /**< \brief Standard interface timer configuration */
    Ifx_STM              *stm;              /**< \brief STM module used for the timer functionality */
    IfxStm_Comparator     comparator;       /**< \brief Comparator used for the timer functionality */
} IfxStm_Timer_Config;

/** \} */

/** \addtogroup IfxLld_Stm_Timer_globalfunction
 * \{ */

/******************************************************************************/
/*-------------------------Global Function Prototypes-------------------------*/
/******************************************************************************/

/** \brief Returns the timer event
 * see IfxStdIf_Timer_AckTimerIrq
 * \param driver STM Timer interface Handle
 * \return Timer interrupt event
 */
IFX_EXTERN boolean IfxStm_Timer_acknowledgeTimerIrq(IfxStm_Timer *driver);

/** \brief Returns the frequency
 * see IfxStdIf_Timer_GetFrequency
 * \param driver STM Timer interface Handle
 * \return Frequency
 */
IFX_EXTERN float32 IfxStm_Timer_getFrequency(IfxStm_Timer *driver);

/** \brief Returns the input frequency
 * see IfxStdIf_Timer_GetInputFrequency
 * \param driver STM Timer interface Handle
 * \return Frequency
 */
IFX_EXTERN float32 IfxStm_Timer_getInputFrequency(IfxStm_Timer *driver);

/** \brief Returns the period of the timer
 * see IfxStdIf_Timer_GetPeriod
 * \param driver STM Timer interface Handle
 * \return Period
 */
IFX_EXTERN Ifx_TimerValue IfxStm_Timer_getPeriod(IfxStm_Timer *driver);

/** \brief Return the resolution of the Timer counters
 * see IfxStdIf_Timer_GetResolution
 * \param driver STM Timer interface handle
 * \return Resolution
 */
IFX_EXTERN float32 IfxStm_Timer_getResolution(IfxStm_Timer *driver);

/** \brief Runs the timer
 * see IfxStdIf_Timer_Run
 * \param driver STM Timer interface Handle
 * \return None
 */
IFX_EXTERN void IfxStm_Timer_run(IfxStm_Timer *driver);

/** \brief Sets the Frequency
 * see IfxStdIf_Timer_SetFrequency
 * \param driver STM Timer interface Handle
 * \param frequency frequency
 * \return TRUE on success else FALSE
 */
IFX_EXTERN boolean IfxStm_Timer_setFrequency(IfxStm_Timer *driver, float32 frequency);

/** \brief Sets the period for the timer
 * see IfxStdIf_Timer_SetPeriod
 * \param driver STM Timer interface handle
 * \param period Period value
 * \return TRUE on success else FALSE
 */
IFX_EXTERN boolean IfxStm_Timer_setPeriod(IfxStm_Timer *driver, Ifx_TimerValue period);

/** \brief Sets the single shot mode of the timer
 * see IfxStdIf_Timer_SetSingleMode
 * \param driver STM Timer interface Handle
 * \param enabled If TRUE, sets the single shot mode
 * \return None
 */
IFX_EXTERN void IfxStm_Timer_setSingleMode(IfxStm_Timer *driver, boolean enabled);

/** \brief Initializes the standard interface timer
 * \param stdif Standard interface object, will be initialized by the function
 * \param driver driver Interface driver to be used by the standard interface. must be initialised separately
 * \return TRUE on success else FALSE
 */
IFX_EXTERN boolean IfxStm_Timer_stdIfTimerInit(IfxStdIf_Timer *stdif, IfxStm_Timer *driver);

/** \brief Stops the timer
 * see IfxStdIf_Timer_Stop
 * \param driver STM Timer interface Handle
 * \return None
 */
IFX_EXTERN void IfxStm_Timer_stop(IfxStm_Timer *driver);

/** \brief Updates the input frequency
 * see IfxStdIf_Timer_UpdateInputFrequency
 * \param driver STM Timer interface Handle
 * \return None
 */
IFX_EXTERN void IfxStm_Timer_updateInputFrequency(IfxStm_Timer *driver);

/** \brief Initialises the timer object
 * \param driver STM Timer interface Handle
 * \param config Configuration structure for STM Timer
 * \return TRUE on success else FALSE
 */
IFX_EXTERN boolean IfxStm_Timer_init(IfxStm_Timer *driver, const IfxStm_Timer_Config *config);

/** \brief Initializes the configuration structure to default
 * \param config Configuration structure for STM Timer
 * \param stm Pointer to STM module
 * \return None
 */
IFX_EXTERN void IfxStm_Timer_initConfig(IfxStm_Timer_Config *config, Ifx_STM *stm);

/** \} */

#endif /* IFXSTM_TIMER_H */
