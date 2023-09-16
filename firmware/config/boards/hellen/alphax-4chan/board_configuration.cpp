/**
 * @file board_configuration.cpp
 *
 *
 * @brief Configuration defaults for the 4chan board
 *
 * @author andreika <prometheus.pcb@gmail.com>
 * @author Andrey Belomutskiy, (c) 2012-2020
 */

#include "pch.h"
#include "hellen_meta.h"
#include "defaults.h"

static OutputPin alphaTachPullUp;
static OutputPin alphaTempPullUp;
static OutputPin alphaCrankPPullUp;
static OutputPin alphaCrankNPullUp;
static OutputPin alpha2stepPullDown;
static OutputPin alphaCamPullDown;
static OutputPin alphaCamVrPullUp;
static OutputPin alphaD2PullDown;
static OutputPin alphaD3PullDown;
static OutputPin alphaD4PullDown;
static OutputPin alphaD5PullDown;

static void setInjectorPins() {
	engineConfiguration->injectionPins[0] = Gpio::H144_LS_1;
	engineConfiguration->injectionPins[1] = Gpio::H144_LS_2;
	engineConfiguration->injectionPins[2] = Gpio::H144_LS_3;
	engineConfiguration->injectionPins[3] = Gpio::H144_LS_4;

	engineConfiguration->clutchDownPin = Gpio::Unassigned;
	engineConfiguration->clutchDownPinMode = PI_PULLDOWN;
	engineConfiguration->launchActivationMode = CLUTCH_INPUT_LAUNCH;
	engineConfiguration->malfunctionIndicatorPin = Gpio::Unassigned;
}

static void setupEtb() {
	// TLE9201 driver
	// This chip has three control pins:
	// DIR - sets direction of the motor
	// PWM - pwm control (enable high, coast low)
	// DIS - disables motor (enable low)

	// PWM pin
	engineConfiguration->etbIo[0].controlPin = Gpio::H144_OUT_PWM2;
	// DIR pin
	engineConfiguration->etbIo[0].directionPin1 = H144_GP1;
	// Disable pin
	engineConfiguration->etbIo[0].disablePin = H144_GP2;
	// Unused
	engineConfiguration->etbIo[0].directionPin2 = Gpio::Unassigned;

	// we only have pwm/dir, no dira/dirb
	engineConfiguration->etb_use_two_wires = false;
}

static void setIgnitionPins() {
	engineConfiguration->ignitionPins[0] = Gpio::H144_IGN_1;
	engineConfiguration->ignitionPins[1] = Gpio::H144_IGN_2;
	engineConfiguration->ignitionPins[2] = Gpio::H144_IGN_3;
	engineConfiguration->ignitionPins[3] = Gpio::H144_IGN_4;
}

static void setupVbatt() {
	// 4.7k high side/4.7k low side = 2.0 ratio divider
	engineConfiguration->analogInputDividerCoefficient = 2.0f;

	// set vbatt_divider 5.835
	// 33k / 6.8k
	engineConfiguration->vbattDividerCoeff = (33 + 6.8) / 6.8; // 5.835

	engineConfiguration->vbattAdcChannel = H144_IN_VBATT;

	engineConfiguration->adcVcc = 3.29f;
}

static void setupDefaultSensorInputs() {
	// trigger inputs, hall
	engineConfiguration->triggerInputPins[0] = Gpio::H144_IN_CRANK;
	engineConfiguration->triggerInputPins[1] = Gpio::H144_IN_CAM;
	engineConfiguration->camInputs[0] = Gpio::Unassigned;

	setTPS1Inputs(H144_IN_TPS, H144_IN_AUX1);

	setPPSInputs(H144_IN_PPS, H144_IN_AUX2);

	// random values to have valid config
	engineConfiguration->tps1SecondaryMin = 1000;
	engineConfiguration->tps1SecondaryMax = 0;

	engineConfiguration->mafAdcChannel = EFI_ADC_NONE;

	engineConfiguration->afr.hwChannel = EFI_ADC_1;

	engineConfiguration->clt.adcChannel = H144_IN_CLT;

	engineConfiguration->iat.adcChannel = H144_IN_IAT;
}

static bool is_F_OrOlder() {
    int16_t hellenBoardId = engine->engineState.hellenBoardId;
    return hellenBoardId == BOARD_ID_ALPHA4CH_D || hellenBoardId == BOARD_ID_ALPHA4CH_E || hellenBoardId == BOARD_ID_ALPHA4CH_F;
}

void boardInitHardware() {
	alphaTachPullUp.initPin("a-tach", Gpio::H144_OUT_IO1);
	alphaTempPullUp.initPin("a-temp", Gpio::H144_OUT_IO4);
	alphaCrankPPullUp.initPin("a-crank-p", Gpio::H144_OUT_IO2);
	alphaCrankNPullUp.initPin("a-crank-n", Gpio::H144_OUT_IO5);
	alpha2stepPullDown.initPin("a-2step", Gpio::H144_OUT_IO7);
	alphaCamVrPullUp.initPin("a-cam-vr", Gpio::H144_OUT_IO9);
	if (is_F_OrOlder()) {
	    alphaD2PullDown.initPin("a-d2", Gpio::H144_LS_5);
		alphaCamPullDown.initPin("a-cam", Gpio::H144_OUT_IO8);
		alphaD3PullDown.initPin("a-d3", Gpio::H144_LS_6);
	} else {
	    alphaD2PullDown.initPin("a-d2", Gpio::H144_OUT_IO13);
		alphaCamPullDown.initPin("a-cam", H144_GP4);
		alphaD3PullDown.initPin("a-d3", Gpio::H144_OUT_IO8);
	}
	
	alphaD4PullDown.initPin("a-d4", Gpio::H144_LS_7);
	alphaD5PullDown.initPin("a-d5", Gpio::H144_LS_8);
	boardOnConfigurationChange(nullptr);
}

