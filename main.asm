	.MODEL	SMALL
	.386
	.STACK	100h
	.DATA
GlobalVariables DW 500 DUP (?)
Parameters DW 500 DUP (?)
Parameter_Offset DW ?
	.CODE
	EXTERN	PutDec : Near
main PROC
	MOV	AX, @DATA
	MOV	DS, AX
	MOV	AX, 1
	MOV	BX, OFFSET Parameters
	ADD	BX, Parameter_Offset
	ADD	BX, 0
	MOV	[BX], AX
	ADD	Parameter_Offset, 2
	CALL	fib
	SUB	Parameter_Offset, 2
	MOV	AL, 0
	MOV	AH, 4CH
	INT	21H
main ENDP
fib PROC
	MOV	BX, OFFSET Parameters
	ADD	BX, Parameter_Offset
	SUB	BX, 2
	MOV	AX, [BX]
	PUSH	AX
	MOV	AX, 1
	POP	BX
	CMP	AX, BX
	LAHF
	SHR	AH, 6
	AND	AH, 1
	MOV	AL, AH
	CBW
	PUSH	AX
	MOV	BX, OFFSET Parameters
	ADD	BX, Parameter_Offset
	SUB	BX, 2
	MOV	AX, [BX]
	PUSH	AX
	MOV	AX, 2
	POP	BX
	CMP	AX, BX
	LAHF
	SHR	AH, 6
	AND	AH, 1
	MOV	AL, AH
	CBW
	POP	BX
	OR	AX, BX
	CMP	AX, 0
	JE	ELSE_CLAUSE_0
	MOV	AX, 1
	JMP	END_IF_0
ELSE_CLAUSE_0:
	MOV	BX, OFFSET Parameters
	ADD	BX, Parameter_Offset
	SUB	BX, 2
	MOV	AX, [BX]
	PUSH	AX
	MOV	AX, 1
	POP	BX
	SUB	BX, AX
	MOV	AX, BX
	MOV	BX, OFFSET Parameters
	ADD	BX, Parameter_Offset
	ADD	BX, 0
	MOV	[BX], AX
	ADD	Parameter_Offset, 2
	CALL	fib
	SUB	Parameter_Offset, 2
	PUSH	AX
	MOV	BX, OFFSET Parameters
	ADD	BX, Parameter_Offset
	SUB	BX, 2
	MOV	AX, [BX]
	PUSH	AX
	MOV	AX, 2
	POP	BX
	SUB	BX, AX
	MOV	AX, BX
	MOV	BX, OFFSET Parameters
	ADD	BX, Parameter_Offset
	ADD	BX, 0
	MOV	[BX], AX
	ADD	Parameter_Offset, 2
	CALL	fib
	SUB	Parameter_Offset, 2
	POP	BX
	ADD	AX, BX
END_IF_0:
	RET
fib ENDP
	END main
