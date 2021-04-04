#ifndef __REMOTE_CODES_H
#define __REMOTE_CODES_H

#ifdef  USE_AV2_MODE

#define BTN_SLEEP                0x030C   //0x24
#define BTN_POWER                0x540C   //0xA
#define BTN_AMP_MENU             0x770D   //x1A
#define BTN_SOURCE               0x6F9E   // Bir sike yaramayan bir kod.

#define BTN_BAL_LEFT             0x320C
#define BTN_BAL_RIGHT            0x720C
#define BTN_PWR_OFF              0x7A0C

#define BTN_TUNER_BACK           0x440C // kendim buldum..

#define BTN_MUTING               0x140C   
#define BTN_LEFT                 0x2F0D   
#define BTN_UP                   0x0F0D   
#define BTN_RIGHT                0x6F0D   
#define BTN_DOWN                 0x4F0D   
#define BTN_SELECT               0x180C   
#define BTN_VOLUME_UP            0x240C   
#define BTN_VOLUME_DOWN          0x640C   

// Input source select
#define BTN_VIDEO1               0x220C
#define BTN_VIDEO2               0x3C0C
#define BTN_DVD                  0x5F0C
#define BTN_TV                   0x2B0C
#define BTN_SA_CD                0x520C  
#define BTN_MD_TAPE              0x4B0C
#define BTN_TUNER                0x420C  //yup

#define BTN_AFD                  0x210D  // calismadi
#define BTN_MOVIE                0x610D  //0x4
#define BTN_MUSIC                0x490D  //0x1C
#define BTN_DVD_MENU             0x5A0D

#define BTN_PRESET               0x320D // cstex A-B-C degisiyor mode+/- daha kullanisli
#define BTN_AUTO_DEC             0x710D  
#define BTN_2CH                  0x410D  
#define BTN_MODE_UP              0x3B0D  
#define BTN_MODE_DOWN            0x7B0D  

#define BTN_MPX_DUAL             0x740C 
#define BTN_ANALOG_DIRECT        0x490C

#define KEY_1                    0x000C
#define KEY_2                    0x400C
#define KEY_3                    0x200C
#define KEY_4                    0x600C
#define KEY_5                    0x100C
#define KEY_6                    0x500C
#define KEY_7                    0x300C
#define KEY_8                    0x700C
#define KEY_9                    0x080C
#define KEY_0                    0x480C
#define KEY_SHIFT                0x280C
#define KEY_ENTER                0x680C
#define BTN_TESTTONE             0x290D
#define BTN_DISPLAY              0x690C
#define DEFAULT_NUM_BITS         15

#else

#define BTN_SLEEP                0x061   //0x24
#define BTN_POWER                0xA81
#define BTN_SHUTDOWN             0xF41
#define BTN_AMP_MENU             0x7709
#define BTN_SOURCE               0x6F9E   // Bir sike yaramayan bir kod.

#define BTN_BAL_LEFT             0x641
#define BTN_BAL_RIGHT            0xE41
#define BTN_PWR_OFF              0x7A0C
#define BTN_MUTING               0x281   
#define BTN_LEFT                 0x2F09 //5e1?   
#define BTN_UP                   0x0F09   
#define BTN_RIGHT                0x6F09   
#define BTN_DOWN                 0x4F09   
#define BTN_SELECT               0x1809   
#define BTN_VOLUME_UP            0x481   
#define BTN_VOLUME_DOWN          0xC81   
#define BTN_CHANNEL_UP           0x096
#define BTN_CHANNEL_DOWN         0x896
#define KEY_SHIFT                0XCD6
#define BTN_VIDEO1               0x441
#define BTN_VIDEO2               0x781
#define BTN_DVD                  0xBE1
#define BTN_TV                   0x2B0C
#define BTN_SA_CD                0xA41
#define BTN_MD_TAPE              0xC41 //961 de oldu
#define BTN_TUNER                0x841  //yup
#define BTN_TUNER_BAND           0xF16
#define BTN_AFD                  0x2109  // calismadi
#define BTN_MOVIE                0x6109  //0x4
#define BTN_MUSIC                0x4909  //0x1C
#define BTN_DVD_MENU             0x5A09
#define BTN_PRESET               0x3209 // cstex A-B-C degisiyor mode+/- daha kullanisli

#define BTN_AUTO_DEC             0x7109
#define BTN_2CH                  0x4109
#define BTN_MODE_UP              0x3B09  
#define BTN_MODE_DOWN            0x7B09  
#define BTN_MPX_DUAL             0x740C
#define BTN_ANALOG_DIRECT        0x4909
#define KEY_1                    0x001
#define KEY_2                    0x801
#define KEY_3                    0x401
#define KEY_4                    0xC01
#define KEY_5                    0x201
#define KEY_6                    0xA01
#define KEY_7                    0x601
#define KEY_8                    0xE01
#define KEY_9                    0x101
#define KEY_0                    0x105 //?
#define KEY_ENTER                0xD10
#define BTN_TESTTONE             0x2909
#define BTN_DISPLAY              0x6909
#define DEFAULT_NUM_BITS         12

#endif 

#endif // __REMOTE_CODES_H



// 6b0c -> dvd/ld ? 


 /*       found these on LIRC idk they work 
  *         
  *       I/O                      0x540C
          SLEEP                    0x030C
          VIDEO1                   0x220C
          VIDEO2                   0x3C0C
          VIDEO3                   0x210C
          DVD_LD                   0x5F0C
          TV_SAT                   0x2B0C
          MD_TAPE                  0x4B0C
          TUNER                    0x420C
          CD_SACD                  0x520C
          PHONO                    0x020C
          AUX                      0x5C0C
          MULTI_CH                 0x270C
          SOURCE                   0x6F9E
          TEST_TONE                0x290D working 
          PRESET                   0x320D working 
          AUTO_DECoding            0x710D
          MPX/DUAL                 0x740C
          ANALOG_DIRECT            0x490C
          2CH                      0x410D
          MODE+                    0x3B0D //ok
          MODE-                    0x7B0D //ok
          MUTING                   0x140C
          MASTER_VOL+              0x240C
          MASTER_VOL-              0x640C
          MAIN_MENU                0x770D
          ON_SCREEN                0x650C
          SYSTEM_STANDBY           0x7620



 */
