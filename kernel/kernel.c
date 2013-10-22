void hltProcessor(void);

void kmain(void)
{
	*(char*)0xB8000 = 'A';
	hltProcessor();
}

void hltProcessor(void)
{
	__asm__("cli\n\t"
			"hlt");
}
