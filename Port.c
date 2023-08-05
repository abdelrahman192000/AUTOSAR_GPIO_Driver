 /******************************************************************************
 *
 * Module: Port
 *
 * File Name: Port.c
 *
 * Description: Source file for TM4C123GH6PM Microcontroller - Port Driver.
 *
 * Author: Abdelrahman Adel
 ******************************************************************************/

#include "Port.h"
#include "Port_Regs.h"

#if (PORT_DEV_ERROR_DETECT == STD_ON)

#include "Det.h"

/* AUTOSAR Version checking between Det and Port Modules */
#if ((DET_AR_MAJOR_VERSION != PORT_AR_RELEASE_MAJOR_VERSION)\
|| (DET_AR_MINOR_VERSION != PORT_AR_RELEASE_MINOR_VERSION)\
  || (DET_AR_PATCH_VERSION != PORT_AR_RELEASE_PATCH_VERSION))
#error "The AR version of Det.h does not match the expected version"
#endif
   
#endif
   
/*******************************************************************************
                          Local Variables                                    
*******************************************************************************/
   
/*Contains the status of the port*/   
uint8 port_status = PORT_NOT_INITIALIZED;

/*Contains the pointer to which points to Port_PinConfig */
const Port_ConfigType* Port_ConfigPtr = NULL_PTR;

/*******************************************************************************
                          Function Definitions                                   
*******************************************************************************/

/*******************************************************************************
* Service Name: Port_Init
* Service ID[hex]: 0x00
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): ConfigPtr - Pointer to post-build configuration data
* Parameters (inout): None
* Parameters (out): None
* Return value: None
* Description: Function to Setup the pin configuration:
*              - Setup the pin as Digital GPIO pin
*              - Setup the direction of the GPIO pin
*              - Setup the internal resistor for i/p pin
*******************************************************************************/
void Port_Init(const Port_ConfigType * ConfigPtr )
{
#if (PORT_DEV_ERROR_DETECT == STD_ON)
  /* check if the input configuration pointer is not a NULL_PTR */
  if(ConfigPtr == NULL_PTR)
  {
    Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_INIT_SID, PORT_E_PARAM_CONFIG);
  }
  else
  {	
    /* Do Nothing */
  }
