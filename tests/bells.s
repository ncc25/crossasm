	#ORG &900
	
.plnote LDA method,X
	CMP #'.
	BEQ plincret
	CMP #'-
	BEQ plret
	STA &70
	LDY #0
.swploop CPY &70
	BEQ noswap
	LDA (&71),Y
	INY
	STA (&72),Y
	LDA (&71),Y
	DEY
	STA (&72),Y
	INY
	INY
	BRA swploop
.noswap LDY &70
	LDA (&71),Y
	STA (&72),Y
	INX
	BRA plnote
.plincret INX
.plret  RTS
.method #EQUS "34.14-1236-36-1236-14.34.12"
	#EQUB 0
	
