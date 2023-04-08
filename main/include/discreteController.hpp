/**
* Ezequiel Pecker-Marcosig
* SEDLab - University of Buenos Aires
*
* One-Wire digital temperature sensor:
* Model to interface with a DS18B20 temperature sensor for Embedded Cadmium.
*/

#ifndef RT_DISCRETE_CONTROLLER_TEST_HPP
#define RT_DISCRETE_CONTROLLER_TEST_HPP

#include "../../components/cadmium_v2/include/cadmium/core/modeling/atomic.hpp"

#ifndef NO_LOGGING
// 	#include <iomanip>
	#include <iostream>
//	#include <fstream>
#endif

using namespace std;

namespace cadmium::blinkySystem {
  
	struct DiscControllerState {
		float temperature;
		double sigma;
		int ctrl_state;
		/**
		*  DiscControllerState struct constructor.
		*/
		explicit DiscControllerState(): temperature(0.0), sigma(0), ctrl_state(0) {}
	}; 

#ifndef NO_LOGGING
	/**
	  * Insertion operator for DS18B20State objects. It only displays the value of the output.
	  * @param out output stream.
	  * @param s state to be represented in the output stream.
	  * @return output stream with sigma already inserted.
	  */
	std::ostream& operator<<(std::ostream &out, const DiscControllerState& state) {
		out << "Controller State: " << state.ctrl_state << ", temperature: " << state.temperature << ", sigma: " << state.sigma; 
		return out;
	}
#endif

	class DiscController : public Atomic<DiscControllerState> { // class DS18B20Input derives from template Atomic with DS18B20State
		private:

		public:
			Port<bool> green_led_out;
			Port<bool> red_led_out;
			Port<bool> rele_out;
			Port<float> temp_in;

			// Parameters to be overwriten when instantiating the atomic model
			double pollingRate;

			// temperature thresholds
			float Tmin;
			float Tmax;

			// default constructor
			DiscController(const std::string& id): Atomic<DiscControllerState>(id, DiscControllerState())  {
			
				ESP_LOGI("DiscController Atomic", "Initialization");
			
				green_led_out = addOutPort<bool>("green_led_out");
				red_led_out = addOutPort<bool>("red_led_out");
				rele_out = addOutPort<bool>("rele_out");
				temp_in       = addInPort<float>("in");
			
				pollingRate = 5; // 5 sec 
				// TODO: declare Tmin and Tmax as parameters received from the Top Coupling
				Tmin = 35.0; // C degrees
				Tmax = 45.0; // C degrees

				state.ctrl_state = 0;
				state.temperature = 0.0;
				state.sigma = 0; // 
			};
		      
			// internal transition
			void internalTransition(DiscControllerState& state) const override {
				ESP_LOGI("DiscController Atomic", "Internal transition function");
				if (state.temperature <= Tmin) {
					state.ctrl_state = 0;
				} else if (state.temperature >= Tmax) {
					state.ctrl_state = 1;
				} else if (state.temperature > Tmin && state.temperature < Tmax ) {
					state.ctrl_state = 2;
				} else { // unknown
					state.ctrl_state = 3;
				}
				state.sigma = pollingRate; // 1 sec
			}
		
			// external transition
			void externalTransition(DiscControllerState& state, double e) const override {
				ESP_LOGI("DiscController Atomic", "External transition function");
 				if(!temp_in->empty()){
 					for( const auto x : temp_in->getBag()){
 						state.temperature = (float)x;
 					}
				}
 				if (state.sigma == 0) {
 					state.sigma = pollingRate - e;
 				} else {
 					state.sigma -= e;
 				}	
			}
		      
			// output function
			void output(const DiscControllerState& state) const override {
				ESP_LOGI("DiscController Atomic", "Output function");
				bool green_led;
				bool red_led;
				bool rele;
				if (state.ctrl_state==0) {
					green_led = false;
					red_led = true;
					rele = true;
				} else if (state.ctrl_state==1) {
					green_led = false;
					red_led = true;
					rele = false;
				} else  if (state.ctrl_state==2) {
					green_led = true;
					red_led = false;
					rele = false;
				} else { // unkwnown
					green_led = true;
					red_led = true;
					rele = false;
				}
				green_led_out->addMessage(green_led);
				red_led_out->addMessage(red_led);
				rele_out->addMessage(rele);
			}
		
			// time_advance function
			[[nodiscard]] double timeAdvance(const DiscControllerState& state) const override {     
				ESP_LOGI("DiscController Atomic", "Time advance function");
				return state.sigma;
			}

	}; // class DiscController
} // namespace cadmium::blinkySystem
#endif // RT_DISCRETE_CONTROLLER_TEST_HPP
