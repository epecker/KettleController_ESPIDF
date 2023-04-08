/**
* Ezequiel Pecker-Marcosig
* SEDLab - University of Buenos Aires
*
* One-Wire digital temperature sensor:
* Model to interface with a DS18B20 temperature sensor for Embedded Cadmium.
*/

#ifndef RT_DS18B20_TEST_HPP
#define RT_DS18B20_TEST_HPP

#include <limits>
#include <math.h> 
#include <assert.h>
#include <memory>
#include <optional>
#include <string>
#include <chrono>
#include <algorithm>
#include <random>

#include "../../components/cadmium_v2/include/cadmium/core/modeling/atomic.hpp"

#ifndef NO_LOGGING
	#include <iomanip>
	#include <iostream>
	#include <fstream>
#endif

#ifdef RT_ESP32
	#include "freertos/FreeRTOS.h"
	#include "freertos/task.h"
	#include "esp_system.h"
	#include "esp_log.h"
	#include "../../components/esp32-owb/include/owb.h"
	#include "../../components/esp32-owb/include/owb_rmt.h"
	#include "../../components/esp32-ds18b20/include/ds18b20.h"
	#define GPIO_DS18B20_0       (CONFIG_ONE_WIRE_GPIO)
	#define DS18B20_RESOLUTION   (DS18B20_RESOLUTION_12_BIT)
	#define SAMPLE_PERIOD        (1000)   // milliseconds
#endif

#ifdef RT_ESP32
	#include "driver/gpio.h"
#endif

using namespace std;

namespace cadmium::blinkySystem {
  
	struct DS18B20State {
		float output;
		double sigma;
		int sample_count;
		/**
		*  DS18B20State struct constructor.
		*/
		explicit DS18B20State(): output(0.0), sigma(2), sample_count(0){}
	}; 

#ifndef NO_LOGGING
	/**
	  * Insertion operator for DS18B20State objects. It only displays the value of the output.
	  * @param out output stream.
	  * @param s state to be represented in the output stream.
	  * @return output stream with sigma already inserted.
	  */
	std::ostream& operator<<(std::ostream &out, const DS18B20State& state) {
		out << "Reading: " << state.output; 
		return out;
	}
#endif

	class DS18B20 : public Atomic<DS18B20State> { // class DS18B20Input derives from template Atomic with DS18B20State
		private:

		public:
			Port<float> out;
			// Parameters to be overwriten when instantiating the atomic model
			double pollingRate;
			// internal variables to handle DS18B20 sensor
			OneWireBus * owb;
			owb_rmt_driver_info rmt_driver_info;
			DS18B20_Info * ds18b20_info = ds18b20_malloc();  // heap allocation

			// default constructor
			DS18B20(const std::string& id): Atomic<DS18B20State>(id, DS18B20State())  {
			
				ESP_LOGI("DS18B20 Atomic", "Initialization");
			
				out = addOutPort<float>("out");
			
				pollingRate = 1; // 1 sec 
				state.output = 0.0;
				
				// Create a 1-Wire bus, using the RMT timeslot driver
				owb = owb_rmt_initialize(&rmt_driver_info, (gpio_num_t)GPIO_DS18B20_0, RMT_CHANNEL_1, RMT_CHANNEL_0);
				
				// Create DS18B20 devices on the 1-Wire bus
				ds18b20_init_solo(ds18b20_info, owb);          // only one device on bus
				ds18b20_use_crc(ds18b20_info, true);           // enable CRC check on all reads
				ds18b20_set_resolution(ds18b20_info, DS18B20_RESOLUTION);
			
				// Stable readings require a brief period before communication
				state.sigma = 2; // for initialization purposes wait 2 sec
			};
		      
			// internal transition
			void internalTransition(DS18B20State& state) const override {
				ESP_LOGI("DS18B20 Atomic", "Internal transition function");
				float readings;
				char buffer[100];
		
				// Convert, wait and read current temperature from device
				ds18b20_convert_and_read_temp(ds18b20_info, &readings);
				
				state.sample_count++;
				sprintf(buffer,"\nTemperature readings (degrees C): value = %f, sample = %d\n", readings, state.sample_count);
				ESP_LOGI("DS18B20 Atomic", "%s", buffer);
		
				state.output = readings;
				state.sigma = pollingRate; // 1 sec
			}
		
			// external transition
			void externalTransition(DS18B20State& state, double e) const override {
				ESP_LOGI("DS18B20 Atomic", "External transition function");
				// MBED_ASSERT(false);
				throw std::logic_error("External transition called in a model with no input ports");
			}
		      
			// output function
			void output(const DS18B20State& state) const override {
				ESP_LOGI("DS18B20 Atomic", "Output function");
				// TODO: use the current value instead of the value taken 1 sec ago
				// float output = state.output;
				// out->addMessage(output);
				out->addMessage(state.output);
			}
		
			// time_advance function
			[[nodiscard]] double timeAdvance(const DS18B20State& state) const override {     
				ESP_LOGI("DS18B20 Atomic", "Time advance function");
				return state.sigma;
			}

	}; // class DS18B20
} // namespace cadmium::blinkySystem
#endif // RT_DS18B20_TEST_HPP
