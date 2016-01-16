#include "gpsdataparser.h"

GPSDataParser::GPSDataParser()
{

}

GPSDataParser::~GPSDataParser()
{

}

bool GPSDataParser::isValidSentence(QString data)
{
	QStringList dataSplitted = data.split(",");
	for (int i = 0; i < dataSplitted.size(); ++i)
	{
		QString token = dataSplitted.at(i);
		for (int c = 0; c < token.size(); c++)
		{
			if (!token.at(c).isLetterOrNumber() && !token.at(c).isSymbol() && !token.at(c).isPunct())
			{
				return false;
			}
		}
	}
	return true;
}

/* Parse GPS Satellites in view data */
__GPGSVData GPSDataParser::ParseGPGSVData(QString data)
{
	/*
	eg. $GPGSV,3,1,11,03,03,111,00,04,15,270,00,06,01,010,00,13,06,292,00*74
	$GPGSV,3,2,11,14,25,170,00,16,57,208,39,18,67,296,40,19,40,246,00*74
	$GPGSV,3,3,11,22,42,067,42,24,14,311,43,27,05,244,00,,,,*4D
	$GPGSV,1,1,13,02,02,213,,03,-3,000,,11,00,121,,14,13,172,05*67


	1    = Total number of messages of this type in this cycle
	2    = Message number
	3    = Total number of SVs in view
	4    = SV PRN number
	5    = Elevation in degrees, 90 maximum
	6    = Azimuth, degrees from true north, 000 to 359
	7    = SNR, 00-99 dB (null when not tracking)
	8-11 = Information about second SV, same as field 4-7
	12-15= Information about third SV, same as field 4-7
	16-19= Information about fourth SV, same as field 4-7
	*/

	listParsedData = QString(data).split(",");

	GPGSVData.totalMsgNumber = 0;
	GPGSVData.totalMsgNumber = (listParsedData.at(1).isEmpty()) ? 0 : listParsedData.at(1).toInt();

	GPGSVData.msgNumber = 0;
	GPGSVData.msgNumber = (listParsedData.at(2).isEmpty()) ? 0 : listParsedData.at(2).toInt();

	GPGSVData.totalSVinView = 0;
	GPGSVData.totalSVinView = (listParsedData.at(3).isEmpty()) ? 0 : listParsedData.at(3).toInt();

	for (int i = 0, c = 0; i < 4; i++, c += 4)
	{
		GPGSVData.svPRNNumber[i] = 0;
		GPGSVData.svPRNNumber[i] = (listParsedData.at(c).isEmpty()) ? 0 : listParsedData.at(c).toInt();

		GPGSVData.elevationDegrees[i] = 0;
		GPGSVData.elevationDegrees[i] = (listParsedData.at(c + 1).isEmpty()) ? 0 : listParsedData.at(c + 1).toFloat();

		GPGSVData.azimuth[i] = 0;
		GPGSVData.azimuth[i] = (listParsedData.at(c + 2).isEmpty()) ? 0 : listParsedData.at(c + 2).toFloat();

		GPGSVData.snr[i] = 0;
		if ((c + 3) == listParsedData.size())
		{
			GPGSVData.snr[i] = (listParsedData.at(c + 3).isEmpty()) ? 0 : listParsedData.at(c + 3).split("*").at(0).toFloat();
		}
		else{
			GPGSVData.snr[i] = (listParsedData.at(i + 3).isEmpty()) ? 0 : listParsedData.at(i + 3).toFloat();
		}

	}
	return GPGSVData;
}

