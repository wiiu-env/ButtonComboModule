FROM ghcr.io/wiiu-env/devkitppc:20241128

COPY --from=ghcr.io/wiiu-env/libbuttoncombo:20250104-0663ede /artifacts $DEVKITPRO
COPY --from=ghcr.io/wiiu-env/libfunctionpatcher:20241012 /artifacts $DEVKITPRO
COPY --from=ghcr.io/wiiu-env/wiiumodulesystem:20240424 /artifacts $DEVKITPRO

WORKDIR project
