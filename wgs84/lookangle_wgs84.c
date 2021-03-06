/* Jimmy Carter, KG4SGP
   Look Angle Calculator
   BSD 3 Clause License at bottom
*/

#include<stdio.h>
#include<math.h>

typedef struct 
{
  float az;
  float el;
  float range;
} s_azelcord;

typedef struct
{
  float lat;
  float lon;
  float alt;
} s_geogcord;

s_azelcord look_angle(s_geogcord ground, s_geogcord obs)
{

  /* WGS 84 Constants */
  float radius_e = 6378137;
  float f = 1/298.257223563;
  float ecc = 8.1819190842621E-2;

  /* Convert from decimal degrees to radians */
  float lat_ground = (M_PI/180)*ground.lat;
  float lon_ground = (M_PI/180)*ground.lon;
  float lat_obs = (M_PI/180)*obs.lat;
  float lon_obs = (M_PI/180)*obs.lon;

  /* Add radius of earth to altitudes */
  float r_ground = radius_e + ground.alt;
  float r_obs = radius_e + obs.alt;

  /* WGS 84 Geoid */
  float N = radius_e / sqrt(1 - pow(ecc,2)*pow(sin(lat_ground),2));

  /* Convert ground to Earth Centered Rotational (ECR) coordinates */
  float x_ground = (N+ground.alt)*cos(lat_ground)*cos(lon_ground);
  float y_ground = (N+ground.alt)*cos(lat_ground)*sin(lon_ground);
  float z_ground = (N*(1 - pow(ecc,2)) + ground.alt)*sin(lat_ground);

  /* Convert obv station to Earth Centered Rotational (ECR) coordinates */
  float x_obs = (N+obs.alt)*cos(lat_obs)*cos(lon_obs);
  float y_obs = (N+obs.alt)*cos(lat_obs)*sin(lon_obs);
  float z_obs = (N*(1 - pow(ecc,2)) + obs.alt)*sin(lat_obs);

  /* Calculate the range vector */
  float range_v_x = x_obs - x_ground;
  float range_v_y = y_obs - y_ground;
  float range_v_z = z_obs - z_ground;

  /* Transform range vector to Topocenteric Horizon */
  float rot_s = sin(lat_ground)*cos(lon_ground)*range_v_x + sin(lat_ground)*sin(lon_ground)*range_v_y - cos(lat_ground)*range_v_z;
  float rot_e = -1*sin(lon_ground)*range_v_x + cos(lon_ground)*range_v_y;
  float rot_z = cos(lat_ground)*cos(lon_ground)*range_v_x + cos(lat_ground)*sin(lon_ground)*range_v_y + sin(lat_ground)*range_v_z;

  float range = sqrt(pow(rot_s, 2) + pow(rot_e, 2) + pow(rot_z, 2));

  /* Calculate elevation and take care of divide by zero if they're the same point */
  float el = 0;
  if (range == 0) {
    el = (M_PI)/2;
  } else {
    el = asin(rot_z/range);
  }

  /* Calculate the azmuth and take care of divide by zero */
  float az = 0;
  if (rot_s == 0) {
    az = (M_PI)/2;
  } else {
    az = atan(-1*(rot_e/rot_s));
  }

  if (az < 0) {
    az = az+(2*M_PI);
  }

  s_azelcord look_here;
  look_here.az = az*(180/M_PI);
  look_here.el = el*(180/M_PI);
  look_here.range = range;

  return look_here;
}

int main(int argc, char* argv[])
{
  s_geogcord ground;
  ground.lat = 40;
  ground.lon = 75;
  ground.alt = 0;

  s_geogcord obs;
  obs.lat = 40;
  obs.lon = 74;
  obs.alt = 0;

  s_azelcord lookangles;
  lookangles = look_angle(ground, obs);

  printf("\nAz: %.2f\nEl: %.2f\nRange: %.2f\n\n", lookangles.az, lookangles.el, lookangles.range);

  return 0;
}

/*
Copyright (c) 2014, Jimmy Carter KG4SGP
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Jimmy Carter KG4SGP nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL Jimmy Carter KG4SGP BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