#endif
  
  port_status = PORT_INITIALIZED;
  Port_ConfigPtr = ConfigPtr;
  
  for(Port_PinType pin = 0; pin < PORT_NUMBER_OF_PINS; pin++)
  {
    
    volatile uint32 * PortGpio_Ptr = NULL_PTR; /* point to the required Port Registers base address */
    volatile uint32 delay = 0;
    
    switch(Port_ConfigPtr->Pin[pin].port_num)
    {
    case  0: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTA_BASE_ADDRESS; /* PORTA Base Address */
    break;
    case  1: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTB_BASE_ADDRESS; /* PORTB Base Address */
    break;
    case  2: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTC_BASE_ADDRESS; /* PORTC Base Address */
    break;
    case  3: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTD_BASE_ADDRESS; /* PORTD Base Address */
    break;
    case  4: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTE_BASE_ADDRESS; /* PORTE Base Address */
    break;
    case  5: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTF_BASE_ADDRESS; /* PORTF Base Address */
    break;
    }
    
    /* Enable clock for PORT and allow time for clock to start*/
    SYSCTL_REGCGC2_REG |= (1<<Port_ConfigPtr->Pin[pin].port_num);
    delay = SYSCTL_REGCGC2_REG;
    
    if( ((Port_ConfigPtr->Pin[pin].port_num == 3) && (Port_ConfigPtr->Pin[pin].pin_num == 7)) || ((Port_ConfigPtr->Pin[pin].port_num == 5) && (Port_ConfigPtr->Pin[pin].pin_num == 0)) ) /* PD7 or PF0 */
    {
      /* Unlock the GPIOCR register */
      *(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_LOCK_REG_OFFSET) = 0x4C4F434B;
      
      /* Set the corresponding bit in GPIOCR register to allow changes on this pin */
      SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_COMMIT_REG_OFFSET) , Port_ConfigPtr->Pin[pin].pin_num);
    }
    else if( (Port_ConfigPtr->Pin[pin].port_num == 2) && (Port_ConfigPtr->Pin[pin].pin_num <= 3)) /* PC0 to PC3 */
    {
      /* Do Nothing ...  this is the JTAG pins */
      continue;
    }
    else
    {
      /* Do Nothing ... No need to unlock the commit register for this pin */
    }
    
    if(Port_ConfigPtr->Pin[pin].direction == PORT_PIN_OUT)
      {
        /* Set the corresponding bit in the GPIODIR register to configure it as output pin */
        SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIR_REG_OFFSET) , Port_ConfigPtr->Pin[pin].pin_num);
        
        if(Port_ConfigPtr->Pin[pin].initial_value == PORT_PIN_LEVEL_HIGH)
        {
          /* Set the corresponding bit in the GPIODATA register to provide initial value 1 */
          SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DATA_REG_OFFSET) , Port_ConfigPtr->Pin[pin].pin_num);
        }
        else
        {
          /* Clear the corresponding bit in the GPIODATA register to provide initial value 0 */
          CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DATA_REG_OFFSET) , Port_ConfigPtr->Pin[pin].pin_num);
        }
      }
      else if(Port_ConfigPtr->Pin[pin].direction == PORT_PIN_IN)
      {
        /* Clear the corresponding bit in the GPIODIR register to configure it as input pin */
        CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIR_REG_OFFSET) , Port_ConfigPtr->Pin[pin].pin_num);
        
        if(Port_ConfigPtr->Pin[pin].resistor == PULL_UP)
        {
          /* Set the corresponding bit in the GPIOPUR register to enable the internal pull up pin */
          SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_PULL_UP_REG_OFFSET) , Port_ConfigPtr->Pin[pin].pin_num);
        }
        else if(Port_ConfigPtr->Pin[pin].resistor == PULL_DOWN)
        {
          /* Set the corresponding bit in the GPIOPDR register to enable the internal pull down pin */
          SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_PULL_DOWN_REG_OFFSET) , Port_ConfigPtr->Pin[pin].pin_num);
        }
        else
        {
          /* Clear the corresponding bit in the GPIOPUR register to disable the internal pull up pin */
          CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_PULL_UP_REG_OFFSET) , Port_ConfigPtr->Pin[pin].pin_num);
          
          /* Clear the corresponding bit in the GPIOPDR register to disable the internal pull down pin */
          CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_PULL_DOWN_REG_OFFSET) , Port_ConfigPtr->Pin[pin].pin_num);
        }
      }
      else
      {
        /* Do Nothing */
      }
    
    if (Port_ConfigPtr->Pin[pin].initial_mode == PORT_PIN_MODE_DIO)
    {
      /* Clear the corresponding bit in the GPIOAMSEL register to disable analog functionality on this pin */
      CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ANALOG_MODE_SEL_REG_OFFSET) , Port_ConfigPtr->Pin[pin].pin_num);
      
      /* Disable Alternative function for this pin by clear the corresponding bit in GPIOAFSEL register */
      CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ALT_FUNC_REG_OFFSET) , Port_ConfigPtr->Pin[pin].pin_num);
      
      /* Clear the PMCx bits for this pin */
      *(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_CTL_REG_OFFSET) &= ~(0x0000000F << (Port_ConfigPtr->Pin[pin].pin_num * 4));
            
      
      /* Set the corresponding bit in the GPIODEN register to enable digital functionality on this pin */
      SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIGITAL_ENABLE_REG_OFFSET) , Port_ConfigPtr->Pin[pin].pin_num);
    }
    else if (Port_ConfigPtr->Pin[pin].initial_mode == PORT_PIN_MODE_ADC)
    {
      /* Clear the corresponding bit in the GPIODEN register to disable digital functionality on this pin */
      CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIGITAL_ENABLE_REG_OFFSET) , Port_ConfigPtr->Pin[pin].pin_num);
      
      /* Enable Alternative function for this pin by setting the corresponding bit in GPIOAFSEL register */
      SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ALT_FUNC_REG_OFFSET) , Port_ConfigPtr->Pin[pin].pin_num);
      
      /* Clear the PMCx bits for this pin */
      *(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_CTL_REG_OFFSET) &= ~(0x0000000F << (Port_ConfigPtr->Pin[pin].pin_num * 4));
      
      /* Set the corresponding bit in the GPIOAMSEL register to enable analog functionality on this pin */
      SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ANALOG_MODE_SEL_REG_OFFSET) , Port_ConfigPtr->Pin[pin].pin_num);
    }
    else /* Another mode */
    {
      /* Clear the corresponding bit in the GPIOAMSEL register to disable analog functionality on this pin */
      CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ANALOG_MODE_SEL_REG_OFFSET) , Port_ConfigPtr->Pin[pin].pin_num);
      
      /* Enable Alternative function for this pin by clear the corresponding bit in GPIOAFSEL register */
      SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ALT_FUNC_REG_OFFSET) , Port_ConfigPtr->Pin[pin].pin_num);
      
      /* Set the PMCx bits for this pin */
      *(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_CTL_REG_OFFSET) |= (Port_ConfigPtr->Pin[pin].initial_mode & 0x0000000F << (Port_ConfigPtr->Pin[pin].pin_num * 4));
      
      /* Set the corresponding bit in the GPIODEN register to enable digital functionality on this pin */
      SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIGITAL_ENABLE_REG_OFFSET) , Port_ConfigPtr->Pin[pin].pin_num);
    }
  }
}

