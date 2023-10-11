#include "button.h"
#include <stdlib.h>
#include "main.h"
#include "string.h"

static ButtonInstance_t    *button_arr;
static uint8_t              button_amount;
static ButtonComboStatus_t  button_combo;

void button_initial(uint8_t amount, ButtonStatus_Handler bs_handler, ButtonEvent_Handler evt_handler, ButtonComboCnt_Handler combo_cnt_handler){

  // Allocate memory for an array of ButtonInstance_t structures
  button_arr    = malloc(sizeof(ButtonInstance_t) * amount);
  button_amount = amount;

  // Allocate memory for 'combo' data structures
  button_combo.co  = malloc(sizeof(Combo_t)                * amount);
  button_combo.re  = malloc(sizeof(ButtonPressTypeCombo_t) * amount);

  // Initialize the button handlers
  for(int i = 0; i < button_amount; i++){
    button_arr[i].status_handler    = bs_handler;
    button_arr[i].event_handler     = evt_handler;
  }

  button_combo.combo_cnt_handler = combo_cnt_handler;
}


// Check if the last button combo event has triggered.
static EventTrigger_t Is_last_button_combo_event_trigger(){

  EventTrigger_t ret = EVENT_NOT_TRIGGERED;

  for(int i = 0; i < button_amount; i++){    
    if(button_combo.co[i] != COMBO_NULL){      
      if(button_combo.re[i] == COMBO_BUTTON_NOT_PRESS){
        ret = EVENT_NOT_TRIGGERED;
        break;        
      }else if(button_combo.re[i] != COMBO_BUTTON_NOT_PRESS){
        ret = EVENT_TRIGGERED;
      }
    }
  }
  return ret;
}

