#include <stdio.h>
#include <stdlib.h>

struct Point
{
	double x;
	double y;
	double z;
};

// вернет 1, если точки лежат в одной плоскости, иначе 0
int are_coplanar(struct Point* p1, struct Point* p2, struct Point* p3, struct Point* p4)
{
	// обратная польская запись:
	// x2 x1 - y3 y1 - * y2 y1 - x3 x1 - * -
	// z4 z1 - * x3 x1 - z2 z1 - * x2 x1 - z3 z1 - * -
	// y4 y1 - * + y2 y1 - z3 z1 - * y3 y1 - z2 z1 - * -
	// x4 x1 - * +
	__asm {
		mov		eax, p1
		mov		ebx, p2
		mov		ecx, p3
		mov		edx, p4

		finit
		fld		qword ptr [eax]			// r7 = x1
		fld		qword ptr [eax + 8]		// r6 = y1
		fld		qword ptr [eax + 16]	// r5 = z1
		
		fld		qword ptr [ebx]			// r4 = x2
		fsub	st, st(3)				// r4 = x2-x1
		
		fld		qword ptr [ecx + 8]		// r3 = y3
		fsub	st, st(3)				// r3 = y3-y1
		fmulp	st(1), st				// r4 = (x2-x1)*(y3-y1)

		fld		qword ptr [ebx + 8]		// r3 = y2
		fsub	st, st(3)				// r3 = y2-y1
		
		fld		qword ptr [ecx]			// r2 = x3
		fsub	st, st(5)				// r2 = x3-x1
		fmulp	st(1), st				// r3 = (y2-y1)*(x3-x1)
		fsubp	st(1), st				// r4 = (x2-x1)*(y3-y1)-(y2-y1)*(x3-x1)
		
		fld		qword ptr [edx + 16]	// r3 = z4
		fsub	st, st(2)				// r3 = z4-z1
		fmulp	st(1), st				// r4 = ((x2-x1)*(y3-y1)-(y2-y1)*(x3-x1)) * (z4-z1) = A

		fld		qword ptr [ecx]			// r3 = x3
		fsub	st, st(4)				// r3 = x3-x1
		
		fld		qword ptr [ebx + 16]	// r2 = z2
		fsub	st, st(3)				// r2 = z2-z1
		fmulp	st(1), st				// r3 = (x3-x1)*(z2-z1)
		
		fld		qword ptr [ebx]			// r2 = x2
		fsub	st, st(5)				// r2 = x2-x1
		
		fld		qword ptr [ecx + 16]	// r1 = z3
		fsub	st, st(4)				// r1 = z3-z1
		fmulp	st(1), st				// r2 = (x2-x1)*(z3-z1)
		fsubp	st(1), st				// r3 = (x3-x1)*(z2-z1)-(x2-x1)*(z3-z1)

		fld		qword ptr [edx + 8]		// r2 = y4
		fsub	st, st(4)				// r2 = y4-y1
		fmulp	st(1), st				// r3 = ((x3-x1)*(z2-z1)-(x2-x1)*(z3-z1)) * (y4-y1) = B
		faddp	st(1), st				// r4 = A + B

		fld		qword ptr [ebx + 8]		// r3 = y2
		fsub	st, st(3)				// r3 = y2-y1
		
		fld		qword ptr [ecx + 16]	// r2 = z3
		fsub	st, st(3)				// r2 = z3-z1
		fmulp	st(1), st				// r3 = (y2-y1)*(z3-z1)
		
		fld		qword ptr [ecx + 8]		// r2 = y3
		fsub	st, st(4)				// r2 = y3-y1
		
		fld		qword ptr [ebx + 16]	// r1 = z2
		fsub	st, st(4)				// r1 = z2-z1
		fmulp	st(1), st				// r2 = (y3-y1)*(z2-z1)
		fsubp	st(1), st				// r3 = (y2-y1)*(z3-z1)-(y3-y1)*(z2-z1)
		
		fld		qword ptr [edx]			// r2 = x4
		fsub	st, st(5)				// r2 = x4-x1
		fmulp	st(1), st				// r3 = ((y2-y1)*(z3-z1)-(y3-y1)*(z2-z1)) * (x4-x1) = C
		faddp	st(1), st				// r4 = A + B + C
		
		xor		eax, eax
		fldz
		fcomi	st, st(1)
		jnz		FALSE
		
		inc		eax
	FALSE:
	}
}

