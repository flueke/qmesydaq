/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2012 by Jens Kr�ger <jens.krueger@frm2.tum.de>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include <QObject>
#include <QRect>
#include <QFileInfo>

#include "libqmesydaq_global.h"
#include "counter.h"
#include "structures.h"
#include "mdefines.h"
#include "mesydaq2.h"

class Histogram;
class Spectrum;
class MapCorrection;
class Mesydaq2;
class QTextStream;

/**
 * \short representation of a measurement

   \author Gregor Montermann <g.montermann@mesytec.com>
*/
class LIBQMESYDAQ_EXPORT Measurement : public QObject
{
	Q_OBJECT
	Q_ENUMS(Mode)
	Q_ENUMS(HistogramType)
	Q_ENUMS(SpectrumType)
	Q_ENUMS(Status)

	//! stores the current mode of the measurement
	Q_PROPERTY(Mode m_mode READ mode)

	//! stores the histogram file name
	Q_PROPERTY(QString m_histfilename READ getHistfilename WRITE setHistfilename)
	//! stores the default path for histogram files
	Q_PROPERTY(QString m_histPath READ getHistfilepath WRITE setHistfilepath)
	//! stores the calibration file name
	Q_PROPERTY(QString m_calibrationfilename READ getCalibrationfilename WRITE setCalibrationfilename)
	//! stores the default path for listmode data files
	Q_PROPERTY(QString m_listPath READ getListfilepath WRITE setListfilepath)
	//! stores the default path for configuration files
	Q_PROPERTY(QString m_configPath READ getConfigfilepath WRITE setConfigfilepath)

        //! stores the currently loaded configfile name
	Q_PROPERTY(QString  m_configfile READ getConfigfilename WRITE setConfigfilename)

	//! stores the height of the histograms
	Q_PROPERTY(quint16 m_height READ height)
	//! stores the width of the histograms
	Q_PROPERTY(quint16 m_width READ width)

public:
	//! Defines the current mode values of a measurement 
	enum Mode {
		DataAcquisition = 0, 	//!< data will be taken from the hardware
		ReplayListFile,		//!< data will be taken from a list mode file
		HistogramLoad,		//!< data will be taken from a histogram file
	};

	//! Defines the histogram type
	enum HistogramType {
		PositionHistogram = 0,		//!< raw position histogram
		AmplitudeHistogram,		//!< raw amplitude histogram
		CorrectedPositionHistogram,	//!< corrected position histogram
	};

	//! Defines the spectrum type
        
	enum SpectrumType {
		TimeSpectrum = 0,	//!< ???
		Diffractogram,		//!< spectrum over all vertical rows
		TubeSpectrum,		//!< spectrum along a tube 
		XSpectrum,		//!< ???
		YSpectrum,		//!< ???
		EnergySpectrum,		//!< ???
		SingleTubeSpectrum,	//!< for MSTD-16
		NoSpectrum,		//!< Do not change this entry, this must be the last entry
	};

	//! Defines the DAQ status
	enum Status {
		Idle = IDLE,	//!< no acquistion 
		Running,	//!< data acquisition is running
		Started,	//!< data acquisition is requested to run
		Stopped,	//!< data acquisition is requested to stop
	};

	//! Defines the setup type
	enum Setup {
		Mpsd = 0,	//!< MPSD in standard configuration in a 2D arrangement
		Mdll,		//!< MDLL connected
		Mstd,		//!< MSTD-16 
	};	

public:
	Measurement(Mesydaq2 *mesy, QObject *parent = 0);

	~Measurement();

	void resizeHistogram(const quint16 w, const quint16 h, const bool = true, const bool = false);

	//! \return the width of the histogram
	quint16 width(void) const;

	//! \return the height of the histogram
	quint16 height(void) const;

	quint64	getMeastime(void) const;

	void	setCurrentTime(const quint64 msecs);
	void	stop(void);
	void	start(void);
	void	cont(void);

	void	calcMeanRates(void);
	quint64	getRate(const quint8 cNum) const;

	void	setCounter(const quint32 cNum, const quint64 val);
	quint64	getCounter(const quint8 cNum) const;

	quint8	isOk(void) const;
	void	setOnline(const bool truth);