void button_process(){

  for(int i = 0; i < button_amount; i++){
    button_arr[i].status_handler(i, &button_arr[i].status);

    if(button_arr[i].status == BUTTON_STATUS_PRESSED){  //   -------------------------- [PRESSED] -----------------------  

      if(button_arr[i].detect.phase == BUTTON_DET_PHASE_0){

        button_arr[i].detect.phase = BUTTON_DET_PHASE_1;    // First Press
        button_arr[i].detect.cnt1 = 0;

        // Combo xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx        
        // Initial detect
        if(button_combo.detect_state == COMBO_DETECT_READY){        
          button_combo.detect_state = COMBO_DETECT_INPROGRESS;
        }

        // co (null) -> (v)
        if(button_combo.detect_state == COMBO_DETECT_INPROGRESS){          
          if(button_combo.co[i] == COMBO_NULL){
            button_combo.co[i] = COMBO_V;
          }
        }
        // xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

      }else if(button_arr[i].detect.phase == BUTTON_DET_PHASE_1){

        if(button_arr[i].detect.cnt1 >= LONG_PRESS_DETECT_INTERVAL){

          // LONG PRESS (C)
          button_arr[i].press_type   = BUTTON_LONG_PRESS;
          button_arr[i].detect.phase = BUTTON_DET_PHASE_5;          

          // Combo [Event]=================================
          if(button_combo.co[i] == COMBO_V){
            button_combo.re[i] = COMBO_BUTTON_LONG_PRESS_C;
          }

          if( Is_last_button_combo_event_trigger() == EVENT_TRIGGERED ){

            // DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD
            // Check Long Press (C) Event > 1 and the corresponding 'co' = COMBO_V            
            uint8_t long_press_evt_cnt = 0;

            for(int i = 0; i < button_amount; i++){
              if(button_combo.re[i] == COMBO_BUTTON_LONG_PRESS_C){
                if(button_combo.co[i] == COMBO_V){
                  long_press_evt_cnt++;
                }
              }
            }

            Combo_t combo[7] = {0};

            if(long_press_evt_cnt > 1){

              for(int i = 0; i < button_amount; i++){
                if(button_combo.re[i] == COMBO_BUTTON_LONG_PRESS_C){
                  combo[i] = COMBO_V;
                }
              }
              button_combo.re[i] = COMBO_BUTTON_LONG_PRESS_D;
              button_arr[i].event_handler(COMBO_BUTTON_LONG_PRESS_D, combo);

            }else{             
              combo[i] = COMBO_V;
              button_arr[i].event_handler(COMBO_BUTTON_LONG_PRESS_C, combo);
            }
            // DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD

            // 'combo_holding_cnt' is set to the 'Cnt1' associated with the last triggering event.
            button_combo.holding_cnt = button_arr[i].detect.cnt1;
          }
          // ==============================================
        }
 
      }else if(button_arr[i].detect.phase == BUTTON_DET_PHASE_2){

        if( button_arr[i].detect.cnt2 < CNT2_INTERVAL_MAX ){
          button_arr[i].detect.phase = BUTTON_DET_PHASE_3;
          button_arr[i].detect.cnt3 = 0;
        }
      }

      if(button_arr[i].detect.phase == BUTTON_DET_PHASE_3){
        
        if (button_arr[i].detect.cnt3 >= LONG_PRESS_DETECT_INTERVAL){

          // LONG PRESS (C)
          button_arr[i].press_type   = BUTTON_LONG_PRESS;
          button_arr[i].detect.phase = BUTTON_DET_PHASE_5;

          // Combo [Event]=================================
          if(button_combo.co[i] == COMBO_V){
            button_combo.re[i] = COMBO_BUTTON_LONG_PRESS_C;
          }

          if( Is_last_button_combo_event_trigger() == EVENT_TRIGGERED ){

            // DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD
            // Check Long Press (C) Event > 1 and the corresponding 'co' = COMBO_V            
            uint8_t long_press_evt_cnt = 0;

            for(int i = 0; i < button_amount; i++){
              if(button_combo.re[i] == COMBO_BUTTON_LONG_PRESS_C){
                if(button_combo.co[i] == COMBO_V){
                  long_press_evt_cnt++;
                }
              }
            }

            Combo_t combo[7] = {0};

            if(long_press_evt_cnt > 1){

              for(int i = 0; i < button_amount; i++){
                if(button_combo.re[i] == COMBO_BUTTON_LONG_PRESS_C){
                  combo[i] = COMBO_V;
                }
              }
              button_combo.re[i] = COMBO_BUTTON_LONG_PRESS_D;
              button_arr[i].event_handler(COMBO_BUTTON_LONG_PRESS_D, combo);

            }else{
              combo[i] = COMBO_V;
              button_arr[i].event_handler(COMBO_BUTTON_LONG_PRESS_C, combo);
            }
            // DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD

            // 'combo_holding_cnt' is set to the 'Cnt1' associated with the last triggering event.
            button_combo.holding_cnt = button_arr[i].detect.cnt1;
          }
          // ==============================================

        } else if(button_arr[i].detect.cnt3 < UINT16_MAX){
          button_arr[i].detect.cnt3++;
        }
      }

      button_arr[i].detect.cnt1++;

    }else if(button_arr[i].status == BUTTON_STATUS_RELEASE){  //-------------------------- [RELEASE] -----------------------  

      // Combo xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
      // After Long Press(C) Event triggering, the 'co' value is maintained (v) as long as the button remains continuously pressed. Releasing it transitions the state (x).
      if(button_combo.re[i] == COMBO_BUTTON_LONG_PRESS_C || button_combo.re[i] == COMBO_BUTTON_LONG_PRESS_D){

        button_combo.co[i] = COMBO_X;
        memset(button_combo.re, COMBO_BUTTON_NOT_PRESS, button_amount);
        button_combo.detect_state = COMBO_DETECT_PENDING;
      }

      // xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

      if(button_arr[i].detect.phase == BUTTON_DET_PHASE_1){ // Button First Release
        button_arr[i].detect.phase = BUTTON_DET_PHASE_2;      
        button_arr[i].detect.cnt2  = 0;  
      }

      if(button_arr[i].detect.phase == BUTTON_DET_PHASE_2){

        if (button_arr[i].detect.cnt2 >= CNT2_INTERVAL_MAX){

          // Single Click (A)
          button_arr[i].press_type   = BUTTON_CLICK_SINGLE;
          button_arr[i].detect.phase = BUTTON_DET_PHASE_0;

          // AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
          // Combo [Event]=================================
          if(button_combo.co[i] == COMBO_V){
            button_combo.co[i] = COMBO_X;
            button_combo.re[i] = COMBO_BUTTON_CLICK_SINGLE_A;
          }

          if( Is_last_button_combo_event_trigger() == EVENT_TRIGGERED ){

            Combo_t combo[7] = {0};

            for(int i = 0; i < button_amount; i++){
              if(button_combo.re[i] == COMBO_BUTTON_CLICK_SINGLE_A){
                combo[i] = COMBO_V;
              }
            }
            button_arr[i].event_handler(COMBO_BUTTON_CLICK_SINGLE_A, combo);
            button_combo.detect_state = COMBO_DETECT_PENDING;
          }
          // ==============================================
          // AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA 

        }else if(button_arr[i].detect.cnt2 < UINT16_MAX){
          button_arr[i].detect.cnt2++;
        }
      }

      if(button_arr[i].detect.phase == BUTTON_DET_PHASE_3){

        if(button_arr[i].detect.cnt3 < CNT3_INTERVAL_MAX){

          // Double Click (B)
          button_arr[i].press_type   = BUTTON_CLICK_DOUBLE;
          button_arr[i].detect.phase = BUTTON_DET_PHASE_0;

          // BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB
          // Combo [Event]=================================
          if(button_combo.co[i] == COMBO_V){
            button_combo.co[i] = COMBO_X;
            button_combo.re[i] = COMBO_BUTTON_CLICK_DOUBLE_B;
          }

          if( Is_last_button_combo_event_trigger() == EVENT_TRIGGERED ){

            Combo_t combo[7] = {0};

            for(int i = 0; i < button_amount; i++){
              if(button_combo.re[i] == COMBO_BUTTON_CLICK_DOUBLE_B){
                combo[i] = COMBO_V;
              }            
            }
            button_arr[i].event_handler(COMBO_BUTTON_CLICK_DOUBLE_B, combo);          
            button_combo.detect_state = COMBO_DETECT_PENDING;
          }
          // ==============================================
          // BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB

        }else if(button_arr[i].detect.cnt3 >= CNT3_INTERVAL_MAX && button_arr[i].detect.cnt3 < LONG_PRESS_DETECT_INTERVAL){

          // Single Click (A)
          button_arr[i].press_type   = BUTTON_CLICK_SINGLE;
          button_arr[i].detect.phase = BUTTON_DET_PHASE_0;

          // AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
          // Combo [Event]=================================
          if(button_combo.co[i] == COMBO_V){
            button_combo.co[i] = COMBO_X;
            button_combo.re[i] = COMBO_BUTTON_CLICK_SINGLE_A;
          }

          if( Is_last_button_combo_event_trigger() == EVENT_TRIGGERED ){

            Combo_t combo[7] = {0};

            for(int i = 0; i < button_amount; i++){
              if(button_combo.re[i] == COMBO_BUTTON_CLICK_SINGLE_A){
                combo[i] = COMBO_V;
              }            
            }
            button_arr[i].event_handler(COMBO_BUTTON_CLICK_SINGLE_A, combo);          
            button_combo.detect_state = COMBO_DETECT_PENDING;

          }
          // ==============================================
          // AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
        }
      }

      if(button_arr[i].detect.phase == BUTTON_DET_PHASE_5){
        button_arr[i].detect.phase  = BUTTON_DET_PHASE_0;
      }

      button_arr[i].detect.cnt1 = 0;
    }
  }

  // Detect when all buttons have been released. ===============
  uint8_t all_release = 0;

  for(int i = 0; i < button_amount; i++){

    if(button_arr[i].status == BUTTON_STATUS_PRESSED){
      all_release = 0;
      break;
    }else if(button_arr[i].status == BUTTON_STATUS_RELEASE){
      all_release = 1;
    }
  }
  // ============================================================

  if(all_release == 1){

    // Reset and initialize the button combo status.
    if(button_combo.detect_state == COMBO_DETECT_PENDING){
      
      button_combo.detect_cnt  = 0;
      button_combo.holding_cnt = 0;
      
      memset(button_combo.co, COMBO_NULL            , button_amount);
      memset(button_combo.re, COMBO_BUTTON_NOT_PRESS, button_amount);
      
      button_combo.detect_state = COMBO_DETECT_READY;
    }

  }

  // Increment the 'combo_detect_cnt' if it has not reached the interval limit.
  if(button_combo.detect_state == COMBO_DETECT_INPROGRESS){

    if(button_combo.detect_cnt >= COMBO_DETECT_INTERVAL){
      button_combo.detect_state = COMBO_DETECT_FINISHED;
    }else{
      button_combo.detect_cnt++;
    }
  }

  // Continuously count until the release of the first Long Press(C) event.
  for(int i = 0; i < button_amount; i++){
    if(button_combo.re[i] == COMBO_BUTTON_LONG_PRESS_C || button_combo.re[i] == COMBO_BUTTON_LONG_PRESS_D ){   
      if(button_combo.holding_cnt < UINT16_MAX){
        button_combo.holding_cnt++;
      }
      break;
    }
  }

  // Call the 'combo_cnt_handler' function with the 'combo_detect_cnt' as an argument.
  button_combo.combo_cnt_handler(button_combo.holding_cnt);
  // ------------------------------------------------------------------------------------------------

}
