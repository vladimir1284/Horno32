<script>
	import { page } from '$app/stores';
	import logo from '$lib/images/logo_ladetec.png';
</script>

<header>
	<div class="corner">
		<img src={logo} alt="Ladetec" />
		<h1>Horno</h1>
	</div>
</header>

<style>
	header {
		display: flex;
		justify-content: space-between;
	}
	@media (max-height: 479px) {
		header {
			display: none;
		}
	}
	.corner {
		display: flex;
		align-items: center; /* Center the items vertically */
		width: 3em;
		height: 3em;
		margin: 1em;
	}
	h1 {
		padding: 1em; /* Fixed typo from '1erm' to '1em' */
		color: red; /* Change text color to red */
		text-align: center; /* Center the text */
		margin-left: 10px; /* Add some space between the image and text */
	}
</style>
