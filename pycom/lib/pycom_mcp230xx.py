# The MIT License (MIT)
#
# Copyright (c) 2017 Tony DiCola for Adafruit Industries
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
"""
`pycom_mcp230xx`
====================================================
orginally written for
CircuitPython module for the MCP23017 and MCP23008 I2C I/O extenders.
Modified for use with the Pycom family of MCU Chips
* Author: Charles Coulton
"""
#import digitalio
from machine import I2C, Pin
#import adafruit_bus_device.i2c_device as i2c_device

from micropython import const


__version__ = "0.0.0-auto.0"
__repo__ = "https://github.com/adafruit/Adafruit_CircuitPython_MCP230xx.git"


#pylint: disable-msg=bad-whitespace
# Registers and other constants:
_MCP23008_ADDRESS       = const(0x20)
_MCP23008_IODIR         = const(0x00)
_MCP23008_IPOL          = const(0x01)
_MCP23008_GPINTEN       = const(0x02)
_MCP23008_DEFVAL        = const(0x03)
_MCP23008_INTCON        = const(0x04)
# - - sread disslw - odr intpol - 
_MCP23008_IOCON         = const(0x05)
_MCP23008_GPPU          = const(0x06)
_MCP23008_INTF          = const(0x07)
_MCP23008_INTCAP        = const(0x08)
_MCP23008_GPIO          = const(0x09)
_MCP23008_OLAT          = const(0x0A)

_MCP23017_ADDRESS       = const(0x20)
_MCP23017_IODIRA        = const(0x00)
_MCP23017_IODIRB        = const(0x01)
_MCP23017_IPOLA         = const(0x02)
_MCP23017_IPOLB         = const(0x03)
_MCP23017_GPINTENA      = const(0x04)
_MCP23017_GPINTENB      = const(0x05)
_MCP23017_DEFVALA       = const(0x06)
_MCP23017_DEFVALB       = const(0x07)
_MCP23017_INTCONA       = const(0x08)
_MCP23017_INTCONB       = const(0x09)
_MCP23017_IOCONA        = const(0x0A)
_MCP23017_IOCONB        = const(0x0B)
_MCP23017_GPPUA         = const(0x0C)
_MCP23017_GPPUB         = const(0x0D)
_MCP23017_INTFA         = const(0x0E)
_MCP23017_INTFB         = const(0x0F)
_MCP23017_INTCAPA       = const(0x10)
_MCP23017_INTCAPB       = const(0x11)
_MCP23017_GPIOA         = const(0x12)
_MCP23017_GPIOB         = const(0x13)
_MCP23017_OLATA         = const(0x14)
_MCP23017_OLATB         = const(0x15)

#pylint: enable-msg=bad-whitespace


# Internal helpers to simplify setting and getting a bit inside an integer.
def _get_bit(val, bit):
    return val & (1 << bit) > 0

def _enable_bit(val, bit):
    return val | (1 << bit)

def _clear_bit(val, bit):
    return val & ~(1 << bit)

# Global buffer for reading and writing registers with the devices.  This is
# shared between both the MCP23008 and MCP23017 class to reduce memory allocations.
# However this is explicitly not thread safe or re-entrant by design!
_BUFFER = bytearray(3)


