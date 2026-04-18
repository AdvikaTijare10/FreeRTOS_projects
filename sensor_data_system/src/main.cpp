#include <Arduino.h>
#include<time.h>

TaskHandle_t sensor_task_handle=NULL;
TaskHandle_t processing_task_handle=NULL;
TaskHandle_t logging_task_handle=NULL;
TaskHandle_t fault_detection_task_handle=NULL;
xQueueHandle queue1;
xQueueHandle queue2;
xQueueHandle queue3;
SemaphoreHandle_t serialMutex;

void sensor_task(void *pvParameters){
xSemaphoreTake(serialMutex,portMAX_DELAY);
Serial.printf("Sensor task running...\n");
xSemaphoreGive(serialMutex);
int data=rand()%100;
uint32_t notifValue=0;
int faultMode=0;
while(1){
  if(xTaskNotifyWait(0,0,&notifValue,0)==pdPASS){ //waittime=0 meaning dont block while waiting for notif to come
    if(notifValue==1){
      faultMode=1;
      xSemaphoreTake(serialMutex,portMAX_DELAY);
      Serial.print("Sensor slowing down due to fault..\n");
      xSemaphoreGive(serialMutex);
    }
    else if(notifValue==0){
      faultMode=0;
      xSemaphoreTake(serialMutex,portMAX_DELAY);
      Serial.print("Sensor back to normal mode..\n");
      xSemaphoreGive(serialMutex);
    }
  }
if(xQueueSend(queue1,&data,0)==pdPASS){ // if queue is full, data will be dropped, 0 only if u r ok losing data, otherwise use portMAX_DELAY to block task until space is available in queue
  xSemaphoreTake(serialMutex,portMAX_DELAY);
  Serial.printf("Sensor task sent data: %d\n",data);
  xSemaphoreGive(serialMutex);
  data=rand()%100;
}
else{
  xSemaphoreTake(serialMutex,portMAX_DELAY);
  Serial.printf("Sensor task queue full!, data is dropped\n");
  xSemaphoreGive(serialMutex);
}
int defaultDelay=500;
if(faultMode==1)defaultDelay=2000;
vTaskDelay(pdMS_TO_TICKS(defaultDelay));
}}

void processing_task(void *pvParameters){
  xSemaphoreTake(serialMutex,portMAX_DELAY);
  Serial.printf("Processing task running...\n");
  xSemaphoreGive(serialMutex);
  int recievedData;
while(1){
  if(xQueueReceive(queue1,&recievedData,portMAX_DELAY)==pdPASS){
  xSemaphoreTake(serialMutex,portMAX_DELAY);
  Serial.printf("Processing task recieved data: %d\n",recievedData); 
  xSemaphoreGive(serialMutex);
  //processing of data here 
  if(xQueueSend(queue2,&recievedData,0)==pdPASS){  // processesing to log
    xSemaphoreTake(serialMutex,portMAX_DELAY);
    Serial.printf("Processing task sent data: %d\n",recievedData);
    xSemaphoreGive(serialMutex);
  }
  else{
    xSemaphoreTake(serialMutex,portMAX_DELAY);
    Serial.printf("Processing task queue full!, data is dropped\n");
    xSemaphoreGive(serialMutex);
  }
  if(recievedData>80 || recievedData<20){ // fault detection condition
  if(xQueueSend(queue3,&recievedData,portMAX_DELAY)==pdPASS){ // processing to fault detection, this data cant afford to lose
    xSemaphoreTake(serialMutex,portMAX_DELAY);
    Serial.printf("Processing task sent data to fault detection queue: %d\n",recievedData);
    xSemaphoreGive(serialMutex);
    xTaskNotify(sensor_task_handle,1,eSetValueWithOverwrite);
  }
  
  else {
    xSemaphoreTake(serialMutex,portMAX_DELAY);
    Serial.printf("Processing task queue full!\n");
    xSemaphoreGive(serialMutex);}
  } 
else{
  xTaskNotify(sensor_task_handle,0,eSetValueWithOverwrite);
}}  }
  //vTaskDelay(pdMS_TO_TICKS(1000));  since added portMAX_DELAY in xQueueReceive, task will be blocked until data is recieved, so no need to add delay here
}

void fault_detection_task(void *pvParameters){
int recievedData;
xSemaphoreTake(serialMutex,portMAX_DELAY);
Serial.printf("Fault detection task running...\n");
xSemaphoreGive(serialMutex);
while(1){
  if(xQueueReceive(queue3,&recievedData,portMAX_DELAY)==pdPASS){
if(recievedData>80){
  xSemaphoreTake(serialMutex,portMAX_DELAY);
  Serial.printf("Overheat detected! value: %d\n",recievedData);
  xSemaphoreGive(serialMutex);
}
if(recievedData<20){
  xSemaphoreTake(serialMutex,portMAX_DELAY);
  Serial.printf("Sensor fault! value detected: %d\n",recievedData);
  xSemaphoreGive(serialMutex);
}
  //este value field overwrites the notif value with 1
  }
}
}
void logging_task(void *pvParameters){
  xSemaphoreTake(serialMutex,portMAX_DELAY); //take mutex to ensure exclusive access to Serial for logging task
  Serial.printf("Logging task running...\n");
  xSemaphoreGive(serialMutex);
  int recievedData;
while(1){
  if(xQueueReceive(queue2,&recievedData,portMAX_DELAY)==pdPASS){
    xSemaphoreTake(serialMutex,portMAX_DELAY);
    Serial.printf("Logging task recieved data: %d\n",recievedData);
    xSemaphoreGive(serialMutex);
  }
  // else{
  //   Serial.printf("Logging task queue empty!, no data to log\n");
  // }  //not needed since u added portMAX_DELAY
//vTaskDelay(pdMS_TO_TICKS(2000)); since added portMAX_DELAY in xQueueReceive, task will be blocked until data is recieved, so no need to add delay here
}}


void setup(){
  srand(time(NULL));
  queue1=xQueueCreate(10,sizeof(int));
  queue2=xQueueCreate(10,sizeof(int));
  queue3=xQueueCreate(10,sizeof(int));
  if(queue1==NULL || queue2==NULL || queue3==NULL){
    Serial.printf("Queue creation failed!\n");
  } 
  Serial.begin(115200);
  serialMutex=xSemaphoreCreateMutex();
  xTaskCreate(sensor_task,"Sensor task",2048,NULL,2,&sensor_task_handle);
  xTaskCreate(processing_task,"Processing task",2048,NULL,2,&processing_task_handle);
  xTaskCreate(logging_task,"Logging task",2048,NULL,1,&logging_task_handle);
  xTaskCreate(fault_detection_task,"Fault detection task",2048,NULL,3,&fault_detection_task_handle);

}