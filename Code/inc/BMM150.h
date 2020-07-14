/******************************************************Library******************************************************/

#include "stm32f1xx.h"

/******************************************************Use Function*************************************************/

void BMM150_Init(void);



uint8_t BMM150_Read8(uint8_t reg);
int8_t BMM150_Read8int(uint8_t reg);
uint8_t BMM150_Write8(uint8_t reg, uint8_t value);


/******************************************************Use defines*************************************************/
/**\name CHIP ID & SOFT RESET VALUES      */
#define BMM150_CHIP_ID                       UINT8_C(0x32)
#define BMM150_SET_SOFT_RESET                UINT8_C(0x82)

/**\name POWER MODE DEFINTIONS      */
#define BMM150_NORMAL_MODE                   UINT8_C(0x00)
#define BMM150_FORCED_MODE                   UINT8_C(0x01)
#define BMM150_SLEEP_MODE                    UINT8_C(0x03)
#define BMM150_SUSPEND_MODE                  UINT8_C(0x04)

/**\name I2C ADDRESS       */
#define BMM150_DEFAULT_I2C_ADDRESS           UINT8_C(0x10)
#define BMM150_I2C_ADDRESS_CSB_LOW_SDO_HIGH  UINT8_C(0x11)
#define BMM150_I2C_ADDRESS_CSB_HIGH_SDO_LOW  UINT8_C(0x12)
#define BMM150_I2C_ADDRESS_CSB_HIGH_SDO_HIGH UINT8_C(0x13)

/**\name Power mode settings  */
#define BMM150_POWER_CNTRL_DISABLE           UINT8_C(0x00)
#define BMM150_POWER_CNTRL_ENABLE            UINT8_C(0x01)

/**\name Sensor delay time settings  */
#define BMM150_SOFT_RESET_DELAY              UINT8_C(1)
#define BMM150_NORMAL_SELF_TEST_DELAY        UINT8_C(2)
#define BMM150_START_UP_TIME                 UINT8_C(3)
#define BMM150_ADV_SELF_TEST_DELAY           UINT8_C(4)

/**\name ENABLE/DISABLE DEFINITIONS  */
#define BMM150_XY_CHANNEL_ENABLE             UINT8_C(0x00)
#define BMM150_XY_CHANNEL_DISABLE            UINT8_C(0x03)

/**\name Register Address */
#define BMM150_CHIP_ID_ADDR                  UINT8_C(0x40)
#define BMM150_DATA_X_LSB                    UINT8_C(0x42)
#define BMM150_DATA_READY_STATUS             UINT8_C(0x48)
#define BMM150_INTERRUPT_STATUS              UINT8_C(0x4A)
#define BMM150_POWER_CONTROL_ADDR            UINT8_C(0x4B)
#define BMM150_OP_MODE_ADDR                  UINT8_C(0x4C)
#define BMM150_INT_CONFIG_ADDR               UINT8_C(0x4D)
#define BMM150_AXES_ENABLE_ADDR              UINT8_C(0x4E)
#define BMM150_LOW_THRESHOLD_ADDR            UINT8_C(0x4F)
#define BMM150_HIGH_THRESHOLD_ADDR           UINT8_C(0x50)
#define BMM150_REP_XY_ADDR                   UINT8_C(0x51)
#define BMM150_REP_Z_ADDR                    UINT8_C(0x52)