	quint64	getPreset(const quint8 cNum);
	void	setPreset(const quint8 cNum, const quint64 prval, const bool mast);

	void	setListmode(const bool truth);
	void	setRemote(const bool truth);
	bool	remoteStart(void) const;

	bool	isMaster(const quint8 cNum) const;
	void	clearCounter(const quint8 cNum);

	//! \return stream the data into a separate file too
	bool	acqListfile(void) const;

	void	readListfile(const QString &readfilename);

	//! \returns the number of counts in the defined ROI
	quint64	getROICounts(void) const;

	void 	setROI(const QRectF &r);

        QRect	getROI(void) const;

	/** 
		gets the value of the defined monitor 1
		\todo monitor mapping configuration
		\return counter value for the monitor 1 
	 */
	quint64	mon1() const 
	{
		return m_counter[MON1ID]->value();
	}

	/** 
		gets the value of the defined monitor 2
		\todo monitor mapping configuration
		\return counter value for the monitor 2 
	 */
	quint64	mon2() const 
	{
		return m_counter[MON2ID]->value();
	}
	
	/** 
		gets the value of the defined monitor 2
		\todo monitor mapping configuration
		\return counter value for the monitor 2 
	 */
	quint64	mon3() const 
	{
		return m_counter[MON3ID]->value();
	}
	
	/** 
		gets the value of the defined monitor 2
		\todo monitor mapping configuration
		\return counter value for the monitor 2 
	 */
	quint64	mon4() const 
	{
		return m_counter[MON4ID]->value();
	}
	
	/** 
		gets the value of the defined event counter
		\todo counter mapping configuration
		\return counter value for the event counter
	 */
	quint64	events() const 
	{
		return m_counter[EVID]->value();
	}

	/** 
		gets the value of the timer
		\todo counter mapping configuration
		\return counter value for the event counter
	 */
	quint64	timer() const 
	{
		return m_counter[TIMERID]->value();
	}

	/** 
		gets the number of all events in the selected ROI
		\todo counter mapping configuration
		\return events in ROI 
	 */
	quint64 eventsInROI(const HistogramType t);

	//! \return time of the last header read from interface
	quint64 getHeadertime(void) const 
	{
		return m_headertime;
	}

	void writeHistograms(const QString &name);

	void readHistograms(const QString &name);

	void readCalibration(const QString &name);

	void clearAllHist(void);

	void clearChanHist(const quint16 chan);

	Spectrum *data(const HistogramType t, const quint16 line);
	Spectrum *data(const HistogramType t);

	/**
	    \param t type of the requested histogram
            \return a histogram
	 */
	Histogram *hist(const HistogramType t) const;

	/**
	  \param t type of the requested spectrum
	  \return a spectrum
         */
	Spectrum *spectrum(const SpectrumType t);

	//! \return the mapping and correction data for position histogram
	MapCorrection*& posHistMapCorrection() { return m_posHistMapCorrection; }

	void getMean(const HistogramType t, float &, float &);
	void getMean(const HistogramType t, quint16, float &, float &);
	void getMean(const SpectrumType t, float &, float &);

	//! \brief store header for list mode file
	void setListFileHeader(const QByteArray& header);

// run ID oriented methods
	//! \return the current run ID
	quint16 runId(void) const 
	{ 
		return m_mesydaq->runId(); 
	}

	/*!
            sets the runid for the measurement
            \param runid
	 */
	void setRunId(const quint16 runid)
	{
		m_mesydaq->setRunId(runid);
	}

	//! returns the current operation mode
	Mode mode(void) const 
	{
		return m_mode;
	}

// calibration file oriented methods
	/**
	 * sets the file name of a calibration data file
         *
         * \param name name of the next histogram data file
         */
	void setCalibrationfilename(const QString &name);

	//! \return name of the current calibration data file
	QString getCalibrationfilename(void) const 
	{
		return m_calibrationfilename;
	}

// histogram file oriented methods
	/**
	 * sets the path for the histogram data files
	 *
	 * \param path to the histogram data files
	 */
	void setHistfilepath(const QString &path); 

	//! \return path to store all histogram data files
	QString getHistfilepath(void) const 
	{
		return m_histPath;
	}

