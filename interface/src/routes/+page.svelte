<script lang="ts">
	import type { PageData } from './$types';
	import { onMount, onDestroy } from 'svelte';
	import Gauge from '$lib/components/panel-components/Voltmeter.svelte';
	import Controller from '$lib/components/Controller.svelte';
	import { user } from '$lib/stores/user';
	import { page } from '$app/state';
	import { notifications } from '$lib/components/toasts/notifications';
	import { socket } from '$lib/stores/socket';
	import type { LightState } from '$lib/types/models';

	let lightState: LightState = $state({ led_on: false });

	let lightOn = $state(false);

	async function getLightstate() {
		try {
			const response = await fetch('/rest/lightState', {
				method: 'GET',
				headers: {
					Authorization: page.data.features.security ? 'Bearer ' + $user.bearer_token : 'Basic',
					'Content-Type': 'application/json'
				}
			});
			const light = await response.json();
			lightOn = light.led_on;
		} catch (error) {
			console.error('Error:', error);
		}
		return;
	}

	onMount(() => {
		// socket.on<LightState>('led', (data) => {
		// 	lightState = data;
		// });
		getLightstate();
	});

	onDestroy(() => socket.off('led'));

	async function postLightstate() {
		try {
			const response = await fetch('/rest/lightState', {
				method: 'POST',
				headers: {
					Authorization: page.data.features.security ? 'Bearer ' + $user.bearer_token : 'Basic',
					'Content-Type': 'application/json'
				},
				body: JSON.stringify({ led_on: lightOn })
			});
			if (response.status == 200) {
				notifications.success('Light state updated.', 3000);
				const light = await response.json();
				lightOn = light.led_on;
			} else {
				notifications.error('User not authorized.', 3000);
			}
		} catch (error) {
			console.error('Error:', error);
		}
	}

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
