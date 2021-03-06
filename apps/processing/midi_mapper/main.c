/*
 * MIDI-Box Channel-Mapper
 *
 * ==========================================================================
 *
 *  Copyright (C) 2008  Matthias Mächler (maechler@mm-computing.ch)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 * 
 * ==========================================================================
 */


/////////////////////////////////////////////////////////////////////////////
// Include files
/////////////////////////////////////////////////////////////////////////////

#include <cmios.h>
#include <pic18fregs.h>


/////////////////////////////////////////////////////////////////////////////
// Application specific 
/////////////////////////////////////////////////////////////////////////////

//each set bit of inputs/outputs assigns an channel to the bus (lsb = channel 0, msb = channel 16)
struct midi_bus{
  unsigned int inputs;
  unsigned int outputs;  
};
  
//--constants--

//--1 tick = 10ms; 100 ticks == 1 sec--
//time that the user has to hold the preset/bus button down to init all presets/all buses
#define init_request_ticks 300


const unsigned int int_bit_ormask[16] = {
  0x0001,0x0002,0x0004,0x0008,  
  0x0010,0x0020,0x0040,0x0080,  
  0x0100,0x0200,0x0400,0x0800,  
  0x1000,0x2000,0x4000,0x8000  
};
  


//--current state--
unsigned char current_screen_num=0;//0:prs;1:bus;2:in;3:out;stored at 0x00 in EEPROM
unsigned char current_preset_num=0;//stored at 0x01 of EEPROM
unsigned char current_bus_num=0;//stored at 0x02 of EEPROM
unsigned char current_shift_state=0;

//holds last pushed value button. will be reset on any button change.
//used for info screens
unsigned char temp_value = 0xff;

//--timed functions: 1: request init; 2:LED flash
unsigned char timed_function=0;
unsigned int timer_ticks_count=0;

//--LED flash--
unsigned char led_flash_led=0;
unsigned char led_flash_count=0;
unsigned char led_flash_ticks=0;

struct midi_bus current_preset[16];//stored in Bank Stick after change
signed char midi_inout_map[16][16];//map the input-> output relations for faster msg forwarding


//------------------------------------------
//----------application functions-----------
//------------------------------------------

void preset_load(unsigned char) __wparam;
void preset_store(unsigned char) __wparam;
void current_screen_set(unsigned char) __wparam;
void current_bus_set(unsigned char) __wparam;
void current_preset_set(unsigned char) __wparam;
void inout_map_build(void) __wparam;
void screen_print(void) __wparam;
void info_screen_print(unsigned char) __wparam;
void timed_function_start(unsigned char) __wparam;
void timed_function_stop(void) __wparam;
void led_flash_start(unsigned char,unsigned char,unsigned char) __wparam;
void led_flash_set(unsigned char) __wparam;


void Init(void) __wparam{
  MIOS_SRIO_UpdateFrqSet(3); // ms
  MIOS_SRIO_NumberSet(3);//need 20 inputs / outputs 
  MIOS_SRIO_DebounceSet(20);
  MIOS_BANKSTICK_CtrlSet(0x40);//stick 0, verify disabled
  //load last application state
  current_screen_num = MIOS_EEPROM_Read(0x00);
  current_preset_num = MIOS_EEPROM_Read(0x01);
  current_bus_num = MIOS_EEPROM_Read(0x02);
  preset_load(current_preset_num);
  //init screen
  screen_print();
}


//---------------preset load/store/build map/init------------

void preset_load(unsigned char num) __wparam{
  unsigned int addr=num;
  addr <<=6;
  MIOS_BANKSTICK_ReadPage(addr,(char*)current_preset);
  inout_map_build();
}


void preset_store(unsigned char num) __wparam{
  unsigned int addr=num;
  addr <<=6;
  MIOS_BANKSTICK_WritePage(addr,(char*)current_preset);  
}
  
void preset_init(unsigned char empty) __wparam{
  unsigned char i;
  for(i=0;i<16;i++){
    current_preset[i].inputs = current_preset[i].outputs = (empty ? 0 : int_bit_ormask[i]);
    midi_inout_map[i][0]= (empty ? 0xff : i);
    midi_inout_map[i][1]=0xff;//terminate    
  }
}
  