void boardOnConfigurationChange(engine_configuration_s * /*previousConfiguration*/) {
	alphaTachPullUp.setValue(engineConfiguration->boardUseTachPullUp);
	alphaTempPullUp.setValue(engineConfiguration->boardUseTempPullUp);
	alphaCrankPPullUp.setValue(engineConfiguration->boardUseCrankPullUp);
	alphaCrankNPullUp.setValue(engineConfiguration->boardUseCrankPullUp);
	alpha2stepPullDown.setValue(engineConfiguration->boardUse2stepPullDown);
	alphaCamPullDown.setValue(engineConfiguration->boardUseCamPullDown);
	alphaCamVrPullUp.setValue(engineConfiguration->boardUseCamVrPullUp);

	alphaD2PullDown.setValue(engineConfiguration->boardUseD2PullDown);
	alphaD3PullDown.setValue(engineConfiguration->boardUseD3PullDown);
	alphaD4PullDown.setValue(engineConfiguration->boardUseD4PullDown);
	alphaD5PullDown.setValue(engineConfiguration->boardUseD5PullDown);
}

#include "hellen_leds_144.cpp"

void setBoardConfigOverrides() {
	setupVbatt();

    if (is_F_OrOlder()) {
        setHellenEnPin(Gpio::H144_OUT_IO3);
	    setHellenSdCardSpi2();
	} else {
	    setHellenMegaEnPin();
	    // rev G and newer uses hellen mega-module
	    setHellenSdCardSpi1();
	    configureHellenMegaAccCS2Pin();
	}

    setDefaultHellenAtPullUps();

	setHellenCan();
}

/**
 * @brief   Board-specific configuration defaults.
 *
 * See also setDefaultEngineConfiguration
 *
 * @todo    Add your board-specific code, if any.
 */
void setBoardDefaultConfiguration() {
	setInjectorPins();
	setIgnitionPins();
	setupEtb();
	if (is_F_OrOlder()) {
        engineConfiguration->tachOutputPin = Gpio::H144_OUT_IO13;
	    engineConfiguration->vvtPins[0] = Gpio::H144_OUT_PWM7;
	    engineConfiguration->vvtPins[1] = Gpio::H144_OUT_PWM8;
	} else {
    	engineConfiguration->vvtPins[0] = Gpio::H144_IGN_7;
	    engineConfiguration->vvtPins[1] = Gpio::H144_IGN_8;
        engineConfiguration->tachOutputPin = Gpio::H144_GP3;
	}


    // todo: should be conditional? currently set best for newest boards based on MegaModule
	setHellenMMbaro();
    engineConfiguration->map.sensor.hwChannel = H144_IN_MAP3; // On-board MAP
    engineConfiguration->map.sensor.type = MT_MPXH6400;
    //	engineConfiguration->baroSensor.type = MT_MPXH6400;
    //	engineConfiguration->baroSensor.hwChannel = H144_IN_MAP3; // On-board MAP

    engineConfiguration->boardUseTempPullUp = true;

	engineConfiguration->acSwitch = Gpio::Unassigned;
	engineConfiguration->fuelPumpPin = Gpio::H144_OUT_IO12;
	engineConfiguration->fanPin = Gpio::H144_OUT_IO11;
	engineConfiguration->mainRelayPin = Gpio::H144_OUT_IO10;
    engineConfiguration->boostControlPin = Gpio::H144_OUT_PWM3;

	// "required" hardware is done - set some reasonable defaults
	setupDefaultSensorInputs();

	engineConfiguration->cylindersCount = 4;
	engineConfiguration->firingOrder = FO_1_3_4_2;

	engineConfiguration->ignitionMode = IM_INDIVIDUAL_COILS; // IM_WASTED_SPARK



	engineConfiguration->launchActivationMode = CLUTCH_INPUT_LAUNCH;
// ?	engineConfiguration->malfunctionIndicatorPin = Gpio::G4; //1E - Check Engine Light
	engineConfiguration->vrThreshold[0].pin = Gpio::H144_OUT_PWM6;
	engineConfiguration->vrThreshold[1].pin = Gpio::H144_OUT_PWM4;
}

void boardPrepareForStop() {
	// Wake on the CAN RX pin
	palEnableLineEvent(PAL_LINE(GPIOD, 0), PAL_EVENT_MODE_RISING_EDGE);
}

static Gpio OUTPUTS[] = {
		Gpio::H144_LS_1,
		Gpio::H144_LS_2,
		Gpio::H144_LS_3,
		Gpio::H144_LS_4,
		// vvt1
		Gpio::H144_IGN_7,
		// vvt2
		Gpio::H144_IGN_8,
};

int getBoardMetaOutputsCount() {
    return efi::size(OUTPUTS);
}

Gpio* getBoardMetaOutputs() {
    return OUTPUTS;
}
