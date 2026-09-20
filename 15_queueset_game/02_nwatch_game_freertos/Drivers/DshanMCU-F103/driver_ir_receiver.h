// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright (c) 2008-2023 100askTeam : Dongshan WEI <weidongshan@qq.com> 
 * Discourse:  https://forums.100ask.net
 */
 
/*  Copyright (C) 2008-2023 深圳百问网科技有限公司
 *  All rights reserved
 *
 * 免责声明: 百问网编写的文档, 仅供学员学习使用, 可以转发或引用(请保留作者信息),禁止用于商业用途！
 * 免责声明: 百问网编写的程序, 可以用于商业用途, 但百问网不承担任何后果！
 * 
 * 本程序遵循GPL V3协议, 请遵循协议
 * 百问网学习平台   : https://www.100ask.net
 * 百问网交流社区   : https://forums.100ask.net
 * 百问网官方B站    : https://space.bilibili.com/275908810
 * 本程序所用开发板 : DShanMCU-F103
 * 百问网官方淘宝   : https://100ask.taobao.com
 * 联系我们(E-mail): weidongshan@qq.com
 *
 *          版权所有，盗版必究。
 *  
 * 修改历史     版本号           作者        修改内容
 *-----------------------------------------------------
 * 2023.08.04      v01         百问科技      创建文件
 *-----------------------------------------------------
 */


#ifndef _DRIVER_IR_RECEIVER_H
#define _DRIVER_IR_RECEIVER_H

#include <stdint.h>

#include "FreeRTOS.h"
#include "queue.h"

#define IR_QUEUE_LEN 10

#define IR_KEY_POWER  0xa2
#define IR_KEY_MENU   0xe2
#define IR_KEY_TEST   0x22
#define IR_KEY_ADD    0x02
#define IR_KEY_RETURN 0xc2
#define IR_KEY_LEFT   0xe0
#define IR_KEY_PLAY   0xa8
#define IR_KEY_RIGHT  0x90
#define IR_KEY_0      0x68
#define IR_KEY_DEC    0x98
#define IR_KEY_C      0xb0
#define IR_KEY_1      0x30
#define IR_KEY_2      0x18
#define IR_KEY_3      0x7a
#define IR_KEY_4      0x10
#define IR_KEY_5      0x38
#define IR_KEY_6      0x5a
#define IR_KEY_7      0x42
#define IR_KEY_8      0x4a
#define IR_KEY_9      0x52
#define IR_KEY_REPEAT 0x00

struct ir_data {
	uint32_t dev;
	uint32_t val;
};

/**********************************************************************
 * 鍑芥暟鍚嶇О锛� GetQueueIR
 * 鍔熻兘鎻忚堪锛� 杩斿洖绾㈠椹卞姩绋嬪簭閲岀殑闃熷垪鍙ユ焺
 * 杈撳叆鍙傛暟锛� 鏃�
 * 杈撳嚭鍙傛暟锛� 鏃�
 * 杩� 鍥� 鍊硷細 闃熷垪鍙ユ焺
 * 淇敼鏃ユ湡锛�      鐗堟湰鍙�     淇敼浜�	      淇敼鍐呭
 * -----------------------------------------------
 * 2023/09/02	     V1.0	  闊︿笢灞�	      鍒涘缓
 ***********************************************************************/
QueueHandle_t GetQueueIR(void);

/**********************************************************************
 * 鍑芥暟鍚嶇О锛� IRReceiver_Init
 * 鍔熻兘鎻忚堪锛� 绾㈠鎺ユ敹鍣ㄧ殑鍒濆鍖栧嚱鏁�
 * 杈撳叆鍙傛暟锛� 鏃�
 * 杈撳嚭鍙傛暟锛� 鏃�
 * 杩� 鍥� 鍊硷細 鏃�
 * 淇敼鏃ユ湡锛�      鐗堟湰鍙�     淇敼浜�	      淇敼鍐呭
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  闊︿笢灞�	      鍒涘缓
 ***********************************************************************/
void IRReceiver_Init(void);

/**********************************************************************
 * 鍑芥暟鍚嶇О锛� IRReceiver_Read
 * 鍔熻兘鎻忚堪锛� 绾㈠鎺ユ敹鍣ㄧ殑璇诲彇鍑芥暟
 * 杈撳叆鍙傛暟锛� 鏃�
 * 杈撳嚭鍙傛暟锛� pDev  - 鐢ㄦ潵淇濆瓨璁惧ID
 *            pData - 鐢ㄦ潵淇濆瓨鎸夐敭鐮�
 * 杩� 鍥� 鍊硷細 0 - 鎴愬姛, (-1) - 澶辫触(鏃犳暟鎹�)
 * 淇敼鏃ユ湡锛�      鐗堟湰鍙�     淇敼浜�	      淇敼鍐呭
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  闊︿笢灞�	      鍒涘缓
 ***********************************************************************/
int IRReceiver_Read(uint8_t *pDev, uint8_t *pData);

/**********************************************************************
 * 鍑芥暟鍚嶇О锛� IRReceiver_CodeToString
 * 鍔熻兘鎻忚堪锛� 鎶婃帴鏀跺埌鐨勬寜閿爜杞崲涓烘寜閿悕瀛�
 * 杈撳叆鍙傛暟锛� code - 鎸夐敭鐮�
 * 杈撳嚭鍙傛暟锛� 鏃�
 * 杩� 鍥� 鍊硷細 NULL - 鏈瘑鍒殑鎸夐敭鐮�; 鍏朵粬鍊� - 鎸夐敭鍚嶅瓧
 * 淇敼鏃ユ湡锛�      鐗堟湰鍙�     淇敼浜�	      淇敼鍐呭
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  闊︿笢灞�	      鍒涘缓
 ***********************************************************************/
const char *IRReceiver_CodeToString(uint8_t code);


/**********************************************************************
 * 鍑芥暟鍚嶇О锛� IRReceiver_Test
 * 鍔熻兘鎻忚堪锛� 绾㈠鎺ユ敹鍣ㄦ祴璇曠▼搴�
 * 杈撳叆鍙傛暟锛� 鏃�
 * 杈撳嚭鍙傛暟锛� 鏃�
 *            鏃�
 * 杩� 鍥� 鍊硷細 鏃�
 * 淇敼鏃ユ湡        鐗堟湰鍙�     淇敼浜�        淇敼鍐呭
 * -----------------------------------------------
 * 2023/08/04        V1.0     闊︿笢灞�       鍒涘缓
 ***********************************************************************/
void IRReceiver_Test(void);


#endif /* _DRIVER_IR_RECEIVER_H */

