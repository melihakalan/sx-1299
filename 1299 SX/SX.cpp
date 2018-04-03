#include <windows.h>
#include <stdio.h>
#include "def.h"

#pragma argsused

void __stdcall leave();
long __stdcall getskill(int id);
int __stdcall makecast(int num);

void __stdcall SX()
{
	FILE* fin = fopen("SX.ini", "r");

	if( !fin )
	{
		leave();
		return;
	}

	int id, offset, val, cb;
	int st = fscanf(fin, "%d %x %d %d", &id, &offset, &val, &cb);

	while(st != EOF)
	{
		if(id < 1000)
			id = makecast(id);

		long base = getskill(id);

		if(base)
		{
			switch(cb)
			{
				case 1:
				*(BYTE*)(base + offset) = (BYTE)val;
				break;

				case 2:
				*(short*)(base + offset) = (short)val;
				break;

				case 4:
				*(int*)(base + offset) = val;
				break;

				case 8:
				*(float*)(base + offset) = (float)val;
				break;
			}
		}

		st = fscanf(fin, "%d %x %d %d", &id, &offset, &val, &cb);
	}

	fclose(fin);
	leave();
}

void __stdcall leave()
{
    FreeLibraryAndExitThread( GetModuleHandle("SX.dll"), 0 );
}

long __stdcall getskill(int id)
{
	long base = 0;
	int _id = id;

	asm
	{
		lea eax, &_id
		push eax
		lea eax, &base
		push eax
		mov ecx, [KO_PSFN]
		lea ecx, [ecx + 0x14]
		mov eax, KO_CSFN
		call eax
	}

	if(!base)
		return 0;
	else
		return base + 0x10;
}

int __stdcall makecast(int num)
{
	int c = *(int*)( *(DWORD*)(KO_CHAR) + KO_CLSS);
	return( c*1000 + num );
}

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
	if(reason == DLL_PROCESS_ATTACH)
		CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)SX,NULL,NULL,NULL);

	return 1;
}
