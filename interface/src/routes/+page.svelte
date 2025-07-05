<script lang="ts">
	import type { PageData } from './$types';
	import { onMount, onDestroy } from 'svelte';
	import Gauge from '$lib/components/panel-components/Voltmeter.svelte';
	import Controller from '$lib/components/Controller.svelte';
	import { notifications } from '$lib/components/toasts/notifications';
	import { tick } from 'svelte';

	interface Props {
		data: PageData;
	}

	let { data }: Props = $props();
	let value = $state(180);

	function getRandomNumber(min: number, max: number) {
		return Math.floor(Math.random() * (max - min + 1)) + min;
	}

	onMount(() => {
		// socket.on<systemState>('led', (merterData) => {
		// 	// systemState = merterData;
		// 	isSocketConnected = true;
		// });
		// timeout = setTimeout(requestData, 500);
		const interval = setInterval(() => {
			value = getRandomNumber(30, 350);
		}, 5000);
		// varCmdStop = false;
	});
</script>

<div class="hero bg-base-100 h-screen flex items-center justify-center">
	<div class="card p-0 md:card-side shadow-primary shadow-2xl">
		<div class="car-instruments">
			<Gauge
				svgWidth={300}
				svgHeight={300}
				magnitude={value}
				minScale={30}
				maxScale={350}
				scaleSmallDivisions={64}
				scaleLargeDivisions={8}
				units="°C"
				decimalPlaces={1}
				needleColor="red"
				varName="T"
			/>
			<Controller />
		</div>
	</div>
</div>

<style>
	.car-instruments {
		display: flex;
		align-items: center;
		justify-content: center;
		gap: 0;
		padding: 0;
	}
	@media (max-width: 640px) {
		.car-instruments {
			flex-direction: column;
		}
	}
</style>
