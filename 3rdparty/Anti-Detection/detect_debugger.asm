IFDEF RAX
	; 64 bit assembly section
	.DATA
	.CODE
     
	IsDebugged PROC

		MOV RAX, GS:[60h]
		MOV AL, [RAX+02h]
		TEST AL,AL
		JNZ DebuggerDetected
		MOV RAX,RAX
		ret
	DebuggerDetected:
		MOV RAX,1
		ret

	IsDebugged ENDP
     
ELSE
	; 32 bit assembly section
	.586
	.MODEL FLAT, C
	.STACK
	.DATA
	.CODE
     
	ASSUME FS:NOTHING
     
	IsDebugged PROC
		MOV EAX, FS:[30h]
		MOV AL, [EAX+02h]
		TEST AL,AL
		JNZ DebuggerDetected

		MOV EAX, FS:[30h]
		MOV AL, [EAX+02h]
		TEST AL,AL
		JNZ DebuggerDetected	

	;single step :
		PUSH SS
		POP SS
		PUSHFD
		TEST BYTE PTR [ESP+01],1
		POP EAX
		JNE DebuggerDetected
	
	;rtdsc:
		RDTSC
		XOR ECX,ECX
		ADD ECX,EAX
		RDTSC
		SUB EAX,ECX
		CMP EAX,0FFFh
		JNB DebuggerDetected
	
	;IsDebuggerPresent:
		MOV EAX, FS:[18h]
		MOV EAX, [EAX+30h]
		MOVZX EAX,BYTE PTR [EAX+02h]
		CMP EAX,01
		JE DebuggerDetected

		MOV EAX,EAX
		ret
	DebuggerDetected:
		MOV EAX,1
		ret
	IsDebugged ENDP
     
ENDIF
     
END