void inout_map_build(void) __wparam{
  unsigned char input,output,bus,i;
  unsigned int outputs[16];
  for(input=0;input<16;input++)
    outputs[input] = 0;
  for(input=0;input<16;input++){
    for(bus=0;bus<16;bus++){
      for(input=0;input<16;input++){
        if(current_preset[bus].inputs & int_bit_ormask[input]){
          outputs[input] |= current_preset[bus].outputs;
        }
      }
    }
  }
  for(input=0;input<16;input++){
    i=0;
    for(output=0;output<16;output++){
      if(outputs[input]&int_bit_ormask[output]){
        midi_inout_map[input][i++]=output;
      }
    }
    midi_inout_map[input][i] = 0xff;
  }
}
  
//-------------screen-------------------

void info_screen_print(unsigned char info_screen) __wparam{
  unsigned int values_set = 0;
  unsigned char bus;
  switch(info_screen){
    case 0://buses assigned to inputs?
      for(bus=0;bus<16;bus++){
        if(current_preset[bus].inputs && (temp_value==0xff || current_preset[bus].inputs & int_bit_ormask[temp_value])){
          values_set |= int_bit_ormask[bus];
        }
      }
      break;
    case 1://buses assigned to outputs?
      for(bus=0;bus<16;bus++){
        if(current_preset[bus].outputs && (temp_value==0xff || current_preset[bus].outputs & int_bit_ormask[temp_value])){
          values_set |= int_bit_ormask[bus];
        }
      }
      break;
    case 2://inputs assigned to buses?
      if(temp_value==0xff){//inputs of all buses
        for(bus=0;bus<16;bus++){
          values_set |= current_preset[bus].inputs;
        }
      }
      else{//only inputs of bus [temp_value]
        values_set = current_preset[temp_value].inputs;
      }
      break;
    case 3://inputs assigned to outputs?
      for(bus=0;bus<16;bus++){
        if(current_preset[bus].outputs && (temp_value==0xff || current_preset[bus].outputs & int_bit_ormask[temp_value])){
          values_set |= current_preset[bus].inputs;
        }
      }
      break;
    case 4://outputs assigned to buses?
      if(temp_value==0xff){//outputs of all buses
        for(bus=0;bus<16;bus++){
          values_set |= current_preset[bus].outputs;
        }
      }
      else{//only outputs of bus [temp_value]
        values_set = current_preset[temp_value].outputs;
      }
      break;
    case 5://outputs assigned to inputs?
      for(bus=0;bus<16;bus++){
        if(current_preset[bus].inputs && (temp_value==0xff || current_preset[bus].inputs & int_bit_ormask[temp_value])){
          values_set |= current_preset[bus].outputs;
        }
      }
      break;
  }
  MIOS_DOUT_SRSet(0x01,(unsigned char)(values_set & 0x00ff));
  MIOS_DOUT_SRSet(0x02,(unsigned char)((values_set >>8) & 0x00ff));
}
    
void screen_print(void) __wparam{
  if(timed_function==2)
    return;//no screen print on LED flash
  //set bitmask for screen button
  MIOS_DOUT_SRSet(0x00,MIOS_HLP_GetBitORMask(current_screen_num));
  //clear value screen
  MIOS_DOUT_SRSet(0x01,0x00);
  MIOS_DOUT_SRSet(0x02,0x00);
  switch(current_screen_num){
    case 0://preset screen
      if(current_preset_num < 8)      
        MIOS_DOUT_SRSet(0x01,MIOS_HLP_GetBitORMask(current_preset_num));
      else
        MIOS_DOUT_SRSet(0x02,MIOS_HLP_GetBitORMask(current_preset_num-8));        
      break;
    case 1://bus screen
      if(current_shift_state == 0x06)//buses assigned to inputs (info screen)
        info_screen_print(0);
      else if(current_shift_state == 0x0A)//buses assigned to outputs (info screen)
        info_screen_print(1);
      else{//regular bus screen
        if(current_bus_num < 8)      
          MIOS_DOUT_SRSet(0x01,MIOS_HLP_GetBitORMask(current_bus_num));
        else
          MIOS_DOUT_SRSet(0x02,MIOS_HLP_GetBitORMask(current_bus_num-8));              
      }
      break;
    case 2://inputs screen
      if(current_shift_state == 0x06)//inputs assigned to bus (info screen)
        info_screen_print(2);
      else if(current_shift_state == 0x0C)//inputs assigned to outputs (info screen)
        info_screen_print(3);
      else{//regular input screen
        MIOS_DOUT_SRSet(0x01,(unsigned char)(current_preset[current_bus_num].inputs & 0x00ff));
        MIOS_DOUT_SRSet(0x02,(unsigned char)((current_preset[current_bus_num].inputs >>8) & 0x00ff));
      }
      break;
    case 3://outputs screen
      if(current_shift_state == 0x0A)//outputs assigned to bus (info screen)
        info_screen_print(4);
      else if(current_shift_state == 0x0C)//outputs assigned to inputs (info screen)
        info_screen_print(5);
      else{//regular output screen
        MIOS_DOUT_SRSet(0x01,(unsigned char)(current_preset[current_bus_num].outputs & 0x00ff));
        MIOS_DOUT_SRSet(0x02,(unsigned char)((current_preset[current_bus_num].outputs >>8) & 0x00ff));
      }
      break;
  }
}
  
