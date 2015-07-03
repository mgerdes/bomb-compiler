	.XLIST
;;  PCMac.inc--Macro INCLUDE file to be used with "Assembly Language
;;   for the IBM PC Family" by William B. Jones, (c) Copyright 1992,
;;   Scott/Jones, Inc.

_LdAddr	MACRO	reg, addr
IF (.TYPE (addr)) AND 10000B	;; Register
        mov	reg, addr
ELSE
    IF (.TYPE (addr)) AND 100B	;; Constant
        mov	reg, addr
    ELSE
	IF TYPE (addr) EQ 1		;; Bytes
        lea	reg, addr
	ELSE
	    IF TYPE (addr) EQ 2		;; Near pointer
	mov	reg, addr
	    ELSE
		IF TYPE (addr) EQ 4		;; Far pointer
	lds	reg, addr
		ELSE
	.ERR
	%OUT	Illegal argument
		ENDIF
	    ENDIF
	ENDIF
    ENDIF
ENDIF
ENDM

_LdSeg	MACRO	reg, segm
IFNB <segm>
    IFIDNI <segm>, <es>		;; Segment register
	mov	ax, segm
	mov	reg, ax
    ELSE
	IFIDNI	<segm>, <ds>
	mov	ax, segm
	mov	reg, ax
    	ELSE
	    IFIDNI	<segm>, <cs>
	mov	ax, segm
	mov	reg, ax
	    ELSE
		IFIDNI	<segm>, <ss>
	mov	ax, segm
	mov	reg, ax
		ELSE
	mov	ax, segm
	mov	reg, ax
		ENDIF
	    ENDIF
	ENDIF
    ENDIF
ENDIF
ENDM

_PutStr	MACRO	addr, segm
	_LdAddr	dx, addr
	_LdSeg	ds, <segm> ;; load segment second in case ax used in addr
	mov	ah, 09h
	int	21h
	ENDM

_PutCh	MACRO	c1, c2, c3, c4, c5, c6, c7, c8, c9, c10
	mov	ah, 02h
IFB <c1>
	int	21h ;;	char already in dl
ELSE
	IRP	x, <c1, c2, c3, c4, c5, c6, c7, c8, c9, c10>
    IFB <x>
	EXITM
    ENDIF
	mov	dl, x
	int	21h
	ENDM
ENDIF
	ENDM

_GetCh	MACRO echo
IFIDNI	<&echo&>, <noEcho>
	mov	ah, 08h
ELSE
    IFIDNI <&echo&>, <0>
	mov	al, 08h
    ELSE
	mov	ah, 01h
    ENDIF
ENDIF
	int     21h
	ENDM

_BIOSCh MACRO
	mov	ah, 0h
	int	16h
	ENDM

_GetStr	MACRO	Structure
	_LdAddr	dx, Structure
	mov	ah, 0Ah
	int	21h
	ENDM

_GetDate MACRO
	mov	ah, 2ah
	int	21h
	ENDM

_GetTime MACRO
	mov	ah, 2ch
	int	21h
	ENDM

_Open	MACRO	filename, accesstype, segm
	_LdAddr	dx, <filename>
	_LdSeg	ds, <segm>
	mov	ah, 3dh
IFB	<&accesstype&>
	mov	al, 0
ELSE
    IFIDNI <&accesstype&>, <Read>
	mov	al, 0
    ELSE
	IFIDNI <&accesstype&>, <Write>
	mov	al, 1
	ELSE
	    IFIDNI <&accesstype&>, <ReadWrite>
	mov	al, 2
	    ELSE
	mov	al, accesstype
	    ENDIF
	ENDIF
    ENDIF
ENDIF
	int	21h
	ENDM

_Creat	MACRO	filename, segm
	sub     cx, cx
	_LdAddr	dx, <filename>
	_LdSeg	ds, <segm>
	mov	ah, 3ch
	int	21h
	ENDM

_Close MACRO	handle
	mov	bx, handle
	mov	ah, 3eh
	int     21h
	ENDM

_Read	MACRO	handle, offs, count, segm
	mov	cx, count
	mov	bx, handle
	_LdAddr	dx, <offs>
	_LdSeg	ds, <segm>
	mov	ah, 3fh
	int	21h
	ENDM

_Write	MACRO	handle, offs, count, segm
	mov	cx, count
	mov	bx, handle
	_LdAddr	dx, <offs>
	_LdSeg	ds, <segm>
	mov	ah, 40h
	int	21h
	ENDM

_LSeek	MACRO	Handle, SeekType, LoDistance, HiDistance
IFNB <HiDistance>
	mov	cx, HiDistance
	mov	dx, LoDistance
ELSE
    IFDIFI <LoDistance>, <ax>
	mov	ax, LoDistance
    ENDIF
	cwd
	mov	cx, dx
	mov	dx, ax
ENDIF
	mov	bx, Handle
	mov	ah, 42h
IFIDNI <SeekType>, <FromStart>
	mov	al, 0
ELSE
    IFIDNI <SeekType>, <FromCur>
	mov	al, 1
    ELSE
	IFIDNI <SeekType>, <FromEnd>
	mov	al, 2
	ELSE
	mov	al, SeekType
	ENDIF
    ENDIF
ENDIF
	int	21h
	ENDM

_Exit	MACRO	ReturnVal
IFNB	<ReturnVal>
	mov	al, ReturnVal
ENDIF
	mov	ah, 4ch
	int	21h
	ENDM

	.LIST
