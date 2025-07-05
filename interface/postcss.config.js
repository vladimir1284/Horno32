import tailwindcss from '@tailwindcss/postcss';
import autoprefixer from 'autoprefixer';
import postcssRemoveClasses from 'postcss-remove-classes';

export default {
	plugins: [
		tailwindcss(),
		autoprefixer()
		//postcssRemoveClasses.default('range')
	]
};
