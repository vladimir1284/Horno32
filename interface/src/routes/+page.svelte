<!-- <script lang="ts">
	import type { PageData } from '../$types';
	import Demo from './Demo.svelte';

	export let data: PageData;
</script>

<div
	class="mx-0 my-1 flex flex-col space-y-4
     sm:mx-8 sm:my-8"
>
	<Demo />
</div> -->


<script lang="ts">

	// import Ampmeter from "./Ampmeter.svelte";

	import { onMount, onDestroy } from 'svelte';
	import { user } from '$lib/stores/user';
	import { page } from '$app/stores';
	import { notifications } from '$lib/components/toasts/notifications';
	import SettingsCard from '$lib/components/SettingsCard.svelte';
	import Light from '~icons/tabler/bulb';
	import Info from '~icons/tabler/info-circle';
	import Save from '~icons/tabler/device-floppy';
	import Reload from '~icons/tabler/reload';
	import { socket } from '$lib/stores/socket';
	import type { LightState } from '$lib/types/models';

	import Voltmeter from "$lib/components/panel-components/Voltmeter.svelte";
	import Freqmeter from "$lib/components/panel-components/Freqmeter.svelte";
	import StatusLedPanel from '$lib/components/panel-components/StatusLedPanel.svelte';
	import ODO from "$lib/components/panel-components/ODO.svelte"
	import StopButton from './StopButton.svelte';
	import CarThermometer from "$lib/components/panel-components/CarThermometer.svelte";
	import CarVoltmeter from "$lib/components/panel-components/CarVoltmeter.svelte";
	import CarOilPressuremeter from "$lib/components/panel-components/CarOilPressuremeter.svelte";

	// let lightState: LightState = {
	let lightState = {
		led_on: false,
		VA: 0,
		VB: 0,
		VC: 0,
		IA: 0,
		IB: 0,
		IC: 0,
		Pt: 0,
		PA: 0,
		PB: 0,
		PC: 0,
		Qt: 0,
		QA: 0,
		QB: 0,
		QC: 0,
		St: 0,
		SA: 0,
		SB: 0,
		SC: 0,
		pft: 0,
		pfA: 0,
		pfB: 0,
		pfC: 0,
		UAB: 0,
		UBC: 0,
		UCA: 0,
		fA: 0,
		fB: 0,
		fC: 0,
		pPenergy: 0,
		rPenergy: 0,
		pQenergy: 0,
		rQenergy: 0,
		tPenergy: 0,
		tQenergy: 0,
		imbalanceVoltage: 0,
		imbalanceCurrent: 0,
		isWorking: false,
		isMainPower: false,
		isBatteryOk: false,
		isBatteryLow: false,
		isBatteryHigh: false,
		isHighTemp: false,
		isLowOilPress: false,
		isOverSpeed: false,
		isOverVoltage: false,
		workingHours: 0,
		temperature: 0,
		batteryLevel: 0,
		oilPressure: 0,
		rpm: 0,
		cmdStop: false,
		modbusQualityTotalReads: 0,
		modbusQualityErrors: 0,
		modbusErrors: 0,
		modbusFails: 0,
		modbusState: false
	};

	let timeStopButtonPressed = 8000;

	$: varCmdStop = false;

	$: mag = 15.8;
	$: f = 60;
	$: temperature = 45;
	$: batteryVoltage = 0;
	$: oilPressure = -0.3;
	
	$: voltageToShow = 0;
	$: voltages = [0, 0, 0, 0, 0, 0, 0, 0, 0];

	$: ledStatus = [false, false, false, false, false, false, false, false, false, false];

	let workingHours = 0;
	$: hoursRunning = 0;
	$: hoursRunningDecimal = 0;
	let dure = 200;

	// for volt/amp meter
	let max = 300;
	let units = "V";
	let varName = "Va";

	const variable = ['Va', 'Vb', 'Vc', 'Vab', 'Vbc', 'Vca', 'Ia', 'Ib', 'Ic'];
	const colors = ['rgb(113, 170, 255)', 'rgb(113, 185, 255)', 'rgb(113, 200, 255)', 
	                'rgb(181, 144, 255)', 'rgb(201, 144, 255)', 'rgb(221, 144, 255)',
				    'rgb(255, 183, 113)', 'rgb(255, 188, 113)', 'rgb(255, 193, 113)'];
	let selected = colors[0];

	let isSocketConnected = false;
	let timeout: string | number | NodeJS.Timeout | undefined;
    
	function intermdiateCalcutation() {
		
		voltages[0] = lightState.VA;
		voltages[1] = lightState.VB;
		voltages[2] = lightState.VC;
		voltages[3] = lightState.UAB;
		voltages[4] = lightState.UBC;
		voltages[5] = lightState.UCA;
		voltages[6] = lightState.IA;
		voltages[7] = lightState.IB;
		voltages[8] = lightState.IC;
		
		mag = voltages[voltageToShow];
		f = lightState.fA;

		ledStatus[0] = lightState.isMainPower;
		ledStatus[1] = lightState.isWorking;
		ledStatus[2] = lightState.isBatteryOk;
		ledStatus[3] = lightState.isBatteryLow;
		ledStatus[4] = lightState.isBatteryHigh;
		ledStatus[5] = false;
		ledStatus[6] = lightState.isHighTemp;
		ledStatus[7] = lightState.isLowOilPress;
		ledStatus[8] = lightState.isOverSpeed;
		ledStatus[9] = lightState.isOverVoltage;
		// ledStatus[9] = lightState.cmdStop;

		hoursRunning = Math.floor(lightState.workingHours);
		hoursRunningDecimal = (lightState.workingHours * 10) % 10;

		temperature = lightState.temperature;
		batteryVoltage = lightState.batteryLevel;
		oilPressure = lightState.oilPressure;
	
	}

	async function getLightstate() {
		try {
			const response = await fetch('/rest/lightState', {
				method: 'GET',
				headers: {
					Authorization: $page.data.features.security ? 'Bearer ' + $user.bearer_token : 'Basic',
					'Content-Type': 'application/json'
				}
			});
			const light = await response.json();
			lightState = light;
			intermdiateCalcutation();
		} catch (error) {
			console.error('Error:', error);
		}
		return;
	}

	function requestData() {
		if (!isSocketConnected) {
			getLightstate();
		}
		isSocketConnected = false;
		timeout = setTimeout(requestData, 2500);
	}

	onMount(()=> {
		socket.on<LightState>('led', (merterData) => {
			lightState = merterData;
			isSocketConnected = true;
			// console.log("se pidio datos al  Panel");
			// console.log(lightState);
			// console.log(`${oilPressure} ${hoursRunning} ${hoursRunningDecimal}`);
			intermdiateCalcutation(); 
		});
		timeout = setTimeout(requestData, 500);
		varCmdStop = false;
	});

	onDestroy(() => {
		socket.off('led');
		clearTimeout(timeout);
	});

	function offCmdStop() {
		varCmdStop = false;
		postLightstate();
	}

	function handleCmdStop() {
		if (!varCmdStop) {
			varCmdStop = true; // Cambia el estado
			setTimeout(offCmdStop, timeStopButtonPressed);
			postLightstate();
		}
	}

	async function postLightstate() {
		try {
			const response = await fetch('/rest/lightState', {
				method: 'POST',
				headers: {
					Authorization: $page.data.features.security ? 'Bearer ' + $user.bearer_token : 'Basic',
					'Content-Type': 'application/json'
				},
				body: JSON.stringify({ cmdStop: varCmdStop })
			});
			if (response.status == 200) {
				// notifications.success('Light state updated.', 3000);
				const light = await response.json();
				varCmdStop = light.cmdStop;
			} else {
				// notifications.error('User not authorized.', 3000);
			}
		} catch (error) {
			console.error('Error:', error);
		}
	}

