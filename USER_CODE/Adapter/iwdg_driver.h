#ifndef IWDG_DRIVER_H
#define IWDG_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif


void HW_IWDG_Init(void);
void IWDG_Start(void);
void IWDG_Reload(void);

#ifdef __cplusplus
}
#endif

#endif
