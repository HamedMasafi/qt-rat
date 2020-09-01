IFDEF RAX
	; 64 bit assembly section
	.DATA
	.CODE
     
	IsVirtualMachineExist PROC

		MOV RAX,564D5868h
		MOV RBX,12345678h
		MOV RCX,0Ah
		MOV RDX,5658h
		IN RAX,DX                
		CMP RBX,564D5868h
		JE VM_Detected


		MOV RAX,RAX
		RET
	VM_Detected:
		MOV RAX,1
		RET

	IsVirtualMachineExist ENDP
     
ELSE
	; 32 bit assembly section
	.586
	.MODEL FLAT, C
	.STACK
	.DATA
	.CODE
     
	ASSUME FS:NOTHING
     
	IsVirtualMachineExist PROC
		MOV EAX,564D5868h
		MOV EBX,12345678h
		MOV ECX,0Ah
		MOV EDX,5658h
		IN EAX,DX                
		CMP EBX,564D5868h
		JE VM_Detected

		MOV EAX,EAX
		RET
	VM_Detected:
		MOV EAX,1
		RET
	IsVirtualMachineExist ENDP
     
ENDIF
     
END