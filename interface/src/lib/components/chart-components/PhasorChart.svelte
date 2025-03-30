<script lang="ts">
	import { beforeUpdate } from 'svelte';

  export let size = 300;
	export let Iabs = [1, 1, 1, 1];
	export let Iang = [0, 0, 0, 0];
	export let Icolor = ["red", "blue", "green", "transparent"];
  export let strokeWidth = [3, 3, 3, 0];
  export let showPhaseVoltages = true;
  export let showLineVoltages = true;
  export let showValue = '';
  export let showValueColor = 'red';
  export let phaseVoltages = [1, 1, 1];
  export let lineVoltages = [2, 2, 2];
  export let scale = 0;

  $: maxScale = 2;
  $: decimalUnits = 0;

  let arrowI = [1, 1, 1, 0];
  let phaseVoltageLarge = [1, 1, 1];
  let phaseVoltageWeight = ['normal', 'normal', 'normal'];
  let lineVoltageWeight = ['normal', 'normal', 'normal'];

	let rs = [25, 50, 75, 100];
	let angs = [0, 30, 60, 120, 150, 180, 210, 240, 300, 330]
	let angL = [0, -30, -60, -120, -150, 180, 150, 120, 60, 30]
	
  beforeUpdate(() => {
    let order = -5;
    let Imax = Iabs.reduce((p, c) => {return c > p ? c : p})
    while (10 ** order < Imax) order++;
    maxScale = 10 ** order;
    
    let lastAjust = maxScale;
    if (Imax / maxScale < 0.6) lastAjust = 0.6 * maxScale;
    if (Imax / maxScale < 0.4) lastAjust = 0.4 * maxScale;
    if (Imax / maxScale < 0.2) lastAjust = 0.2 * maxScale;
    if (Imax / maxScale < 0.1) lastAjust = 0.1 * maxScale;
    maxScale = lastAjust;

    if (scale != 0) maxScale = scale;

    decimalUnits = 0;
    if (maxScale < 5) decimalUnits = 1;
    if (maxScale <= 1) decimalUnits = 2;
    if (maxScale <= 0.1) decimalUnits = 3;
    if ((maxScale >= 5) && (((maxScale * 10) % 2) != 0)) decimalUnits++;
    if ((maxScale >= 5) && (((maxScale * 1) % 4) != 0)) decimalUnits++;
    if (decimalUnits > 3) decimalUnits = 3;

    arrowI[0] = 6 * Iabs[0] / maxScale;
    if (Math.abs(arrowI[0]) > 1) arrowI[0] = Math.sign(arrowI[0]);
    arrowI[1] = 6 * Iabs[1] / maxScale;
    if (Math.abs(arrowI[1]) > 1) arrowI[1] = Math.sign(arrowI[1]);
    arrowI[2] = 6 * Iabs[2] / maxScale;
    if (Math.abs(arrowI[2]) > 1) arrowI[2] = Math.sign(arrowI[2]);
    arrowI[3] = 6 * Iabs[3] / maxScale;
    if (Math.abs(arrowI[3]) > 1) arrowI[3] = Math.sign(arrowI[3]);

    let largestPhaseVoltage = Math.abs(phaseVoltages[0]);
    if (Math.abs(phaseVoltages[1]) > largestPhaseVoltage) largestPhaseVoltage = Math.abs(phaseVoltages[1]);
    if (Math.abs(phaseVoltages[2]) > largestPhaseVoltage) largestPhaseVoltage = Math.abs(phaseVoltages[2]);

    let largestLineVoltage = Math.abs(lineVoltages[0]);
    if (Math.abs(lineVoltages[1]) > largestLineVoltage) largestLineVoltage = Math.abs(lineVoltages[1]);
    if (Math.abs(lineVoltages[2]) > largestLineVoltage) largestLineVoltage = Math.abs(lineVoltages[2]);

    for(let i = 0; i < 3; i++) {
      phaseVoltageLarge[i] = phaseVoltages[i] / largestPhaseVoltage;
      if (Math.abs(phaseVoltages[i]) === largestPhaseVoltage) phaseVoltageWeight[i] = 'bold'
      else phaseVoltageWeight[i] = 'normal';
      if (Math.abs(lineVoltages[i]) === largestLineVoltage) lineVoltageWeight[i] = 'bold'
      else lineVoltageWeight[i] = 'normal';
    }

  });

</script>