void find_coplanar_points(struct Point arr[], int arr_size)
{
	const char* msg = "{ (%lf, %lf, %lf), (%lf, %lf, %lf), (%lf, %lf, %lf), (%lf, %lf, %lf) }\n";
	__asm {
		mov		eax, arr_size
		mov		ebx, 24
		mul		ebx				 // eax = 24n = максимальное смещение в массиве
		
	CYCLE_1:
		cmp		eax, 0
		jz		END_1
		sub		eax, 24
			
		mov		ebx, eax
		CYCLE_2:
			cmp		ebx, 0
			jz		END_2
			sub		ebx, 24
			
			mov		ecx, ebx
			CYCLE_3:
				cmp		ecx, 0
				jz		END_3
				sub		ecx, 24
			
				mov		edx, ecx
				CYCLE_4:
					cmp		edx, 0
					jz		END_4
					sub		edx, 24

					// сохраняем регистры
					push	edx
					push	ecx
					push	ebx
					push	eax
						
					// аргументы функции are_coplanar
					mov		edi, arr
						
					mov		esi, edi
					add		esi, eax
					push	esi
					mov		esi, edi
					add		esi, ebx
					push	esi
					mov		esi, edi
					add		esi, ecx
					push	esi
					mov		esi, edi
					add		esi, edx
					push	esi

					call	are_coplanar
					add		esp, 16

					cmp		eax, 1
					jne		DONT_PRINT
						
					// аргументы функции printf_s для вывода точек
					mov		eax, [esp]
					mov		ebx, [esp + 4]
					mov		ecx, [esp + 8]
					mov		edx, [esp + 12]
					mov		esi, arr
					push	dword ptr [esi + eax + 16 + 4] 
					push	dword ptr [esi + eax + 16]
					push	dword ptr [esi + eax + 8 + 4]
					push	dword ptr [esi + eax + 8]
					push	dword ptr [esi + eax + 4]
					push	dword ptr [esi + eax]
					push	dword ptr [esi + ebx + 16 + 4] 
					push	dword ptr [esi + ebx + 16]
					push	dword ptr [esi + ebx + 8 + 4]
					push	dword ptr [esi + ebx + 8]
					push	dword ptr [esi + ebx + 4]
					push	dword ptr [esi + ebx]
					push	dword ptr [esi + ecx + 16 + 4] 
					push	dword ptr [esi + ecx + 16]
					push	dword ptr [esi + ecx + 8 + 4]
					push	dword ptr [esi + ecx + 8]
					push	dword ptr [esi + ecx + 4]
					push	dword ptr [esi + ecx]
					push	dword ptr [esi + edx + 16 + 4] 
					push	dword ptr [esi + edx + 16]
					push	dword ptr [esi + edx + 8 + 4]
					push	dword ptr [esi + edx + 8]
					push	dword ptr [esi + edx + 4]
					push	dword ptr [esi + edx]

					push	msg
					call	printf_s
					add		esp, 100
				DONT_PRINT:
					
					// восстанавливаем регистры
					pop		eax
					pop		ebx
					pop		ecx
					pop		edx

					jmp		CYCLE_4
				END_4:
				jmp		CYCLE_3
			END_3:
			jmp		CYCLE_2
		END_2:
		jmp		CYCLE_1
	END_1:

		xor		eax, eax
	}
}

int main()
{
	//struct Point _arr[] = {{3, 2, -5}, {-1, 4, -3}, {-3, 8, -5}, {-3, 2, 1}, {1, 1, 1}};
	////struct Point _arr[] = {{0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}, {1, 1, 1}};
	//int _n = 5;
	//find_coplanar_points(_arr, _n);
	//return 0;

	const char msg1[] = "input array size (>=4):";
	const char msg2[] = "input points like x y z:";
	const char scanf_params[] = "%d";
	const char scanf_params2[] = "%lf %lf %lf";
	int arr_size = 0;
	struct Point* arr = nullptr;

	__asm {
		// puts(msg1);
		lea		eax, msg1
		push	eax
		call	puts
		add		esp, 4

		// scanf_s("%d", &arr_size);
		lea		eax, arr_size
		push	eax
		lea		eax, scanf_params
		push	eax
		call	scanf_s
		add		esp, 4 // 4 освободится в END
		
		mov		ecx, arr_size
		cmp		ecx, 4
		jl		END
		
		// puts(msg2);
		lea		eax, msg2
		mov		[esp], eax
		call	puts

        // arr = (struct Point*) malloc(arr_size * 24);
		mov		eax, arr_size
		mov		ecx, 24
		mul		ecx
		mov		[esp], eax
		call	malloc
		mov		[arr], eax

	CYCLE:
		mov		ecx, [esp]
		cmp		ecx, 0
		jle		END
		sub		ecx, 24
		mov		[esp], ecx

		// scanf_s("%lf %lf %lf", &arr[i].x, &arr[i].y, &arr[i].z);
		mov		esi, arr
		lea		eax, [esi + ecx + 16]
		push	eax
		lea		eax, [esi + ecx + 8]
		push	eax
		lea		eax, [esi + ecx]
		push	eax
		lea		eax, scanf_params2
		push	eax
		call	scanf_s
		add		esp, 16

		jmp CYCLE
	END:
		// find_coplanar_points(arr, arr_size);
		push	dword ptr arr_size
		push	dword ptr arr
		call	find_coplanar_points
		add		esp, 8
		
		mov		eax, arr
		mov		[esp], eax
		call	free
		add		esp, 4

		xor		eax, eax
	}
}
