
#ifndef SERIALDEVICE_H
#define SERIALDEVICE_H

#include <iostream>
#include <string>
#include "SerialResponse.hpp"
#include "LibSerial/SerialStream.h"

namespace LibSerial{


class SerialDevice : public LibSerial::SerialStream
{
public:

		/*Handling Notice:
		 * If you use structs in your TResponseMask types you might need a
		 * special compiler pragma around it.
		 *
		 * Usually the Compiler creates a struct like the following:
		 * struct example{
		 * 		int   ex1;  //Reserved size 4 Bytes
		 * 		short ex2;  //Reserved size 4 Bytes
		 *      //(2 Bytes for Short + 2 Dummy Bytes to fit the biggest data size)
		 * };
		 * If you want to use the struct as a data mask you might need to get around this.
		 * Therefore you can use this compiler macro around the data type:
		 *
		 * #pragma pack(push, 1)
		 * struct example{
		 * 		int   ex1;  //Reserved size 4 Bytes
		 * 		short ex2;  //Reserved size 2 Bytes
		 * };
		 * #pragma pack(pop)
		 */

		// Constructors/Destructors
		//

		enum ERROR
		{
			NONE	 				=  0,
			OPENFILE_FAILED 		= -1,
			SETBAUD_FAILED 			= -2,
			SETCHAR_FAILED			= -3,
			SETPARITY_FAILED		= -4,
			SETFLOWCONTROL_FAILED	= -5
		};

		/**
		 * Empty Constructor
		 */
		SerialDevice ( );

		/**
		 * Empty Destructor
		 */
		virtual ~SerialDevice ( );

		/**
		 * Send command to the device.
		 * Returns object from Type SerialResponse<type>.
		 * @return LibSerial::Dev::SerialResponse<class TResponseMask>
		 * @param  std::string Command
		 * @param  unsigned int DelayResponse_us
		 * @param  unsigned int DelayChar_us
		 */

		template <class TResponseMask>
		SerialResponse<TResponseMask> SendCommand(std::string Command,
												  unsigned int DelayResponse_us,
												  unsigned int DelayChar_us,
												  unsigned int size_info_header,
												  unsigned int size_info_location,
												  unsigned int size_info_len,
												  bool small_endian,
												  unsigned int size_repetitive,
												  void *progressbar_object,
												  void (*progressbar_function)(void* progressbar_object,unsigned int current_byte, unsigned int byte_amount))
		{

		    //Open interface file
			this->Open(this->DevFilename);
			if(!this->good()) return this->CreateErrorResponse<TResponseMask>("Could not open interface file", OPENFILE_FAILED);

			//Set BaudRate 		FIXME: Remove SerialStreamBuf::BaudRateEnumwhen using LibSerial > 0.7.X
			this->SetBaudRate((const SerialStreamBuf::BaudRateEnum) this->DevBaudrate);
			if(!this->good()) return this->CreateErrorResponse<TResponseMask>("Could not set baudrate", SETBAUD_FAILED);

			//Set CharSize 		FIXME: Remove SerialStreamBuf::CharSizeEnum when using LibSerial > 0.7.X
			this->SetCharSize((const SerialStreamBuf::CharSizeEnum) this->DevCharsize);
			if(!this->good()) return this->CreateErrorResponse<TResponseMask>("Could not set charsize", SETCHAR_FAILED);

			//Set FlowControl 	FIXME: Remove SerialStreamBuf::FlowControlEnum when using LibSerial > 0.7.X
			this->SetFlowControl((const SerialStreamBuf::FlowControlEnum) this->DevFlowcontrol);
			if(!this->good()) return this->CreateErrorResponse<TResponseMask>("Could not set flowcontrol", SETFLOWCONTROL_FAILED);

			//Set Parity  		FIXME: Remove SerialStreamBuf::ParityEnum when using LibSerial > 0.7.X
			this->SetParity((const SerialStreamBuf::ParityEnum) this->DevParity);
			if(!this->good()) return this->CreateErrorResponse<TResponseMask>("Could not set parity", SETPARITY_FAILED);

			//Flush Serial Interface
			this->flush();

			//Send Command
			*this<<Command;

			//Wait for response of device
			usleep(DelayResponse_us);




			SerialResponse<TResponseMask> response;
#ifdef _DEBUG_INCOMING_SERIAL_DATA_
			cout<<endl<<"Transmission from SerialDevice:"<<endl;
#endif

			unsigned int current_byte=0;
			unsigned long size_info=0, current_size_info_byte=0, size_info_transmission=0;

			while(this->rdbuf()->in_avail()>0)
			{

				//TODO: ADD progress bar caller ...
				//Get next char from interface
				unsigned char incomingdata=this->get();
#ifdef _DEBUG_INCOMING_SERIAL_DATA_
				printf("%02x",(unsigned char)incomingdata);  //TODO REMOVE
#endif
				response.push_back(incomingdata);
				//Wait for next char to be transmitted
				usleep(DelayChar_us);
				//increase byte counter
				current_byte++;


				//Doing progressbar stuff
				if(*progressbar_function != 0 && progressbar_object != 0) //Is a progress bar function defined?
				{
					//if repetitive amount info len is 0 or repetitive data sets are 0 set amount of bytes to header size
					if((size_info_len == 0 || size_repetitive==0) && size_info_header != 0 )
					{
						size_info_transmission = size_info_header;
					}

					if(current_byte >= size_info_location && current_size_info_byte < size_info_len)
					{
						if(!small_endian)
						{
							size_info|=(incomingdata<<((current_size_info_byte)*8)); //place byte in variable
							current_size_info_byte++; //which byte of the repetitive information length is it?
						}
						else
						{
							//TODO Not implemented yet
						}
					}

					if( current_byte == size_info_len + size_info_location)
					{
						//Calculate Size of whole Transmission
						size_info_transmission=size_info*size_repetitive+size_info_header;
					}

					progressbar_function(progressbar_object,current_byte,size_info_transmission);
				}




			}
			//std::cout<<current_byte<<":"<<size_info_transmission<<std::endl;

			this->Close();
#ifdef _DEBUG_INCOMING_SERIAL_DATA_
			cout<<endl<<"Transmission Ended"<<endl;
#endif
			return response;
		}

