#include "button.h"
#include <stdlib.h>
#include "main.h"
#include "string.h"

ButtonInstance_t        *button_arr;
static uint8_t           button_amount;

ButtonComboStatus_t      button_combo;


void button_initial(uint8_t amount, ButtonStatus_Handler bs_handler, ButtonEvent_Handler evt_handler){
  
  button_arr    = malloc(sizeof(ButtonInstance_t) * amount);
  button_amount = amount;
  
  button_combo.co  = malloc(sizeof(Combo_t)                * amount);
  button_combo.re  = malloc(sizeof(ButtonPressTypeCombo_t) * amount);
	
  for(int i = 0; i < button_amount; i++){
    button_arr[i].status_handler = bs_handler;
    button_arr[i].event_handler  = evt_handler;
  }
  
}

uint8_t all_release = 0;


void button_process(){
   
  
  for(int i = 0; i < button_amount; i++){
    
    button_arr[i].status_handler(i, &button_arr[i].status);
    
    if(button_arr[i].status == BUTTON_STATUS_PRESSED){
      
      if(button_arr[i].detect.phase == BUTTON_DET_PHASE_0){
        
        button_arr[i].detect.phase = BUTTON_DET_PHASE_1;    // First Press
        button_arr[i].detect.cnt1 = 0;
        
      }else if(button_arr[i].detect.phase == BUTTON_DET_PHASE_1){
        
        if(button_arr[i].detect.cnt1 >= LONG_PRESS_DETECT_INTERVAL){
          
          // LONG PRESS (C)
          button_arr[i].press_type   = BUTTON_LONG_PRESS;
          button_arr[i].detect.phase = BUTTON_DET_PHASE_5;
          
          button_arr[i].event_handler(i, &button_arr[i].press_type, &button_combo);
          __ASM("NOP");
          
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
          
          button_arr[i].event_handler(i, &button_arr[i].press_type, &button_combo);
          __ASM("NOP");
          
        } else if(button_arr[i].detect.cnt3 < UINT16_MAX){
          button_arr[i].detect.cnt3++;
        }
      }
      
      button_arr[i].detect.cnt1++;
      

    
    }else if(button_arr[i].status == BUTTON_STATUS_RELEASE){
          
      if(button_arr[i].detect.phase == BUTTON_DET_PHASE_1){ // Button First Release
        
        button_arr[i].detect.phase = BUTTON_DET_PHASE_2;      
        button_arr[i].detect.cnt2  = 0;  
      }
      
      if(button_arr[i].detect.phase == BUTTON_DET_PHASE_2){
      
        if (button_arr[i].detect.cnt2 >= CNT2_INTERVAL_MAX){
          
          // Single Click (A)
          button_arr[i].press_type   = BUTTON_CLICK_SINGLE;
          button_arr[i].detect.phase = BUTTON_DET_PHASE_0;
          
          button_arr[i].event_handler(i, &button_arr[i].press_type, &button_combo);
          __ASM("NOP");     
          
        }else if(button_arr[i].detect.cnt2 < UINT16_MAX){
          button_arr[i].detect.cnt2++;
        }
        
      }
      
      if(button_arr[i].detect.phase == BUTTON_DET_PHASE_3){
        
        if(button_arr[i].detect.cnt3 < CNT3_INTERVAL_MAX){
          
          // Double Click (B)
          button_arr[i].press_type   = BUTTON_CLICK_DOUBLE;
          button_arr[i].detect.phase = BUTTON_DET_PHASE_0;
          
          button_arr[i].event_handler(i, &button_arr[i].press_type, &button_combo);
          __ASM("NOP");
          
        }else if(button_arr[i].detect.cnt3 >= CNT3_INTERVAL_MAX && button_arr[i].detect.cnt3 < LONG_PRESS_DETECT_INTERVAL){
          
          // Single Click (A)
          button_arr[i].press_type   = BUTTON_CLICK_SINGLE;
          button_arr[i].detect.phase = BUTTON_DET_PHASE_0;
          
          button_arr[i].event_handler(i, &button_arr[i].press_type, &button_combo);
          __ASM("NOP");
        }
         
      }
             
      if(button_arr[i].detect.phase == BUTTON_DET_PHASE_5){
        
        button_arr[i].detect.phase  = BUTTON_DET_PHASE_0;
      }

      button_arr[i].detect.cnt1 = 0;
      
    }
    
  }
  
  // combo process [Detecting the Release of All Buttons]  ------------------------------------------
  
  // uint8_t all_release = 0;
  all_release = 0;
  
  for(int i = 0; i < button_amount; i++){

    if(button_arr[i].status == BUTTON_STATUS_PRESSED){
      all_release = 0;
      break;
    }else if(button_arr[i].status == BUTTON_STATUS_RELEASE){
      all_release = 1;
    }
  }
  
  if(all_release == 1){

  }
  
  // ------------------------------------------------------------------------------------------------
  
}
