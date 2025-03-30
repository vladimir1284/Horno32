<script lang="ts">
	import { onMount, onDestroy } from 'svelte';
	import { user } from '$lib/stores/user';
	import { socket } from '$lib/stores/socket';
	import { page } from '$app/stores';
	import type { LightState } from '$lib/types/models';


	let isCollapsedEnergy = false;
    let isCollapsedLineCurrent = false;
    let isCollapsedPhaseVoltage = false;
    let isCollapsedLineVoltage = false;
    let isCollapsedPowerFactor = true;
    let isCollapsedActivePower = true;
    let isCollapsedReactivePower = true;
    let isCollapsedAparentPower = true;
    let isCollapsedOtherParameters = false;

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

	let isSocketConnected = false;
	let timeout;
    
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
			console.log(lightState);
		});
		timeout = setTimeout(requestData, 500); // Request data every 2 seconds
	});

	onDestroy(() => {
		socket.off('led');
		clearTimeout(timeout);
	});


</script>


<!-- Energy -->
<div> 
    <div class="energy" on:click={() => isCollapsedEnergy = !isCollapsedEnergy}>
        <span class="collapsible-icon" style="transform: rotate({isCollapsedEnergy ? '180deg' : '0'})">&#9650;</span> 
        <h3>Energía</h3>
    </div>
    <div class="energy-items" style="transition: max-height 1s; overflow: hidden; max-height: {isCollapsedEnergy ? '0' : '300px'};">
        <table>
            <tr>
                <td class="cell-item">Activa total [kWh]</td>
                <td class="cell-data-right">{lightState.tPenergy.toFixed(2)}</td>
            </tr>
			<tr>
                <td class="cell-item">Activa positiva [kWh]</td>
                <td class="cell-data-right">{lightState.pPenergy.toFixed(2)}</td>
            </tr>
			<tr>
                <td class="cell-item">Activa inversa [kWh]</td>
                <td class="cell-data-right">{lightState.rPenergy.toFixed(2)}</td>
            </tr>
			<tr>
                <td class="cell-item">Reactiva total [kVArh]</td>
                <td class="cell-data-right">{lightState.tQenergy.toFixed(2)}</td>
            </tr>
			<tr>
                <td class="cell-item">Reactiva positiva [kVArh]</td>
                <td class="cell-data-right">{lightState.pQenergy.toFixed(2)}</td>
            </tr>
			<tr>
                <td class="cell-item">Reactiva inversa [kVArh]</td>
                <td class="cell-data-right">{lightState.rQenergy.toFixed(2)}</td>
            </tr>
        </table>
    </div>
</div>

<!-- Line Current -->
<div>
    <div class="line-current" on:click={() => isCollapsedLineCurrent = !isCollapsedLineCurrent}>
		<div class="collapsible-icon" style="transform: rotate({isCollapsedLineCurrent ? '180deg' : '0'})">&#9650;</div> 
		<h3 style="font-weight: normal;">Corriente de fase [A]</h3>
	</div>
	<div class="line-current-items" style="transition: max-height 1s; overflow: hidden; max-height: {isCollapsedLineCurrent ? '0' : '60px'};">
        <table>
            <tr>
                <td class="cell-data">A: {lightState.IA.toFixed(3)}</td>
                <td class="cell-data">B: {lightState.IB.toFixed(3)}</td>
                <td class="cell-data">C: {lightState.IC.toFixed(3)}</td>
            </tr>
        </table>
    </div>
</div>

<!-- Phase Voltage -->
<div>
    <div class="phase-voltage" on:click={() => isCollapsedPhaseVoltage = !isCollapsedPhaseVoltage}>
		<div class="collapsible-icon" style="transform: rotate({isCollapsedPhaseVoltage ? '180deg' : '0'})">&#9650;</div> 
		<h3 style="font-weight: normal;">Voltajes de fase [V]</h3>
	</div>
	<div class="phase-voltage-items" style="transition: max-height 1s; overflow: hidden; max-height: {isCollapsedPhaseVoltage ? '0' : '60px'};">
        <table>
            <tr>
                <td class="cell-data">A: {lightState.VA.toFixed(1)}</td>
                <td class="cell-data">B: {lightState.VB.toFixed(1)}</td>
                <td class="cell-data">C: {lightState.VC.toFixed(1)}</td>
            </tr>
        </table>
    </div>
</div>

<!-- Line Voltage -->
<div>
    <div class="line-voltage" on:click={() => isCollapsedLineVoltage = !isCollapsedLineVoltage}>
		<div class="collapsible-icon" style="transform: rotate({isCollapsedLineVoltage ? '180deg' : '0'})">&#9650;</div> 
		<h3 style="font-weight: normal;">Voltajes de línea [V]</h3>
	</div>
	<div class="line-voltage-items" style="transition: max-height 1s; overflow: hidden; max-height: {isCollapsedLineVoltage ? '0' : '60px'};">
        <table>
            <tr>
                <td class="cell-data">A: {lightState.UAB.toFixed(1)}</td>
                <td class="cell-data">B: {lightState.UBC.toFixed(1)}</td>
                <td class="cell-data">C: {lightState.UCA.toFixed(1)}</td>
            </tr>
        </table>
    </div>
