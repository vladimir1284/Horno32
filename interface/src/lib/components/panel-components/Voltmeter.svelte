<script lang="ts">
	import { onMount } from 'svelte';

	export let svgWidth = 100;
	export let svgHeight = 100;
	export let magnitude = 0;
	export let minScale = 0;
	export let maxScale = 300;
	export let scaleSmallDivisions = 60;
	export let scaleLargeDivisions = 6;
	export let units = 'V';
	export let decimalPlaces = 1;
	export let needleColor = 'blue';
	export let varName = 'Va';

	let displayValue = '';
	let x1 = 0,
		y1 = 0,
		x2 = 0,
		y2 = 0,
		x3 = 0,
		y3 = 0;
	let smallScaleAngles = [],
		largeScaleAngles = [];

	function computeScaleDivisions() {
		smallScaleAngles = [];
		largeScaleAngles = [];
		for (let i = 0; i <= scaleSmallDivisions; i++) {
			smallScaleAngles.push((i * Math.PI) / 2 / scaleSmallDivisions);
		}
		for (let i = 0; i <= scaleLargeDivisions; i++) {
			largeScaleAngles.push((i * Math.PI) / 2 / scaleLargeDivisions);
		}
	}

	function updateDisplay(value: number) {
		// Clamp value
		let clamped = value;
		const margin = 0.05 * (maxScale - minScale);
		if (clamped < minScale - margin) clamped = minScale - margin;
		if (clamped > maxScale + margin) clamped = maxScale + margin;

		const angle = (Math.PI / 2) * ((clamped - minScale) / (maxScale - minScale));
		const radius = 74;
		const center = 87;

		x1 = center - radius * 0.35 * Math.cos(angle);
		y1 = center - radius * 0.35 * Math.sin(angle);
		x2 = center - radius * 0.7 * Math.cos(angle);
		y2 = center - radius * 0.7 * Math.sin(angle);
		x3 = center - radius * 1.0 * Math.cos(angle);
		y3 = center - radius * 1.0 * Math.sin(angle);

		displayValue = clamped.toFixed(decimalPlaces);
	}

	onMount(() => {
		computeScaleDivisions();
		updateDisplay(magnitude);
	});

	// Rerun display logic when any reactive inputs change
	$: updateDisplay(magnitude);
</script>

<svg width={svgWidth} height={svgHeight} viewBox="0 0 100 100">
	<defs>
		<filter id="drop-shadow" x="-50%" y="-50%" width="200%" height="200%">
			<feGaussianBlur in="SourceAlpha" stdDeviation="4" />
			<feOffset dx="4" dy="4" result="offsetblur" />
			<feFlood flood-color="rgba(0,0,0,0.5)" />
			<feComposite in2="offsetblur" operator="in" />
			<feMerge>
				<feMergeNode />
				<feMergeNode in="SourceGraphic" />
			</feMerge>
		</filter>

		<radialGradient id="gradiente" cx="50%" cy="50%" fx="50%" fy="50%">
			<stop offset="0%" style="stop-color: #ffffff; stop-opacity: 0" />
			<stop offset="300%" style="stop-color: #0b5467; stop-opacity: 0.2" />
		</radialGradient>

		<clipPath id="cut-needle-volt">
			<rect x="8" y="8" width="84" height="84" rx="8" />
		</clipPath>
	</defs>

	<rect x="0" y="0" width="100" height="100" rx="2" fill="url(#gradiente)" />
	<rect x="1" y="1" width="98" height="98" rx="2" fill="white" filter="url(#drop-shadow)" />

	<text
		x="23"
		y="23"
		font-size="20"
		dominant-baseline="middle"
		text-anchor="middle"
		font-weight="bold"
		fill="rgb(44, 44, 44)"
	>
		{units}
	</text>

	{#each smallScaleAngles as angle}
		<line
			x1={87 - 74 * 0.95 * Math.cos(angle)}
			y1={87 - 74 * 0.95 * Math.sin(angle)}
			x2={87 - 74 * Math.cos(angle)}
			y2={87 - 74 * Math.sin(angle)}
			stroke="gray"
			stroke-width="0.5"
		/>
	{/each}

	{#each largeScaleAngles as angle, i}
		<line
			x1={87 - 74 * 0.9 * Math.cos(angle)}
			y1={87 - 74 * 0.9 * Math.sin(angle)}
			x2={87 - 74 * Math.cos(angle)}
			y2={87 - 74 * Math.sin(angle)}
			stroke="gray"
			stroke-width="1"
		/>
		<text
			x={87 - 74 * 0.8 * Math.cos(angle)}
			y={87 - 74 * 0.8 * Math.sin(angle)}
			font-size="5"
			dominant-baseline="middle"
			text-anchor="middle"
		>
			{Math.round(minScale + (i * (maxScale - minScale)) / scaleLargeDivisions)}
		</text>
	{/each}

	<text
		x="87"
		y="87"
		font-size="8.5"
		dominant-baseline="auto"
		text-anchor="end"
		font-weight="bold"
		fill={needleColor}
	>
		{displayValue}
	</text>

	<text
		x="87"
		y="76.5"
		font-size="10"
		dominant-baseline="auto"
		text-anchor="end"
		font-weight="bold"
		fill={needleColor}
	>
		{varName}
	</text>

	<!-- Main needle -->
	<line
		{x1}
		{y1}
		{x2}
		{y2}
		stroke={needleColor}
		stroke-width="3"
		stroke-linecap="round"
		clip-path="url(#cut-needle-volt)"
	/>
	<!-- Secondary line (shadow) -->
	<line
		{x1}
		{y1}
		x2={x3}
		y2={y3}
		stroke="rgb(44, 44, 44)"
		stroke-width="1"
		stroke-linecap="round"
		clip-path="url(#cut-needle-volt)"
	/>
</svg>

<style>
	svg {
		display: block;
	}
</style>
