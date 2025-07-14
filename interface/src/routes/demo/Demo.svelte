<script lang="ts">
	import { onMount, onDestroy } from 'svelte';
	import { user } from '$lib/stores/user';
	import { page } from '$app/state';
	import { notifications } from '$lib/components/toasts/notifications';
	import SettingsCard from '$lib/components/SettingsCard.svelte';
	import Light from '~icons/tabler/bulb';
	import Info from '~icons/tabler/info-circle';
	import Save from '~icons/tabler/device-floppy';
	import Reload from '~icons/tabler/reload';
	import { socket } from '$lib/stores/socket';
	import type { HornoState } from '$lib/types/models';

	let hornoState: HornoState = $state({ led_on: false });

	let hornoOn = $state(false);

	async function getLightstate() {
		try {
			const response = await fetch('/rest/hornoState', {
				method: 'GET',
				headers: {
					Authorization: page.data.features.security ? 'Bearer ' + $user.bearer_token : 'Basic',
					'Content-Type': 'application/json'
				}
			});
			const horno = await response.json();
			hornoOn = horno.led_on;
		} catch (error) {
			console.error('Error:', error);
		}
		return;
	}

	onMount(() => {
		socket.on<HornoState>('led', (data) => {
			hornoState = data;
		});
		getLightstate();
	});

	onDestroy(() => socket.off('led'));

	async function postLightstate() {
		try {
			const response = await fetch('/rest/hornoState', {
				method: 'POST',
				headers: {
					Authorization: page.data.features.security ? 'Bearer ' + $user.bearer_token : 'Basic',
					'Content-Type': 'application/json'
				},
				body: JSON.stringify({ led_on: hornoOn })
			});
			if (response.status == 200) {
				notifications.success('Light state updated.', 3000);
				const horno = await response.json();
				hornoOn = horno.led_on;
			} else {
				notifications.error('User not authorized.', 3000);
			}
		} catch (error) {
			console.error('Error:', error);
		}
	}
</script>

<SettingsCard collapsible={false}>
	{#snippet icon()}
		<Light class="lex-shrink-0 mr-2 h-6 w-6 self-end" />
	{/snippet}
	{#snippet title()}
		<span>Light State</span>
	{/snippet}
	<div class="w-full">
		<h1 class="text-xl font-semibold">REST Example</h1>
		<div class="alert alert-info my-2 shadow-lg">
			<Info class="h-6 w-6 shrink-0 stroke-current" />
			<span>The form below controls the LED via the RESTful service exposed by the ESP device.</span
			>
		</div>
		<div class="flex flex-row flex-wrap justify-between gap-x-2">
			<div class="fieldset w-52">
				<label class="label cursor-pointer">
					<span class="mr-4 text-base">Light State?</span>
					<input type="checkbox" bind:checked={hornoOn} class="checkbox checkbox-primary" />
				</label>
			</div>
			<div class="grow"></div>
			<button class="btn btn-primary inline-flex items-center" onclick={postLightstate}
				><Save class="mr-2 h-5 w-5" /><span>Save</span></button
			>
			<button class="btn btn-primary inline-flex items-center" onclick={getLightstate}
				><Reload class="mr-2 h-5 w-5" /><span>Reload</span></button
			>
		</div>
		<div class="divider"></div>
		<h1 class="text-xl font-semibold">Event Socket Example</h1>
		<div class="alert alert-info my-2 shadow-lg">
			<Info class="h-6 w-6 shrink-0 stroke-current" />
			<span
				>The switch below controls the LED via the event system which uses WebSocket under the hood.
				It will automatically update whenever the LED state changes.</span
			>
		</div>
		<div class="fieldset w-52">
			<label class="label cursor-pointer">
				<span class="text-base">Light State?</span>
				<input
					type="checkbox"
					class="toggle toggle-primary"
					bind:checked={hornoState.led_on}
					onchange={() => {
						socket.sendEvent('led', hornoState);
					}}
				/>
			</label>
		</div>
	</div>
</SettingsCard>
