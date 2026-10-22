#include "search_helper.h"

int32_t Search_Wrap_Angle(int32_t ticks) {
  if (ticks < -18000) ticks += 36000;
  if (ticks > 18000) ticks -= 36000;
  return ticks;
}

void Search_Reset(SearchMeasurement *scan) {
  *scan = (SearchMeasurement){0};// Reset all fields to zero
}

bool Search_Confirm_Clear(SearchMeasurement *scan, uint8_t required_samples) {
  if (scan->clear_samples < required_samples) ++scan->clear_samples;
  return scan->clear_samples >= required_samples;
}

// Ignore an initial partial object; start measuring after confirmed clear space.
bool Search_Find_Leading_Edge(SearchMeasurement *scan,bool detected, int16_t angle,uint8_t required_samples) {
  if (!detected) {//item not found
    if (Search_Confirm_Clear(scan, required_samples)) scan->clear_seen = true;
  } else if (scan->clear_seen) {
    //initialize the scan measurement for the leading edge
    scan->previous_angle = angle;
    scan->width_ticks = 0;
    scan->clear_seen = false;
    scan->clear_samples = 0;
    return true;
  } else {
    scan->clear_samples = 0;
  }
  return false;
}

// Preserve forward-only accumulation and the existing one-revolution limit.
void Search_Update_Width(SearchMeasurement *scan, int16_t angle) {
  int32_t delta = Search_Wrap_Angle((int32_t)angle - scan->previous_angle);
  scan->previous_angle = angle;
  if (delta > 0 && scan->width_ticks < 36000U) {
    scan->width_ticks += (uint32_t)delta;
  }
}

// Freeze the candidate edge at the first clear reading, then confirm it.
bool Search_Find_Trailing_Edge(SearchMeasurement *scan,bool detected, int16_t angle,uint8_t required_samples) {
  Search_Update_Width(scan, angle);
  if (detected) {
    //if item still found
    scan->clear_samples = 0;
    return false;
  }
  if (scan->clear_samples == 0) {
    // freeze the candidate edge at the first clear reading
    scan->edge_width_ticks = scan->width_ticks;
    scan->first_clear_angle = angle;
  }
  return Search_Confirm_Clear(scan, required_samples);
}

bool Search_Width_Valid(const SearchMeasurement *scan,uint16_t max_width_deg) {
  return scan->edge_width_ticks > 0 &&scan->edge_width_ticks < (uint32_t)max_width_deg*100;
}

uint16_t Search_Width_Degrees(const SearchMeasurement *scan) {
  return (uint16_t)(scan->edge_width_ticks / 100U);
}

int16_t Search_Midpoint_Turn_Degrees(const SearchMeasurement *scan,  int16_t angle) {
  int32_t extra_ticks = Search_Wrap_Angle((int32_t)angle - scan->first_clear_angle);//extra rotation ticks
  int32_t turn_ticks = -(int32_t)(scan->edge_width_ticks/2)- extra_ticks*2;
  return (int16_t)(Search_Wrap_Angle(turn_ticks)/100);//in degrees
}

void Search_Resume_Scan(SearchMeasurement *scan) {
  scan->clear_seen = true;
  scan->clear_samples = 0;
}
