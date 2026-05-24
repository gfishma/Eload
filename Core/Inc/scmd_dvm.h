/*
 * scmd_dvm.h
 *
 *  Created on: 2020年7月22日
 *      Author: timecy
 */

#ifndef SCMD_APP_SCMD_DVM_H_
#define SCMD_APP_SCMD_DVM_H_

#include "Module_DVM_V2.h"
#include "bsp.h"

void DVM_config(void);
float DVM_GET(unsigned char ch);

#endif /* SCMD_APP_SCMD_DVM_H_ */
