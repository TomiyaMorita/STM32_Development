#ifndef _leg_message
#define _leg_message

#include <stdint.h>

typedef struct {
    float p_des, v_des, kp, kd, t_ff;
    }joint_control;

typedef struct {
    float p, v, t;
    }joint_state;

typedef struct {
	joint_state a, h, k;
    }leg_state;

typedef struct {
    joint_control a, h, k;
    }leg_control;

#endif
