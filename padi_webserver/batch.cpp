
#include "batch.h"
#include "cmsis_os.h"

void batch_task(void const *arg) {
  while (1) {
    // rtl_printf("batch\n");
    osDelay(100);
    osThreadYield();
  }
}
