#include "HX711.h"                  //< Loadcell library: https://github.com/RobTillaart/HX711 

#define BUADRATE 57600U             ///< BUAD RATE   
#define LOADCELL_DOUT_PIN 2U        ///< Data Input Pin on the Arduino
#define LOADCELL_SCK_PIN  3U        ///< Serial Clock Output Pin on the Arduino
#define BUFFER_LENGTH     4U        ///< 4 Byte Buffer Length for float serial transfer 

/// HX711 Scale Class
HX711 scale;                        

/**
  * @struct input_data
  * @brief Structure containing the state character and state numeric value sent by the program.
  */
struct {
  char    state = 'X';    ///< State character
  uint8_t numeric = 0;    ///< Numeric value associated with the state
} input_data;

/**
  * @union scale_reading  
  * @brief Union for sending raw 4 Byte float values overal serial communication.
  */
union {
  float data;
  byte  buffer[BUFFER_LENGTH];
} scale_reading;

/**
  * @brief Microcontroller setup function: Initialses serial communication and configures the HX711 loadcell 
  */
void setup( void ) {
  
  Serial.begin( BUADRATE );                           ///< Set up serial communication with baud rate defined above                             
  
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);   ///< Initiate scale class using HX711.h library 
  
  while ( Serial.available() == 0 ){}                 ///< Wait for serial availablity 
}

/**
  * @brief Finite State Machine to process and serial data
  * 
  * Reads a packet of data consisting of a state character and a numeric value, and update the system's state.  
*/
void stateMachine()
{
  
  byte input_buffer[2];                           ///< Buffer to hold incomming serial data (2 bytes).
  Serial.readBytesUntil('\n', input_buffer, 2);   
  

  input_data.state   = (char)input_buffer[0];
  input_data.numeric = (uint8_t)input_buffer[1];

  return;
}


/**
  * @brief Main program Loop.
  * 
  * Continuously checks for serial input, processes the state machine,
  * and reads data from the HX711 load cell if logging is initialized. 
*/
void loop() {
   
  if ( Serial.available() > 0 ) { ///< Check if the scale is available for reading.
    stateMachine();
  }
  
  if ( input_data.state != 'X' && scale.is_ready() ) {    ///< Check if program has initialised the starting of the logging.
    scale_reading.data = scale.read();                    ///< Get readings RAW from the scale.
    Serial.write(scale_reading.buffer, BUFFER_LENGTH);    ///< Output the readings to the serial port.
  } 
}