/************************************************************************************
* Service Name: Port_SetPinDirection
* Service ID[hex]: 0x01
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): Pin - Port Pin id number
Direction - Port Pin direction
* Parameters (inout): None
* Parameters (out): None
* Return value: None
* Description: Function sets the port pin direction
************************************************************************************/
#if (PORT_SET_PIN_DIRECTION_API == STD_ON)
void Port_SetPinDirection(Port_PinType Pin, Port_PinDirectionType Direction)
{
#if (PORT_DEV_ERROR_DETECT == STD_ON)
  /* Check if the Driver is initialized before using this function */
  if(port_status == PORT_NOT_INITIALIZED)
  {
    Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_SET_PIN_DIR_SID, PORT_E_UNINIT);
  }
  else
  {
    /* Do Nothing */
  }
  
  /* check if incorrect Port Pin ID passed */
  if(Pin >= PORT_NUMBER_OF_PINS)
  {
    Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_SET_PIN_DIR_SID, PORT_E_PARAM_PIN);
  }
  else
  {
    /* Do Nothing */
  }
  
  /* check if Port Pin not configured as changeable */
  if(Port_ConfigPtr->Pin[Pin].pin_direction_change == STD_OFF)
  {
    Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_SET_PIN_DIR_SID, PORT_E_DIRECTION_UNCHANGEABLE);
  }
  else
  {
    /* Do Nothing */
  }
#endif
  
  volatile uint32 * PortGpio_Ptr = NULL_PTR; /* point to the required Port Registers base address */
  
  switch(Port_ConfigPtr->Pin[Pin].port_num)
  {
  case  0: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTA_BASE_ADDRESS; /* PORTA Base Address */
  break;
  case  1: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTB_BASE_ADDRESS; /* PORTB Base Address */
  break; 
  case  2: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTC_BASE_ADDRESS; /* PORTC Base Address */
  break;
  case  3: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTD_BASE_ADDRESS; /* PORTD Base Address */
  break; 
  case  4: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTE_BASE_ADDRESS; /* PORTE Base Address */
  break;
  case  5: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTF_BASE_ADDRESS; /* PORTF Base Address */
  break;
  }
  
  if( (Port_ConfigPtr->Pin[Pin].port_num == 2) && (Port_ConfigPtr->Pin[Pin].pin_num <= 3)) /* PC0 to PC3 */
  {
    /* Do Nothing ...  this is the JTAG pins */
    return;
  }
  
  else if(Direction == PORT_PIN_OUT)
  {
    /* Set the corresponding bit in the GPIODIR register to configure it as output pin */
    SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIR_REG_OFFSET) , Port_ConfigPtr->Pin[Pin].pin_num);
  }
  else if(Direction == PORT_PIN_IN)
  {
    /* Clear the corresponding bit in the GPIODIR register to configure it as input pin */
    CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIR_REG_OFFSET) , Port_ConfigPtr->Pin[Pin].pin_num);
  }
  else
  {
    /* Do Nothing */
  }
}
#endif

/************************************************************************************
* Service Name: Port_RefreshPortDirection
* Service ID[hex]: 0x02
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): None
* Parameters (inout): None
* Parameters (out): None
* Return value: None
* Description: Function refreshes port direction
************************************************************************************/
void Port_RefreshPortDirection(void)
{
#if (PORT_DEV_ERROR_DETECT == STD_ON)
  /* Check if the Driver is initialized before using this function */
  if(port_status == PORT_NOT_INITIALIZED)
  {
    Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_REFRESH_PIN_DIR_SID, PORT_E_UNINIT);
  }
  else
  {
    /* Do Nothing */
  }
