<script lang="ts">
	import { onMount, onDestroy } from 'svelte';
	import { user } from '$lib/stores/user';
	import { socket } from '$lib/stores/socket';
	import { page } from '$app/stores';
	import type { LightState } from '$lib/types/models';

	import PhasorChart from "$lib/components/chart-components/PhasorChart.svelte";
	import PowerChart from '$lib/components/chart-components/PowerChart.svelte';

	import Complex from 'complex.js'

	$: lightState = {
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

	$: angIa = 0;
	$: angIb = 0;
	$: angIc = 0;

	let isSocketConnected = false;
	let isCollapsedSimetricalComp = true;
	let timeout;
    
	let maxScale = 1;
	let previousScale = 1;
	let phasorDecimalPlaces = 1;

	let powerMaxScale = 1;
	let phasePowerMaxScale = 1;
	let previousPowerScale = 1;

	let powerScaleOptions = [
		{ option: 0, name: 'auto', selected: true },
		{ option: 1, name: 'Igual a total  ', selected: false },
        { option: 2, name: '1/3 de total  ', selected: false }
    ];
	let phasePowerScaleOption = 0;

	let Iabs = [0, 0, 0];
	let Iang = [0, 0, 0];
	
	let A   = new Complex(0, 0);
	let B   = new Complex(0, 0);
	let C   = new Complex(0, 0);
	let Ia1 = new Complex(0, 0);
	let Ib1 = new Complex(0, 0);
	let Ic1 = new Complex(0, 0);
	let Ia2 = new Complex(0, 0);
	let Ib2 = new Complex(0, 0);
	let Ic2 = new Complex(0, 0);
	let I0  = new Complex(0, 0);

	let calculatedImbalanceVoltage = 0;
	let calculatedImbalanceCurrent = 0;

	function handleCheckboxChange(selectedOption) {
        powerScaleOptions = powerScaleOptions.map(option => {
            if (option === selectedOption) {
				phasePowerScaleOption = option.option;
                return { ...option, selected: true };
            } else {
                return { ...option, selected: false };
            }
        });
		intermediateCalculation();
    }

	function intermediateCalculation() {
		if (lightState.PA != 0)
			angIa = (Math.sign(lightState.QA) * Math.atan(Math.abs(lightState.QA)/lightState.PA)) * 180/Math.PI;
		else angIa = Math.sign(lightState.QA) * 90;
		if (lightState.PB != 0)
			angIb = Math.sign(lightState.QB) * Math.atan(Math.abs(lightState.QB)/lightState.PB) * 180/Math.PI;
		else angIb = Math.sign(lightState.QA) * 90;
		if (lightState.PC != 0)
			angIc = Math.sign(lightState.QC) * Math.atan(Math.abs(lightState.QC)/lightState.PC) * 180/Math.PI;
		else angIc = Math.sign(lightState.QA) * 90;

		Iabs = [Math.abs(lightState.IA), Math.abs(lightState.IB), Math.abs(lightState.IC)];
		Iang = [angIa, 240+angIb, 120+angIc];
		
		let order = -5;
		let Imax = Iabs.reduce((p, c) => {return c > p ? c : p})
		let Iave = Iabs.reduce((accumulator, currentValue) => accumulator + currentValue, 0) / 3.0;
		let Idif = [0, 0, 0];
		for (let i = 0; i < 3; i++) Idif[i] = Math.abs(Iabs[i] - Iave);
		calculatedImbalanceCurrent = Idif.reduce((p, c) => {return c > p ? c : p})
		calculatedImbalanceCurrent = 100 * calculatedImbalanceCurrent / Iave;
		
		let Vdif = [lightState.VA, lightState.VB, lightState.VC];
		let Vave = Vdif.reduce((accumulator, currentValue) => accumulator + currentValue, 0) / 3.0;
		for (let i = 0; i < 3; i++) Vdif[i] = Math.abs(Vdif[i] - Vave);
		calculatedImbalanceVoltage = Vdif.reduce((p, c) => {return c > p ? c : p})
		calculatedImbalanceVoltage = 100 * calculatedImbalanceVoltage / Vave;
		
		if (Imax > previousScale * 1.08 || Imax < previousScale * 0.8) {
			while (10 ** order < Imax) order++;
			maxScale = 10 ** order;
			let lastAjust = maxScale;
			if (Imax / maxScale < 0.8) lastAjust = 0.8 * maxScale;
			if (Imax / maxScale < 0.6) lastAjust = 0.6 * maxScale;
			if (Imax / maxScale < 0.48) lastAjust = 0.52 * maxScale;
			if (Imax / maxScale < 0.4) lastAjust = 0.4 * maxScale;
			if (Imax / maxScale < 0.32) lastAjust = 0.32 * maxScale;
			if (Imax / maxScale < 0.24) lastAjust = 0.24 * maxScale;
			if (Imax / maxScale < 0.2) lastAjust = 0.2 * maxScale;
			if (Imax / maxScale < 0.16) lastAjust = 0.16 * maxScale;
			if (Imax / maxScale < 0.12) lastAjust = 0.12 * maxScale;
			maxScale = lastAjust;
		} else {
			maxScale = previousScale;
		}
		previousScale = maxScale;

		A = new Complex(Iabs[0]*Math.cos(Iang[0]*Math.PI/180), Iabs[0]*Math.sin(Iang[0]*Math.PI/180));
		B = new Complex(Iabs[1]*Math.cos(Iang[1]*Math.PI/180), Iabs[1]*Math.sin(Iang[1]*Math.PI/180));
		C = new Complex(Iabs[2]*Math.cos(Iang[2]*Math.PI/180), Iabs[2]*Math.sin(Iang[2]*Math.PI/180));

		let a = new Complex(1*Math.cos(120*Math.PI/180), 1*Math.sin(120*Math.PI/180));

		let oneThird = new Complex(1/3, 0);
		
		// positive sequence
		Ia1 = (A.add(a.mul(B)).add(a.mul(a).mul(C))).mul(oneThird);
		Ib1 = a.mul(a).mul(Ia1);
		Ic1 = a.mul(Ia1);

		// negative sequence
		Ia2 = (A.add(a.mul(a).mul(B)).add(a.mul(C))).mul(oneThird);
		Ib2 = a.mul(Ia2);
		Ic2 = a.mul(a).mul(Ia2);

		// zero sequence
		I0 = (A.add(B).add(C)).mul(oneThird);

		if (Imax < 10) phasorDecimalPlaces = 2; else phasorDecimalPlaces = 1;

		// for power charts
		let maxPower = Math.abs(lightState.Pt);
    	if (maxPower < Math.abs(lightState.Qt)) maxPower = Math.abs(lightState.Qt);
    
		order = -5;
		if (maxPower > previousPowerScale * 1.05 || maxPower < previousPowerScale * 1) {
			while (10 ** order < maxPower) order++;
			powerMaxScale = 10 ** order;
			let lastAjust = powerMaxScale;
			if (maxPower / powerMaxScale < 0.9) lastAjust = 0.9 * powerMaxScale;
			if (maxPower / powerMaxScale < 0.72) lastAjust = 0.72 * powerMaxScale;
			if (maxPower / powerMaxScale < 0.60) lastAjust = 0.6 * powerMaxScale;
			if (maxPower / powerMaxScale < 0.54) lastAjust = 0.54 * powerMaxScale;
			if (maxPower / powerMaxScale < 0.39) lastAjust = 0.39 * powerMaxScale;
			if (maxPower / powerMaxScale < 0.3) lastAjust = 0.3 * powerMaxScale;
			if (maxPower / powerMaxScale < 0.24) lastAjust = 0.24 * powerMaxScale;
			if (maxPower / powerMaxScale < 0.18) lastAjust = 0.18 * powerMaxScale;
			if (maxPower / powerMaxScale < 0.12) lastAjust = 0.12 * powerMaxScale;
			powerMaxScale = lastAjust;
		} else {
			powerMaxScale = previousPowerScale;
		}
		previousPowerScale = powerMaxScale;

		phasePowerMaxScale = 0;
		if (phasePowerScaleOption === 1) phasePowerMaxScale = powerMaxScale;
		if (phasePowerScaleOption === 2) phasePowerMaxScale = -powerMaxScale / 3;

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
			intermediateCalculation();

			console.log("ppppp --> getLightState");

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
			intermediateCalculation();
			// console.log(lightState);
			// console.log(`${lightState.PB} ${lightState.QB} ${angIb.toFixed(2)} ${lightState.pfB}`);
		});
		timeout = setTimeout(requestData, 500); // Request data every 2 seconds
	});

	onDestroy(() => {
		socket.off('led');
		clearTimeout(timeout);
	});

