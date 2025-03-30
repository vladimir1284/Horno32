<script lang="ts">

	import { beforeUpdate } from 'svelte';
	import { spring } from 'svelte/motion';

    export let temperature = 80;
	export let alarm = false;

	let temperatureNeedle = 80;
	let color = "rgb(255, 71, 72)";
	
	$: {
		if (alarm) color = "rgb(255, 71, 71)"; else color = "rgb(204, 245, 135)";
		temperatureNeedle = temperature;
		if (temperatureNeedle < 39)
			temperatureNeedle = 39;
		else if (temperatureNeedle > 131)
			temperatureNeedle = 131;
	}

    let angle1 = (90 - 1.18*(temperatureNeedle - 85)) * Math.PI / 180;
	
	$: angle = (90 - 1.18*(temperatureNeedle - 85)) * Math.PI / 180;;

	$: x2 = 80 * Math.cos(angle);
	$: y2 = 50 - 60 * Math.sin(angle);

	$: coords1 = spring({ x: 80 * Math.cos(angle1), y: 50 - 60 * Math.sin(angle1) }, {
		stiffness: 0.08,
		damping: 0.28
	});

	beforeUpdate(() => {
		coords1.set({x:x2, y:y2});
	});

</script>

<svg width="200" height="110" viewBox="-100 -100 200 200">
    <defs>
        <radialGradient id="RadialGradient">
            <stop offset="86%" stop-color="black" />
            <stop offset="92%" stop-color="white" />
            <stop offset="100%" stop-color="black" />
            
        </radialGradient>

        <clipPath id="cut-needle-temp">
            <path 
                d="
                    M -45, 30
					L  45, 30
					Q  51.5 30 55 25
					L  68 -5
					Q  70 -9 68 -12
					Q   0 -68 -68 -12
					Q  -70 -9 -68 -5
					L  -55 25
					Q  -51.5 30 -45 30 
					"
			/>
        </clipPath>
    </defs>
    <circle cx="0" cy="0" r="100" fill="url(#RadialGradient)"/>
    
    <circle class="instrument-background" cx="0" cy="4" r="85" />
		
    <path class="scale-background"
		d="
			M -45, 30
			L  45, 30
			Q  51.5 30 55 25
			L  68 -5
			Q  70 -9 68 -12
			Q   0 -68 -68 -12
			Q  -70 -9 -68 -5
			L  -55 25
			Q  -51.5 30 -45 30 
			"
			stroke-width="1"
  	/>
	
	<path stroke={color}
		d="
			M -17 65
			q   5 -5   8 -2
			q   3  4   6  0
			q   3 -3   6  0
			q   3  4   6  0
			q   3 -3   8  2
			"
			stroke-width="3"
  	/>

	<rect class="instrument-background" x="-6" y="59" width="12" height="10"/>

	<path stroke={color}
		d="
			M -15   73
			q   3    4   6  0
			q   3   -3   6  0
			q   3    4   6  0
			q   3   -3   6  0
			q   3    4   6  0
			m -15, -34
		    l   0,  23
		    l   0, -20
		    l   8,   0
		    l  -8,   0
		    l   0,   6
		    l   8,   0
		    l  -8,   0 
		    l   0,   6
		    l   8,   0
		    l  -8,   0
		    "
			stroke-width="3"
  	/>

	<circle cx="0" cy="64" r="2" stroke-width="4" stroke={color}/>

	<path class="scale-foreground"
	  d="
		  M  -52   6   
		  L  -44  15 
		  Q    0 -20 44 15
		  L   52   6
		  "
		  stroke="white"
		  stroke-width="3"
	/>

	<path class="scale-foreground-mark"
	  d="
		  M  -10 -2
		  L  -10.5 -9
		  L  -4 -9
		  L   -4  -6
		  L   12  -6
		  L   12  -3
		  Q   0 -5 -10 -2
		  M   20 1
		  L   22 -5
		  "
		  stroke-width="3"
	/>

	<text class="units" x="0" y="-58" 
		font-size = 25
		dominant-baseline = "middle"
		text-anchor = "middle"
		font-weight = "bold"
		fill="white"
	>{temperature.toFixed(1)}</text>

	<text class="units" x="0" y="19" 
		font-size = 18	
		dominant-baseline = "middle"
		text-anchor = "middle"
		font-weight = "bold"
		fill="white"
	>&degC</text>

	<text class="units" x="-44" y="-37" 
		font-size = 12
		dominant-baseline = "middle"
		text-anchor = "middle"
		font-weight = "bold"
		fill="white"
        transform = "rotate(-35)"
	>50</text>

    <text class="units" x="-10" y="-26" 
		font-size = 12
		dominant-baseline = "middle"
		text-anchor = "middle"
		font-weight = "bold"
		fill="white"
        transform = "rotate(-4)"
	>80</text>

	<text class="units" x="16" y="-30" 
		font-size = 12
		dominant-baseline = "middle"
		text-anchor = "middle"
		font-weight = "bold"
		fill="white"
        transform = "rotate(19)"
	>100</text>

    <text class="units" x="43" y="-37" 
		font-size = 12
		dominant-baseline = "middle"
		text-anchor = "middle"
		font-weight = "bold"
		fill="white"
        transform = "rotate(35)"
	>120</text>


    <line class="needle" 
		x1= 0 
		y1= 65 
		x2= {$coords1.x} 
		y2= {$coords1.y}
		stroke-width="6" 
        stroke-linecap="round"
        clip-path="url(#cut-needle-temp)"
    >
    </line>

    <!-- <circle class="needle" cx="0" cy="50" r="78"/> -->

    <!-- <circle cx="30" cy="100" r="2" fill="red" />
	<circle cx="45" cy="100" r="2" fill="red" /> -->

</svg>

<style>
    .instrument-background {
        fill: #4b4c6b
    }

    .scale-background {
        fill: #0b1c39
    }

    .scale-foreground {
        stroke: #ccf587;
        fill: transparent
    }

    .scale-foreground-mark {
        stroke: #ccf587;
        fill: #ccf587;
    }

    .units {
        fill: #c6f4fa;
    }

    .needle {
        stroke: #ff9f2a;
        fill: transparent;
    }
</style>