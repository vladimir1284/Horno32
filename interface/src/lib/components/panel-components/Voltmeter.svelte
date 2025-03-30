<script lang="ts">

  import { beforeUpdate, onMount } from 'svelte';
  import { spring } from 'svelte/motion';

  export let svgWidth = 100 ;
  export let svgHeight = 100;
  export let magnitude = 0;
  export let minScale = 0;
  export let maxScale = 300;
  export let scaleSmallDivisions = 60;
  export let scaleLargeDivisions = 6;
  export let units = "V";
  export let decimalPlaces = 1;
  export let needleColor = "blue";
  export let varName = "Va";
  export let needleStiffness = 0.08;
  export let needleDamping = 0.28;

  let magnitudeNeedle = 0;

  $: {
      magnitudeNeedle = magnitude;
        if (magnitudeNeedle < (minScale - 5 * (maxScale - minScale) / 100.0))
      magnitudeNeedle = minScale - 5 * (maxScale - minScale) / 100.0;
      if (magnitudeNeedle > (maxScale + 5 * (maxScale - minScale) / 100.0))
        magnitudeNeedle = maxScale + 5 * (maxScale - minScale) / 100.0;
  }

  $: angleRadians = (Math.PI / 2) * ((magnitudeNeedle - minScale) / (maxScale - minScale));

  $: x1 = 87 - 74 * 0.35 * Math.cos(angleRadians);
  $: y1 = 87 - 74 * 0.35 * Math.sin(angleRadians);
  $: x2 = 87 - 74 * 0.7  * Math.cos(angleRadians);
  $: y2 = 87 - 74 * 0.7  * Math.sin(angleRadians);
  $: x3 = 87 - 74 * 1.0  * Math.cos(angleRadians);
  $: y3 = 87 - 74 * 1.0  * Math.sin(angleRadians);
  
  let angle1 = (Math.PI / 2) * ((magnitudeNeedle - minScale) / (maxScale - minScale));

  $: coords1 = spring({ x: 87 - 74 * 0.35 * Math.cos(angle1), y: 87 - 74 * 0.35 * Math.sin(angle1) }, {
		stiffness: needleStiffness,
		damping: needleDamping
	});

  $: coords2 = spring({ x: 87 - 74 * 0.7 * Math.cos(angle1), y: 87 - 74 * 0.7 * Math.sin(angle1) }, {
		stiffness: needleStiffness,
		damping: needleDamping
	});

  $: coords3 = spring({ x: 87 - 74 * 1 * Math.cos(angle1), y: 87 - 74 * 1 * Math.sin(angle1) }, {
		stiffness: needleStiffness,
		damping: needleDamping
	});
  
  let smallScaleAngles = [];
  for (let i = 0; i < scaleSmallDivisions + 1; i++) {
    smallScaleAngles[i] = i * (Math.PI / 2) / (scaleSmallDivisions);
  }
  let largeScaleAngles = [];
  for (let i = 0; i < scaleLargeDivisions + 1; i++) {
    largeScaleAngles[i] = i * (Math.PI / 2) / (scaleLargeDivisions);
  }



  onMount(() => {
  });

  beforeUpdate(() => {
    coords1.set({x:x1, y:y1});
    coords2.set({x:x2, y:y2});
    coords3.set({x:x3, y:y3});
  })

</script>


<svg id = "gauge" width = {svgWidth} height = {svgHeight} viewBox="0 0 100 100">

  <defs>
    <filter id="drop-shadow" x="-50%" y="-50%" width="200%" height="200%">
      <feGaussianBlur in="SourceAlpha" stdDeviation="4"/>
      <feOffset dx="4" dy="4" result="offsetblur"/>
      <feFlood flood-color="rgba(0,0,0,0.5)"/>
      <feComposite in2="offsetblur" operator="in"/>
      <feMerge>
        <feMergeNode/>
        <feMergeNode in="SourceGraphic"/>
      </feMerge>
    </filter>
  
    <radialGradient id="gradiente" cx="50%" cy="50%" fx="50%" fy="50%">
        <stop offset="0%" style="stop-color: #ffffff; stop-opacity: 0" />
        <stop offset="300%" style="stop-color: #0b5467; stop-opacity: 0.2" />
    </radialGradient>

    <clipPath id="cut-needle-volt">
      <rect x=8 y=8 width=84 height=84 rx=8/>  
    </clipPath>
</defs>

  <rect x=0 y=0 width=100 height=100 rx=8  fill="url(#gradiente)"/>
  <rect x=8 y=8 width=84 height=84 rx=5 fill="white" filter="url(#drop-shadow)" />

  <text
      x=23
      y=23
      font-size=20
      dominant-baseline = "middle"
      text-anchor = "middle"
      font-weight = "bold"
      fill = "rgb(44, 44, 44)"
      >
      {units}
    </text>

  {#each smallScaleAngles as angle}
    <line 
      x1 = {87 - 74 * 0.95 * Math.cos(angle)} 
      y1 = {87 - 74 * 0.95 * Math.sin(angle)}
      x2 = {87 - 74 * Math.cos(angle)}
      y2 = {87 - 74 * Math.sin(angle)}
      stroke = "gray"
      stroke-width = 0.5
    />
  {/each} 

  {#each largeScaleAngles as angle, i}
    <line 
      x1 = {87 - 74 * 0.9 * Math.cos(angle)} 
      y1 = {87 - 74 * 0.9 * Math.sin(angle)}
      x2 = {87 - 74 * Math.cos(angle)}
      y2 = {87 - 74 * Math.sin(angle)}
      stroke = "gray"
      stroke-width = 1
    />
    <text
      x = {87 - 74 * 0.8 * Math.cos(angle)}
      y = {87 - 74 * 0.8 * Math.sin(angle)}
      font-size = 5
      dominant-baseline = "middle"
      text-anchor = "middle"
      >
      {Math.round(minScale + i * (maxScale - minScale) / (scaleLargeDivisions))}
    </text>
  {/each} 

  <text
    x = 87
    y = 87
    font-size = 8.5
    dominant-baseline = "auto"
    text-anchor = "end"
    font-weight = "bold"
    fill = {needleColor}
    >
    {magnitude.toFixed(decimalPlaces)}
  </text>     

  <text
    x = 87
    y = 76.5
    font-size = 10
    dominant-baseline = "auto"
    text-anchor = "end"
    font-weight = "bold"
    fill = {needleColor}
    >
    {varName}
  </text>          

  // Draw the needle
  <line 
    x1 = {$coords1.x}
    y1 = {$coords1.y}
    x2 = {$coords2.x}
    y2 = {$coords2.y}
    stroke = {needleColor}
    stroke-width = 3
    stroke-linecap = "round"
    clip-path="url(#cut-needle-volt)"
  />
  <line 
    x1 = {$coords1.x}
    y1 = {$coords1.y}
    x2 = {$coords3.x}
    y2 = {$coords3.y}
    stroke = "rgb(44, 44, 44)"
    stroke-width = 1
    stroke-linecap = "round"
    clip-path="url(#cut-needle-volt)"
  />
    
</svg>

<style>
  svg {
        position: relative;
  }
</style>
