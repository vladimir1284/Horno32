<script>
    export let name;
    export let active;
    export let color = 'red';

</script>

<div class="status-led" class:active>
    {#if color === 'green'}
        <div class="led-ok"></div>
    {:else if color === 'red'}
        <div class="led-fail" ></div>
    {/if}
    <span class=ledName>{name}</span>
</div>

<style>
    .status-led {
        display: flex;
        align-items: center;
        margin-bottom: 10px;
    }

    .led-ok, .led-fail {
        width: 20px;
        height: 20px;
        border-radius: 50%;
        background-color: gray;
        margin-right: 7px;
    }

    .ledName {
        font-size: small;
    }

    .active .led-ok {
        background-color: rgb(0, 189, 0);
    }

    .active .led-fail {
        animation: blink 0.5s infinite;
    }

    @keyframes blink {
        0%, 50% {
            background-color: rgb(255, 0, 0);
        }
        51%, 100% {
            background-color: rgb(255, 131, 131);
        }
    }
</style>
