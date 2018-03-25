/* system header files */
#include <stdio.h>
#include <BCDS_Basics.h>
/* additional interface header files */
#include "FreeRTOS.h"
#include "timers.h"
#include "BCDS_CmdProcessor.h"
#include "BCDS_Assert.h"
#include "BSP_BoardType.h"
#include "BCDS_BSP_LED.h"
#include "BCDS_LightSensor.h"
#include "BCDS_Magnetometer.h"

/* Interface for all sensors on the XDK */
#include "XdkSensorHandle.h"

/* Sensor Handler for the BMM150 Sensor */
extern Magnetometer_HandlePtr_T xdkMagnetometer_BMM150_Handle;

/* Sensor Handler for the BME280 Sensor */
extern Environmental_HandlePtr_T xdkEnvironmental_BME280_Handle;


/* Macro used to define blocktime of a timer */
#define TIMERBLOCKTIME UINT32_C(0xffff)
#define TIMER_AUTORELOAD_ON pdTRUE
#define SECONDS(x) ((portTickType) (x * 1000) / portTICK_RATE_MS)

unsigned int LightSensorData;
long int bme280_pressure,bme280_temperature,bme280_humidity;


void initEnviromentalSensor(void){
	Retcode_T environmentalInitReturnValue = RETCODE_FAILURE;
	Retcode_T returnOverSamplingValue = RETCODE_FAILURE;
	Retcode_T returnFilterValue = RETCODE_FAILURE;

	environmentalInitReturnValue = Environmental_init(xdkEnvironmental_BME280_Handle);

	if (RETCODE_OK != environmentalInitReturnValue) {
	 // do something
	}
	returnOverSamplingValue = Environmental_setOverSamplingPressure(xdkEnvironmental_BME280_Handle,ENVIRONMENTAL_BME280_OVERSAMP_2X);
	if (RETCODE_OK != returnOverSamplingValue) {
	 // do something
	}
	returnFilterValue =
	Environmental_setFilterCoefficient(xdkEnvironmental_BME280_Handle,ENVIRONMENTAL_BME280_FILTER_COEFF_2);
	if (RETCODE_OK != returnFilterValue) {
	 // do something
	}
}
void readEnviromentalSensor(xTimerHandle extimer){
	/* initialize magnetometer */
	BCDS_UNUSED(extimer);
	Environmental_Data_T bme280 = { INT32_C(0), UINT32_C(0), UINT32_C(0) };
	Retcode_T returnValue = RETCODE_FAILURE;

	returnValue = Environmental_readData(xdkEnvironmental_BME280_Handle, &bme280);
	if ( RETCODE_OK == returnValue) {
//	 printf("BME280 Environmental Conversion Data :\n\rp =%ld Pa\n\rt =%ld mDeg \n\rh =%ld %%rh\n\r",
//			 (long int) bme280.pressure,
//			 (long int) bme280.temperature,
//			 (long int) bme280.humidity);
		bme280_pressure 		=	(long int) bme280.pressure ;
		bme280_temperature 	= 	(long int) bme280.temperature;
		bme280_humidity 		= 	(long int) bme280.humidity;

	}

}


void initMagniteSensor(void){
	Retcode_T magnetometerInitReturnValue = RETCODE_FAILURE;
	Retcode_T returnDataRateValue = RETCODE_FAILURE;
	Retcode_T returnPresetModeValue = RETCODE_FAILURE;
	magnetometerInitReturnValue = Magnetometer_init(xdkMagnetometer_BMM150_Handle);
	if (RETCODE_OK != magnetometerInitReturnValue) {
	 // do something
	}
	returnDataRateValue = Magnetometer_setDataRate(xdkMagnetometer_BMM150_Handle,
	MAGNETOMETER_BMM150_DATARATE_20HZ);
	if (RETCODE_OK != returnDataRateValue) {
	 // do something
	}
	returnPresetModeValue = Magnetometer_setPresetMode(xdkMagnetometer_BMM150_Handle,MAGNETOMETER_BMM150_PRESETMODE_REGULAR);
	if (RETCODE_OK != returnPresetModeValue) {
		printf("Magnetometer initialization Failed\n\r");
	}
}

void readMagniteSensor(xTimerHandle mxtimer){
	/* initialize magnetometer */
	BCDS_UNUSED(mxtimer);
	Retcode_T returnValue = RETCODE_FAILURE;
	returnValue = Magnetometer_init(xdkMagnetometer_BMM150_Handle);
	if (RETCODE_OK != returnValue){
	 // do something
	}
	/* read and print BMM150 magnetometer data */
	Magnetometer_XyzData_T bmm150 = {INT32_C(0), INT32_C(0), INT32_C(0),
	INT32_C(0)};
	returnValue = Magnetometer_readXyzTeslaData(xdkMagnetometer_BMM150_Handle,
	&bmm150);
	if (RETCODE_OK == returnValue){
//	 printf("Data :\t%ld \t%ld \t%ld \t%ld\n\r",
//	 (long int) bmm150.xAxisData, (long int) bmm150.yAxisData, (long int)bmm150.zAxisData,(long int)LightSensorData);

		printf("XDK,%ld,%ld,%ld,%ld,%ld,%ld,%ld\r\n",
			 (long int) bmm150.xAxisData, (long int) bmm150.yAxisData, (long int)bmm150.zAxisData,(long int)LightSensorData,bme280_temperature,bme280_humidity,bme280_pressure);


	}
}



