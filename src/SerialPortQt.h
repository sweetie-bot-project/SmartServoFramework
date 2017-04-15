/*!
 * This file is part of SmartServoFramework.
 * Copyright (c) 2014, INRIA, All rights reserved.
 *
 * SmartServoFramework is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this software. If not, see <http://www.gnu.org/licenses/lgpl-3.0.txt>.
 *
 * \file SerialPortQt.h
 * \date 28/08/2016
 * \author Emeric Grange <emeric.grange@gmail.com>
 */

#ifndef SERIALPORT_QT_H
#define SERIALPORT_QT_H

#include <QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)

#include "SerialPort.h"

// Qt modules
#include <QtSerialPort>
#include <QLockFile>

/*!
 * \brief The serial ports scanner function.
 * \param[out] availableSerialPorts: A list of serial port nodes (ex: /dev/ttyUSB0).
 * \return The number of serial ports found.
 *
 * \todo Use stat() instead of open().
 *
 * Scans for /dev/ttyUSB* and /dev/ttyACM* ports. Regular serial devices on
 * /dev/ttyS* are not scanned as they are always considered as valid even with
 * no USB2Dynamixel / USB2AX or TTL adapter attached.
 */
int serialPortsScannerQt(std::vector <std::string> &availableSerialPorts);

/*!
 * \brief The SerialPortQt class.
 */
class SerialPortQt: public SerialPort
{
    QSerialPort *serial = NULL;
    QLockFile *lock = NULL;

    /*!
     * \brief Get current time since the Epoch.
     * \return Current time since the Epoch in milliseconds.
     */
    double getTime();

    /*!
     * \brief Set baudrate for this interface.
     * \param baud: Can be a 'baudrate' (in bps) or a Dynamixel / HerkuleX 'baudnum'.
     *
     * Must be called before openLink(), otherwise it will have no effect until the
     * next connection.
     */
    void setBaudRate(const int baud);

    /*!
     * \brief SerialPortQt::convertBaudRateFlag.
     * \param baudrate: baudrate in baud.
     * \return A baudRateFlag representing the target speed of the serial device, from a <termios.h> enum.
     *
     * This function will try to match a baudrate with an existing baudrate flag,
     * or at least one +/- 1.5% close. When this is not possible, the 'ttyCustomSpeed'
     * flag is set, and the openLink() function will try to set a custom speed.
     */
    int convertBaudRateFlag(int baudrate);

    /*!
     * \brief Check if the serial device has been locked by another instance or program.
     * \return True if a lock has been found for this serial device, false otherwise.
     */
    bool isLocked();

    /*!
     * \brief Set a lock for this serial device.
     * \return True if a lock has been placed successfully for this serial device, false otherwise.
     *
     * We have several ways of doing that:
     * - Use flock(). Should work on Linux and Mac if carefully implemented. Not working ATM.
     * - Use a file lock in "/tmp" directory. Will only work accross SmartServoFramework instances.
     * - Use a file lock in "/var/lock/lockdev" directory. Almost standard way of locking devices, but need "lock" group credential.
     * - Use a file lock with lockdev library. Standard way of locking devices, but need "lockdev" library.
     */
    bool setLock();

    /*!
     * \brief Remove the lock we put on this serial device.
     * \return True if the lock has been removed successfully for this serial device, false otherwise.
     */
    bool removeLock();

public:
    /*!
     * \brief SerialPortQt constructor will only init some variables to default values.
     * \param devicePath: The path to the serial device (ex: /dev/ttyUSB0") (cannot be changed after the constructor).
     * \param baud: Can be a 'baudrate' (in bps) or a Dynamixel / HerkuleX 'baudnum'.
     * \param serialDevice: Specify (if known) what TTL converter is in use.
     * \param servoDevices: Specify if we use this serial port with Dynamixel or HerkuleX devices.
     *
     * \note: devicePath can be set to "auto", serial port autodetection will be
     * triggered, and the first serial port available will be used.
     */
    SerialPortQt(std::string &devicePath, const int baud, const int serialDevice = SERIAL_UNKNOWN, const int servoDevices = SERVO_UNKNOWN);
    ~SerialPortQt();

    int openLink();
    bool isOpen();
    void closeLink();

    int tx(unsigned char *packet, int packetLength);
    int rx(unsigned char *packet, int packetLength);
    void flush();

    /*!
     * \brief switchHighSpeed() should enable ASYNC_LOW_LATENCY flag and reduce latency_timer per tty device (need root credential)
     * \return True in case of success.
     * \todo This function is not implemented yet.
     * \todo latency_timer value auto-detection doesn't produce intended result yet.
     */
    bool switchHighSpeed();

    void setLatency(int latency);
    void setTimeOut(int packetLength);
    void setTimeOut(double msec);
    int checkTimeOut();
};

#endif /* QT_VERSION >= QT_VERSION_CHECK(5, 7, 0) */

#endif /* SERIALPORT_QT_H */