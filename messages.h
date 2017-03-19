#ifndef MESSAGES_H
#define MESSAGES_H


#define HOVEBOARD_PACKET_SIZE 8
#define HEDGEHOG_PACKET_SIZE 13
#define HEDGEHOG_PACKET_HEADER_SIZE 5

#define ADDRESS         0
#define PACKET_TYPE     1
#define DATA_CODE       2
#define DATA_AMOUNT     4
#define COMMAND_MODE    5
/*
0 – stop
1 – power control
2 – speed control	N
*/
#define MOVEMENT        6
/*
1	uint8_t	Type of movement (for modes 1 and 2):
0 – move forward
1 – move backward
2 – rotate right (clockwise)
3 – rotate left (counterclockwise)	
*/
#define PERCENTS        7
#define RESERVED				8
#define CONTROL_TYPE    10
/*
0 – control by sliders in motors window
1 – control by arrows in main window
*/
#define CHECKSUM        11


#endif
