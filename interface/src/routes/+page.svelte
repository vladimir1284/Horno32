<script lang="ts">
	import type { PageData } from './$types';
	import { onMount, onDestroy } from 'svelte';
	import Gauge from '$lib/components/panel-components/Voltmeter.svelte';
	import Controller from '$lib/components/Controller.svelte';
	import { user } from '$lib/stores/user';
	import { page } from '$app/state';
	import { notifications } from '$lib/components/toasts/notifications';
	import { socket } from '$lib/stores/socket';
	import type { HornoState } from '$lib/types/models';

	let hornoState: HornoState = $state({
		on: false,
		mode: 'auto',
		setpoint: 0,
		manual_power_top: 0,
		manual_power_bottom: 0,
		temperature: 0,
		actual_power_top: 0,
		actual_power_bottom: 0,
		hard_max_duty_top: 0,
		hard_max_duty_bottom: 0
	});

	async function getHornoState() {
		try {
			const response = await fetch('/rest/hornoState', {
				method: 'GET',
				headers: {
					Authorization: page.data.features.security ? 'Bearer ' + $user.bearer_token : 'Basic',
					'Content-Type': 'application/json'
				}
			});
			hornoState = await response.json();
		} catch (error) {
			console.error('Error:', error);
		}
	}

	onMount(() => {
		socket.on<HornoState>('horno', (data) => {
			hornoState = data;
		});
	});

	onDestroy(() => socket.off('horno'));

	interface Props {
		data: PageData;
	}

	let { data }: Props = $props();

	onMount(() => {
		const interval = setInterval(() => {
			getHornoState();
		}, 5000);
		getHornoState();
	});
</script>

<div class="hero bg-base-100 h-screen flex items-center justify-center">
	<div class="card p-0 md:card-side shadow-primary shadow-2xl">
		<div class="car-instruments">
			<Gauge
				svgWidth={300}
				svgHeight={300}
				magnitude={hornoState.temperature}
				minScale={30}
				maxScale={350}
				scaleSmallDivisions={64}
				scaleLargeDivisions={8}
				units="°C"
				decimalPlaces={1}
				needleColor="red"
				varName="T"
			/>
			<Controller {hornoState} onUpdated={(updated) => (hornoState = updated)} />
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
