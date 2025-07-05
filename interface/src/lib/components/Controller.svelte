<script>
	let modo = 'automatico';

	let temperatura = 180;
	let potenciaSuperior = 50;
	let potenciaInferior = 50;

	const confirmarAutomatico = () => {
		console.log(`Temperatura automática establecida en ${temperatura}°C`);
	};

	const aplicarManual = () => {
		console.log(`Manual - Superior: ${potenciaSuperior}%, Inferior: ${potenciaInferior}%`);
	};
</script>

<div class="contenedor">
	<h2>Modo de Control del Horno</h2>

	<div class="modo">
		<label>
			<input type="radio" bind:group={modo} value="automatico" />
			<span>Automático</span>
		</label>
		<label>
			<input type="radio" bind:group={modo} value="manual" />
			<span>Manual</span>
		</label>
	</div>

	{#if modo === 'automatico'}
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
</style>
