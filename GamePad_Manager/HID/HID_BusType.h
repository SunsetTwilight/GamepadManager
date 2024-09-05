#ifndef _HID_BUS_TYPE
#define _HID_BUS_TYPE

enum HID_BusType
{
	/** Unknown bus type */
	HID_BUS_TYPE_UNKNOWN = 0x00,

	/** USB bus
	   Specifications:
	   https://usb.org/hid */
	HID_BUS_TYPE_USB = 0x01,

	/** Bluetooth or Bluetooth LE bus
	   Specifications:
	   https://www.bluetooth.com/specifications/specs/human-interface-device-profile-1-1-1/
	   https://www.bluetooth.com/specifications/specs/hid-service-1-0/
	   https://www.bluetooth.com/specifications/specs/hid-over-gatt-profile-1-0/ */
	HID_BUS_TYPE_BLUETOOTH = 0x02,

	/** I2C bus
	   Specifications:
	   https://docs.microsoft.com/previous-versions/windows/hardware/design/dn642101(v=vs.85) */
	HID_BUS_TYPE_I2C = 0x03,

	/** SPI bus
	   Specifications:
	   https://www.microsoft.com/download/details.aspx?id=103325 */
	HID_BUS_TYPE_SPI = 0x04,

};

#endif // !_HID_BUS_TYPE
