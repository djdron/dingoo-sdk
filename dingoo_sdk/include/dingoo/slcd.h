#ifndef __slcd_h__
#define __slcd_h__

#ifdef __cplusplus
extern "C"
{
#endif

extern void  _lcd_set_frame();
extern void* _lcd_get_frame();

// From slcd.a in the lib folder of the mipsel toolchain.
//extern void  lcd_backlight_on();  // Enables the screen?
//extern void  lcd_backlight_off(); // Disables the screen.

extern void  lcd_set_frame();
extern void* lcd_get_frame();

#ifdef __cplusplus
}
#endif

#endif
