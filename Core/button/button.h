#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>

#define BUTTON_AMOUNT_1  1U
#define BUTTON_AMOUNT_2  2U
#define BUTTON_AMOUNT_3  3U
#define BUTTON_AMOUNT_4  4U
#define BUTTON_AMOUNT_5  5U
#define BUTTON_AMOUNT_6  6U
#define BUTTON_AMOUNT_7  7U
#define BUTTON_AMOUNT_8  8U


#define COMBO_DETECT_INTERVAL      200  // Unit(ms)
#define LONG_PRESS_DETECT_INTERVAL 500  // Unit(ms)
#define CNT2_INTERVAL_MAX          160  // Unit(ms)
#define CNT3_INTERVAL_MAX          160  // Unit(ms)

// 
typedef enum{

  BUTTON_STATUS_PRESSED = 0x0,
  BUTTON_STATUS_RELEASE = 0x1

}ButtonStatusDefinition_t;

typedef enum{

  BUTTON_DET_PHASE_0 = 0x0,
  BUTTON_DET_PHASE_1 = 0x1,
  BUTTON_DET_PHASE_2 = 0x2,
  BUTTON_DET_PHASE_3 = 0x3,
  BUTTON_DET_PHASE_4 = 0x4,
  BUTTON_DET_PHASE_5 = 0x5

}ButtonDetectionPhase_t;

typedef enum{

  BUTTON_NOT_PRESS,
  BUTTON_CLICK_SINGLE,
  BUTTON_CLICK_DOUBLE,
  BUTTON_LONG_PRESS,

}ButtonPressType_t;


typedef enum{

  EVENT_NOT_TRIGGERED,
  EVENT_TRIGGERED,

}EventTrigger_t;

typedef enum{

  COMBO_NULL,
  COMBO_X,
  COMBO_V,

}Combo_t;

typedef enum{

  COMBO_BUTTON_NOT_PRESS,
  COMBO_BUTTON_CLICK_SINGLE_A,
  COMBO_BUTTON_CLICK_DOUBLE_B,
  COMBO_BUTTON_LONG_PRESS_C,
  COMBO_BUTTON_LONG_PRESS_D,

}ButtonPressTypeCombo_t;

typedef enum{

  COMBO_DETECT_PENDING,
  COMBO_DETECT_READY,       // combo_detect_cnt = 0
  COMBO_DETECT_INPROGRESS,  // combo_detect_cnt < COMBO_DETECT_INTERVAL  combo_detect_cnt++
  COMBO_DETECT_FINISHED,    // combo_detect_cnt >= COMBO_DETECT_INTERVAL

}ComboDetectionState_t;


typedef void(*ButtonStatus_Handler)(uint8_t gpio_num, ButtonStatusDefinition_t *btn_status);
typedef void(*ButtonEvent_Handler)(ButtonPressTypeCombo_t btn_combo_press_type, Combo_t combo[7]);
typedef void(*ButtonComboCnt_Handler)(uint16_t combo_cnt);


#pragma pack(push)   // preserves the current alignment setting.
#pragma pack(1)      // directive aligns structure members tightly in sequential order without padding, conserving memory by avoiding unnecessary gaps.

typedef struct{

  Combo_t                 *co;
  ButtonPressTypeCombo_t  *re;

  uint16_t                detect_cnt;
  uint16_t                holding_cnt;
  ComboDetectionState_t   detect_state;
  ButtonComboCnt_Handler  combo_cnt_handler;

}ButtonComboStatus_t;

typedef struct{

  struct{
    uint16_t               cnt1;
    uint16_t               cnt2;
    uint16_t               cnt3;
    ButtonDetectionPhase_t phase;
  }detect;

  ButtonStatus_Handler      status_handler;
  ButtonStatusDefinition_t  status;
  ButtonPressType_t         press_type;
  ButtonEvent_Handler       event_handler;

}ButtonInstance_t;


#pragma pack(pop)   //restores the original alignment setting.

void button_initial(uint8_t amount, ButtonStatus_Handler bs_handler, ButtonEvent_Handler evt_handler, ButtonComboCnt_Handler combo_cnt_handler);
void button_process(void);


#endif // BUTTON_H
