 /******************************************************************************
 *
 * Module: Port
 *
 * File Name: Port.h
 *
 * Description: Header file for TM4C123GH6PM Microcontroller - Port Driver.
 *
 * Author: Abdelrahman Adel
 ******************************************************************************/

#ifndef PORT_H
#define PORT_H

#include "Common_Macros.h"
#include "Std_Types.h"
#include "Port_Cfg.h"

/* Id for the company in the AUTOSAR
 * for example Mohamed Tarek's ID = 1000 :) */
#define PORT_VENDOR_ID    (1000U)

/* Port Module Id */
#define PORT_MODULE_ID    (124U)

/* Port Instance Id */
#define PORT_INSTANCE_ID  (0U)

/*
 * Module Version 1.0.0
 */
#define PORT_SW_MAJOR_VERSION           (1U)
#define PORT_SW_MINOR_VERSION           (0U)
#define PORT_SW_PATCH_VERSION           (0U)

/*
 * AUTOSAR Version 4.0.3
 */
#define PORT_AR_RELEASE_MAJOR_VERSION   (4U)
#define PORT_AR_RELEASE_MINOR_VERSION   (0U)
#define PORT_AR_RELEASE_PATCH_VERSION   (3U)

/*
 * Macros for Port Status
 */
#define PORT_INITIALIZED                (1U)
#define PORT_NOT_INITIALIZED            (0U)

/* AUTOSAR checking between Std Types and Port Modules */
#if ((STD_TYPES_AR_RELEASE_MAJOR_VERSION != PORT_AR_RELEASE_MAJOR_VERSION)\
 ||  (STD_TYPES_AR_RELEASE_MINOR_VERSION != PORT_AR_RELEASE_MINOR_VERSION)\
 ||  (STD_TYPES_AR_RELEASE_PATCH_VERSION != PORT_AR_RELEASE_PATCH_VERSION))
  #error "The AR version of Std_Types.h does not match the expected version"
#endif

/* AUTOSAR Version checking between Port_Cfg.h and Dio.h files */
#if ((PORT_CFG_AR_RELEASE_MAJOR_VERSION != PORT_AR_RELEASE_MAJOR_VERSION)\
 ||  (PORT_CFG_AR_RELEASE_MINOR_VERSION != PORT_AR_RELEASE_MINOR_VERSION)\
 ||  (PORT_CFG_AR_RELEASE_PATCH_VERSION != PORT_AR_RELEASE_PATCH_VERSION))
  #error "The AR version of Port_Cfg.h does not match the expected version"
#endif

/* Software Version checking between Port_Cfg.h and Dio.h files */
#if ((PORT_CFG_SW_MAJOR_VERSION != PORT_SW_MAJOR_VERSION)\
 ||  (PORT_CFG_SW_MINOR_VERSION != PORT_SW_MINOR_VERSION)\
 ||  (PORT_CFG_SW_PATCH_VERSION != PORT_SW_PATCH_VERSION))
  #error "The SW version of Port_Cfg.h does not match the expected version"
#endif

/*******************************************************************************
                                DET Errors                       
*******************************************************************************/
/* Invalid Port ID request */
#define PORT_E_PARAM_PIN                 ((uint8)0x0A)

/*Port Pin not configured as changeable*/
#define PORT_E_DIRECTION_UNCHANGEABLE    ((uint8)0x0B)

/* API Port_Init service called with wrong parameters*/
#define PORT_E_PARAM_CONFIG              ((uint8)0x0C)

/* API Port_SetPinMode service called with invalid mode*/
#define PORT_E_PARAM_INVALID_MODE        ((uint8)0x0D)

/* API Port_SetPinMode service called, when mode is unchangeable*/
#define PORT_E_MODE_UNCHANGEABLE         ((uint8)0x0E)

/* API sevice called without module initialization*/
#define PORT_E_UNINIT                    ((uint8)0x0F)

/* APIs called with a NULL Pointer, Error shall be reported*/
#define PORT_E_PARAM_POINTER             ((uint8)0x10)
   
