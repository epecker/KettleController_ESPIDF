#ifndef CADMIUM_EXAMPLE_BLINKY_HPP_
#define CADMIUM_EXAMPLE_BLINKY_HPP_

#include <../../components/cadmium_v2/include/cadmium/core/modeling/coupled.hpp>

#ifdef RT_ESP32
	#include <../../components/cadmium_v2/include/cadmium/core/real_time/arm_mbed/io/digitalOutput.hpp>
//	#include <../../components/cadmium_v2/include/cadmium/core/real_time/arm_mbed/io/digitalInput.hpp>
#endif

// #include "blinky.hpp"
#include "ds18b20probe.hpp"
#include "discreteController.hpp"

// #ifndef RT_ESP32
// 	#include "generator.hpp"
// #endif

namespace cadmium::blinkySystem {

	struct blinkySystem : public Coupled {
	
		/**
		 * Constructor function for the blinkySystem model.
		 * @param id ID of the blinkySystem model.
		 */
		blinkySystem(const std::string& id) : Coupled(id) {
	
			// auto blinky = addComponent<Blinky>("blinky");
#ifdef RT_ESP32		
			// ESP32 
			auto greenLedOutput  = addComponent<DigitalOutput>("greenLed", 15); // GPIO2: onboard LED
			auto redLedOutput  = addComponent<DigitalOutput>("redLed", 16);
			auto releOutput  = addComponent<DigitalOutput>("rele", 17);
			// auto digitalInput   = addComponent<DigitalInput>("digitalInput", 0);
			auto ds18b20        = addComponent<DS18B20>("tempSensor");
			auto tempController = addComponent<DiscController>("tempController");
			// BLUE PILL
			// auto digitalOutput = addComponent<DigitalOutput>("digitalOuput", PC_13);
			// auto digitalInput  = addComponent<DigitalInput>("digitalInput", PB_14);
			// addCoupling(digitalInput->out, blinky->in);
			// addCoupling(blinky->out, digitalOutput->in);
			addCoupling(ds18b20->out, tempController->temp_in);
			addCoupling(tempController->green_led_out, greenLedOutput->in);
			addCoupling(tempController->red_led_out, redLedOutput->in);
			addCoupling(tempController->rele_out, releOutput->in);
#else
			// auto generator = addComponent<Generator>("generator");
			// addCoupling(generator->out, blinky->in);
#endif
		}
	};
} // namespace cadmium::blinkySystem

#endif //CADMIUM_EXAMPLE_BLINKY_HPP_
