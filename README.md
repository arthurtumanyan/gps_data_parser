# gps_data_parser

This is an attempt to parse NMEA compatible data received from gps receiver (tested on G-STAR IV) using QT5.

Use the following scrap of code as a usage example:

	QLabel *labelTotalMsgNumber;
	QLabel *labelMsgNumber;
	QLabel *labelTotalSVinView;

	QLabel *labelSvPRNnumber[4];

	QLabel *labelElevationDegree[4];

	QLabel *labelAzimuth[4];

	QLabel *labelSNR[4];

	QLabel *labelMode1;

	QLabel *labelMode2;

	QLabel *labelSvUsed[12];

	QLabel *labelPdop;

	QLabel *labelHdop;

	QLabel *labelVdop;
	
	QLabel *labelLatitude;
	
	QLabel *labelLongitude;

	QLabel *labelGpsQualityIndicator;

	QLabel *labelNumberOfSatInUse;

	QLabel *labelAntennaAltitudeAboveBelow;

	QLabel *labelGeoidalSeparation;

	QLabel *labelDataStatus;

	QLabel *labelSpeedOverGroundInKnots;

	QLabel *labelTrueCourse;


	void MainWindow::parseAndShow(const QByteArray &data)
	{
	QString dataType = QString(data);
	QStringList splitted = dataType.split(",");

	if (12 > splitted.size())
	{
		return;
	}

	if (!gpsDataParser->isValidSentence(data))
	{
		return;
	}

	if (0 == splitted.at(0).compare("$GPGSV", Qt::CaseSensitive))
	{
		if (20 != splitted.size())
		{
			return;
		}
		__GPGSVData gpsData = gpsDataParser->ParseGPGSVData(data);
		labelTotalMsgNumber->setText(QString("%1").arg(gpsData.totalMsgNumber));
		labelMsgNumber->setText(QString("%1").arg(gpsData.msgNumber));
		labelTotalSVinView->setText(QString("%1").arg(gpsData.totalSVinView));
		for (int i = 0; i < 4; i++)
		{
			labelSvPRNnumber[i]->setText(QString("%1").arg(gpsData.svPRNNumber[i]));

			labelElevationDegree[i]->setText(QString("%1 %2").arg(gpsData.elevationDegrees[i]).arg(QChar(0260)));

			labelAzimuth[i]->setText(QString("%1 %2").arg(gpsData.azimuth[i]).arg(QChar(0260)));

			labelSNR[i]->setText(QString("%1 dB").arg(gpsData.snr[i]));
		}
	}
	else if (0 == splitted.at(0).compare("$GPRMC", Qt::CaseSensitive))
	{
		if (13 != splitted.size())
		{
			return;
		}
		__GPRMCData gpsData = gpsDataParser->ParseGPRMCData(data);

		if (gpsData.dataStatus.isEmpty())
		{
			labelDataStatus->setText("-");
		}
		else
		{
			labelDataStatus->setText(QString("%1").arg((0 == gpsData.dataStatus.compare("V", Qt::CaseSensitive)) ? "Warning" : "OK"));
		}


		labelSpeedOverGroundInKnots->setText(QString("%1 km/hr").arg(gpsData.speedOverGroundInKnots * 1.852));
		if (gpsData.trueCourse.isEmpty())
		{
			labelTrueCourse->setText("-");
		}
		else
		{
			labelTrueCourse->setText(QString("%1%2").arg(gpsData.trueCourse).arg(QChar(0260)));
		}

	}
	else if (0 == splitted.at(0).compare("$GPGGA", Qt::CaseSensitive))
	{
		if (15 != splitted.size())
		{
			return;
		}
		__GPGGAData gpsData = gpsDataParser->ParseGPGGAData(data);

		if (gpsData.latitude.isEmpty())
		{
			labelLatitude->setText("-");
		}
		else
		{
			labelLatitude->setText(QString("%1%2 %3").arg(gpsData.latitude).arg(QChar(0260)).arg(gpsData.ns));
		}

		if (gpsData.longitude.isEmpty())
		{
			labelLongitude->setText("-");
		}
		else
		{
			labelLongitude->setText(QString("%1%2 %3").arg(gpsData.longitude).arg(QChar(0260)).arg(gpsData.ew));
		}

		mapView->setCoordinates(gpsData.latitude, gpsData.longitude);

		if (0 == gpsData.gpsQualityIndicator)
		{
			labelGpsQualityIndicator->setText("Invalid");
		}
		else if (1 == gpsData.gpsQualityIndicator)
		{
			labelGpsQualityIndicator->setText("GPS fix");
		}
		else if (2 == gpsData.gpsQualityIndicator)
		{
			labelGpsQualityIndicator->setText("Diff GPS fix");
		}
		else
		{
			labelGpsQualityIndicator->setText("-");
		}

		labelNumberOfSatInUse->setText(QString("%1").arg(gpsData.numberOfSatInUse));
		labelAntennaAltitudeAboveBelow->setText(QString("%1").arg(gpsData.antennaAltitudeAboveBelow));
		labelGeoidalSeparation->setText(QString("%1").arg(gpsData.geoidalSeparation));

	}
	else if (0 == splitted.at(0).compare("$GPGSA", Qt::CaseSensitive))
	{
		if (18 != splitted.size())
		{
			return;
		}
		__GPSGSAData gpsData = gpsDataParser->ParseGPGSAData(data);

		if (0 == gpsData.mode1.compare("M"))
		{
			labelMode1->setText("Manual");
		}
		else if (0 == gpsData.mode1.compare("A"))
		{
			labelMode1->setText("Automatic");
		}
		else
		{
			labelMode1->setText("-");
		}

		if (1 == gpsData.mode2)
		{
			labelMode2->setText("Fix not available");
		}
		else if (2 == gpsData.mode2)
		{
			labelMode2->setText("2D");
		}
		else if (3 == gpsData.mode2)
		{
			labelMode2->setText("3D");
		}
		else
		{
			labelMode2->setText("-");
		}

		labelPdop->setText(QString("%1").arg(gpsData.pdop));
		labelVdop->setText(QString("%1").arg(gpsData.vdop));
		labelHdop->setText(QString("%1").arg(gpsData.hdop));

		for (int i = 0; i < 12; i++)
		{
			if (0 > gpsData.svUsedInPosFixID[i])
			{
				labelSvUsed[i]->setStyleSheet("QLabel {background-color : red;}");
				labelSvUsed[i]->setText("");
			}
			else{
				labelSvUsed[i]->setText(QString("%1").arg(gpsData.svUsedInPosFixID[i]));
				labelSvUsed[i]->setStyleSheet("QLabel {background-color : green; border: none;}");
			}
		}
	}
	}