void current_screen_set(unsigned char value) __wparam{
  current_screen_num = value;
  MIOS_EEPROM_Write(0x00, current_screen_num);    
}
  
void current_bus_set(unsigned char value) __wparam{
  current_bus_num = value;
  MIOS_EEPROM_Write(0x02,value);
}
  
void current_preset_set(unsigned char value) __wparam{
  current_preset_num = value;
  preset_load(value);
  MIOS_EEPROM_Write(0x01,value);
}

  
//----------timed functions----------------

void Timer(void) __wparam{
  timer_ticks_count++;
}


void timed_function_start(unsigned char func) __wparam{
  timer_ticks_count=0;
  timed_function = func;
  MIOS_TIMER_Init(0x01,50000);
}
  
void timed_function_stop(void) __wparam{
  MIOS_TIMER_Stop();
  timed_function = 0;
  timer_ticks_count = 0;
}
  
void led_flash_start(unsigned char led,unsigned char times,unsigned char ticks) __wparam{
  led_flash_led = led;
  led_flash_ticks = ticks;
  led_flash_count = (times<<1) + 1;
  led_flash_set(0);
  timed_function_start(2);
}
  
void led_flash_set(unsigned char state) __wparam{
  if(led_flash_led == 0xff){
    state = (state ? 0xff : 0x00);
    MIOS_DOUT_SRSet(0x01,state);
    MIOS_DOUT_SRSet(0x02,state);
  }
  else{
    MIOS_DOUT_PinSet(led_flash_led,state);
  }
}


void Tick(void) __wparam{
  unsigned char i;
  if(!timed_function)
    return;
  if(timed_function==1 && (timer_ticks_count >= init_request_ticks)){
    timed_function_stop();
    if(!current_screen_num){//screen is preset
      preset_init(0);
      preset_store(0);
      preset_init(1);
      for(i=1;i<16;i++){
        preset_store(i);
      }
      current_preset_set(0);
      current_bus_set(0);
      led_flash_start(0xff,5,10);//flash 5 times
    }
    else{//screen is bank
      preset_init(1);//empty current preset
      current_bus_set(0);
      led_flash_start(0xff,2,10);//flash 2 times
    }
  }
  else if(timed_function==2 && (timer_ticks_count >= led_flash_ticks)){
    if(led_flash_count > 1){
      led_flash_set(led_flash_count-- & 0x01);
      timer_ticks_count = 0;
    }
    else{
      timed_function_stop();
      screen_print();
    }
  }
}

  
//------------------button handling-----------------------

