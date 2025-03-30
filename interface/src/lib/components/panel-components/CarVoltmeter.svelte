<script>

	import { beforeUpdate } from 'svelte';
	import { spring } from 'svelte/motion';

    export let voltage = 12;
	export let alarm = false;

	let voltageNeedle = 12;
	let color = "rgb(255, 71, 71)";

	$:{
		if (alarm) color = "rgb(255, 71, 71)"; else color = "rgb(204, 245, 135)";
		voltageNeedle = voltage;
		if (voltageNeedle < 9.12)
			voltageNeedle = 9.12;
		else if (voltageNeedle > 16.88)
			voltageNeedle = 16.88;
	}

	let angle1 = (90 - 14*(voltageNeedle - 13)) * Math.PI / 180;

    $: angle = (90 - 14*(voltageNeedle - 13)) * Math.PI / 180;

	$: x2 = 80 * Math.cos(angle);
  	$: y2 = 50 - 60 * Math.sin(angle);
  
	$: coords1 = spring({ x: 80 * Math.cos(angle1), y: 50 - 60 * Math.sin(angle1) }, {
		stiffness: 0.08,
		damping: 0.28
	});

	// console.log(`${voltage} ${voltageToShow}`)


	beforeUpdate(() => {
		// console.log(`${voltage} ${voltageToShow}`)

		coords1.set({x:x2, y:y2});
	})

</script>

<svg width="200" height="110" viewBox="-100 -100 200 200">
    <defs>
        <radialGradient id="RadialGradient">
            <stop offset="86%" stop-color="black" />
            <stop offset="92%" stop-color="white" />
            <stop offset="100%" stop-color="black" />
            
        </radialGradient>

        <clipPath id="cut-needle-bat">
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
			M -17 48
			l  34  0
			l   0 20
			l -34  0
			Z
			m   3  0
			l   0 -3
			l   5  0
			l  -0  3
			m  23  0
			l   0 -3
			l  -5  0
			l   0  3
			"
			stroke-width="3"
  	/>

	<path stroke={color} fill=transparent
	  d="
		  M -13  55 
		  l   6   0
		  m  20   0
		  l  -6   0
		  l   3   0
		  l   0  -3
		  l   0   6
		  "
		  stroke-width="1"
	/>

	<path class="scale-foreground"
	  d="
		  M  -52 6  
		  L  -44   15 
		  Q    0 -25 44 15
		  L   52 6
		  "
		  stroke-width="3"
	/>

	<path class="scale-right-mark"
	  d="
		  M  34 7
		  Q  31 5 44 15
		  L  52 6
		  "
		  stroke-width="3"
	/>

	<path class="scale-central-mark"
	  d="
		  M  -19 -1
		  Q  0 -9 19 -1
		  M  0 -5
		  L  0 -13
		  "
		  stroke-width="3"
	/>

	<text class="units" x="0" y="-58" 
		font-size = 25
		dominant-baseline = "middle"
		text-anchor = "middle"
		font-weight = "bold"
		fill="white"
	>{voltage.toFixed(1)}</text>

	<text class="units" x="0" y="19" 
		font-size = 18	
		dominant-baseline = "middle"
		text-anchor = "middle"
		font-weight = "bold"
		fill="white"
	>V</text>

    <text class="units" x="-46" y="-37" 
		font-size = 12
		dominant-baseline = "middle"
		text-anchor = "middle"
		font-weight = "bold"
		fill="white"
        transform = "rotate(-35)"
	>10</text>

    <text class="units" x="0" y="-27" 
		font-size = 12
		dominant-baseline = "middle"
		text-anchor = "middle"
		font-weight = "bold"
		fill="white"
        transform = "rotate(0)"
	>13</text>

    <text class="units" x="43" y="-36" 
		font-size = 12
		dominant-baseline = "middle"
		text-anchor = "middle"
		font-weight = "bold"
		fill="white"
        transform = "rotate(35)"
	>16</text>


    <line class="needle" 
        x1= 0 
        y1= 65 
        x2= {$coords1.x} 
        y2= {$coords1.y}
        stroke-width="6" 
        stroke-linecap="round"
        clip-path="url(#cut-needle-bat)"
    >
    </line>

    <!-- <circle class="needle" cx="0" cy="65" r="60"/> -->

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
        stroke: #ff8000;
        fill: transparent
    }

    .scale-central-mark {
        stroke: #ecf8d7;
    }

	.scale-right-mark {
        stroke: red;
		fill:transparent
    }

    .units {
        fill: #c6f4fa;
    }

    .needle {
        stroke: #ff9f2a;
        fill: transparent;
    }
</style>