</script>

<div>

	<h3 class="header">Voltajes y Corrientes Trifásicas</h3>

	<div class="phasors">
		<div class="c-phasor">Ic={lightState.IC.toFixed(phasorDecimalPlaces)}∠{angIc.toFixed(1)}&deg</div>
		<div class="b-phasor">Ib={lightState.IB.toFixed(phasorDecimalPlaces)}∠{angIb.toFixed(1)}&deg</div>
		<div class="a-phasor">Ia={lightState.IA.toFixed(phasorDecimalPlaces)}∠{angIa.toFixed(1)}&deg</div>
	</div>

	<!-- phasor diagram -->
	<div class="phasor-diagram">
		<PhasorChart scale={maxScale} Iabs={[A.abs(), B.abs(), C.abs()]} Iang={[A.arg()*180/Math.PI, B.arg()*180/Math.PI, C.arg()*180/Math.PI]}
					phaseVoltages={[lightState.VA, lightState.VB, lightState.VC]} lineVoltages={[lightState.UAB, lightState.UBC, lightState. UCA]}
					showPhaseVoltages={true} showLineVoltages={true}
					/>
	</div>

	<div class="umbalance">
		<table>
			<tr>
				<th class="cell-item">Desbalance</th>
				<th class="cell-data-right">Medidor</th>
				<th class="cell-data-right">Calculado</th>
			</tr>
			<tr>
				<td class="cell-item">Voltaje</td>
				<td class="cell-data-right">{lightState.imbalanceVoltage.toFixed(1)} %</td>
				<td class="cell-data-right">{calculatedImbalanceVoltage.toFixed(1)} %</td>
			</tr>
			<tr>
				<td class="cell-item">Corriente</td>
				<td class="cell-data-right">{lightState.imbalanceCurrent.toFixed(1)} %</td>
				<td class="cell-data-right">{calculatedImbalanceCurrent.toFixed(1)} %</td>
			</tr>
		</table>


	</div>

	<!-- svelte-ignore a11y-click-events-have-key-events -->
	<!-- svelte-ignore a11y-no-static-element-interactions -->
	<div class="symetric-components" on:click={() => isCollapsedSimetricalComp = !isCollapsedSimetricalComp}>
		<span class="collapsible-icon" style="transform: rotate({isCollapsedSimetricalComp ? '180deg' : '0'})">&#9650;</span> 
		<div>Componentes Simétricas de las Corrientes</div>
	</div>
		<div class="symetric-components-items" style="transition: max-height 1s; overflow: hidden; max-height: {isCollapsedSimetricalComp ? '0' : '600px'};">
			<div class="chart-container">
			<div class="chart-column">
				<div class="chart-title-first">Secuencia positiva</div>
				<PhasorChart size={160} scale={maxScale} showPhaseVoltages={false} showLineVoltages={false} showValue={'Ia+'} showValueColor={'gray'}
					strokeWidth={[1, 1, 1]}
					Iabs={[Ia1.abs(), Ib1.abs(), Ic1.abs()]} 
					Iang={[Ia1.arg()*180/Math.PI, Ib1.arg()*180/Math.PI, Ic1.arg()*180/Math.PI]}
				/>
				<div class="chart-title">Secuencia negativa</div>
				<PhasorChart size={160} scale={maxScale} showPhaseVoltages={false} showLineVoltages={false} showValue={'Ia-'} showValueColor={'gray'}
					strokeWidth={[1, 1, 1]}
					Iabs={[Ia2.abs(), Ib2.abs(), Ic2.abs()]} 
					Iang={[Ia2.arg()*180/Math.PI, Ib2.arg()*180/Math.PI, Ic2.arg()*180/Math.PI]}
				/>
				<div class="chart-title">Secuencia cero</div>
				<PhasorChart size={160} scale={maxScale} showPhaseVoltages={false} showLineVoltages={false} showValue={'Io'} showValueColor={'gray'}
					strokeWidth={[1, 1, 1]}
					Iabs={[I0.abs()]} Iang={[I0.arg()*180/Math.PI]}
				/>
			</div>
			<div class="separator"></div>
			<div class="chart-column">
				<div class="chart-title-first">Ia</div>
				<PhasorChart size={160} scale={maxScale} showPhaseVoltages={false} showLineVoltages={false} showValue={'Ia'} showValueColor={'red'}
					Icolor={['rgba(255, 0, 0, 0.3)', 'red', 'red', 'red']}
					strokeWidth={[3, 1, 1, 1]}
					Iabs={[A.abs(), Ia1.abs(), Ia2.abs(), I0.abs()]} 
					Iang={[A.arg()*180/Math.PI, Ia1.arg()*180/Math.PI, Ia2.arg()*180/Math.PI, I0.arg()*180/Math.PI]}
				/>
				<div class="chart-title">Ib</div>
				<PhasorChart size={160} scale={maxScale} showPhaseVoltages={false} showLineVoltages={false} showValue={'Ib'} showValueColor={'blue'}
					Icolor={['rgba(0, 0, 255, 0.3)', 'blue', 'blue', 'blue']}
					strokeWidth={[3, 1, 1, 1]}
					Iabs={[B.abs(), Ib1.abs(), Ib2.abs(), I0.abs()]} 
					Iang={[B.arg()*180/Math.PI, Ib1.arg()*180/Math.PI, Ib2.arg()*180/Math.PI, I0.arg()*180/Math.PI]}
				/>
				<div class="chart-title">Ic</div>
				<PhasorChart size={160} scale={maxScale} showPhaseVoltages={false} showLineVoltages={false}  showValue={'Ic'} showValueColor={'green'}
					Icolor={['rgba(0, 128, 0, 0.3)', 'green', 'green', 'green']}
					strokeWidth={[3, 1, 1, 1]}
					Iabs={[C.abs(), Ic1.abs(), Ic2.abs(), I0.abs()]} 
					Iang={[C.arg()*180/Math.PI, Ic1.arg()*180/Math.PI, Ic2.arg()*180/Math.PI, I0.arg()*180/Math.PI]}
				/>
			</div>
		</div>

	</div>

	<!-- power triangle -->

	<h3 class="powers">Potencias</h3>

	<div class="scale-title">Escala para gráficos de fases: </div>
	<div class="scale-container">
		{#each powerScaleOptions as option}
			<label class="scale-options">
				<input type="checkbox" bind:checked={option.selected} on:change={() => handleCheckboxChange(option)} />
				{option.name}
			</label>
		{/each}
	</div>

	<div class="power-diagrams" style="display: flex;">
		<div class="power-diagram" style="display: flex; justify-content: center; align-items: center; flex: 1;">
			<PowerChart scale={powerMaxScale} P={lightState.Pt} Q={lightState.Qt} S={lightState.St} fp={lightState.pft} charWidth={240} chartHeight={384} />
		</div>
		<div class="power-diagram" style="display: flex; justify-content: center; align-items: center; flex-direction: column; flex: 1;">
			<div style="display: flex; justify-content: center; align-items: center; flex-direction: row; flex: 1;">
				<div style="writing-mode: vertical-rl; transform: rotate(180deg);">fase A</div>
				<PowerChart scale={phasePowerMaxScale} P={lightState.PA} Q={lightState.QA} S={lightState.SA} fp={lightState.pfA} charWidth={75} chartHeight={130}/>
			</div>
			<div style="display: flex; justify-content: center; align-items: center; flex-direction: row; flex: 1;">
				<div style="writing-mode: vertical-rl; transform: rotate(180deg);">fase B</div>
				<PowerChart scale={phasePowerMaxScale} P={lightState.PB} Q={lightState.QB} S={lightState.SB} fp={lightState.pfB} charWidth={75} chartHeight={130}/>
			</div>
			<div style="display: flex; justify-content: center; align-items: center; flex-direction: row; flex: 1;">
				<div style="writing-mode: vertical-rl; transform: rotate(180deg);">fase C</div>
				<PowerChart scale={phasePowerMaxScale} P={lightState.PC} Q={lightState.QC} charWidth={75} S={lightState.SC} fp={lightState.pfC} chartHeight={130}/>
			</div>
		</div>
	</div>

</div>


<style>

	.scale-container {
        display: flex;
        align-items: center;
		margin-bottom: 10px;
    }
    .scale-options {
        margin-right: 10px;
    }


	.header {
		font-weight: bold;
		text-align: center;
		margin: 0px 0px 10px ;
	}
	.phasors {
        display: flex;
        justify-content: space-between;
		margin: 0px 0px 10px 0px;
    }

    .c-phasor {
		color: green;
    }
	
	.b-phasor {
		color: blue;
    }
	
	.a-phasor {
		color: red;
    }
	.phasor-diagram {
        display: flex;
        justify-content: center;
    }

	h3 {
		font-size: 15px;
		color: white;
		padding-top: 2px;
		padding-bottom: 2px;
		background-color: rgb(176, 177, 113);
	}

	table {
        border-collapse: collapse;
        width: 100%;
    }
    td, th {
        border: 1px solid gray;
        padding: 2px;
        text-align: center;
    }
	th {
		font-weight: normal;
        background-color: #c4c4c46d;
    }
	.collapsible-icon {
		margin-right: 15px; /* Adjust the spacing between elements */
		transition: transform 0.5s;
        transform-origin: center;
        text-align: center;
    }
	.symetric-components {
		display: flex;
        align-items: center;
		font-size: 15px;
		font-weight: bold;
		color: white;
		background-color: rgb(177, 177, 177);
		margin: 20px 0% 0px 0%;
		padding-top: 2px;
		padding-bottom: 2px;
	}
	.symetric-components-items {
		background-color: rgba(242, 242, 239, 0.5);
		margin: 0px 0% 0px 0%;
	}

	.chart-container {
        display: grid;
        grid-template-columns: 1fr auto 1fr; /* Divide el espacio en tres partes iguales */
        width: 100%;
    }
	
	.chart-column {
        display: flex;
        flex-direction: column;
        align-items: center;
    }

	.chart-title-first {
		padding-top: 10px;
		font-weight: bold;
	}
	.chart-title {
		padding-top: 0px;
		font-weight: bold;
	}
	.separator {
        width: 1px;
		background-color: lightgray;
    }

	.scale-title {
        text-align: center;
		margin-top: 5px;
    }
	.scale-container {
        display: flex;
        align-items: center;
    }
    .scale-options {
        margin-left: auto;
        margin-right: auto;
    }
	input[type="checkbox"] {
		accent-color: rgb(178, 177, 177);
    }

	.powers {
		font-weight: bold;
		text-align: center;
		margin: 20px 0px 0px 0px;
	}
	.scale-options{
		margin-left: 20px;
	}
</style>