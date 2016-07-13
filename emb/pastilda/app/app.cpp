#include "app.h"
#include "stdio.h"
using namespace Application;

#ifdef DEBUG
    #define DEBUG_PRINT(x) printf(x)
#else
    #define DEBUG_PRINT(x) do {} while (0)
#endif

App *app_pointer;

App::App()
{
	app_pointer = this;
	clock_setup();
	systick_init();

	_leds_api = new LEDS_api();
/////////////////////////////////////////////////////////////////////////////////////////
	//spi and cs pin for sst25_driver
//	SPI_Conf spi_conf;
//	spi_conf.spi_number = 1;
//	spi_conf.scl_pin = PA5;
//	spi_conf.miso_pin = PA6;
//	spi_conf.mosi_pin = PA7;
//	_spi_ext = new SPI_ext(spi_conf);
//	_spi_ext->disable();
//	_spi_ext->set_baudrate_prescaler(SPI_CPP_Extension::BaudRate::BAUDRATE_FPCLK_DIV_2);
//	_spi_ext->set_clock_polarity(SPI_CPP_Extension::Polarity::POLARITY_HIGH);
//	_spi_ext->set_clock_phase(Phase::PHASE_HIGH);
//	_spi_ext->set_data_drame_format(DataFrameFormat::DFF_8BIT);
//	_spi_ext->set_bit_position(BitPos::MSB_FIRST);
//	_spi_ext->set_software_slave_management(State::ENABLE);
//	_spi_ext->set_nss(NssState::HIGH);
//	_spi_ext->enable();
//
//	GPIO_ext cs_pin(PA4);
//	cs_pin.mode_setup(Mode::OUTPUT, PullMode::NO_PULL);
//	cs_pin.set_output_options(OutputType::PUSH_PULL, Speed::FAST_50MHz);
///////////////////////////////////////////////////////////////////////////////////////////
	delay_ms(1000);

	usb_dispatcher = new USB_dispatcher();
}
void App::process()
{
	_leds_api->toggle();
	usb_dispatcher->process();
}
