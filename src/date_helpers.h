#pragma once

/*
 * Function:  date_get_weekday 
 * ----------------------------------------------------------------------------
 *  Gets the current weekday.
 *  returns: current weekday from tm->tm_wday
 * ----------------------------------------------------------------------------
 */
int date_get_weekday();

/*
 * Function:  date_get_yearday 
 * ----------------------------------------------------------------------------
 *  Gets the current yearday.
 *  returns: current yearday from tm->tm_yday
 * ----------------------------------------------------------------------------
 */
int date_get_yearday();

/*
 * Function:  date_get_hour 
 * ----------------------------------------------------------------------------
 *  Gets the current hour.
 *  returns: current hour from tm->tm_hour
 * ----------------------------------------------------------------------------
 */
int date_get_hour();

