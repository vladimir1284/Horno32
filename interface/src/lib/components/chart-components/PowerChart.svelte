<script lang="ts">
	import { beforeUpdate } from 'svelte';

	export let P = 1;
  export let Q = 0;
  export let S = 1; //Math.sqrt(P ** 2 + Q ** 2);
  export let fp = 1; //Math.cos(Math.atan(Q/P));
  export let charWidth = 150;
  export let chartHeight = 240;
  export let scale = 0;

	let Pr = P;
  let Qr = Q;
  let Sr = S;
  let maxScale = 2;
  let maxScaleA = 2;
  let gridMarks : number[] = [-8, -6, -4, -2, 0, 2, 4, 6, 8];
  let decimalUnits = 0;
  let arrowP = 1;
  let arrowQ = 1;
  let arrowS = 1;
  let unitsPrefix = '';
  let magDecimal = 0;
  let yLedgend = -60;
  let angRotateS = 0;

  function updateScales() {
    // S = Math.sqrt(P ** 2 + Q ** 2);
    // fp = Math.cos(Math.atan(Q/P));
    
    let maxPower = Math.abs(P);
    if (maxPower < Math.abs(Q)) maxPower = Math.abs(Q);
    
    if (scale != 0) maxPower = Math.abs(scale);

    Pr = P;
    Qr = Q;
    Sr = S;
    unitsPrefix = '';
    magDecimal = 0;
    
    let order = -5; 
    while (10 ** order < maxPower) order++;
    maxScaleA = 10 ** order;
    maxScale = maxScaleA;
    if (maxScaleA > 1000000) {
      Pr = P / 1000000;
      Qr = Q / 1000000;
      Sr = S / 1000000;
      maxScale = maxScaleA / 1000000;
      maxPower = maxPower / 1000000;
      unitsPrefix = 'M';
      magDecimal = 2;
    } else if (maxScaleA > 10000) {
      Pr = P / 1000;
      Qr = Q / 1000;
      Sr = S / 1000;
      maxScale = maxScaleA / 1000;
      maxPower = maxPower / 1000;
      unitsPrefix = 'k';
      magDecimal = 2;
    }
    
    if (scale < 0) {
      maxScale = Math.abs(scale);
      if (unitsPrefix === 'k') maxScale = maxScale / 1000;
      if (unitsPrefix === 'M') maxScale = maxScale / 1000000;
    } else if (scale === 0) {
      let lastAjust = maxScale;
      if (maxPower / maxScale < 0.9) lastAjust = 0.9 * maxScale;
			if (maxPower / maxScale < 0.72) lastAjust = 0.72 * maxScale;
			if (maxPower / maxScale < 0.60) lastAjust = 0.6 * maxScale;
			if (maxPower / maxScale < 0.54) lastAjust = 0.54 * maxScale;
			if (maxPower / maxScale < 0.39) lastAjust = 0.39 * maxScale;
			if (maxPower / maxScale < 0.3) lastAjust = 0.3 * maxScale;
			if (maxPower / maxScale < 0.24) lastAjust = 0.24 * maxScale;
			if (maxPower / maxScale < 0.18) lastAjust = 0.18 * maxScale;
			if (maxPower / maxScale < 0.12) lastAjust = 0.12 * maxScale;
			
      maxScale = lastAjust;
    } else {
      maxScale = scale;
      if (unitsPrefix === 'k') maxScale = maxScale / 1000;
      if (unitsPrefix === 'M') maxScale = maxScale / 1000000;
    }

    gridMarks = [-8, -6, -4, -2, 0, 2, 4, 6, 8];
    for (let i = -4; i < 5; i++) {
      gridMarks[i+4] = 100 * i/4;
    }
    
    decimalUnits = 0;
    if (maxScale <= 10) decimalUnits = 1;
    if (maxScale < 1) decimalUnits = 2;
    if (maxScale > 1 && (maxScale % 4) != 0) decimalUnits++;
    // if (maxScale > 1 && (maxScale % 2) != 0) decimalUnits++;

    if (Qr / maxScale > 0.1) {
        yLedgend = 60;
    } 
    if (Qr / maxScale < -0.1) {
        yLedgend = -60;
    } 
    
    arrowP = 7 * Pr / maxScale;
    if (Math.abs(arrowP) > 0.7) arrowP = Math.sign(arrowP) * 0.7;
    arrowQ = 7 * Qr / maxScale;
    if (Math.abs(arrowQ) > 0.7) arrowQ = Math.sign(arrowQ) * 0.7;
    arrowS = 7 * Sr / maxScale;
    if (Math.abs(arrowS) > 0.7) arrowS = Math.sign(arrowS) * 0.7;

    if (Pr != 0) angRotateS = -Math.atan(Qr/Pr) * 180 / Math.PI;
    else angRotateS = - Math.sign(Qr) * 90;
    
    // console.log(`${Qr.toFixed(2)} ${(arrowQ).toFixed(2)} ${maxPower} ${maxScale}`);

  }
  
  beforeUpdate(() => {
    updateScales(); 
  });

  updateScales(); 

</script>