/* Parse recommended minimum specific GPS/Transit data */
__GPRMCData GPSDataParser::ParseGPRMCData(QString data)
{
	/*
	Recommended minimum specific GPS/Transit data

	eg1. $GPRMC,081836,A,3751.65,S,14507.36,E,000.0,360.0,130998,011.3,E*62
	eg2. $GPRMC,225446,A,4916.45,N,12311.12,W,000.5,054.7,191194,020.3,E*68


	225446       Time of fix 22:54:46 UTC
	A            Navigation receiver warning A = OK, V = warning
	4916.45,N    Latitude 49 deg. 16.45 min North
	12311.12,W   Longitude 123 deg. 11.12 min West
	000.5        Speed over ground, Knots
	054.7        Course Made Good, True
	191194       Date of fix  19 November 1994
	020.3,E      Magnetic variation 20.3 deg East
	*68          mandatory checksum


	eg3. $GPRMC,220516,A,5133.82,N,00042.24,W,173.8,231.8,130694,004.2,W*70
	1    2    3    4    5     6    7    8      9     10  11 12


	1   220516     Time Stamp
	2   A          validity - A-ok, V-invalid
	3   5133.82    current Latitude
	4   N          North/South
	5   00042.24   current Longitude
	6   W          East/West
	7   173.8      Speed in knots
	8   231.8      True course
	9   130694     Date Stamp
	10  004.2      Variation
	11  W          East/West
	12  *70        checksum


	eg4. $GPRMC,hhmmss.ss,A,llll.ll,a,yyyyy.yy,a,x.x,x.x,ddmmyy,x.x,a*hh
	1    = UTC of position fix
	2    = Data status (V=navigation receiver warning)
	3    = Latitude of fix
	4    = N or S
	5    = Longitude of fix
	6    = E or W
	7    = Speed over ground in knots
	8    = Track made good in degrees True
	9    = UT date
	10   = Magnetic variation degrees (Easterly var. subtracts from true course)
	11   = E or W
	12   = Checksum

	*/
	listParsedData = QString(data).split(",");

	GPRMCData.utcPosFix = 0;
	GPRMCData.utcPosFix = (listParsedData.at(1).isEmpty()) ? 0 : listParsedData.at(1).toInt();

	GPRMCData.dataStatus = QString("");
	GPRMCData.dataStatus = (listParsedData.at(2).isEmpty()) ? QString("") : listParsedData.at(2);

	GPRMCData.latitudeOfFix = QString("");
	GPRMCData.latitudeOfFix = (listParsedData.at(3).isEmpty()) ? QString("") : ConvertLatLng(listParsedData.at(3));

	GPRMCData.ns = QString("");
	GPRMCData.ns = (listParsedData.at(4).isEmpty()) ? QString("") : listParsedData.at(4);

	GPRMCData.longitudeOfFix = QString("");
	GPRMCData.longitudeOfFix = (listParsedData.at(5).isEmpty()) ? 0 : ConvertLatLng(listParsedData.at(5));

	GPRMCData.ew = QString("");
	GPRMCData.ew = (listParsedData.at(6).isEmpty()) ? QString("") : listParsedData.at(6);

	GPRMCData.speedOverGroundInKnots = 0;
	GPRMCData.speedOverGroundInKnots = (listParsedData.at(7).isEmpty()) ? 0 : listParsedData.at(7).toFloat();

	GPRMCData.trueCourse = QString("");
	GPRMCData.trueCourse = (listParsedData.at(8).isEmpty()) ? QString("") : listParsedData.at(8);

	GPRMCData.universalTime = 0;
	GPRMCData.universalTime = (listParsedData.at(9).isEmpty()) ? 0: listParsedData.at(9).toInt();

	GPRMCData.magneticVariation = QString("");
	GPRMCData.magneticVariation = (listParsedData.at(10).isEmpty()) ? QString("") : listParsedData.at(10);

	GPRMCData.magneticVariationEW = QString("");
	GPRMCData.magneticVariationEW = (listParsedData.at(11).isEmpty()) ? QString("") : listParsedData.at(11);

	GPRMCData.checkSum = QString("");

	GPRMCData.checkSum = (listParsedData.at(12).isEmpty()) ? QString("") : (!listParsedData.at(12).contains("*")) ? QString("") : listParsedData.at(12).split("*").at(1);

	return GPRMCData;
}

/* Parse Global Positioning System Fix Data */
__GPGGAData GPSDataParser::ParseGPGGAData(QString data)
{
	/*
	Global Positioning System Fix Data

	eg. $GPGGA,hhmmss.ss,llll.ll,a,yyyyy.yy,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx*hh
	1    = UTC of Position
	2    = Latitude
	3    = N or S
	4    = Longitude
	5    = E or W
	6    = GPS quality indicator (0=invalid; 1=GPS fix; 2=Diff. GPS fix)
	7    = Number of satellites in use [not those in view]
	8    = Horizontal dilution of position
	9    = Antenna altitude above/below mean sea level (geoid)
	10   = Meters  (Antenna height unit)
	11   = Geoidal separation (Diff. between WGS-84 earth ellipsoid and
	mean sea level.  -=geoid is below WGS-84 ellipsoid)
	12   = Meters  (Units of geoidal separation)
	13   = Age in seconds since last update from diff. reference station
	14   = Diff. reference station ID#
	15   = Checksum
	*/
	listParsedData = QString(data).split(",");

	GPGGAData.utcOfPosition = 0;
	GPGGAData.utcOfPosition = (listParsedData.at(1).isEmpty()) ? 0 : listParsedData.at(1).toInt();

	GPGGAData.latitude = QString("");
	GPGGAData.latitude = (listParsedData.at(2).isEmpty()) ? QString("") : ConvertLatLng(listParsedData.at(2));

	GPGGAData.ns = QString("");
	GPGGAData.ns = (listParsedData.at(3).isEmpty()) ? QString("") : listParsedData.at(3);

	GPGGAData.longitude = QString("");
	GPGGAData.longitude = (listParsedData.at(4).isEmpty()) ? QString("") : ConvertLatLng(listParsedData.at(4));

	GPGGAData.ew = QString("");
	GPGGAData.ew = (listParsedData.at(5).isEmpty()) ? QString("") : listParsedData.at(5);

	GPGGAData.gpsQualityIndicator = 0;
	GPGGAData.gpsQualityIndicator = (listParsedData.at(6).isEmpty()) ? 0 : listParsedData.at(6).toInt();

	GPGGAData.numberOfSatInUse = 0;
	GPGGAData.numberOfSatInUse = (listParsedData.at(7).isEmpty()) ? 0 : listParsedData.at(7).toInt();

	GPGGAData.horizontalDilutionOfPosition = QString("");
	GPGGAData.horizontalDilutionOfPosition = (listParsedData.at(8).isEmpty()) ? QString("") : listParsedData.at(8);

	GPGGAData.antennaAltitudeAboveBelow = 0;
	GPGGAData.antennaAltitudeAboveBelow = (listParsedData.at(9).isEmpty()) ? 0 : listParsedData.at(9).toFloat();

	GPGGAData.antennaHeightInMeter = 0;
	GPGGAData.antennaHeightInMeter = (listParsedData.at(10).isEmpty()) ? 0 : listParsedData.at(10).toFloat();

	GPGGAData.geoidalSeparation = QString("");
	GPGGAData.geoidalSeparation = (listParsedData.at(11).isEmpty()) ? QString("") : listParsedData.at(11);

	GPGGAData.geoidalSeparationInMeters = 0;
	GPGGAData.geoidalSeparationInMeters = (listParsedData.at(12).isEmpty()) ? 0 : listParsedData.at(12).toFloat();

	GPGGAData.ageInSecondsSinceLastUpdate = 0;
	GPGGAData.ageInSecondsSinceLastUpdate = (listParsedData.at(13).isEmpty()) ? 0 : listParsedData.at(13).toFloat();

	GPGGAData.diffReferenceStationIDNo = 0;
	GPGGAData.diffReferenceStationIDNo = (listParsedData.at(14).isEmpty()) ? 0 : listParsedData.at(14).split("*").at(0).toFloat();

	GPGGAData.checksum = QString("");
	GPGGAData.checksum = (listParsedData.at(14).isEmpty()) ? QString("") : (!listParsedData.at(14).contains("*")) ? QString("") : listParsedData.at(14).split("*").at(1);

	return GPGGAData;
}

