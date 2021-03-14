#include "PCI.h"

uint32_t pci_size_map[100];
PICdev_T dev_zero= {0};

uint32_t PCI::read(PICdev_T dev, uint32_t field) {
	// Only most significant 6 bits of the field
	dev.field_num = (field & 0xFC) >> 2;
	dev.enable = 1;
	port_long_out(PCI_CONFIG_ADDRESS, dev.bits);

	// What size is this field supposed to be ?
	uint32_t size = pci_size_map[field];
	if(size == 1) {
		// Get the first byte only, since it's in little endian, it's actually the 3rd byte
		uint8_t t =port_byte_in(PCI_CONFIG_DATA + (field & 3));
		return t;
	}
	else if(size == 2) {
		uint16_t t = port_word_in(PCI_CONFIG_DATA + (field & 2));
		return t;
	}
	else if(size == 4){
		// PCI::read entire 4 bytes
		uint32_t t = port_long_in(PCI_CONFIG_DATA);
		return t;
	}
	return 0xffff;
}

void PCI::write(PICdev_T dev, uint32_t field, uint32_t value) {
	dev.field_num = (field & 0xFC) >> 2;
	dev.enable = 1;
	// Tell where we want to write
	port_long_out(PCI_CONFIG_ADDRESS, dev.bits);
	// Value to write
	port_long_out(PCI_CONFIG_DATA, value);
}

uint32_t PCI::get_device_type(PICdev_T dev) {
	uint32_t t = PCI::read(dev, PCI_CLASS) << 8;
	return t | PCI::read(dev, PCI_SUBCLASS);
}

uint32_t PCI::get_secondary_bus(PICdev_T dev) {
	return PCI::read(dev, PCI_SECONDARY_BUS);
}

uint32_t PCI::reach_end(PICdev_T dev) {
	uint32_t t = PCI::read(dev, PCI_HEADER_TYPE);
	return !t;
}

PICdev_T PCI::scan_function(uint16_t vendor_id, uint16_t device_id, uint32_t bus, uint32_t device, uint32_t function, int device_type) {
	PICdev_T dev = {0};
	dev.bus_num = bus;
	dev.device_num = device;
	dev.function_num = function;
	// If it's a PCI Bridge device, get the bus it's connected to and keep searching
	if(PCI::get_device_type(dev) == PCI_TYPE_BRIDGE) {
		scan_bus(vendor_id, device_id, PCI::get_secondary_bus(dev), device_type);
	}
	// If type matches, we've found the device, just return it
	if(device_type == -1 || device_type == get_device_type(dev)) {
		uint32_t devid  = PCI::read(dev, PCI_DEVICE_ID);
		uint32_t vendid = PCI::read(dev, PCI_VENDOR_ID);
		if(devid == device_id && vendor_id == vendid)
			return dev;
	}
	return dev_zero;
}

PICdev_T PCI::scan_device(uint16_t vendor_id, uint16_t device_id, uint32_t bus, uint32_t device, int device_type) {
	PICdev_T dev = {0};
	dev.bus_num = bus;
	dev.device_num = device;

	if(PCI::read(dev,PCI_VENDOR_ID) == PCI_NONE)
		return dev_zero;

	PICdev_T t = PCI::scan_function(vendor_id, device_id, bus, device, 0, device_type);
	if(t.bits)
		return t;

	if(PCI::reach_end(dev))
		return dev_zero;

	for(int function = 1; function < FUNCTION_PER_DEVICE; function++) {
		if(PCI::read(dev,PCI_VENDOR_ID) != PCI_NONE) {
			t = PCI::scan_function(vendor_id, device_id, bus, device, function, device_type);
			if(t.bits)
				return t;
		}
	}
	return dev_zero;
}

PICdev_T PCI::scan_bus(uint16_t vendor_id, uint16_t device_id, uint32_t bus, int device_type) {
	for(int device = 0; device < DEVICE_PER_BUS; device++) {
		PICdev_T t = PCI::scan_device(vendor_id, device_id, bus, device, device_type);
		if(t.bits)
			return t;
	}
	return dev_zero;
}


PICdev_T PCI::get_device(uint16_t vendor_id, uint16_t device_id, int device_type) {

	PICdev_T t = PCI::scan_bus(vendor_id, device_id, 0, device_type);
	if(t.bits)
		return t;

	// Handle multiple pci host controllers

	if(PCI::reach_end(dev_zero)) {
		VGA::kprintf("PCI Get device failed...\n");
	}
	for(int function = 1; function < FUNCTION_PER_DEVICE; function++) {
		PICdev_T dev = {0};
		dev.function_num = function;

		if(PCI::read(dev, PCI_VENDOR_ID) == PCI_NONE)
			break;
		t = PCI::scan_bus(vendor_id, device_id, function, device_type);
		if(t.bits)
			return t;
	}
	return dev_zero;
}

void PCI::pci_init() {
	// Init size map
    VGA::PRINT_STR("PCI INIT ");
	pci_size_map[PCI_VENDOR_ID] =	2;
	pci_size_map[PCI_DEVICE_ID] =	2;
	pci_size_map[PCI_COMMAND]	=	2;
	pci_size_map[PCI_STATUS]	=	2;
	pci_size_map[PCI_SUBCLASS]	=	1;
	pci_size_map[PCI_CLASS]		=	1;
	pci_size_map[PCI_CACHE_LINE_SIZE]	= 1;
	pci_size_map[PCI_LATENCY_TIMER]		= 1;
	pci_size_map[PCI_HEADER_TYPE] = 1;
	pci_size_map[PCI_BIST] = 1;
	pci_size_map[PCI_BAR0] = 4;
	pci_size_map[PCI_BAR1] = 4;
	pci_size_map[PCI_BAR2] = 4;
	pci_size_map[PCI_BAR3] = 4;
	pci_size_map[PCI_BAR4] = 4;
	pci_size_map[PCI_BAR5] = 4;
	pci_size_map[PCI_INTERRUPT_LINE]	= 1;
	pci_size_map[PCI_SECONDARY_BUS]		= 1;

    G_OK;

}