	/**
	 * sets the file name of a histogram data file
         *
         * \param name name of the next histogram data file
         */
	void setHistfilename(const QString &name);

	//! \return name of the current histogram data file
	QString getHistfilename(void) const 
	{
		return m_histfilename;
	}

// list mode oriented methods
	/**
	 * sets the path for the list mode data files
	 *
	 * \param path to the list mode data files
	 */
	void setListfilepath(const QString &path);

	//! \return path to store all list mode data files
	QString getListfilepath(void) const 
	{
		return m_listPath;
	}

// configuration file oriented methods
	/**
	 * sets the path for the config files
	 *
	 * \param path to the config files
	 */
	void setConfigfilepath(const QString &path); 

	//! \return path to store all config files
	QString getConfigfilepath(void) const 
	{
		return m_configPath;
	}

	/**
	 * sets the config file name
	 *
	 * \param name config file name
	 */
	void setConfigfilename(const QString &name);

	//! \return last loaded config file name
	QString getConfigfilename(void) const;

	bool loadSetup(const QString &name);

	bool saveSetup(const QString &name);

        //! returns the current setup type of the configuration
	Setup setupType(void) const 
	{
		return m_setup;
	}

	/**
         * sets the setup type of the found configuration
	 *
	 * \param val setup type
	 */
	void setSetupType(const Setup val);
	
	
public slots:
	void analyzeBuffer(const DATA_PACKET &pd);

	void calcRates();

private slots:
	void requestStop(void);

signals:
	//! will be emitted if one of the master counter reaches its limit
	void stopSignal(bool = false);

	//! will be emitted in case of change in respect to the handling of list mode data files
	void acqListfile(bool);

protected:
	void timerEvent(QTimerEvent *event);

private:
	void fillHistogram(QTextStream &t, Histogram *hist);

	void destroyHistogram(void);

	void storeLastFile(void);

	bool getNextBlock(QDataStream &datStream, DATA_PACKET &dataBuf);

private:
	//! separator in the list mode data file
	static const quint16  	sep0 = 0x0000;
	//! separator in the list mode data file
	static const quint16  	sep5 = 0x5555;    
	//! separator in the list mode data file
	static const quint16  	sepA = 0xAAAA;
	//! separator in the list mode data file
	static const quint16  	sepF = 0xFFFF;

	//! Access to hardware
	Mesydaq2	*m_mesydaq;

	//! histogram buffer
	Histogram	*m_Hist[3];

	//! time spectrum
	Spectrum	*m_Spectrum[NoSpectrum];

	//! mapping and correction data for position histogram
	MapCorrection   *m_posHistMapCorrection;

	//! time stamp for the start of measurement
	quint64 	m_starttime_msec;
	
	//! time for the duration of measurement
	quint64 	m_meastime_msec;

	//! current state of measurement
	quint8 		m_status;

	//! are we only or not
	bool 		m_online;
	
	//! it's set to true and nothing else ????
	bool 		m_working; 		

	//! stores the info whether it will be controlled remotely or not
	bool 		m_remote;

	//! stores the header timer of the current data package
	quint64		m_headertime;

	//! definitions of the counters
	QHash<int, MesydaqCounter *>	m_counter; // [TIMERID + 1];

	//! timer for the ratemeter 
	int		m_rateTimer;

	//! timer for online checking
	int 		m_onlineTimer;

	//! stores the ROI
	QRect		m_roi;

	//! contains the number of received data packages
	quint32		m_packages;

	//! contains the number of received trigger events
	quint64		m_triggers;

	quint16		m_width;

	quint16		m_height;

	Mode		m_mode;

	//! currently used histogram file
	QString 	m_histfilename;

	//! histogram data file name path
	QString 	m_histPath;

	//! currently used list mode file
	QString		m_listfilename;

	//! list mode data file name path
	QString 	m_listPath;

	//! config file name path
	QString 	m_configPath;

	//! currently used config file
	QFileInfo 	m_configfile;

	//! currently used calibration file
	QString		m_calibrationfilename;

	//! number of triggers
	quint64 	g_triggers;

	//! number of neutrons
	quint64		m_neutrons;

	Setup		m_setup;
};

#endif
