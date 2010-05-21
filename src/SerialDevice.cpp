#include "SerialDevice.hpp"

using namespace LibSerial;

// Constructors/Destructors
//

SerialDevice::SerialDevice ( ) {
initAttributes();
}

SerialDevice::~SerialDevice ( ) { }

//
// Methods
//




// Accessor methods
//


// Other methods
//

void SerialDevice::initAttributes ( ) {
		Manufacturer = "";
		DeviceName = "";
}

