
<script>
import StatusLed from '$lib/components/panel-components/StatusLed.svelte';

export let ledStatus = [false, false, false, false, false, false, false, false, false, false]; 

$: leds1 = [
    { name: '', active: ledStatus[0], color: 'no' },
    { name: '', active: ledStatus[0], color: 'no' },
    { name: 'Encendido', active: ledStatus[0], color: 'green' },
    { name: '', active: ledStatus[0], color: 'no' },
    { name: '', active: ledStatus[0], color: 'no' },
    { name: 'Funcionando', active: ledStatus[1], color: 'green' },
    { name: '', active: ledStatus[0], color: 'no' },
    { name: '', active: ledStatus[0], color: 'no' },
    { name: 'Batería Ok', active: ledStatus[2], color: 'green' },    
    ];

$: leds2 = [
        { name: 'Voltaje Batería Bajo', active: ledStatus[3], color: 'red' },
        { name: 'Voltaje Batería Alto', active: ledStatus[4], color: 'red' },
        { name: 'Temp. refrigerante alta', active: ledStatus[6], color: 'red' },
        { name: 'Presión de aceite baja', active: ledStatus[7], color: 'red' },
        { name: 'Velocidad fuera rango', active: ledStatus[8], color: 'red' },
        { name: 'Voltaje fuera rango', active: ledStatus[9], color: 'red' },
        
    ];
    
</script>

<div class="led-container">
	<div class="LedColumn">
		{#each leds1 as led, i}
			<StatusLed name={led.name} active={led.active} color={led.color}/>
		{/each}
	</div>
	<div class="separator"></div>
	<div class="LedColumn">
		{#each leds2 as led}
			<StatusLed name={led.name} active={led.active} color={led.color}/>
		{/each}
	</div>
</div>

<style>

    .led-container {
        display: grid;
        grid-template-columns: 1fr auto 1fr; /* Divide el espacio en tres partes iguales */
        width: 100%;
    }
	
	.LedColumn {
        display: flex;
        flex-direction: column;
        align-items: left;
    }

	.separator {
        width: 10px;
    }
	
</style>