/* Include this code in XdkApplicationTemplate.c */
void initLightSensor(void){
 /* initialize light sensor */

LightSensorData = 0;
 Retcode_T returnValue = RETCODE_FAILURE;
 returnValue = LightSensor_init(xdkLightSensor_MAX44009_Handle);
 if ( RETCODE_OK != returnValue){
 printf("Light Sensor initialization Failed\n\r");
 }
}

/* Include this code in XdkApplicationTemplate.c */
void readLightSensor(xTimerHandle xTimer)
{
BCDS_UNUSED(xTimer);
 /* Read and print light sensor data */
 uint32_t milliLuxData = UINT32_C(0);
 Retcode_T returnValue = RETCODE_FAILURE;
 returnValue = LightSensor_readLuxData(
 xdkLightSensor_MAX44009_Handle,&milliLuxData);
 if (RETCODE_OK == returnValue)
 {
 //printf("Light sensor:%d \n\r",(unsigned int) milliLuxData);
 LightSensorData = (unsigned int) milliLuxData;
 }
}

static Retcode_T LedInitialize(void)
{
    Retcode_T returnVal = RETCODE_OK;
    returnVal = BSP_LED_Connect();
    if (RETCODE_OK == returnVal)
    {
        returnVal = BSP_LED_Enable((uint32_t) BSP_XDK_LED_R);
    }
    if (RETCODE_OK == returnVal)
    {
        returnVal = BSP_LED_Enable((uint32_t) BSP_XDK_LED_O);
    }
    if (RETCODE_OK == returnVal)
    {
        returnVal = BSP_LED_Enable((uint32_t) BSP_XDK_LED_Y);
    }
    if (RETCODE_OK == returnVal)
    {
        printf("LED Initialization succeed without error %u \n", (unsigned int) returnVal);
    }
    else
    {
        printf(" Error occurred in LED Initialization routine %u \n", (unsigned int) returnVal);
    }
    return returnVal;
}



/* Print string "Hello World" on the console */
void printHelloWorld(xTimerHandle pxTimer)
{
 BCDS_UNUSED(pxTimer);
// printf("Hello world\r\n");
 BSP_LED_Switch((uint32_t) BSP_XDK_LED_O, (uint32_t) BSP_LED_COMMAND_TOGGLE);
}





/* Application to print "hello world" on serial console. */
void applicationInit(void){
	 xTimerHandle applicationTimer, applicationTimer2, applicationTimer3,applicationTimer4;

 initLightSensor();
 initEnviromentalSensor();
 initEnviromentalSensor();
 LedInitialize();

 /* create timer task to print Hello World in the output console every three
 seconds */
 /* Validated for portMAX_DELAY to assist the task to wait Infinitely
(without timing out) and ticks cannot be 0 in FreeRTOS timer. So ticks is
assigned to 1 */

 applicationTimer = xTimerCreate((char * const) "Hello World",
 SECONDS(1),
 TIMER_AUTORELOAD_ON,
 NULL,
 printHelloWorld);

 applicationTimer2 = xTimerCreate((char * const) "readLight",
  SECONDS(0.5),
  TIMER_AUTORELOAD_ON,
  NULL,
  readLightSensor);

 applicationTimer3 = xTimerCreate((char * const) "Magnite",
   SECONDS(0.1),
   TIMER_AUTORELOAD_ON,
   NULL,
   readMagniteSensor);

 applicationTimer4 = xTimerCreate((char * const) "Enviroment",
    SECONDS(5),
    TIMER_AUTORELOAD_ON,
    NULL,
	readEnviromentalSensor);


 /*start the timer*/
 xTimerStart(applicationTimer, TIMERBLOCKTIME);
 xTimerStart(applicationTimer2, TIMERBLOCKTIME);
 xTimerStart(applicationTimer3, TIMERBLOCKTIME);
 xTimerStart(applicationTimer4, TIMERBLOCKTIME);

}
/* This is a template function where the user can write his/her custom
application */
void appInitSystem(void * CmdProcessorHandle, uint32_t param2){
 if (CmdProcessorHandle == NULL){
 printf("Command processor handle is null \n\r");
 assert(false);
 }
 BCDS_UNUSED(param2);
 applicationInit();
}
