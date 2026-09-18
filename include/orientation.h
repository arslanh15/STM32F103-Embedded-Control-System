#ifndef ORIENTATION_H
#define ORIENTATION_H

void Orientation_Init(float ax, float az);
float Orientation_Update_Roll(float ax, float az, float gy, float dt);

#endif