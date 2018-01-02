/* 
 * File:   application.h
 * Author: Raph
 *
 * Created on 15 November 2017, 11:37
 * 
 * CAN Power application.
 * Requires the initialisation function to be called once after all drivers and services have
 * been initialised.
 * ApplicationTasks must be invoked repeatedly at least once per timer tick.
 * 
 */

#ifndef APPLICATION_H
#define	APPLICATION_H

#ifdef	__cplusplus
extern "C" {
#endif


/* Must be invoked once shortly after power on after the drivers and services are
 * all initialised */
void InitializeApplication(void);


/* Must be invoked repeatedly at least once per timer tick */
void ApplicationTasks(void);


#ifdef	__cplusplus
}
#endif

#endif	/* APPLICATION_H */