/*******************************************************************************
                              API Service Id 
*******************************************************************************/
/*API service ID for Port_Init function */
#define PORT_INIT_SID                    (uint8)0x00

/*API service ID for Port_SetPinDirection function*/
#define PORT_SET_PIN_DIR_SID         (uint8)0x01

/*API service ID for Port_RefreshPortDirection function*/
#define PORT_REFRESH_PIN_DIR_SID     (uint8)0x02

/*API service ID for Port_GetVersionInfo function*/
#define PORT_GET_VERSION_INFO_SID          (uint8)0x03

/*API service ID for Port_SetPinMode function*/
#define PORT_SET_PIN_MODE_SID              (uint8)0x04   

/*******************************************************************************
                              Module Data Types                              
*******************************************************************************/

/*Description: Data type for the symbolic name of a port pin*/
typedef uint8 Port_PinType;
   
/* Description: Enum to hold PIN direction */
typedef enum
{
    PORT_PIN_IN,PORT_PIN_OUT
}Port_PinDirectionType;

/* Description: Different port pin modes */
typedef uint8 Port_PinModeType;

/* Description: Port Pin Level value from Port pin list */
typedef enum
{
	PORT_PIN_LEVEL_LOW,PORT_PIN_LEVEL_HIGH
}Port_PinLevelValue;

/* Description: Enum to hold internal resistor type for PIN */
typedef enum
{
    OFF,PULL_UP,PULL_DOWN
}Port_InternalResistor;

typedef enum
{
  PORT_PIN_MODE_DIO         ,
  PORT_PIN_MODE_ALT1        ,
  PORT_PIN_MODE_ALT2        ,
  PORT_PIN_MODE_ALT3        ,
  PORT_PIN_MODE_ALT4        ,
  PORT_PIN_MODE_ALT5        ,
  PORT_PIN_MODE_ALT6        ,
  PORT_PIN_MODE_ALT7        ,
  PORT_PIN_MODE_ALT8        ,
  PORT_PIN_MODE_ALT9        ,
  PORT_PIN_MODE_ALT14 = 14  ,
  PORT_PIN_MODE_ADC
}Port_PinInitialMode;

/* Description: Structure to configure each individual PIN:
 *	1. the PORT Which the pin belongs to. 0, 1, 2, 3, 4 or 5
 *	2. the number of the pin in the PORT.
 *      3. the direction of pin --> INPUT or OUTPUT
 *      4. the internal resistor --> Disable, Pull up or Pull down
 *      5. the initial value of the port pin
 *      6. the initial port pin mode
 *      7. the changing of the pin direction
 *      8. the changing of the pin mode
 */
typedef struct 
{
    uint8 port_num; 
    uint8 pin_num; 
    Port_PinDirectionType direction;
    Port_InternalResistor resistor;
    Port_PinLevelValue initial_value;
    Port_PinInitialMode initial_mode;
    boolean pin_direction_change;
    boolean pin_mode_change;
}Port_ConfigPin;

typedef struct
{
  Port_ConfigPin Pin[PORT_NUMBER_OF_PINS];
}Port_ConfigType;  

/*******************************************************************************
                      Function Prototypes                                    
*******************************************************************************/


void Port_Init(const Port_ConfigType *ConfigPtr);


#if (PORT_SET_PIN_DIRECTION_API == STD_ON)
void Port_SetPinDirection(Port_PinType Pin,Port_PinDirectionType Direction);
#endif


void Port_RefreshPortDirection(void);


#if (PORT_VERSION_INFO_API == STD_ON)
void Port_GetVersionInfo(Std_VersionInfoType *Versioninfo );
#endif


#if (PORT_SET_PIN_MODE_API == STD_ON)
void Port_SetPinMode(Port_PinType Pin,Port_PinModeType Mode);
#endif

/*******************************************************************************
                         External Variables                                    
*******************************************************************************/

/* Post build structure used with Port_Init API */
extern const Port_ConfigType Port_Configuration;

#endif /* PORT_H */
