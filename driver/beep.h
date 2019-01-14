#ifndef __BEEP_H__
#define __BEEP_H__

extern void beep_work(int work, int idol);
extern int beep_init(void);
extern void beep_finished(void);
extern void beep_warning(void);
extern void beep_clicked(void);
extern void beep_clicked_delay(void);
extern void beep_warning_delay(void);
extern void beep_finished_delay(void);

#endif