void DIN_NotifyToggle(unsigned char pin, unsigned char pin_value) __wparam{
  unsigned char i,chn_out,chn_in;
  if(timed_function==1)
    timed_function_stop();//stop clear request on every button change
  else if(timed_function==2 && !pin_value)
    return;//button push will only be processed when no LED's are flashing.
  if(pin > 7 && !pin_value){//this is a temp_value button. only handle if button is pushed
    temp_value = pin - 8;
    if((current_shift_state & -current_shift_state) == current_shift_state){//max. one screen button pushed, one bit set
      switch(current_screen_num){
        case 0://preset screen
          if (current_shift_state & MIOS_HLP_GetBitORMask(0)){
            preset_store(current_preset_num);
            led_flash_start(current_preset_num+8,0,50);//LED off for half a second to indicate save
          }
          else{
            current_preset_set(temp_value);
          }
          break;      
        case 1://bus screen
          current_bus_set(temp_value);
          break;      
        case 2://input screen
          if (current_shift_state & MIOS_HLP_GetBitORMask(2)){//add/remove input chanel
            current_preset[current_bus_num].inputs = 
              (current_preset[current_bus_num].inputs & int_bit_ormask[temp_value])?
                (current_preset[current_bus_num].inputs ^ int_bit_ormask[temp_value]):
                (current_preset[current_bus_num].inputs | int_bit_ormask[temp_value]);
          }
          else{//set / unset input channel
            current_preset[current_bus_num].inputs = 
              (current_preset[current_bus_num].inputs==int_bit_ormask[temp_value]) ? 0x0000 :int_bit_ormask[temp_value];
          }    
          inout_map_build();
          break;      
        case 3://output screen
          if (current_shift_state & MIOS_HLP_GetBitORMask(3)){//set/unset output chanel
            current_preset[current_bus_num].outputs = 
              (current_preset[current_bus_num].outputs & int_bit_ormask[temp_value])?
                (current_preset[current_bus_num].outputs ^ int_bit_ormask[temp_value]):
                (current_preset[current_bus_num].outputs | int_bit_ormask[temp_value]);
          }
          else{//add/remove output chanel
            current_preset[current_bus_num].outputs = 
              (current_preset[current_bus_num].outputs==int_bit_ormask[temp_value]) ? 
                0x0000 :int_bit_ormask[temp_value];
          }        
          inout_map_build();
          break;
      }
    }
  }
  else if(pin < 4){
    temp_value = 0xff;
    if(pin_value){
      current_shift_state &= ~MIOS_HLP_GetBitORMask(pin);//remove shift flag
    }
    else{
      if(!current_shift_state){
        current_screen_set(pin);
        if(pin < 2)//start init request countdown
          timed_function_start(1);
      }
      current_shift_state |= MIOS_HLP_GetBitORMask(pin);//add shift flag
    }
  }
  screen_print();
}
  


//----------------MIDI Bytes / Events receive-------------------------------

//this function forwards all system messages to the output
void MPROC_NotifyReceivedByte(unsigned char byte) __wparam{
  static unsigned char fx_status = 0;
  if(byte >= 0xf0){//system status byte
    MIOS_MIDI_TxBufferPut(byte);
     //determine status
    switch(byte){
      case 0xf1://midi timecode 
      case 0xf3://songselect
        fx_status = 1;
        break;   
      case 0xf2://songposition pointer
        fx_status = 2;        
        break;
      case 0xf0://sysex, forward until 0xf7
        fx_status = 0xff;
        break;
      default://applies also on 0xf7!
        fx_status = 0;
    }   
  }
  else if(fx_status){
    MIOS_MIDI_TxBufferPut(byte);
    if(fx_status!=0xff){
      fx_status--;
    }
  }
}


void MPROC_NotifyReceivedEvnt(unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam{
  unsigned char i;
  unsigned char evnt_type = evnt0 & 0xf0;
  unsigned char in_chn = evnt0 & 0x0f;
  unsigned char evnt2_send = !(evnt_type == 0xc0 || evnt_type == 0xd0);
  for(i=0;i<16 && midi_inout_map[in_chn][i]!=-1;i++){
    MIOS_MIDI_TxBufferPut(evnt_type+midi_inout_map[in_chn][i]);
    MIOS_MIDI_TxBufferPut(evnt1);
    if(evnt2_send){
      MIOS_MIDI_TxBufferPut(evnt2);
    }
  }
}




//---------------------------not used in this application----------------

void DISPLAY_Init(void) __wparam{
}

void DISPLAY_Tick(void) __wparam{
}

void MPROC_NotifyFoundEvent(unsigned entry, unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam{
}

void MPROC_NotifyTimeout(void) __wparam{
}

void SR_Service_Prepare(void) __wparam{
}

void SR_Service_Finish(void) __wparam{
}

void ENC_NotifyChange(unsigned char encoder, char incrementer) __wparam{
}

void AIN_NotifyChange(unsigned char pin, unsigned int pin_value) __wparam{
}