#endif
  
  for(Port_PinType pin = 0; pin < PORT_NUMBER_OF_PINS; pin++)
  {
    volatile uint32 * PortGpio_Ptr = NULL_PTR; /* point to the required Port Registers base address */
    switch(Port_ConfigPtr->Pin[pin].port_num)
    {
    case  0: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTA_BASE_ADDRESS; /* PORTA Base Address */
    break;
    case  1: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTB_BASE_ADDRESS; /* PORTB Base Address */
    break;
    case  2: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTC_BASE_ADDRESS; /* PORTC Base Address */
    break;
    case  3: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTD_BASE_ADDRESS; /* PORTD Base Address */
    break;
    case  4: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTE_BASE_ADDRESS; /* PORTE Base Address */
    break;
    case  5: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTF_BASE_ADDRESS; /* PORTF Base Address */
    break;
    }
    if( (Port_ConfigPtr->Pin[pin].port_num == 2) && (Port_ConfigPtr->Pin[pin].pin_num <= 3)) /* PC0 to PC3 */
    {
      /* Do Nothing ...  this is the JTAG pins */
      continue;
    }
    /* Checking if the pin direction is changeable during the run time */
    if (Port_ConfigPtr->Pin[pin].pin_direction_change == STD_OFF)
    {
      if(Port_ConfigPtr->Pin[pin].direction == PORT_PIN_OUT)
      {
        /* Set the corresponding bit in the GPIODIR register to configure it as output pin */
        SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIR_REG_OFFSET) , Port_ConfigPtr->Pin[pin].pin_num);
      }
      else if(Port_ConfigPtr->Pin[pin].direction == PORT_PIN_IN)
      {
        /* Clear the corresponding bit in the GPIODIR register to configure it as input pin */
        CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIR_REG_OFFSET) , Port_ConfigPtr->Pin[pin].pin_num);
      }
      else
      {
	/* Do Nothing */
      }
    }
    else
    {
      /* Do Nothing */
    }
  }
}

/************************************************************************************
* Service Name: Port_GetVersionInfo
* Service ID[hex]: 0x03
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): None
* Parameters (inout): None
* Parameters (out): versioninfo - Pointer to where to store the version information of this module
* Return value: None
* Description: Function to get the version info of the GPIO pin
************************************************************************************/
#if (PORT_VERSION_INFO_API == STD_ON)
void Port_GetVersionInfo(Std_VersionInfoType *versioninfo )
{
#if (PORT_DEV_ERROR_DETECT == STD_ON)
  /* check if the input configuration pointer is not a NULL_PTR */
  if(versioninfo == NULL_PTR)
  {
    Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_GET_VERSION_INFO_SID, PORT_E_PARAM_POINTER);
  }
  else
  {
    /* Do Nothing */
  }
  
  /* Check if the Driver is initialized before using this function */
  if(port_status == PORT_NOT_INITIALIZED)
  {
    Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_GET_VERSION_INFO_SID, PORT_E_UNINIT);
  }
  else
  {
    /* Do Nothing */
  }
#endif
  
  /* Get the module Id */
  versioninfo->moduleID = (uint16)PORT_MODULE_ID;
  /* Get the vendor Id */
  versioninfo->vendorID = (uint16)PORT_VENDOR_ID;
  /* Get the Software Major Version */
  versioninfo->sw_major_version = (uint8)PORT_SW_MAJOR_VERSION;
  /* Get the Software Minor Version */
  versioninfo->sw_minor_version = (uint8)PORT_SW_MINOR_VERSION;
  /* Get the Software Patch Version */
  versioninfo->sw_patch_version = (uint8)PORT_SW_PATCH_VERSION;
}
#endif

/************************************************************************************
* Service Name: Port_SetPinMode
* Service ID[hex]: 0x04
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): Pin - Port Pin id number
Mode - New Port Pin mode to be set on port pin
* Parameters (inout): None
* Parameters (out): None
* Return value: None
* Description: Function to get the version info of the GPIO pin
************************************************************************************/
#if (PORT_SET_PIN_MODE_API == STD_ON)
void Port_SetPinMode(Port_PinType Pin,Port_PinModeType Mode)
{
#if (PORT_DEV_ERROR_DETECT == STD_ON)
  /* Check if the Driver is initialized before using this function */
  if(port_status == PORT_NOT_INITIALIZED)
  {
    Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_SET_PIN_MODE_SID, PORT_E_UNINIT);
  }
  else
  {
    /* Do Nothing */
  }
  
  /* Check if incorrect Port Pin ID passed */
  if(Pin >= PORT_NUMBER_OF_PINS)
  {
    Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_SET_PIN_MODE_SID, PORT_E_PARAM_PIN);
  }
  else
  {
    /* Do Nothing */
  }
  
  /* Check if the Port Pin Mode valid or not*/
  if(Mode > PORT_PIN_MODE_ADC)
  {
    Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_SET_PIN_MODE_SID, PORT_E_PARAM_INVALID_MODE);
  }
  else
  {
    /* Do Nothing */
  }
  
  /* check if the API called when the mode is unchangeable */
  if(Port_ConfigPtr->Pin[Pin].pin_mode_change == STD_OFF)
  {
    Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_SET_PIN_MODE_SID, PORT_E_MODE_UNCHANGEABLE);
  }
  else
  {
    /* Do Nothing */
  }