class DigitalInOut:
    """Digital input/output of the MCP230xx.  The interface is exactly the
    same as the digitalio.DigitalInOut class (however the MCP230xx does not
    support pull-down resistors and an exception will be thrown
    attempting to set one).
    """

    def __init__(self, pin_number, mcp230xx):
        """Specify the pin number of the MCP230xx (0...7 for MCP23008, or 0...15
        for MCP23017) and MCP23008 instance.
        """
        self._pin = pin_number
        self._mcp = mcp230xx

    # kwargs in switch functions below are _necessary_ for compatibility
    # with DigitalInout class (which allows specifying pull, etc. which
    # is unused by this class).  Do not remove them, instead turn off pylint
    # in this case.
    #pylint: disable=unused-argument
    def switch_to_output(self, value=False, **kwargs):
        """Switch the pin state to a digital output with the provided starting
        value (True/False for high or low, default is False/low).
        """
        #self.direction = digitalio.Direction.OUTPUT
        self.direction = Pin.OUT
        self.value = value

    def switch_to_input(self, pull=None, **kwargs):
        """Switch the pin state to a digital input with the provided starting
        pull-up resistor state (optional, no pull-up by default).  Note that
        pull-down resistors are NOT supported!
        """
        #self.direction = digitalio.Direction.INPUT
        self.direction = Pin.IN
        self.pull = pull
    #pylint: enable=unused-argument

    @property
    def value(self):
        """The value of the pin, either True for high or False for
        low.  Note you must configure as an output or input appropriately
        before reading and writing this value.
        """
        return _get_bit(self._mcp.gpio, self._pin)

    @value.setter
    def value(self, val):
        if val:
            self._mcp.gpio = _enable_bit(self._mcp.gpio, self._pin)
        else:
            self._mcp.gpio = _clear_bit(self._mcp.gpio, self._pin)

    @property
    def direction(self):
        """The direction of the pin, either True for an input or
        False for an output.
        """
        if _get_bit(self._mcp.iodir, self._pin):
            return Pin.IN
        return Pin.OUT

    @direction.setter
    def direction(self, val):
        if val == Pin.IN:
            self._mcp.iodir = _enable_bit(self._mcp.iodir, self._pin)
        elif val == Pin.OUT:
            self._mcp.iodir = _clear_bit(self._mcp.iodir, self._pin)
        else:
            raise ValueError('Expected INPUT or OUTPUT direction!')

    @property
    def pull(self):
        """Enable or disable internal pull-up resistors for this pin.  A
        value of digitalio.Pull.UP will enable a pull-up resistor, and None will
        disable it.  Pull-down resistors are NOT supported!
        """
        if _get_bit(self._mcp.gppu, self._pin):
            return Pin.PULL_UP
        return None

    @pull.setter
    def pull(self, val):
        if val is None:
            self._mcp.gppu = _clear_bit(self._mcp.gppu, self._pin)
        elif val == Pin.PULL_UP:
            self._mcp.gppu = _enable_bit(self._mcp.gppu, self._pin)
        elif val == Pin.PULL_DOWN:
            raise ValueError('Pull-down resistors are not supported!')
        else:
            raise ValueError('Expected UP, DOWN, or None for pull state!')


class MCP23008:
    """Initialize MCP23008 instance on specified I2C bus and optionally
    at the specified I2C address.
    """

    def __init__(self, i2c, address=_MCP23008_ADDRESS):
        #self._device = i2c_device.I2CDevice(i2c, address)
        self._device = i2c
        self._address = address
        # Reset device state to all pins as inputs (safest option).
        # Write to MCP23008_IODIR register 0xFF followed by 9 zeros
        # for defaults of other registers.
        self._device.writeto_mem(address, _MCP23008_IODIR, '0xff')
        for i in range(1, 10):
            self._device.writeto(address, 0x00)

    def _read_u8(self, register):
        # Read an unsigned 8 bit value from the specified 8-bit register.
        return self._device.readfrom_mem(self._address, register, 1)


    def _write_u8(self, register, val):
        # Write an 8 bit value to the specified 8-bit register.
        self._device.writeto_mem(self._address, register, val)

    @property
    def gpio(self):
        #The raw GPIO output register.  Each bit represents the
        #output value of the associated pin (0 = low, 1 = high), assuming that
        #pin has been configured as an output previously.
        return self._read_u8(_MCP23008_GPIO)

    @gpio.setter
    def gpio(self, val):
        self._write_u8(_MCP23008_GPIO, val)

    @property
    def iodir(self):
        #The raw IODIR direction register.  Each bit represents
        #direction of a pin, either 1 for an input or 0 for an output mode
        return self._read_u8(_MCP23008_IODIR)

    @iodir.setter
    def iodir(self, val):
        self._write_u8(_MCP23008_IODIR, val)

    @property
    def gppu(self):
        #The raw GPPU pull-up register.  Each bit represents
        #if a pull-up is enabled on the specified pin (1 = pull-up enabled,
        #0 = pull-up disabled).  Note pull-down resistors are NOT supported
        return self._read_u8(_MCP23008_GPPU)

    @gppu.setter
    def gppu(self, val):
        self._write_u8(_MCP23008_GPPU, val)

    @property
    def defval(self):
        #The raw default value register. Sets that the default input should be.
        return self._read_u8(_MCP23008_DEFVAL)

    @defval.setter
    def defval(self, val):
        self._write_u8(_MCP23008_DEFVAL, val)

    @property
    def gpinten(self):
        return self._read_u8(_MCP23008_GPINTEN)

    @gpinten.setter
    def gpinten(self, val):
        self._write_u8(_MCP23008_GPINTEN, val)

    @property
    def intcon(self):
        return self._read_u8(_MCP23008_INTCON)

    @intcon.setter
    def intcon(self, val):
        self._write_u8(_MCP23008_INTCON, val)

    def get_pin(self, pin):
        #Convenience function to create an instance of the DigitalInOut class
        #pointing at the specified pin of this MCP23008 device.
        assert 0 <= pin <= 7
        return DigitalInOut(pin, self)


