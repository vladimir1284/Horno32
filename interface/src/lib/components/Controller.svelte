<script lang="ts">
	import { page } from '$app/state';
	import { user } from '$lib/stores/user';
	import { notifications } from '$lib/components/toasts/notifications';
	import type { HornoState } from '$lib/types/models';

	interface Props {
		hornoState: HornoState;
		onUpdated: (state: HornoState) => void;
	}

	let { hornoState, onUpdated }: Props = $props();

	let modo = $state(hornoState.mode);
	let temperatura = $state(hornoState.setpoint);
	let potenciaSuperior = $state(hornoState.manual_power_top);
	let potenciaInferior = $state(hornoState.manual_power_bottom);

	async function postHornoState(partial: Partial<HornoState>) {
		try {
			const response = await fetch('/rest/hornoState', {
				method: 'POST',
				headers: {
					Authorization: page.data.features.security ? 'Bearer ' + $user.bearer_token : 'Basic',
					'Content-Type': 'application/json'
				},
				body: JSON.stringify(partial)
			});
			if (response.status == 200) {
				const updated: HornoState = await response.json();
				onUpdated(updated);
			} else {
				notifications.error('User not authorized.', 3000);
			}
		} catch (error) {
			console.error('Error:', error);
		}
	}

	const toggleOn = () => {
		postHornoState({ on: !hornoState.on });
	};

	const confirmarAutomatico = () => {
		postHornoState({ mode: 'auto', setpoint: temperatura });
	};

	const aplicarManual = () => {
		postHornoState({
			mode: 'manual',
			manual_power_top: potenciaSuperior,
			manual_power_bottom: potenciaInferior
		});
	};
</script>

<div class="contenedor">
	<h2>Control del Horno</h2>

	<button class="power-toggle" class:on={hornoState.on} on:click={toggleOn}>
		{hornoState.on ? 'Apagar' : 'Encender'}
	</button>

	<div class="modo">
		<label>
			<input type="radio" bind:group={modo} value="auto" />
			<span>Automático</span>
		</label>
		<label>
			<input type="radio" bind:group={modo} value="manual" />
			<span>Manual</span>
		</label>
	</div>

	{#if modo === 'auto'}
		<div>
			<div class="campo">
				<label for="temperatura">Temperatura (°C):</label>
				<div class="fila">
					<input id="temperatura" type="range" min="30" max="400" bind:value={temperatura} />
					<input class="num-input" type="number" min="30" max="400" bind:value={temperatura} />
				</div>
				<div class="valor">{temperatura} °C</div>
			</div>

			<button on:click={confirmarAutomatico}> Confirmar Temperatura </button>
		</div>
	{:else}
		<div>
			<div class="campo">
				<label for="superior">Potencia Superior (%):</label>
				<input id="superior" type="range" min="0" max="100" bind:value={potenciaSuperior} />
				<div class="valor">{potenciaSuperior} %</div>
			</div>

			<div class="campo">
				<label for="inferior">Potencia Inferior (%):</label>
				<input id="inferior" type="range" min="0" max="100" bind:value={potenciaInferior} />
				<div class="valor">{potenciaInferior} %</div>
			</div>

			<button on:click={aplicarManual}> Aplicar Configuración Manual </button>
		</div>
	{/if}

	<div class="potencia-actual">
		<span>Potencia real - Superior: {hornoState.actual_power_top.toFixed(0)}%</span>
		<span>Potencia real - Inferior: {hornoState.actual_power_bottom.toFixed(0)}%</span>
		<span class="tope">
			Tope de seguridad (fijo): Superior {hornoState.hard_max_duty_top.toFixed(0)}% / Inferior {hornoState.hard_max_duty_bottom.toFixed(
				0
			)}%
		</span>
	</div>
</div>

<style>
	.contenedor {
		max-width: 400px;
		margin: 0;
		padding: 20px;
		background: #ffffff1e;
		border-radius: 12px;
		box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
		font-family: sans-serif;
		text-align: center;
	}

	h2 {
		margin-bottom: 20px;
	}

	.modo {
		display: flex;
		justify-content: center;
		gap: 20px;
		margin-bottom: 20px;
	}

	.modo label {
		padding: 6px 12px;
		border-radius: 6px;
		cursor: pointer;
		background-color: #ccc;
		color: white;
	}

	.modo input[type='radio'] {
		display: none;
	}

	.modo input[type='radio']:checked + span {
		background-color: #4caf50;
	}

	.modo span {
		display: inline-block;
		padding: 6px 12px;
		border-radius: 6px;
		background-color: #888;
		transition: background-color 0.3s;
	}

	.campo {
		margin-bottom: 15px;
	}

	label {
		display: block;
		margin-bottom: 4px;
	}

	.fila {
		display: flex;
		justify-content: center;
		align-items: center;
		gap: 0;
	}

	input[type='range'] {
		width: 50%;
	}

	input[type='number'] {
		width: 80px;
		padding: 6px;
		border: 1px solid #ccc;
		border-radius: 6px;
		text-align: center;
	}

	.valor {
		font-weight: bold;
		margin-top: 4px;
	}

	button {
		background-color: #007bff;
		color: white;
		border: none;
		padding: 10px 16px;
		border-radius: 6px;
		cursor: pointer;
		margin-top: 10px;
	}

	button:hover {
		background-color: #0056b3;
	}

	.num-input {
		background-color: white;
	}

	.power-toggle {
		background-color: #888;
		margin-top: 0;
		margin-bottom: 16px;
	}

	.power-toggle.on {
		background-color: #4caf50;
	}

	.potencia-actual {
		display: flex;
		flex-direction: column;
		gap: 4px;
		margin-top: 16px;
		font-size: 0.9em;
		opacity: 0.85;
	}

	.tope {
		font-size: 0.8em;
		opacity: 0.6;
	}
</style>
