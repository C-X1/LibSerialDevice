
#ifndef SERIALRESPONSE_H
#define SERIALRESPONSE_H

#include <string>

namespace LibSerial {


template<class TResponseMask>
class SerialResponse : public std::string
{
public:

		// Constructors/Destructors
		//  


		/**
		 * Empty Constructor
		 */
		SerialResponse ( )
		{
			this->Error=0;
		}

		/**
		 * Empty Destructor
		 */
		virtual ~SerialResponse ( )
		{

		}


private:
		// Private attributes
		//
		char* rawdata;
		TResponseMask* DataMaskPtr;


public:


		// Protected attribute accessor methods
		//  

		/**
		 * Update Get the value of DataMask
		 * @return the value of DataMask
		 */
		TResponseMask* Data( )		 {

			this->rawdata=(char*)this->data();

			DataMaskPtr=(TResponseMask *)this->rawdata;
			return DataMaskPtr;
		}
protected:


private:

		// Private attributes
		//  

		int Error;
		std::string ErrorMsg;
public:

private:

public:


		// Private attribute accessor methods
		//  


		/**
		 * Set the value of Error
		 * @param new_var the new value of Error
		 */
		void setError ( int new_var )		 {
						Error = new_var;
		}

		/**
		 * Get the value of Error
		 * @return the value of Error
		 */
		int getError ( )		 {
				return Error;
		}

		/**
		 * Set the value of ErrorMsg
		 * @param new_var the new value of ErrorMsg
		 */
		void setErrorMsg ( std::string new_var )		 {
						ErrorMsg = new_var;
		}

		/**
		 * Get the value of ErrorMsg
		 * @return the value of ErrorMsg
		 */
		std::string getErrorMsg ( )		 {
				return ErrorMsg;
		}



private:


		void initAttributes ( ) ;

};
}; // end of namespace LibSerial
#endif // SERIALRESPONSE_H