<svg width={size} height={size} viewBox="-130 -130 260 260">
	
	<circle cx="0" cy="0" r="1" fill="gray" />
  
  <circle cx=0 cy=0 r=100 stroke=gray stroke-width=0.5 fill=transparent stroke-dasharray="3,2"/>

	{#each rs as  r, i}
    {#if i < 3}
		  <circle cx=0 cy=0 r={r} stroke=gray stroke-width=0.5 fill=transparent stroke-dasharray="4,6"/>
		{/if}
      <text x=0 y={-r}
			font-size = 10 
			dominant-baseline = "middle"
			text-anchor = "middle"
			fill="gray"
		>{((i+1) * maxScale / 4).toFixed(decimalUnits)}</text>
		<text x=0 y={r}
			font-size = 10 
			dominant-baseline = "middle"
			text-anchor = "middle"
			fill="gray"
		>{((i+1) * maxScale / 4).toFixed(decimalUnits)}</text>
	{/each}

	{#each angs as  a, i}
		<line x1=0 y1=0 x2=100 y2=0 stroke=gray	stroke-width=0.5 stroke-dasharray="4,6" transform="rotate({a})"/>
		<text x={110*Math.cos(a*Math.PI/180)} y={110*Math.sin(a*Math.PI/180)}
			font-size = 10 
			dominant-baseline = "middle"
			text-anchor = "middle"
			fill="gray"
		>{angL[i]}&deg</text>
	{/each}

  {#if showPhaseVoltages}
    <text x={125*Math.cos(90*Math.PI/180)} y={125*Math.sin(90*Math.PI/180)}
      font-size = 10 
      dominant-baseline = "middle"
      text-anchor = "middle"
      fill="red"
      transform="rotate(-90)"
      font-weight={phaseVoltageWeight[0]}
    >Va = {phaseVoltages[0].toFixed(1)} V</text>

    <text x={125*Math.cos(120*Math.PI/180)} y={125*Math.sin(120*Math.PI/180)}
      font-size = 10 
      dominant-baseline = "middle"
      text-anchor = "middle"
      fill="blue"
      font-weight={phaseVoltageWeight[1]}
      transform="rotate(0)"
    >Vb = {phaseVoltages[1].toFixed(1)} V</text>
    <text x={125*Math.cos(-120*Math.PI/180)} y={125*Math.sin(-120*Math.PI/180)}
      font-size = 10 
      dominant-baseline = "middle"
      text-anchor = "middle"
      fill="green"
      font-weight={phaseVoltageWeight[2]}
      transform="rotate(0)"
    >Vb = {phaseVoltages[2].toFixed(1)} V</text>
  {/if}

  {#if showLineVoltages}
    <text x={125*Math.cos(90*Math.PI/180)} y={125*Math.sin(90*Math.PI/180)}
      font-size = 10 
      dominant-baseline = "middle"
      text-anchor = "middle"
      fill="purple"
      font-weight={lineVoltageWeight[0]}
      transform="rotate(-30)"
    >Vab = {lineVoltages[0].toFixed(1)} V</text>
    <text x={125*Math.cos(-90*Math.PI/180)} y={125*Math.sin(-90*Math.PI/180)}
      font-size = 10 
      dominant-baseline = "middle"
      text-anchor = "middle"
      fill="purple"
      font-weight={lineVoltageWeight[1]}
      transform="rotate(-90)"
    >Vbc = {lineVoltages[1].toFixed(1)} V</text>
    <text x={125*Math.cos(-90*Math.PI/180)} y={125*Math.sin(-90*Math.PI/180)}
      font-size = 10 
      dominant-baseline = "middle"
      text-anchor = "middle"
      fill="purple"
      font-weight={lineVoltageWeight[2]}
      transform="rotate(30)"
    >Vca = {lineVoltages[2].toFixed(1)} V</text>
  {/if}
  
  {#each Iabs as  I, i}

    {#if showPhaseVoltages}
		  <line x1=0 y1=0 x2={100*phaseVoltageLarge[i]} y2=0 stroke={Icolor[i]}	stroke-width=2 stroke-dasharray="4,4" transform="rotate({i*120})"/>
		{/if}
    
    <path stroke-width={strokeWidth[i]} stroke={Icolor[i]} fill="transparent" transform="rotate({-Iang[i]})"
			d="M 0 0 L {100 * I / maxScale - 5*arrowI[i]} 0 l {-10*arrowI[i]} {3*arrowI[i]} l {10*arrowI[i]} {-3*arrowI[i]} l {-10*arrowI[i]} {-3*arrowI[i]}"		
			/>

    {#if showLineVoltages}
      <path stroke-dasharray="2,1"
          d="M {125*Math.cos(-80*Math.PI/180)}  {125*Math.sin(-80*Math.PI/180)} 
            Q {127*Math.cos(-90*Math.PI/180)} {127*Math.sin(-90*Math.PI/180)} {125*Math.cos(-100*Math.PI/180)} {125*Math.sin(-100*Math.PI/180)}
            l 4 -4 m -4 4 l 5 3"
          stroke=gray
          stroke-width=0.5
          fill=transparent
          transform="rotate({i * 120})"
        />

      <path stroke-dasharray="2,1"
          d="M {125*Math.cos(-40*Math.PI/180)}  {125*Math.sin(-40*Math.PI/180)} 
            Q {127*Math.cos(-30*Math.PI/180)} {127*Math.sin(-30*Math.PI/180)} {125*Math.cos(-20*Math.PI/180)} {125*Math.sin(-20*Math.PI/180)}
            l 3 -5 m -3 5 l -5 -2"
          stroke=gray
          stroke-width=0.5
          fill=transparent
          transform="rotate({i * 120})"
        />
    {/if}

	{/each}

  {#if showValue}
    <text x=0 y=-118
      font-size = 15 
      dominant-baseline = "middle"
      text-anchor = "middle"
      fill={showValueColor}
      font-weight="bold"
    >{showValue} = {Iabs[0].toFixed(3)}∠{Iang[0].toFixed(1)}&deg</text>
  {/if}
	
</svg>
