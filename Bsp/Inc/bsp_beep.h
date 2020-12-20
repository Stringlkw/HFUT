/*
 * bspbeep.h
 *
 *  Created on: 2020年7月14日
 *      Author: lkw
 */

#ifndef BEEP_BSPBEEP_H_
#define BEEP_BSPBEEP_H_

/********************************************************
 本模块提供给外部调用的函数
 ********************************************************/
void BSP_BEEP_Config();	//多个指示灯初始化
void BSP_BEEP_StartRing();
void BSP_BEEP_StopRing();

#endif /* BEEP_BSPBEEP_H_ */
