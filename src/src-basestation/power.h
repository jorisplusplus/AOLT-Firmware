#ifndef BSP_POWER_H
#define BSP_POWER_H

/** Type defintions *********************************/
typedef enum
{
    POWER_SOURCE_USB,
    POWER_SOURCE_MAINS
} POWER_SOURCE;


/*********************************************************************
* Function: POWER_SOURCE POWER_SourceGet(void)
*
* Overview: Gets the current source of power for the board
*
* PreCondition: None
*
* Input: None
*
* Output: POWER_SOURCE - the current source of power for the board
*
********************************************************************/
POWER_SOURCE POWER_SourceGet(void);
#define POWER_SourceGet() POWER_SOURCE_MAINS

#endif //BSP_POWER_H
