#ifndef _GROVE_GPS_H_
#define _GROVE_GPS_H_

#include "mbed.h"
#include <stdlib.h>
#include <string>

class GroveGPS {

public:

	GroveGPS() : _last_line("") {}

	void readCharacter(char newCharacter) {
		if (newCharacter == '\n') {
			parseLine();
			_last_line = "";
		} else {
			_last_line += newCharacter;
		}
	}

	struct GGA {
		double  utc_time; 		// Format: hhmmss.sss
		double  latitude; 		// Format: ddmm.mmmm
		char    ns_indicator; 	// Format: N=north or S=south
		double  longitude;		// Format: dddmm.mmmm
		char    ew_indicator;	// Format: E=east or W=west
		uint8_t position_fix;	// Options: [0=not available, 1=GPS SPS mode, 2=Differential GPS, 6=dead reckoning]
		uint8_t sats_used;		// Range: 0-12
		double  hdop;			// Horizontal Dilution of Precision
		double  msl_altitude;
		char    msl_altitude_units;
		double  geoid_separation;
		char    geoid_separation_units;
		long    age_of_diff;
		long    diff_ref_station_id;
		std::string  checksum;
	} gps_gga;

	void getTimestamp(char* buffer) {
		sprintf(buffer, "%f", gps_gga.utc_time);
	}

	void getLatitude(char* buffer) {
		double coordinate = convertGPSToDecimal(gps_gga.latitude);
		if (gps_gga.position_fix==0)
			sprintf(buffer, "N/A");
		else
			sprintf(buffer, "%c%fTest", gps_gga.ns_indicator, coordinate);
	}

	void getLongitude(char* buffer) {
		double coordinate = convertGPSToDecimal(gps_gga.longitude);
		if (gps_gga.position_fix==0)
			sprintf(buffer, "N/A");
		else
		sprintf(buffer, "%c%fTest", gps_gga.ew_indicator, coordinate);
	}

private:
	std::string _last_line;

	double convertGPSToDecimal(double coordinate) {
		int degrees = coordinate/100.0;
		int minutes = ((int)coordinate) % 100;
		double seconds = coordinate - ((int)coordinate);
		return degrees + (minutes+seconds)/60;

	}

	void parseLine() {
		if (_last_line.find("GPGGA") != std::string::npos) {
			parseGGA();
		}
	}

	void parseGGA() {
  		char* pEnd;
		for (int i=0; i<14; i++) {
			std::string current_item = _last_line.substr(0,_last_line.find(","));
			_last_line = _last_line.substr(_last_line.find(",")+1);
			if (i==0) { 		// NMEA Tag
			} else if (i==1) { 	// UTC time
				gps_gga.utc_time = strtod(current_item.c_str(), &pEnd);
			} else if (i==2) { 	// Latitude
				gps_gga.latitude = strtod(current_item.c_str(), &pEnd);
			} else if (i==3) { 	// Latitude North/South indicator
				gps_gga.ns_indicator = current_item[0];
			} else if (i==4) { 	// Longitude
				gps_gga.longitude = strtod(current_item.c_str(), &pEnd);
			} else if (i==5) { 	// Longitude indicator
				gps_gga.ew_indicator = current_item[0];
			}
		}
	}
};

#endif