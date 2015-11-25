#ORG &900
#SYMB temp &70
#SYMB rkey &80
	
.encrypt  LDX #0
.round	  LDA &76
	  EOR rkey,X
	  STA &70
	  INX
	  LDA &77
	  EOR rkey,X
	  STA &71
	  INX
	  PHX
	  JSR roundfn

	  LDA &74
	  EOR &72
	  STA &76
	  LDA &75
	  EOR &73
	  STA &77

	  LDA &70
	  STA &74
	  LDA &71
	  STA &75

	  PLX
	  CPX #32
	  BCC round

	  LDA &76
	  EOR rkey,X
	  STA &72
	  INX
	  LDA &77
	  EOR rkey,X
	  STA &73
	  INX
	  LDA &74
	  EOR rkey,X
	  STA &76
	  INX
	  LDA &75
	  EOR rkey,X
	  STA &77
	  LDA &72
	  STA &74
	  LDA &73
	  STA &75
	
.roundfn  LDA &70
	  TAY
	  AND #&F
	  ASL A
	  TAX
	  LDA s1,X
	  STA &72
	  INX
	  LDA s1,X
	  STA &73
	
	  TYA
	  AND #&F0
	  LSR A
	  LSR A
	  LSR A
	  TAX
	  LDA s2,X
	  CLC
	  ADC &72
	  STA &72
	  INX
	  LDA s2,X
	  ADC &73
	  STA &73
	
	  LDA &71
	  TAY
	  AND #&F
	  ASL A
	  TAX
	  LDA s3,X
	  EOR &72
	  STA &72
	  INX
	  LDA s3,X
	  EOR &73
	  STA &73

	  TYA
	  AND #&F0
	  LSR A
	  LSR A
	  LSR A
	  TAX
	  LDA s4,X
	  CLC
	  ADC &72
	  STA &72
	  INX
	  LDA s4,X
	  ADC &73
	  STA &73
	  RTS

.s1	  #EQUW 32000
.s2	  #EQUW 32000
.s3	  #EQUW 32000
.s4	  #EQUW 32000