		template <class TResponseMask>
		SerialResponse<TResponseMask> CreateErrorResponse(const char* ErrorString, int ErrorNumber)
		{
			std::cerr<<ErrorString<<"("<<ErrorNumber<<")"<<std::endl;
			SerialResponse<TResponseMask> ErrorMask;
			ErrorMask.setErrorMsg(ErrorString);
			ErrorMask.setError(ErrorNumber);
			return ErrorMask;
		}




private:

		// Private attributes
		//

		std::string Manufacturer;
		std::string DeviceName;
		//This string stores the serial interface filename
		std::string DevFilename;

		SerialPort::BaudRate 		DevBaudrate;
		SerialPort::CharacterSize 	DevCharsize;
		SerialPort::StopBits 		DevStopbits;
		SerialPort::Parity 			DevParity;
		SerialPort::FlowControl 	DevFlowcontrol;



		//TODO Add error flag registers for all set operations...
		//TODO Add exception configuration
		//TODO throw instances of type failure...



public:



		// Private attribute accessor methods
		//
		//TODO add documentation
		void SetDeviceBaudRate(SerialPort::BaudRate devbaudrate)
		{
			this->DevBaudrate=devbaudrate;
		}

		void SetDeviceCharacterSize(SerialPort::CharacterSize devcharsize)
		{
			this->DevCharsize=devcharsize;
		}

		void SetDeviceStopBits(SerialPort::StopBits devstopbits)
		{
			this->DevStopbits=devstopbits;
		}

		void SetDeviceParity(SerialPort::Parity devparity)
		{
			this->DevParity=devparity;
		}

		void SetDeviceFlowControl(SerialPort::FlowControl devflowcontrol)
		{
			this->DevFlowcontrol=devflowcontrol;
		}

		void SetDeviceFilename(std::string filename)
		{
			this->DevFilename=filename;
		}


		/**
		 * Set the value of Manufacturer
		 * This data field is optional.
		 * @param manufacturer the new value of Manufacturer
		 */
		void SetManufacturer ( std::string manufacturer )
		{
			Manufacturer = manufacturer;
		}

		/**
		 * Get the value of Manufacturer
		 * This data field is optional.
		 * @return the value of Manufacturer
		 */
		std::string getManufacturer ( )		 {
				return Manufacturer;
		}

		/**
		 * Set the value of DeviceName
		 * This data field is optional.
		 * @param name the new value of DeviceName
		 */
		void SetDeviceName ( std::string name )		 {
						DeviceName = name;
		}

		/**
		 * Get the value of DeviceName
		 * This data field is optional.
		 * @return the value of DeviceName
		 */
		std::string getDeviceName ( )		 {
				return DeviceName;
		}
private:

		void initAttributes ( ) ;

};
};
#endif // SERIALDEVICE_H
