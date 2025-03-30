import type { PageLoad } from './$types';

export const ssr = false;

export const load = (async ({ fetch }) => {
	return {
		// title: 'Demo App'
	};
}) satisfies PageLoad;