#endif
  
  volatile uint32 * PortGpio_Ptr = NULL_PTR; /* point to the required Port Registers base address */
  
  switch(Port_ConfigPtr->Pin[Pin].port_num)
  {
  case  0: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTA_BASE_ADDRESS; /* PORTA Base Address */
  break;
  case  1: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTB_BASE_ADDRESS; /* PORTB Base Address */
  break;
  case  2: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTC_BASE_ADDRESS; /* PORTC Base Address */
  break;
  case  3: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTD_BASE_ADDRESS; /* PORTD Base Address */
  break;
  case  4: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTE_BASE_ADDRESS; /* PORTE Base Address */
  break;
  case  5: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTF_BASE_ADDRESS; /* PORTF Base Address */
  break;
  }
  
  if( (Port_ConfigPtr->Pin[Pin].port_num == 2) && (Port_ConfigPtr->Pin[Pin].pin_num <= 3) ) /* PC0 to PC3 */
  {
    /* Do Nothing ...  this is the JTAG pins */
    return;
  }
  
  if (Mode == PORT_PIN_MODE_DIO)
  {
    /* Clear the corresponding bit in the GPIOAMSEL register to disable analog functionality on this pin */
    CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ANALOG_MODE_SEL_REG_OFFSET) , Port_ConfigPtr->Pin[Pin].pin_num);
    
    /* Disable Alternative function for this pin by clear the corresponding bit in GPIOAFSEL register */
    CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ALT_FUNC_REG_OFFSET) , Port_ConfigPtr->Pin[Pin].pin_num);
    
    /* Clear the PMCx bits for this pin */
    *(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_CTL_REG_OFFSET) &= ~(0x0000000F << (Port_ConfigPtr->Pin[Pin].pin_num * 4));
    
    /* Set the corresponding bit in the GPIODEN register to enable digital functionality on this pin */
    SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIGITAL_ENABLE_REG_OFFSET) , Port_ConfigPtr->Pin[Pin].pin_num);
  }
  else if (Mode == PORT_PIN_MODE_ADC)
  {
    /* Clear the corresponding bit in the GPIODEN register to disable digital functionality on this pin */
    CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIGITAL_ENABLE_REG_OFFSET) , Port_ConfigPtr->Pin[Pin].pin_num);
    
    /* Enable Alternative function for this pin by setting the corresponding bit in GPIOAFSEL register */
    SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ALT_FUNC_REG_OFFSET) , Port_ConfigPtr->Pin[Pin].pin_num);
    
    /* Clear the PMCx bits for this pin */
    *(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_CTL_REG_OFFSET) &= ~(0x0000000F << (Port_ConfigPtr->Pin[Pin].pin_num * 4));
    
    /* Set the corresponding bit in the GPIOAMSEL register to enable analog functionality on this pin */
    SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ANALOG_MODE_SEL_REG_OFFSET) , Port_ConfigPtr->Pin[Pin].pin_num);
  }
  else /* Another mode */
  {
    /* Clear the corresponding bit in the GPIOAMSEL register to disable analog functionality on this pin */
    CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ANALOG_MODE_SEL_REG_OFFSET) , Port_ConfigPtr->Pin[Pin].pin_num);
    
    /* Enable Alternative function for this pin by clear the corresponding bit in GPIOAFSEL register */
    SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ALT_FUNC_REG_OFFSET) , Port_ConfigPtr->Pin[Pin].pin_num);
    
    /* Set the PMCx bits for this pin */
    *(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_CTL_REG_OFFSET) |= (Mode & 0x0000000F << (Port_ConfigPtr->Pin[Pin].pin_num * 4));
    
    /* Set the corresponding bit in the GPIODEN register to enable digital functionality on this pin */
    SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIGITAL_ENABLE_REG_OFFSET) , Port_ConfigPtr->Pin[Pin].pin_num);
  }
}
#endif