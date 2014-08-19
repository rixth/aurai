help:
	@echo "This Makefile has no default rule. Use one of the following:"
	@echo "make hub.hex ......... to build hub.hex"
	@echo "make spoke.hex ....... to build spoke.hex"
	@echo "make hex ............. to build both hex files"
	@echo "make flash-hub ....... flash the hub"
	@echo "make flash-spoke ..... flash the spoke"
	@echo "make flashh........... flash both devices (hub first)"
	@echo "make flashs........... flash both devices (spoke first)"

hub.hex:
	$(MAKE) -C hub clean hex

spoke.hex:
	$(MAKE) -C spoke clean hex

hex: hub.hex spoke.hex

flash-hub: hub.hex
	$(MAKE) -C hub flash

flash-spoke: spoke.hex
	$(MAKE) -C spoke flash

flashh:
	@echo "Connect hub & press enter"
	@read -n 1
	$(MAKE) -C hub flash
	@echo "Connect spoke & press enter"
	@read -n 1
	$(MAKE) -C spoke flash

flashs:
	@echo "Connect spoke & press enter"
	@read -n 1
	$(MAKE) -C spoke flash
	@echo "Connect hub & press enter"
	@read -n 1
	$(MAKE) -C hub flash