<script lang="ts">
	import type { LayoutData } from './$types';
	import { onDestroy, onMount } from 'svelte';
	import { user } from '$lib/stores/user';
	import { telemetry } from '$lib/stores/telemetry';
	import { analytics } from '$lib/stores/analytics';
	import { socket } from '$lib/stores/socket';
	import type { userProfile } from '$lib/stores/user';
	import { page } from '$app/stores';
	import { Modals, closeModal } from 'svelte-modals';
	import Toast from '$lib/components/toasts/Toast.svelte';
	import { notifications } from '$lib/components/toasts/notifications';
	import { fade } from 'svelte/transition';
	import '../app.css';
	import Menu from '$lib/components/old-project-components/menu.svelte';
	import Statusbar from '$lib/components/old-project-components/statusbar.svelte';
	import Login from '$lib/components/old-project-components/login.svelte';
	import type { Analytics } from '$lib/types/models';
	import type { RSSI } from '$lib/types/models';
	import type { Battery } from '$lib/types/models';
	import type { DownloadOTA } from '$lib/types/models';

	export let data: LayoutData;

	import Header from './Header.svelte';
	import './styles.css';

	onMount(async () => {
		if ($user.bearer_token !== '') {
			await validateUser($user);
		}
		const ws_token = $page.data.features.security ? '?access_token=' + $user.bearer_token : '';
		socket.init(
			`ws://${window.location.host}/ws/events${ws_token}`,
			$page.data.features.event_use_json
		);
		addEventListeners();
		const element0 = document.getElementById('connect0');
		if (element0) element0.hidden = true;
		const element1 = document.getElementById('connect1');
		if (element1) element1.hidden = true;
		const element2 = document.getElementById('connect2');
		if (element2) element2.hidden = true;
	});

	onDestroy(() => {
		removeEventListeners();
	});

	const addEventListeners = () => {
		socket.on('open', handleOpen);
		socket.on('close', handleClose);
		socket.on('error', handleError);
		socket.on('rssi', handleNetworkStatus);
		socket.on('notification', handleNotification);
		if ($page.data.features.analytics) socket.on('analytics', handleAnalytics);
		if ($page.data.features.battery) socket.on('battery', handleBattery);
		if ($page.data.features.download_firmware) socket.on('otastatus', handleOAT);
	};

	const removeEventListeners = () => {
		socket.off('analytics', handleAnalytics);
		socket.off('open', handleOpen);
		socket.off('close', handleClose);
		socket.off('rssi', handleNetworkStatus);
		socket.off('notification', handleNotification);
		socket.off('battery', handleBattery);
		socket.off('otastatus', handleOAT);
	};

	async function validateUser(userdata: userProfile) {
		try {
			const response = await fetch('/rest/verifyAuthorization', {
				method: 'GET',
				headers: {
					Authorization: 'Bearer ' + userdata.bearer_token,
					'Content-Type': 'application/json'
				}
			});
			if (response.status !== 200) {
				user.invalidate();
			}
		} catch (error) {
			console.error('Error:', error);
		}
	}

	const handleOpen = () => {
		notifications.success('Connection to device established', 2000);
	};

	const handleClose = () => {
		notifications.error('Connection to device lost', 2000);
		telemetry.setRSSI({ rssi: 0, ssid: '' });
	};

	const handleError = (data: any) => console.error(data);

	const handleNotification = (data: any) => {
		switch (data.type) {
			case 'info':
				notifications.info(data.message, 2000);
				break;
			case 'warning':
				notifications.warning(data.message, 2000);
				break;
			case 'error':
				notifications.error(data.message, 2000);
				break;
			case 'success':
				notifications.success(data.message, 2000);
				break;
			default:
				break;
		}
	};

	const handleAnalytics = (data: Analytics) => analytics.addData(data);

	const handleNetworkStatus = (data: RSSI) => telemetry.setRSSI(data);

	const handleBattery = (data: Battery) => telemetry.setBattery(data);

	const handleOAT = (data: DownloadOTA) => telemetry.setDownloadOTA(data);

	let menuOpen = false;
</script>

<svelte:head>
	<title>{$page.data.title}</title>
</svelte:head>

{#if $page.data.features.security && $user.bearer_token === ''}
	<Login />
{:else}
	<!-- <div class="drawer lg:drawer-open"> -->
	<!-- <input id="main-menu" type="checkbox" class="drawer-toggle" bind:checked={menuOpen} /> -->
	<!-- <div class="drawer-content flex flex-col"> -->
	<!-- Status bar content here -->
	<!-- <Statusbar /> -->

	<!-- Main page content here -->
	<!-- <slot /> -->
	<!-- </div> -->
	<!-- Side Navigation -->
	<!-- <div class="drawer-side z-30 shadow-lg">
			<label for="main-menu" class="drawer-overlay" />
			<Menu
				on:menuClicked={() => {
					menuOpen = false;
				}}
			/>
		</div> -->
	<!-- </div> -->

	<div class="app">
		<Header />

		<main>
			<slot />
		</main>
	</div>
{/if}

<!-- <Modals> -->
<!-- svelte-ignore a11y-click-events-have-key-events -->
<!-- <div -->
<!-- slot="backdrop" -->
<!-- class="fixed inset-0 z-40 max-h-full max-w-full bg-black/20 backdrop-blur" -->
<!-- transition:fade -->
<!-- on:click={closeModal} -->
<!-- /> -->
<!-- </Modals> -->

<Toast />

<style>
	.app {
		display: flex;
		flex-direction: column;
		height: 100vh; /* Set the height of the app container to full viewport height */
		min-height: 100vh;
	}

	main {
		flex: 1;
		display: flex;
		flex-direction: column;
		padding: 1rem;
		width: 100%;
		max-width: 64rem;
		margin: 0 auto;
		box-sizing: border-box;
		overflow-y: auto; /* Enable vertical scrolling for the main element */
		padding: 0;
	}
</style>