class MCP23017:
    #Initialize MCP23017 instance on specified I2C bus and optionally
    #at the specified I2C address

    def __init__(self, i2c, address=_MCP23017_ADDRESS):
        #self._device = i2c_device.I2CDevice(i2c, address)
        self._device = i2c
        self._address = address
        # Reset to all inputs with no pull-ups and no inverted polarity.
        self.iodir = 0xFFFF
        self.gppu = 0x0000
        self._write_u16le(_MCP23017_IPOLA, 0x0000)

    def _read_u16le(self, register):
        # Read an unsigned 16 bit little endian value from the specified 8-bit
        # register.
        self._device.readfrom_mem_into(self._address, register, _BUFFER)
        return (_BUFFER[1] << 8) | _BUFFER[0]

    def _write_u16le(self, register, val):
        # Write an unsigned 16 bit little endian value to the specified 8-bit
        # register.
        buffer = bytearray(2)
        buffer[0] = val & 0xFF
        buffer[1] = (val >> 8) & 0xFF
        self._device.writeto_mem(self._address, register, buffer)

    def _read_u8(self, register):
        # Read an unsigned 8 bit value from the specified 8-bit register.
        return self._device.readfrom_mem(self._address, register, 1)

    def _write_u8(self, register, val):
        # Write an 8 bit value to the specified 8-bit register.
        return self._device.writeto_mem(self._address, register, val)

    @property
    def gpio(self):
        #The raw GPIO output register.  Each bit represents the
        #output value of the associated pin (0 = low, 1 = high), assuming that
        #pin has been configured as an output previously.
        return self._read_u16le(_MCP23017_GPIOA)

    @gpio.setter
    def gpio(self, val):
        self._write_u16le(_MCP23017_GPIOA, val)

    @property
    def gpioa(self):
        #The raw GPIO A output register.  Each bit represents the
        #output value of the associated pin (0 = low, 1 = high), assuming that
        #pin has been configured as an output previously.
        return self._read_u8(_MCP23017_GPIOA)

    @gpioa.setter
    def gpioa(self, val):
        self._write_u8(_MCP23017_GPIOA, val)

    @property
    def gpiob(self):
        #The raw GPIO B output register.  Each bit represents the
        #output value of the associated pin (0 = low, 1 = high), assuming that
        #pin has been configured as an output previously.
        return self._read_u8(_MCP23017_GPIOB)

    @gpiob.setter
    def gpiob(self, val):
        self._write_u8(_MCP23017_GPIOB, val)

    @property
    def iodir(self):
        #
        #The raw IODIR direction register.  Each bit represents
        #direction of a pin, either 1 for an input or 0 for an output mode.
        return self._read_u16le(_MCP23017_IODIRA)

    @iodir.setter
    def iodir(self, val):
        self._write_u16le(_MCP23017_IODIRA, val)

    @property
    def iodira(self):
        #The raw IODIR A direction register.  Each bit represents
        #direction of a pin, either 1 for an input or 0 for an output mode.
        return self._read_u8(_MCP23017_IODIRA)

    @iodira.setter
    def iodira(self, val):
        self._write_u8(_MCP23017_IODIRA, val)

    @property
    def iodirb(self):
        #The raw IODIR B direction register.  Each bit represents
        #direction of a pin, either 1 for an input or 0 for an output mode.
        return self._read_u8(_MCP23017_IODIRB)

    @iodirb.setter
    def iodirb(self, val):
        self._write_u8(_MCP23017_IODIRB, val)

    @property
    def gppu(self):
        #The raw GPPU pull-up register.  Each bit represents
        #if a pull-up is enabled on the specified pin (1 = pull-up enabled,
        #0 = pull-up disabled).  Note pull-down resistors are NOT supported!
        return self._read_u16le(_MCP23017_GPPUA)

    @gppu.setter
    def gppu(self, val):
        self._write_u16le(_MCP23017_GPPUA, val)

    @property
    def gppua(self):
        #The raw GPPU A pull-up register.  Each bit represents
        #if a pull-up is enabled on the specified pin (1 = pull-up enabled,
        #0 = pull-up disabled).  Note pull-down resistors are NOT supported!
        return self._read_u8(_MCP23017_GPPUA)

    @gppua.setter
    def gppua(self, val):
        self._write_u8(_MCP23017_GPPUA, val)

    @property
    def gppub(self):
        #The raw GPPU B pull-up register.  Each bit represents
        #if a pull-up is enabled on the specified pin (1 = pull-up enabled,
        #0 = pull-up disabled).  Note pull-down resistors are NOT supported!
        return self._read_u8(_MCP23017_GPPUB)

    @gppub.setter
    def gppub(self, val):
        self._write_u8(_MCP23017_GPPUB, val)

    def get_pin(self, pin):
        #Convenience function to create an instance of the DigitalInOut class
        #pointing at the specified pin of this MCP23017 device.
        assert 0 <= pin <= 15
        return DigitalInOut(pin, self)
