#ifndef GPSDATAPARSER_H
#define GPSDATAPARSER_H

#include <QObject>
#include <QStringList>


/* http://aprs.gids.nl/nmea/  */
/* gpsworld.com/the-almanac/ */

/* GPS DOP and active satellites */
typedef struct  
{
	QString mode1; /* M=Manual, forced to operate in 2D or 3D , A=Automatic, 3D/2D */
	int mode2; /* 1=Fix not available , 2=2D, 3=3D */
	int svUsedInPosFixID[12]; /* IDs of SVs used in position fix (null for unused fields) */
	float pdop; /* PDOP */
	float hdop; /* HDOP */
	float vdop; /* VDOP */

} __GPSGSAData;

/* Global Positioning System Fix Data */

typedef struct  
{
	int utcOfPosition; /* UTC of Position */
	QString latitude; /* Latitude */
	QString ns; /* N or S */
	QString longitude; /* Longitude */
	QString ew; /* E or W */
	int gpsQualityIndicator; /* GPS quality indicator (0=invalid; 1=GPS fix; 2=Diff. GPS fix) */
	int numberOfSatInUse; /* Number of satellites in use [not those in view] */
	QString horizontalDilutionOfPosition; /* Horizontal dilution of position */
	float antennaAltitudeAboveBelow; /* Antenna altitude above/below mean sea level (geoid) */
	float antennaHeightInMeter; /* Meters  (Antenna height unit) */
	QString geoidalSeparation; /* Geoidal separation (Diff. between WGS-84 earth ellipsoid and mean sea level.  -=geoid is below WGS-84 ellipsoid) */
	float geoidalSeparationInMeters; /* Meters  (Units of geoidal separation) */
	float ageInSecondsSinceLastUpdate; /* Age in seconds since last update from diff. reference station */
	float diffReferenceStationIDNo; /* Diff. reference station ID# */
	QString checksum; /* Checksum */

} __GPGGAData;

/* Recommended minimum specific GPS/Transit data */

typedef struct
{
	int utcPosFix; /* UTC of position fix */
	QString dataStatus; /* Data status (V=navigation receiver warning) */
	QString latitudeOfFix; /* Latitude of fix */
	QString ns; /* N or S */
	QString longitudeOfFix; /* Longitude of fix */
	QString ew; /* E or W */
	float speedOverGroundInKnots; /* Speed over ground in knots */
	QString trueCourse; /* Track made good in degrees True */
	int universalTime; /* UT date */
	QString magneticVariation; /* Magnetic variation degrees (Easterly var. subtracts from true course) */
	QString magneticVariationEW; /* E or W */
	QString checkSum; /* Checksum */

}__GPRMCData;

/* Parse GPS Satellites in view data */

typedef struct
{
	int totalMsgNumber; /* Total number of messages of this type in this cycle */
	int msgNumber; /* Message number */
	int totalSVinView; /* Total number of SVs in view */
	int svPRNNumber[4]; /* SV PRN number */
	float elevationDegrees[4]; /* Elevation in degrees, 90 maximum */
	float azimuth[4]; /* Azimuth, degrees from true north, 000 to 359 */
	float snr[4]; /* SNR, 00-99 dB (null when not tracking) */

} __GPGSVData;

class GPSDataParser
{

public:
	GPSDataParser();
	~GPSDataParser();
	
	bool isValidSentence(QString data);

	QStringList listParsedData;


	__GPGSVData GPGSVData;
	__GPRMCData GPRMCData;
	__GPGGAData GPGGAData;
	__GPSGSAData GPSGSAData;

	/* Parse GPS Satellites in view data */
	__GPGSVData ParseGPGSVData(QString data);

	/* Parse recommended minimum specific GPS/Transit data */
	__GPRMCData ParseGPRMCData(QString data);

	/* Parse Global Positioning System Fix Data */
	__GPGGAData ParseGPGGAData(QString data);

	/* Parse GPS DOP and active satellites data*/
	__GPSGSAData ParseGPGSAData(QString data);

	/* Convert coordinates to appropriate format */
	QString ConvertLatLng(QString coord);
private:
	
};

#endif // GPSDATAPARSER_H