<svg width={charWidth} height={chartHeight} viewBox="-25 -120 150 240">
	  
	<!-- <rect x=-25 y=-120 width=150 height=240 stroke=gray fill=white/> -->
	
  <path class="axes"
			d="M 0 110 L 0 -116 l -3 10 m 3 -10 l 3 10
         M -2 0 L 120 0 l -10 -3 m 10 3 l -10 3"		
			/>

      {#each gridMarks as  r, i}
        <line class="grid" x1=1 y1={r} x2= 100 y2={r} stroke-dasharray="4,6"></line>
        {#if i > 4}
        <line class="grid" x1={r} y1=-100 x2={r} y2=100 stroke-dasharray="4,6"></line>
          <text class="grid-label" x={r} y={9}
            font-size = 7.5 
            dominant-baseline = "end"
            text-anchor = "middle"
            >{((i-4)*maxScale/4).toFixed(decimalUnits)}</text>
        {/if}
        <text class="grid-label" x={-2} y={r}
          font-size = 7.5 
          dominant-baseline = "middle"
          text-anchor = "end"
          >{-((i-4)*maxScale/4).toFixed(decimalUnits)}</text>
      {/each}
  
    <text class="grid-label" x=112 y={-7}
			font-size = 9 
			dominant-baseline = "middle"
			text-anchor = "middle"
		>{unitsPrefix}W</text>
    <text class="grid-label" x=5 y={-110}
			font-size = 9 
			dominant-baseline = "middle"
			text-anchor = "start"
		>{unitsPrefix}VAr</text>
    
    
    <path class="Q" stroke-width = {Math.abs(2*arrowQ/0.7)}
			d="M 0 0 L {100 * Qr / maxScale - 4*arrowQ} 0 l {-10*arrowQ} {3*arrowQ} l {10*arrowQ} {-3*arrowQ} l {-10*arrowQ} {-3*arrowQ}"
      transform="rotate(-90) translate(0,{100 * Pr / maxScale})"
			/>

    <path class="S" stroke-width = {Math.abs(2*arrowS/0.7)}
			d="M 0 0 L {100 * Sr / maxScale - 4*arrowS} 0 l {-10*arrowS} {3*arrowS} l {10*arrowS} {-3*arrowS} l {-10*arrowS} {-3*arrowS}"
      transform="rotate({angRotateS})"
			/>

    <path class="P" stroke-width = {Math.abs(2*arrowP/0.7)}
			d="M 0 0 L {100 * Pr / maxScale - 4*arrowP} 0 l {-10*arrowP} {3*arrowP} l {10*arrowP} {-3*arrowP} l {-10*arrowP} {-3*arrowP}"		
			/>

    <text class="P-label" x=7 y={yLedgend - 25}
      font-size = 13 
      dominant-baseline = "middle"
      text-anchor = "start"
      stroke-width= 1
    >P:</text>
    <text class="P-label" x=72 y={yLedgend - 25}
			font-size = 13 
			dominant-baseline = "middle"
			text-anchor = "end"
      stroke-width= 1
		>{Pr.toFixed(magDecimal)}</text>
    <text class="P-label" x=77 y={yLedgend - 25}
			font-size = 13 
			dominant-baseline = "middle"
			text-anchor = "start"
      stroke-width= 1
		>{unitsPrefix}W</text>
    
    <text class="Q-label" x=7 y={yLedgend}
      font-size = 13 
      dominant-baseline = "middle"
      text-anchor = "start"
      stroke-width= 1
    >Q:</text>
    <text class="Q-label" x=72 y={yLedgend}
			font-size = 13 
      dominant-baseline = "middle"
			text-anchor = "end"
      stroke-width= 1
		>{Qr.toFixed(magDecimal)}</text> 
    <text class="Q-label" x=77 y={yLedgend}
      font-size = 13 
      dominant-baseline = "middle"
      text-anchor = "start"
      stroke-width= 1
    >{unitsPrefix}VAr</text>

    <text class="S-label" x=7 y={yLedgend + 25}
      font-size = 13 
      dominant-baseline = "middle"
      text-anchor = "start"
      stroke-width= 1
    >S:</text>
    <text class="S-label" x=72 y={yLedgend + 25}
			font-size = 13 
			dominant-baseline = "middle"
			text-anchor = "end"
      stroke-width= 1
		>{Sr.toFixed(magDecimal)}</text>
    <text class="S-label" x=77 y={yLedgend + 25}
			font-size = 13 
			dominant-baseline = "middle"
			text-anchor = "start"
      stroke-width= 1
		>{unitsPrefix}VA</text>
    
    <text class="fp-label" x=6 y=110
      font-size = 13 
      dominant-baseline = "middle"
      text-anchor = "start"
      stroke-width = 1
      font-weight = "normal"
    >fp: {fp.toFixed(2)}, &#x3c6: {(Math.atan(Qr/Pr)*180/Math.PI).toFixed(1)}&deg</text>
      <!-- >fp: {fp.toFixed(2)}, &#x3c6: {(Math.sign(Qr)*(Math.acos(fp)*180/Math.PI)).toFixed(1)}&deg</text> -->
    
  
  <!-- <circle cx=0 cy=0 r=3 fill=red /> -->

</svg>

<style>
  .axes {
    stroke: gray;
    fill: transparent;
  }

  .grid {
    stroke: gray;
    stroke-width: 0.3;
    fill: transparent;
  }

  .grid-label, .fp-label {
    fill: rgb(67, 67, 67);
    font-weight: normal
  }

  .P {
    stroke: rgb(0, 103, 89);
    fill: transparent;
    font-weight: normal
  }

  .Q {
    stroke: rgb(255, 145, 0);
    fill: transparent;
    font-weight: normal
  }

  .S {
    stroke: rgb(155, 0, 251);
    fill: transparent;
    font-weight: normal
  }

  .P-label {
    fill: rgb(0, 103, 89);
    font-weight: normal
  }

  .Q-label {
    fill: rgb(255, 145, 0);
    font-weight: normal
  }

  .S-label {
    fill: rgb(155, 0, 251);
    font-weight: normal
  }

</style>