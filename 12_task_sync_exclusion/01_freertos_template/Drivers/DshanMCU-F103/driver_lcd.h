// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright (c) 2008-2023 100askTeam : Dongshan WEI <weidongshan@qq.com> 
 * Discourse:  https://forums.100ask.net
 */

 
/*  Copyright (C) 2008-2023 深圳百问网科技有限公司
 *  All rights reserved
 *
 *
 * 免责声明: 百问网编写的文档，仅供学员学习使用，可以转发或引用(请保留作者信息)，禁止用于商业用途！
 * 免责声明: 百问网编写的程序，可以用于商业用途，但百问网不承担任何后果！
 * 
 * 摘要：
 *			本程序遵循GPL V3协议，使用请遵循协议许可
 *			本程序所用的开发板：	DShanMCU-F103
 *			百问网嵌入式学习平台：https://www.100ask.net
 *			百问网技术交流社区：	https://forums.100ask.net
 *			百问网官方B站：				https://space.bilibili.com/275908810
 *			百问网官方淘宝：			https://100ask.taobao.com
 *			联系我们(E-mail)：	  weidongshan@qq.com
 *
 *			版权所有，盗版必究。
 *  
 * 修改历史     版本号           作者        修改内容
 *-----------------------------------------------------
 * 2023.08.04      v01         百问科技      创建文件
 *-----------------------------------------------------
*/

#ifndef __DRIVER_LCD_H
#define __DRIVER_LCD_H

#include <stdint.h>

/*
 *  鍑芥暟鍚嶏細LCD_Init
 *  鍔熻兘鎻忚堪锛氬垵濮嬪寲LCD
 *  杈撳叆鍙傛暟锛氭棤
 *  杈撳嚭鍙傛暟锛氭棤
 *  杩斿洖鍊硷細鏃�
 */
void LCD_Init(void);


/*
 *  鍑芥暟鍚嶏細LCD_Clear
 *  鍔熻兘鎻忚堪锛氭竻灞忓嚱鏁�
 *  杈撳叆鍙傛暟锛氭棤
 *  杈撳嚭鍙傛暟锛氭棤
 *  杩斿洖鍊硷細鏃�
*/
void LCD_Clear(void);

/*
 *  鍑芥暟鍚嶏細LCD_PutChar
 *  鍔熻兘鎻忚堪锛氭樉绀轰竴涓瓧绗�
 *  杈撳叆鍙傛暟锛歺 --> x鍧愭爣
 *            y --> y鍧愭爣
 *            c -->   鏄剧ず鐨勫瓧绗�
 *  杈撳嚭鍙傛暟锛氭棤
 *  杩斿洖鍊硷細鏃�
*/
void LCD_PutChar(uint8_t x, uint8_t y, char c);


/*
 *  鍑芥暟鍚嶏細LCD_PrintString
 *  鍔熻兘鎻忚堪锛氭樉绀轰竴涓瓧绗︿覆
 *  杈撳叆鍙傛暟锛歺 --> x鍧愭爣
 *            y --> y鍧愭爣
 *            str -->   鏄剧ず鐨勫瓧绗︿覆
 *  杈撳嚭鍙傛暟锛氭棤
 *  杩斿洖鍊硷細鎵撳嵃浜嗗灏戜釜瀛楃
*/
int LCD_PrintString(uint8_t x, uint8_t y, const char *str);

/*
 *  鍑芥暟鍚嶏細OLED_ClearLine
 *  鍔熻兘鎻忚堪锛氭竻闄や竴琛�
 *  杈撳叆鍙傛暟锛歺 - 浠庤繖閲屽紑濮�
 *            y - 娓呴櫎杩欒
 *  杈撳嚭鍙傛暟锛氭棤
 *  杩斿洖鍊硷細鏃�
 */
void LCD_ClearLine(uint8_t x, uint8_t y);

/*
 *  LCD_PrintHex
 *  鍔熻兘鎻忚堪锛氫互16杩涘埗鏄剧ず鏁板€�
 *  杈撳叆鍙傛暟锛歺 - x鍧愭爣
 *            y - y鍧愭爣
 *            val -   鏄剧ず鐨勬暟鎹�
 *            pre -   闈為浂鏃舵樉绀�"0x"鍓嶇紑
 *  杈撳嚭鍙傛暟锛氭棤
 *  杩斿洖鍊硷細鎵撳嵃浜嗗灏戜釜瀛楃
 */
int LCD_PrintHex(uint8_t x, uint8_t y, uint32_t val, uint8_t pre);

/*
 *  LCD_PrintSignedVal
 *  鍔熻兘鎻忚堪锛氫互10杩涘埗鏄剧ず涓€涓暟鍊�
 *  杈撳叆鍙傛暟锛歺 --> x鍧愭爣(0~15)
 *            y --> y鍧愭爣(0~7)
 *            val -->   鏄剧ず鐨勬暟鎹�
 *  杈撳嚭鍙傛暟锛氭棤
 *  杩斿洖鍊硷細鎵撳嵃浜嗗灏戜釜瀛楃
 */
int LCD_PrintSignedVal(uint8_t x, uint8_t y, int32_t val);


/**********************************************************************
 * 鍑芥暟鍚嶇О锛� LCD_Test
 * 鍔熻兘鎻忚堪锛� LCD娴嬭瘯绋嬪簭
 * 杈撳叆鍙傛暟锛� 鏃�
 * 杈撳嚭鍙傛暟锛� 鏃�
 *            鏃�
 * 杩� 鍥� 鍊硷細 0 - 鎴愬姛, 鍏朵粬鍊� - 澶辫触
 * 淇敼鏃ユ湡        鐗堟湰鍙�     淇敼浜�        淇敼鍐呭
 * -----------------------------------------------
 * 2023/08/03        V1.0     闊︿笢灞�       鍒涘缓
 ***********************************************************************/
void LCD_Test(void);

#endif /* __DRIVER_LCD_H */

