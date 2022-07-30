{ pkgs ? import <nixpkgs> {} }:

with pkgs; mkShell {
	buildInputs = [
		ruby_2_7
	];

	nativeBuildInputs = [
		bundler
	];
}
