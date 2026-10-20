#pragma once

#include <stdbool.h>
#include <stdint.h>

// All angles ending in "ticks" use hundredths of a degree.
typedef struct {
  int16_t previous_angle;
  uint32_t width_ticks;
  uint32_t edge_width_ticks;
  int16_t first_clear_angle;
  uint8_t clear_samples;
  bool clear_seen;
} SearchMeasurement;

int32_t Search_Wrap_Angle(int32_t ticks);// Wraps an angle in ticks to the range [-18000, 18000]

void Search_Reset(SearchMeasurement *scan);// Resets the search measurement to its initial state

// Confirms that a clear space has been detected for the required number of samples
bool Search_Confirm_Clear(SearchMeasurement *scan,uint8_t required_samples);

//detects the leading edge of an object after a confirmed clear space
bool Search_Find_Leading_Edge(SearchMeasurement *scan,bool detected, int16_t angle,uint8_t required_samples);

// Updates the width of the detected object based on the current angle
void Search_Update_Width(SearchMeasurement *scan, int16_t angle);

// Detects the trailing edge of an object after a confirmed clear space
bool Search_Find_Trailing_Edge(SearchMeasurement *scan,bool detected, int16_t angle,uint8_t required_samples);

// Checks if the detected object's width is within the specified maximum width in degrees
bool Search_Width_Valid(const SearchMeasurement *scan,uint16_t max_width_deg);

//degree converter
uint16_t Search_Width_Degrees(const SearchMeasurement *scan);

//return angle of rotation to midpoint of object, including extra rotation during confirmation of clear space
int16_t Search_Midpoint_Turn_Degrees(const SearchMeasurement *scan,int16_t angle);

//Resumes scanning for an object after a confirmed clear space
void Search_Resume_Scan(SearchMeasurement *scan);
