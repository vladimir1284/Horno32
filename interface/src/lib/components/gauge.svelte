<script lang="ts">
	export let size = 280;
	export let value = 0;
</script>

<canvas
	data-type="radial-gauge"
	id="thermometer"
	data-units="°C"
	data-title="Temperatura"
	data-width={size}
	data-height={size}
	data-value={value}
	data-min-value="0"
	data-max-value="390"
	data-major-ticks="0,30,60,90,120,150,180,210,240,270,300,330,360,390"
	data-minor-ticks="3"
	data-stroke-ticks="false"
	data-highlights=""
	data-color-plate="#222"
	data-color-major-ticks="#f5f5f5"
	data-color-minor-ticks="#ddd"
	data-color-title="#fff"
	data-color-units="#ccc"
	data-color-numbers="#eee"
	data-color-needle-start="rgba(240, 128, 128, 1)"
	data-color-needle-end="rgba(255, 160, 122, .9)"
	data-value-box="true"
	data-animation-rule="bounce"
	data-animation-duration="500"
	data-font-value="Led"
	data-animated-value="true"
></canvas>
