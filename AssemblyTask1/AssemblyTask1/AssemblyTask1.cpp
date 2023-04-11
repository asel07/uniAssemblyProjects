#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* removeChars(const char *str1, int str1_size, const char *str2) {
	__asm {
		mov		eax, str1_size
		inc		eax							
		push	eax							
		call	malloc						
		add		esp, 4

		mov		esi, str1
		mov		edi, str2
			
		xor		ecx, ecx					
		xor		edx, edx					
	STR1_CYCLE:
		cmp		byte ptr [esi + ecx], 0		
		jz		END
		xor		ebx, ebx					
	STR2_CYCLE:
		cmp		byte ptr [edi + ebx], 0		
		jz		ENDstr12_CYCLE

		push	eax // свободные регистры кончились
		mov		ah, [esi + ecx]
		cmp		ah, [edi + ebx]				
		pop		eax //
		je		ENDstr12_CYCLE				

		inc		ebx							
		jmp		STR2_CYCLE					
	ENDstr12_CYCLE:
		cmp		byte ptr [edi + ebx], 0		
		jnz		END_IF
			
		push	ebx // свободные регистры кончились
		mov		bh, [esi + ecx]
		mov		[eax + edx], bh				
		pop		ebx //

		inc		edx						
	END_IF:									
		inc		ecx							
		jmp		STR1_CYCLE					
	END:
		mov		[eax + edx], 0				
	}
}

int main()
{
	const char msg1[] = "input first string:";
	const char msg2[] = "input second string:";
	const char msg3[] = "first string without characters from the second one:\n%s\n";

	const int MAX_SIZE = 100;
	char *str1 = nullptr, *str2 = nullptr;

	__asm {
        // str1 = (char*) malloc(MAX_SIZE);
		push	dword ptr MAX_SIZE
		call	malloc
		mov		[str1], eax

		// str2 = (char*) malloc(MAX_SIZE);
		call	malloc
		mov		[str2], eax

		// puts(msg1);
		lea		eax, msg1
		push	eax
		call	puts

		// gets_s(str1, MAX_SIZE);
		mov		eax, str1
		mov		[esp], eax
		call	gets_s

		// puts(msg2);
		lea		eax, msg2
		mov		[esp], eax
		call	puts

		// gets_s(str2, MAX_SIZE);
		mov		eax, MAX_SIZE
		mov		[esp + 4], eax
		mov		eax, str2
		mov		[esp], eax
		call	gets_s

		// char * result = removeChars(str1, MAX_SIZE, str2);
		mov		eax, str2
		mov		[esp + 4], eax
		mov		eax, MAX_SIZE
		mov		[esp], eax
		push	dword ptr str1
		call	removeChars

		// printf_s(msg3, result);
		mov		[esp + 4], eax
		lea		eax, msg3
		mov		[esp], eax
		call	printf_s
			
		// free(result);
		add		esp, 4
		call	free

		// free(str1);
		mov		eax, str1
		mov		[esp], eax
		call	free
		
		// free(str2);
		mov		eax, str2
		mov		[esp], eax
		call	free

		add		esp, 8
		xor		eax, eax
	}
}