</script>



<!-- VoltMeter and frequencyMeter -->
<div class="container">
	<Voltmeter magnitude={mag} svgWidth={140} svgHeight={140} varName={varName} units={units} needleColor={selected} maxScale={max}/>
	<Freqmeter magnitude={f} svgWidth={140} svgHeight={140} units={["Hz", "RPM"]} minScale={45} maxScale={70} scaleLargeDivisions={5} scaleSmallDivisions={25} decimalPlaces={1} needleColor="gray"/>
</div>

<!-- Variable selector -->
<div class="variable-selector" style="display: flex;">
	{#each colors as color, i}
		<button 
			aria-current={selected === color}
			style="background: {color}"
			on:click={() => {
								selected = color;
								voltageToShow = i;
								mag = voltages[voltageToShow];
								varName = variable[i];
								if (i < 6) {
									max = 300;
									units = "V"
								} else {
									max = 30;
									units = "A";
								}
							}
			}
		>{variable[i]}</button>
	{/each}
</div>

<!-- Leds indicator -->
<div class="status-led-panel">
	<StatusLedPanel ledStatus={ledStatus}/>
</div>

<div class="hours-and-stop-button">

	<div >
		<!-- hour meter -->
		<div class="hour-label">Horas de trabajo:</div>
		<div class="hour-meter">
			<div style="display: flex; align-items: center;">
				<ODO value={hoursRunning} nrDigits={5} {dure} color="white" background="rgb(70, 70, 70)"/>
				<div class="decimal-point">.</div>
				<ODO value={hoursRunningDecimal} nrDigits={1} {dure} color="red" background="white"/>
			</div>
		</div>
	</div>

	<!-- stop button -->
	<StopButton timeStopButtonPressed={timeStopButtonPressed} enabled={lightState.isWorking} on:cmdStop={handleCmdStop} />
	
</div>

<!-- instruments -->
<div class="car-instruments">
    <div style="display: flex;">
		<CarThermometer temperature={temperature} alarm={lightState.isHighTemp}/>
		<CarVoltmeter voltage={batteryVoltage} alarm={lightState.isBatteryLow || lightState.isBatteryHigh}/>
		<CarOilPressuremeter oilPressure={oilPressure} alarm={lightState.isLowOilPress}/>
	</div>
</div>




<style>
	.test {
		color: rgb(44, 44, 44);
	}

	.container {
        margin-top: 10px;
		margin-left: 5%;
		margin-right: 0%;
		display: flex;
		justify-content: space-between;
		width: 90%;
    }

	.variable-selector {
		margin-top: 10px;
		display: flex;
		justify-content: space-between;
		width: 100%;
	}

	.variable-selector button {
		flex: 0;
		min-width: 31px;
		max-width: 50px;
		margin: 0 2px; /* Adjust margin as needed */
  	}

	button {
		aspect-ratio: 0.9;
		border-radius: 40%;
		background: var(--color, #9595f5);
		transform: translate(-2px,-2px);
		filter: drop-shadow(2px 2px 3px rgba(0,0,0,0.2));
		transition: all 0.5s;
	}

	button[aria-current="true"] {
		transform: none;
		filter: none;
		color: white;
		font-weight: bold;
		box-shadow: inset 3px 3px 4px rgba(0,0,0,0.2);
	}

	button:not([aria-current="true"]) {
		color: gray;

	}

	.status-led-panel {
		margin-top: 30px;
		margin-bottom: 30px;
	}

	.hours-and-stop-button {
		display: flex;
		align-items: center;
		justify-content: space-between;
		margin-left: 30px;
		margin-right: 30px;
	}

	.hour-label {
		margin-top: 0px;
		display: flex;
        align-items: center;
		justify-content: center;
    }
	.hour-meter {
		margin-top: 0px;
		display: flex;
        align-items: center;
		justify-content: center;
    }

	.decimal-point{
		margin: 0px -4px
	}

	.car-instruments {
		margin-top: 30px;
	}
	
</style>


