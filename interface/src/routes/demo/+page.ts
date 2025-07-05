<script lang="ts">
	// import Ampmeter from "./Ampmeter.svelte";

	import { onMount, onDestroy } from 'svelte';
	import { user } from '$lib/stores/user';
	import { page } from '$app/stores';
	import { notifications } from '$lib/components/toasts/notifications';
	import Gauge from '$lib/components/gauge.svelte';
	import { socket } from '$lib/stores/socket';
	// import type { systemState } from '$lib/types/models';

	import Controller from '$lib/components/Controller.svelte';

	let systemState = {
		modbusErrors: 0,
		modbusFails: 0,
		modbusState: false
	};
	let value = 180;

	let timeStopButtonPressed = 8000;

	$: varCmdStop = false;

	$: ledStatus = [false, false, false, false, false, false, false, false, false, false];

	let isSocketConnected = false;
	let timeout: string | number | NodeJS.Timeout | undefined;

	async function getsystemState() {
		try {
			const response = await fetch('/rest/systemState', {
				method: 'GET',
				headers: {
					Authorization: $page.data.features.security ? 'Bearer ' + $user.bearer_token : 'Basic',
					'Content-Type': 'application/json'
				}
			});
			const light = await response.json();
			systemState = light;
		} catch (error) {
			console.error('Error:', error);
		}
		return;
	}

	// function requestData() {
	// 	if (!isSocketConnected) {
	// 		getsystemState();
	// 	}
	// 	isSocketConnected = false;
	// 	timeout = setTimeout(requestData, 2500);
	// }
	function getRandomNumber(min: number, max: number) {
		return Math.floor(Math.random() * (max - min + 1)) + min;
	}
	onMount(() => {
		socket.on<systemState>('led', (merterData) => {
			// systemState = merterData;
			isSocketConnected = true;
		});
		// timeout = setTimeout(requestData, 500);
		const interval = setInterval(() => {
			value = getRandomNumber(30, 350);
		}, 5000);
		varCmdStop = false;
	});

	onDestroy(() => {
		socket.off('led');
		clearTimeout(timeout);
	});

	function offCmdStop() {
		varCmdStop = false;
		// postsystemState();
	}

	function handleCmdStop() {
		if (!varCmdStop) {
			varCmdStop = true; // Cambia el estado
			setTimeout(offCmdStop, timeStopButtonPressed);
			// postsystemState();
		}
	}

	// async function postsystemState() {
	// 	try {
	// 		const response = await fetch('/rest/systemState', {
	// 			method: 'POST',
	// 			headers: {
	// 				Authorization: $page.data.features.security ? 'Bearer ' + $user.bearer_token : 'Basic',
	// 				'Content-Type': 'application/json'
	// 			},
	// 			body: JSON.stringify({ cmdStop: varCmdStop })
	// 		});
	// 		if (response.status == 200) {
	// 			// notifications.success('Light state updated.', 3000);
	// 			const light = await response.json();
	// 			varCmdStop = light.cmdStop;
	// 		} else {
	// 			// notifications.error('User not authorized.', 3000);
	// 		}
	// 	} catch (error) {
	// 		console.error('Error:', error);
	// 	}
	// }
</script>



<!-- instruments -->
<div class="car-instruments">
	<Gauge {value} />
	<Controller />
</div>

<style>
	.car-instruments {
		display: flex; /* Use Flexbox to arrange elements side by side */
		margin-top: 1rem;
	}
	@media (max-width: 640px) {
		.car-instruments {
			flex-direction: column; /* Stack elements vertically on smaller screens */
		}
	}
</style>