</div>

<!-- Power Factor -->
<div>
    <div class="power-factor" on:click={() => isCollapsedPowerFactor = !isCollapsedPowerFactor}>
		<div class="collapsible-icon" style="transform: rotate({isCollapsedPowerFactor ? '180deg' : '0'})">&#9650;</div> 
		<h3 style="font-weight: normal;">Factor de potecia:</h3>
		<div class="total">total:</div>
		<div class="data">{lightState.pft.toFixed(2)}</div>
	</div>
	<div class="power-factor-items" style="transition: max-height 1s; overflow: hidden; max-height: {isCollapsedPowerFactor ? '0' : '60px'};">
        <table>
            <tr>
                <td class="cell-data">A: {lightState.pfA.toFixed(2)}</td>
                <td class="cell-data">B: {lightState.pfB.toFixed(2)}</td>
                <td class="cell-data">C: {lightState.pfC.toFixed(2)}</td>
            </tr>
        </table>
    </div>
</div>

<!-- Active Power -->
<div>
    <div class="active-power" on:click={() => isCollapsedActivePower = !isCollapsedActivePower}>
		<div class="collapsible-icon" style="transform: rotate({isCollapsedActivePower ? '180deg' : '0'})">&#9650;</div> 
		<h3 style="font-weight: normal;">Potencia activa P [W]:</h3>
		<div class="total">total:</div>
		<div class="data">{lightState.Pt.toFixed(0)}</div>
	</div>
	<div class="active-power-items" style="transition: max-height 1s; overflow: hidden; max-height: {isCollapsedActivePower ? '0' : '60px'};">
        <table>
            <tr>
                <td class="cell-data">A: {lightState.PA.toFixed(0)}</td>
                <td class="cell-data">B: {lightState.PB.toFixed(0)}</td>
                <td class="cell-data">C: {lightState.PC.toFixed(0)}</td>
            </tr>
        </table>
    </div>
</div>

<!-- Reactive Power -->
<div>
    <div class="reactive-power" on:click={() => isCollapsedReactivePower = !isCollapsedReactivePower}>
		<div class="collapsible-icon" style="transform: rotate({isCollapsedReactivePower ? '180deg' : '0'})">&#9650;</div> 
		<h3 style="font-weight: normal;">Potencia reactiva Q [VAr]:</h3>
		<div class="total">total:</div>
		<div class="data">{lightState.Qt.toFixed(0)}</div>
	</div>
	<div class="reactive-power-items" style="transition: max-height 1s; overflow: hidden; max-height: {isCollapsedReactivePower ? '0' : '60px'};">
        <table>
            <tr>
                <td class="cell-data">A: {lightState.QA.toFixed(0)}</td>
                <td class="cell-data">B: {lightState.QB.toFixed(0)}</td>
                <td class="cell-data">C: {lightState.QC.toFixed(0)}</td>
            </tr>
        </table>
    </div>
</div>

<!-- Aparent Power -->
<div>
    <div class="aparent-power" on:click={() => isCollapsedAparentPower = !isCollapsedAparentPower}>
		<div class="collapsible-icon" style="transform: rotate({isCollapsedAparentPower ? '180deg' : '0'})">&#9650;</div> 
		<h3 style="font-weight: normal;">Potencia aparente S [VA]:</h3>
		<div class="total">total:</div>
		<div class="data">{lightState.St.toFixed(0)}</div>
	</div>
	<div class="aparent-power-items" style="transition: max-height 1s; overflow: hidden; max-height: {isCollapsedAparentPower ? '0' : '60px'};">
        <table>
            <tr>
                <td class="cell-data">A: {lightState.SA.toFixed(0)}</td>
                <td class="cell-data">B: {lightState.SB.toFixed(0)}</td>
                <td class="cell-data">C: {lightState.SC.toFixed(0)}</td>
            </tr>
        </table>
    </div>
</div>