/* Parse GPS DOP and active satellites data*/
__GPSGSAData GPSDataParser::ParseGPGSAData(QString data)
{
	/*
	GPS DOP and active satellites

	eg1. $GPGSA,A,3,,,,,,16,18,,22,24,,,3.6,2.1,2.2*3C
	eg2. $GPGSA,A,3,19,28,14,18,27,22,31,39,,,,,1.7,1.0,1.3*35


	1    = Mode:
	M=Manual, forced to operate in 2D or 3D
	A=Automatic, 3D/2D
	2    = Mode:
	1=Fix not available
	2=2D
	3=3D
	3-14 = IDs of SVs used in position fix (null for unused fields)
	15   = PDOP
	16   = HDOP
	17   = VDOP

	*/

	listParsedData = QString(data).split(",");

	GPSGSAData.mode1 = QString("");
	GPSGSAData.mode1 = listParsedData.at(1);

	GPSGSAData.mode2 = 0;
	GPSGSAData.mode2 = listParsedData.at(2).toInt();
	for (int i = 3, c = 0; i <= 14; i++, c++)
	{
		GPSGSAData.svUsedInPosFixID[c] = 0;
		GPSGSAData.svUsedInPosFixID[c] = (listParsedData.at(i).isEmpty()) ? -1 : listParsedData.at(i).toInt();
	}

	GPSGSAData.pdop = 0;
	GPSGSAData.pdop = (listParsedData.at(15).isEmpty()) ? 0 : listParsedData.at(15).toFloat();

	GPSGSAData.hdop = 0;
	GPSGSAData.hdop = (listParsedData.at(16).isEmpty()) ? 0 : listParsedData.at(16).toFloat();

	GPSGSAData.vdop = 0;
	GPSGSAData.vdop = (listParsedData.at(17).isEmpty()) ? 0 : listParsedData.at(17).split("*").at(0).toFloat();

	return GPSGSAData;
}

QString GPSDataParser::ConvertLatLng(QString coord)
{
	QString coordinate;
	if (9 == coord.size())
	{
		/* Latitude */
		for (int i = 0, c = 0; i < coord.size() && c < coord.size(); i++, c++)
		{
			if (i == 0 && '0' == coord[i])
			{
				c++;
				coordinate[i] = coord[c];
			}
			else if (i == 2)
			{
				coordinate[i] = '.';
				i++;
				coordinate[i] = coord[c];
			}
			else if ('.' == coord[c])
			{
				c++;
				coordinate[i] = coord[c];
			}
			else
			{
				coordinate[i] = coord[c];
			}
		}

	}
	else if (10 == coord.size())
	{
		/* Longitude */
		for (int i = 0, c = 0; i < coord.size() && c < coord.size(); i++, c++)
		{
			if (i == 0 && '0' == coord[i])
			{
				c++;
				coordinate[i] = coord[c];
			}
			else if (i == 2)
			{
				coordinate[i] = '.';
				i++;
				coordinate[i] = coord[c];
			}
			else if ('.' == coord[c])
			{
				c++;
				coordinate[i] = coord[c];
			}
			else
			{
				coordinate[i] = coord[c];
			}
		}
	}
	return coordinate;
}