/*******************************************************************************
Copyright 2016-2018 anxzhu (github.com/anxzhu)
Copyright 2018-2020 Valerio Nappi (github.com/valerionew) (changes)
Based on segment-lcd-with-ht1621 from anxzhu (2016-2018)
(https://github.com/anxzhu/segment-lcd-with-ht1621)

Partially rewritten and extended by Valerio Nappi (github.com/valerionew) in 2018

This file is part of the HT1621 arduino library, and thus under the MIT license.
More info on the project and the license conditions on :
https://github.com/valerionew/ht1621-7-seg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*******************************************************************************/

#ifndef HT1621_H_
#define HT1621_H_   //防止重复包含

#define  BIAS     0x52             //0b1000 0101 0010  1/3duty 4com
#define  SYSDIS   0X00             //0b1000 0000 0000  关振系统荡器和LCD偏压发生器
#define  SYSEN    0X02             //0b1000 0000 0010 打开系统振荡器
#define  LCDOFF   0X04             //0b1000 0000 0100  关LCD偏压
#define  LCDON    0X06             //0b1000 0000 0110  打开LCD偏压
#define  XTAL     0x28             //0b1000 0010 1000 外部接时钟
#define  RC256    0X30             //0b1000 0011 0000  内部时钟
#define  TONEON   0X12             //0b1000 0001 0010  打开声音输出
#define  TONEOFF  0X10             //0b1000 0001 0000 关闭声音输出
#define  WDTDIS1  0X0A             //0b1000 0000 1010  禁止看门狗
#define  BUFFERSIZE 12

// #define HT1621_DEBUG

// Special markers
#define CLASSIC 7
#define ROCK 15
#define POP 23
#define CH21 31
#define CH51 39
#define USB 40
#define CLOCK 41
#define TREBLE 42
#define COA 43
#define PREVIUOS 48
#define MP3 49
#define D3 50
#define CD 51
#define FORWARD 52
#define SP 53
#define OPT 55
#define FM 56
#define AM 57
#define AUX 59
#define STANDBY 60
#define PLAY 61
#define PAUSE 62
#define DVD 63
#define WOOFER 64
#define REAR 65
#define CENTER 66
#define FLAT 67
#define MUTE 68
#define VOLUME 69
#define FRONT 70
#define RADIO 71
#define DECIMAL_POINT 84
#define COLON 85
#define JASS 87
#define MHZ 88
#define KHZ 89
#define MS 90
#define DELAY 91
#define LOGIC 92
#define PRO 93
#define BASS 94
#define ECHO 95


class  HT1621
{
public:
	HT1621();
	void begin(int cs_p, int wr_p, int data_p, int backlight_p);
	void begin(int cs_p, int wr_p, int data_p);
	void clear();
	void clear_buffer();
	void backlight();
	void noBacklight();
	void setBatteryLevel(int level);
	void print(long num, char* flags="%6li", int precision = 0);
	void print(double num, int precision = 3);
	void printCelsius(double num); // precision is always 1
	void print(char* str, bool leftPadded = false);
	void display();
	void noDisplay();
    void setMarker(int marker_idx);
    void print_digit(int digit, char value);
private:
	int _cs_p;
	int _wr_p;
	int _data_p;
	int _backlight_p;
	bool _backlight_en;
	char _buffer[BUFFERSIZE];
	unsigned char _battery[3];
	void wrone(unsigned char addr, unsigned char sdata, unsigned char len);
	void wrclrdata(unsigned char addr, unsigned char sdata);
	void wrCLR(unsigned char len);
	void wrDATA(unsigned char data, unsigned char cnt);
	void wrCMD(unsigned char CMD);
	void setdecimalseparator(int dpposition);
	void config(); // legacy: why not in begin func
	void update();
	char charToSegBits(char character);
};
#endif
