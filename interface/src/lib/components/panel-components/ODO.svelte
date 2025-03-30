<script>
// @ts-nocheck


	import OdoDrum from "$lib/components/panel-components/OdoDrum.svelte";
	export let value;
	export let nrDigits = 1;
	export let dure = 200;
	export let color = "red";
	export let background = "black";

    
    function split(number) {
      return Array.from(number.toString().padStart(nrDigits, '0'));
	}
</script>

<div class="total" >
	{#each split(value) as newNumber}
		<OdoDrum {newNumber} {dure} color={color} background={background}/>
	{/each}
</div>

<style>
  .total {
		display: block;
		margin: 5px;
  }
</style>
