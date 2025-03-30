<script>

	import { beforeUpdate } from 'svelte';
	import { spring } from 'svelte/motion';

    export let oilPressure = 4;
	export let alarm = false;

	let oilPressureNeedle = 4;
	let color = "rgb(255, 71, 72)";

	$: {
		if (alarm) color = "rgb(255, 71, 71)"; else color = "rgb(204, 245, 135)";
		oilPressureNeedle = oilPressure;
		if (oilPressureNeedle < -0.9)
			oilPressureNeedle = -0.9;
		else if (oilPressureNeedle > 8.9)
			oilPressureNeedle = 8.9;
	}

	// let angle1 = (90 - 13.9*(oilPressureNeedle - 3)) * Math.PI / 180;
	let angle1 = (0.8383*oilPressureNeedle*oilPressureNeedle - 16.46*oilPressureNeedle + 128.3) * Math.PI / 180;
	
	// $: angle = (90 - 13.9*(oilPressureNeedle - 3)) * Math.PI / 180;
	$: angle = (0.8383*oilPressureNeedle*oilPressureNeedle - 16.46*oilPressureNeedle + 128.3) * Math.PI / 180;

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

        <clipPath id="cut-needle-oil">
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
    
    <circle class="instrument-background" cx="0" cy="0" r="85" />
	
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

	<path stroke={color} fill=transparent
		d="
			M -12  54
			l  17   0
			l   3   3
			l  13  -5
			l   3   3
			m  -7  -1
			l  -9  10
			l -20   0
			l   0 -15
			l  -7  -2
			l  -1   4
			l  12   3
			m   6   0
			l   0  -6
			l   5   0
			l -10   0
			"
			stroke-width="3"
  	/>
	  
	<path stroke={color} fill=transparent
	  d="
		  M 24 58
		  l  1  4
		  q -1  3 -2 0
		  l  1 -3
		  "
		  stroke-width="1"
	/>

	<path class="scale-foreground"
	  d="
		  M  -52 6  
		  L  -44   15 
		  Q    0 -20 44 15
		  L   52 6
		  M   16.8 -0.8
		  L   18.5 -10
		  "
		  stroke-width="3"
	/>

	<circle class="scale-foreground" cx="35" cy="-1" r="1" stroke-width="2" />

	<text class="units" x="0" y="-58" 
		font-size = 25
		dominant-baseline = "middle"
		text-anchor = "middle"
		font-weight = "bold"
		fill="white"
	>{oilPressure.toFixed(1)}</text>

    <text class="units" x="0" y="19" 
		font-size = 18	
		dominant-baseline = "middle"
		text-anchor = "middle"
		font-weight = "bold"
		fill="white"
	>kg/cm&sup2</text>

    <text class="units" x="-47" y="-37" 
		font-size = 12
		dominant-baseline = "middle"
		text-anchor = "middle"
		font-weight = "bold"
		fill="white"
        transform = "rotate(-35)"
	>0</text>

    <text class="units" x="15" y="-26" 
		font-size = 12
		dominant-baseline = "middle"
		text-anchor = "middle"
		font-weight = "bold"
		fill="white"
        transform = "rotate(14)"
	>4</text>

    <text class="units" x="43" y="-40" 
		font-size = 12
		dominant-baseline = "middle"
		text-anchor = "middle"
		font-weight = "bold"
		fill="white"
        transform = "rotate(39)"
	>8</text>


    <line class="needle" 
        x1= 0 
        y1= 65 
        x2= {$coords1.x} 
        y2= {$coords1.y}
        stroke-width="6" 
        stroke-linecap="round"
        clip-path="url(#cut-needle-oil)"
    >
    </line>

    <!-- <circle class="needle" cx="0" cy="50" r="78"/> -->

    <!-- <circle cx="25" cy="-11.6" r="2" fill="red" />
	<circle cx="-5" cy="-17.2" r="2" fill="red" /> -->

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

    .units {
        fill: #c6f4fa;
    }

    .needle {
        stroke: #ff9f2a;
        fill: transparent;
    }
</style>