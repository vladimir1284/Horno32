<script lang="ts">

  import { afterUpdate, beforeUpdate, onMount } from 'svelte';
  import { spring } from 'svelte/motion';

  // import * as d3 from 'd3';
  import {fly} from 'svelte/transition';

  export let svgWidth = 400 ;
  export let svgHeight = 400;
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

  const borderPercent = 8;
  const scaleSeparationPercent = 5;
  const scaleSmallDeepPercent = 5;
  const scaleLargeDeepPercent = 10;
  const scaleTextSizePercent = 5
  const scaleSmallDeep = ((100 - scaleSmallDeepPercent) / 100);
  const scaleLargeDeep = ((100 - scaleLargeDeepPercent) / 100);
  const scaleTextDeep = ((100 - scaleLargeDeepPercent * 2) / 100);

  let magnitudeNeedle = 0;

  $: {
      magnitudeNeedle = magnitude;
        if (magnitudeNeedle < (minScale - scaleSeparationPercent * (maxScale - minScale) / 100.0))
      magnitudeNeedle = minScale - scaleSeparationPercent * (maxScale - minScale) / 100.0;
      if (magnitudeNeedle > (maxScale + scaleSeparationPercent * (maxScale - minScale) / 100.0))
        magnitudeNeedle = maxScale + scaleSeparationPercent * (maxScale - minScale) / 100.0;
  }

  
  $: border = svgWidth * borderPercent / 100;
 
  $: scaleInitX = svgWidth * (borderPercent + scaleSeparationPercent) / 100;
  $: scaleInitY = svgHeight - svgHeight * (borderPercent + scaleSeparationPercent) / 100;
  $: scalesvgWidth = svgWidth - 2 * svgWidth * (borderPercent + scaleSeparationPercent) / 100;
  $: scalesvgHeight = svgHeight - 2 * svgHeight * (borderPercent + scaleSeparationPercent) / 100;
  
  $: angleRadians = (Math.PI / 2) * ((magnitudeNeedle - minScale) / (maxScale - minScale));

  $: x1 = scaleInitX + scalesvgWidth - scalesvgWidth * 0.35 * Math.cos(angleRadians);
  $: y1 = scaleInitY  - scalesvgHeight * 0.35 * Math.sin(angleRadians);
  $: x2 = scaleInitX + scalesvgWidth - scalesvgWidth * 0.7 * Math.cos(angleRadians);
  $: y2 = scaleInitY  - scalesvgHeight * 0.7 * Math.sin(angleRadians);
  $: x3 = scaleInitX + scalesvgWidth - scalesvgWidth * 1 * Math.cos(angleRadians);
  $: y3 = scaleInitY  - scalesvgHeight * 1 * Math.sin(angleRadians);
  
  let angle1 = (Math.PI / 2) * ((magnitudeNeedle - minScale) / (maxScale - minScale));

  $: coords1 = spring({ x: scaleInitX + scalesvgWidth - scalesvgWidth * 0.35 * Math.cos(angle1), y: scaleInitY  - scalesvgHeight * 0.35 * Math.sin(angle1) }, {
		stiffness: needleStiffness,
		damping: needleDamping
	});

  $: coords2 = spring({ x: scaleInitX + scalesvgWidth - scalesvgWidth * 0.7 * Math.cos(angle1), y: scaleInitY  - scalesvgHeight * 0.7 * Math.sin(angle1) }, {
		stiffness: needleStiffness,
		damping: needleDamping
	});

  $: coords3 = spring({ x: scaleInitX + scalesvgWidth - scalesvgWidth * 1 * Math.cos(angle1), y: scaleInitY  - scalesvgHeight * 1 * Math.sin(angle1) }, {
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

<svg id = "gauge" width = {svgWidth} height = {svgHeight}
on:mousemove={(e) => {

}}
on:mousedown={() => null}
on:mouseup={() => null}
role="presentation"
  
  >

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
      <rect x = {border} y = {border} width = {svgWidth - 2 * border} height = {svgHeight - 2 * border} rx =  {borderPercent} />  
    </clipPath>
</defs>

  <rect x = {0*svgWidth} y = {0*svgHeight} width = {svgWidth} height = {svgHeight} rx =  {borderPercent}  fill="url(#gradiente)"/>
  <rect x = {border} y = {border} width = {svgWidth - 2 * border} height = {svgHeight - 2 * border} rx =  {borderPercent} fill="white" filter="url(#drop-shadow)" />
  <text
      x = {0.23 * svgWidth}
      y = {0.23 * svgHeight}
      font-size = {svgWidth * scaleTextSizePercent * 4 / 100}
      dominant-baseline = "middle"
      text-anchor = "middle"
      font-weight = "bold"
      fill = "rgb(44, 44, 44)"
      >
      {units}
    </text>

  {#each smallScaleAngles as angle}
    <line 
      x1 = {scaleInitX + scalesvgWidth - scalesvgWidth * scaleSmallDeep * Math.cos(angle)} 
      y1 = {scaleInitY  - scalesvgHeight * scaleSmallDeep * Math.sin(angle)}
      x2 = {scaleInitX + scalesvgWidth - scalesvgWidth * Math.cos(angle)}
      y2 = {scaleInitY  - scalesvgHeight * Math.sin(angle)}
      stroke = "gray"
      stroke-width = 0.5
    />
  {/each} 

  {#each largeScaleAngles as angle, i}
    <line 
      x1 = {scaleInitX + scalesvgWidth - scalesvgWidth * scaleLargeDeep * Math.cos(angle)} 
      y1 = {scaleInitY  - scalesvgHeight * scaleLargeDeep * Math.sin(angle)}
      x2 = {scaleInitX + scalesvgWidth - scalesvgWidth * Math.cos(angle)}
      y2 = {scaleInitY  - scalesvgHeight * Math.sin(angle)}
      stroke = "gray"
      stroke-width = 1
    />
    <text
      x = {scaleInitX + scalesvgWidth - scalesvgWidth * scaleTextDeep * Math.cos(angle)}
      y = {scaleInitY  - scalesvgHeight * scaleTextDeep * Math.sin(angle)}
      font-size = {svgWidth * scaleTextSizePercent / 100}
      dominant-baseline = "middle"
      text-anchor = "middle"
      >
      {Math.round(minScale + i * (maxScale - minScale) / (scaleLargeDivisions))}
    </text>
  {/each} 

  <text
    x = {scaleInitX + scalesvgWidth}
    y = {scaleInitY}
    font-size = {svgWidth * scaleTextSizePercent * 1.7 / 100}
    dominant-baseline = "auto"
    text-anchor = "end"
    font-weight = "bold"
    fill = {needleColor}
    >
    {magnitude.toFixed(decimalPlaces)}
  </text>     

  <text
    x = {scaleInitX + scalesvgWidth}
    y = {scaleInitY * 0.88}
    font-size = {svgWidth * scaleTextSizePercent * 2 / 100}
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
    stroke-width = {svgWidth * 0.03}
    stroke-linecap = "round"
    clip-path="url(#cut-needle-volt)"
  />
  <line 
    x1 = {$coords1.x}
    y1 = {$coords1.y}
    x2 = {$coords3.x}
    y2 = {$coords3.y}
    stroke = "rgb(44, 44, 44)"
    stroke-width = {svgWidth * 0.01}
    stroke-linecap = "round"
    clip-path="url(#cut-needle-volt)"
  />
    
</svg>

<style>
  svg {
        position: relative;
  }
</style>