<!-- Other Parameters -->
<div>
    <div class="other-parameters" on:click={() => isCollapsedOtherParameters = !isCollapsedOtherParameters}>
		<div class="collapsible-icon" style="transform: rotate({isCollapsedOtherParameters ? '180deg' : '0'})">&#9650;</div> 
		<h3 style="font-weight: normal;">Otros parámteros</h3>
	</div>
	<div class="other-parameters-items" style="transition: max-height 1s; overflow: hidden; max-height: {isCollapsedOtherParameters ? '0' : '350px'};">
        <table>
            <tr>
                <td class="cell-item">Frecuencia [Hz]</td>
                <td class="cell-data-right">{lightState.fA.toFixed(2)}</td>
            </tr>
			<tr>
                <td class="cell-item">Horas de trabajo [h]</td>
                <td class="cell-data-right">{lightState.workingHours.toFixed(1)}</td>
            </tr>
			<tr>
                <td class="cell-item">Temperatura [&degC]</td>
                <td class="cell-data-right">{lightState.temperature.toFixed(1)}</td>
            </tr>
			<tr>
                <td class="cell-item">Voltaje de batería [V]</td>
                <td class="cell-data-right">{lightState.batteryLevel.toFixed(1)}</td>
            </tr>
			<tr>
                <td class="cell-item">Presión de aceite [kg/cm&sup2]</td>
                <td class="cell-data-right">{lightState.oilPressure.toFixed(1)}</td>
            </tr>
			<tr>
                <td class="cell-item">RPM</td>
                <td class="cell-data-right">{lightState.rpm.toFixed(1)}</td>
            </tr>
			<tr>
                <td class="cell-item">Desbalance de voltaje [%]</td>
                <td class="cell-data-right">{lightState.imbalanceVoltage.toFixed(1)}</td>
            </tr>
			<tr>
                <td class="cell-item">Desbalance de corriente [%]</td>
                <td class="cell-data-right">{lightState.imbalanceCurrent.toFixed(1)}</td>
            </tr>

        </table>
    </div>
</div>


<style>
    table {
        border-collapse: collapse;
        width: 100%;
    }
    th, td {
        border: 1px solid black;
        padding: 8px;
        text-align: center;
    }
    th {
        background-color: #f2f2f2;
    }
    .collapsible-icon {
		margin-right: 15px; /* Adjust the spacing between elements */
		transition: transform 0.5s;
        transform-origin: center;
        text-align: center;
    }
	.energy {
		display: flex;
        align-items: center;
		font-size: 15px;
		font-weight: bold;
		color: white;
		background-color: rgb(119, 201, 144);
		margin: 20px 0% 0px 0%;
	}
	.energy-items {
		background-color: rgb(236, 247, 239);
		margin: 0px 0% 0px 0%;


	}
	.line-current {
		display: flex;
        align-items: center;
		font-size: 15px;
		font-weight: bold;
		color: white;
		background-color: rgb(173, 172, 108);
		margin: 20px 0% 0px 0%;
	}
	.line-current-items {
		background-color: rgb(231, 231, 221);
		margin: 0px 0% 0px 0%;
	}
	.phase-voltage {
		display: flex;
        align-items: center;
		font-size: 15px;
		font-weight: bold;
		color: white;
		background-color: rgb(181, 135, 135);
		margin: 20px 0% 0px 0%;
	}
	.phase-voltage-items {
		background-color: rgb(248, 222, 222);
		margin: 0px 0% 0px 0%;
	}
	.line-voltage {
		display: flex;
        align-items: center;
		font-size: 15px;
		font-weight: bold;
		color: white;
		background-color: rgb(138, 138, 188);
		margin: 20px 0% 0px 0%;
	}
	.line-voltage-items {
		background-color: rgb(243, 243, 250);
		margin: 0px 0% 0px 0%;
	}
	.power-factor {
		display: flex;
        align-items: center;
		font-size: 15px;
		font-weight: bold;
		color: white;
		background-color: rgb(186, 124, 183);
		margin: 20px 0% 0px 0%;
	}
	.power-factor-items {
		background-color: rgb(253, 238, 252);
		margin: 0px 0% 0px 0%;
	}
	.active-power {
		display: flex;
        align-items: center;
		font-size: 15px;
		font-weight: bold;
		color: white;
		background-color: rgb(117, 165, 159);
		margin: 20px 0% 0px 0%;
	}
	.active-power-items {
		background-color: rgb(238, 250, 249);
		margin: 0px 0% 0px 0%;
	}
	.reactive-power {
		display: flex;
        align-items: center;
		font-size: 15px;
		font-weight: bold;
		color: white;
		background-color: rgb(191, 156, 109);
		margin: 20px 0% 0px 0%;
	}
	.reactive-power-items {
		background-color: rgb(248, 237, 225);
		margin: 0px 0% 0px 0%;
	}
	.aparent-power {
		display: flex;
        align-items: center;
		font-size: 15px;
		font-weight: bold;
		color: white;
		background-color: rgb(189, 145, 216);
		margin: 20px 0% 0px 0%;
	}
	.aparent-power-items {
		background-color: rgb(250, 241, 255);
		margin: 0px 0% 0px 0%;
	}
	.other-parameters {
		display: flex;
        align-items: center;
		font-size: 15px;
		font-weight: bold;
		color: white;
		background-color: rgb(169, 167, 167);
		margin: 20px 0% 0px 0%;
	}
	.other-parameters-items {
		background-color: rgb(238, 237, 237);
		margin: 0px 0% 10px 0%;
	}
	.total {
		font-size: 18px;
		font-weight: normal;
		margin-left: auto;
		margin-right: 5px;
	}
	.data {
		font-size: 18px;
		margin-right: 5px;
	}
	.cell-item {
		width: 66.67%;
        text-align: start;
	}
	.cell-data {
		width: 33.30%;
        text-align: center;
	}
	.cell-data-right {
		width: 33.30%;
        text-align: end;
	}
</style>