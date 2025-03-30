
<script>
  import { createEventDispatcher } from 'svelte';

  export let enabled = true;
  export let timeStopButtonPressed = 300;

  const dispatch = createEventDispatcher();

  let isPressed = false;

  function handleClick() {
    if (enabled) {
      console.log("Botón presionado");
      isPressed = true;

      // Despachar el evento con el nuevo valor de isStartFail
      dispatch('cmdStop');

      // Restablecer el estado después de un breve período para mostrar el efecto
      setTimeout(() => {
        isPressed = false;
      }, timeStopButtonPressed); // Duración del efecto en milisegundos
    }
  }
</script>

<style>
  .button {
    background-color: #ff4d4d; /* Color de fondo */
    color: white; /* Color del texto */
    border: none; /* Sin borde */
    border-radius: 5px; /* Bordes redondeados */
    padding: 10px 20px; /* Espaciado interno */
    font-size: 16px; /* Tamaño de la fuente */
    cursor: pointer; /* Cambia el cursor al pasar el mouse */
    transition: background-color 0.3s, transform 0.1s; /* Transición suave */
  }

  .button:disabled {
    background-color: #ccc; /* Color cuando está deshabilitado */
    cursor: not-allowed; /* Cursor de no permitido */
  }

  .button:hover:not(:disabled) {
    background-color: #ff1a1a; /* Color al pasar el mouse */
  }

  .button-pressed {
    background-color: #d1725d; /* Color de fondo */
    color: white; /* Color del texto */
    border: none; /* Sin borde */
    border-radius: 5px; /* Bordes redondeados */
    padding: 10px 20px; /* Espaciado interno */
    font-size: 16px; /* Tamaño de la fuente */
    cursor: pointer; /* Cambia el cursor al pasar el mouse */
    transform: scale(0.95); /* Efecto de reducción al presionar */
  }
</style>

<button class="button{isPressed ? '-pressed' : ''}" on:click={handleClick} disabled={!enabled}>
  Parar
</button>